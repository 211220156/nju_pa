#include "nemu.h"
#include "cpu/reg.h"
#include "cpu/cpu.h"
#include "memory/memory.h"

#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum
{
	NOTYPE = 256,
	
	NUM,
	REG,
	SYMB,
    HEX,
	/* TODO: Add more token types */
	LEFTP,
	RIGHTP,//262
	EQ,
    ADD,
    SUB,
    DIV,
    MUL,
    DEREF,//解引用
    NEG//取负数
};

static struct rule
{
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +", NOTYPE}, // white space	
	{"0[xX][0-9a-fA-F]{1,8}", HEX},
	{"[0-9]{1,10}", NUM},
	{"\\$e[abcd]x", REG},
	{"\\$e[sb]p", REG},
	{"\\$e[sd]i", REG},
	{"\\$[abcd]x", REG},
	{"\\$[abcd]l", REG},
	{"\\$[abcd]h", REG},
	{"\\$[sb]p", REG},
	{"\\$[sd]i", REG},
	{"\\*", MUL},
    {"-", SUB},	
	{"/", DIV},
	{"\\(", LEFTP},
	{"\\)", RIGHTP},
	{"\\+", ADD},
	{"==", EQ}

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX];

/* Rules are used for more times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
	int i;
	char error_msg[128];
	int ret;

	for (i = 0; i < NR_REGEX; i++)
	{
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if (ret != 0)
		{
			regerror(ret, &re[i], error_msg, 128);
			assert(ret != 0);
		}
	}
}

typedef struct token
{
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;
static bool make_token(char *e)
{
	int position = 0;
	int i;
	regmatch_t pmatch;

	nr_token = 0;

	while (e[position] != '\0')
	{
		/* Try all rules one by one. */
		for (i = 0; i < NR_REGEX; i++)
		{
			if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
			{
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				printf("match regex[%d] at position %d with len %d: %.*s\n", i, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. 
				 * Add codes to perform some actions with this token.
				 */

				switch (rules[i].token_type)
				{
				case REG:
				case HEX:
				case NUM:
				{
				    tokens[nr_token].type = rules[i].token_type;
                    strncpy(tokens[nr_token].str, substr_start, substr_len);
					nr_token++;
					break;
				}
				case NOTYPE:
				    break;//空格直接丢弃
				default:
					tokens[nr_token].type = rules[i].token_type;
					nr_token++;
				}
				break;
			}
		}

		if (i == NR_REGEX)
		{
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true;
}

bool check_parentheses(int p, int q, bool* success)
{
//    printf("in check!\n");
    int stk[32], top = -1, lastOutP = 0;
    for (int i = p; i <= q; i++){
        if (tokens[i].type == LEFTP)
            stk[++top] = i;
        else if (tokens[i].type == RIGHTP) {
            if (top != -1)
                lastOutP = stk[top--];//左括号出栈
            else {//出现多出来的右括号
                *success = false;
                return false; 
            }
        }
    }
    if (top != -1) {
        *success = false;
        return false;
    }
    if (lastOutP == p && tokens[p].type == LEFTP && tokens[q].type == RIGHTP)
        return true;
    else 
        return false;
}
uint32_t eval(int p, int q, bool* success)
{
    if(p > q) {
        if (tokens[p].type == NEG || tokens[p].type == DEREF){
            return 0;
        }
        *success = false;
        return 0;
    }
    else if(p == q) { 
        /* Single token.
         * For now this token should be a number. 
         * Return the value of the number.
         */ 
        switch (tokens[p].type) {
        case NUM: 
            return atoi(tokens[p].str);
        case HEX: {
            int temp = 0;
            for (int i = 2;  i < strlen(tokens[p].str); i++){
                temp *= 16;
                if (tokens[p].str[i] >= 65 && tokens[p].str[i] <= 90)//大写字母
                    temp += tokens[p].str[i] - 'A' + 10;
                else if (tokens[p].str[i] >= 97 && tokens[p].str[i] <= 122)//小写字母
                    temp += tokens[p].str[i] - 'a' + 10;
                else 
                    temp += tokens[p].str[i] - '0';
            }
            return temp;
        }
        case REG:
        {   //实际gpr顺序:eax ecx edx ebx esp ebp esi edi
            if (strcmp(tokens[p].str, "$eax") == 0) return cpu.eax;
            if (strcmp(tokens[p].str, "$ebx") == 0) return cpu.ebx;
            if (strcmp(tokens[p].str, "$ecx") == 0) return cpu.ecx;
            if (strcmp(tokens[p].str, "$edx") == 0) return cpu.edx;
            if (strcmp(tokens[p].str, "$esp") == 0) return cpu.esp;
            if (strcmp(tokens[p].str, "$ebp") == 0) return cpu.ebp;
            if (strcmp(tokens[p].str, "$esi") == 0) return cpu.esi;
            if (strcmp(tokens[p].str, "$edi") == 0) return cpu.edi;
            if (strcmp(tokens[p].str, "$ax") == 0) return cpu.gpr[0]._16;
            if (strcmp(tokens[p].str, "$cx") == 0) return cpu.gpr[1]._16;
            if (strcmp(tokens[p].str, "$dx") == 0) return cpu.gpr[2]._16;
            if (strcmp(tokens[p].str, "$bx") == 0) return cpu.gpr[3]._16;
            if (strcmp(tokens[p].str, "$sp") == 0) return cpu.gpr[4]._16;
            if (strcmp(tokens[p].str, "$bp") == 0) return cpu.gpr[5]._16;
            if (strcmp(tokens[p].str, "$si") == 0) return cpu.gpr[6]._16;
            if (strcmp(tokens[p].str, "$di") == 0) return cpu.gpr[7]._16;
            if (strcmp(tokens[p].str, "$al") == 0) return cpu.gpr[0]._8[0];//？
            if (strcmp(tokens[p].str, "$cl") == 0) return cpu.gpr[1]._8[0];
            if (strcmp(tokens[p].str, "$dl") == 0) return cpu.gpr[2]._8[0];
            if (strcmp(tokens[p].str, "$bl") == 0) return cpu.gpr[3]._8[0];
            if (strcmp(tokens[p].str, "$ah") == 0) return cpu.gpr[0]._8[1];//？
            if (strcmp(tokens[p].str, "$ch") == 0) return cpu.gpr[1]._8[1];
            if (strcmp(tokens[p].str, "$dh") == 0) return cpu.gpr[2]._8[1];
            if (strcmp(tokens[p].str, "$bh") == 0) return cpu.gpr[3]._8[1];
        }
        default:
            return 0;
        }
    }
    else if(check_parentheses(p, q, success) == true ) {
        /* The expression is surrounded by a matched pair of parentheses. 
         * If that is the case, just throw away the parentheses.
         */
        return eval(p + 1, q - 1, success); 
    }
    else {
        //check返回false有两种可能：一种是表达式不合法，这时候success也会变成false，另一种是表达式合法，正常计算
        if (*success == false){
            return 0;
        }
        //下面是正常计算
        int op = 0;
        bool inParentheses = false;
        for (int i = p; i <= q; i++){
            if (op == 0 && tokens[i].type > 262){
                op = i;
            } else {
                if (tokens[i].type == LEFTP)
                    inParentheses = true;
                else if (tokens[i].type == RIGHTP)
                    inParentheses = false;
                else if (tokens[i].type > 262 && tokens[i].type < tokens[op].type && !inParentheses)
                    //优先级底且不在括号里
                    op = i;
            }
        }
        uint32_t val1 = eval(p, op - 1, success);
        uint32_t val2 = eval(op + 1, q, success);
        switch(tokens[op].type) {
            case ADD: return val1 + val2;
            case NEG:
            case SUB: return val1 - val2;
            case DEREF: {
                
            }
            case MUL: return val1 * val2;
            case DIV: return val1 / val2;
            case EQ: return val1 == val2;
            default: return 0;
        
        }
        
    }
}

uint32_t expr(char *e, bool *success)
{
	if (!make_token(e))
	{
		*success = false;
		return 0;
	}
/*	printf("\nPlease implement expr at expr.c\n");
    fflush(stdout);
	assert(0);*/
	for(int i = 0; i < nr_token; i ++) {
        if(tokens[i].type == MUL && (i == 0 || tokens[i - 1].type > 260)) {//若*前一位是运算符（>260） 
            tokens[i].type = DEREF;
        }
        if (tokens[i].type == SUB && (i == 0 || tokens[i - 1].type > 260)){
            tokens[i].type = NEG;
        }
    }
    
	return eval(0, nr_token - 1, success);
}

