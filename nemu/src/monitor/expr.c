#include "nemu.h"
#include "cpu/reg.h"
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
	EQ,
	NUM,
	REG,
	SYMB,
    HEX,
    DEREF,//解引用
    NEG,//取负数
	/* TODO: Add more token types */
	LEFTP,
	RIGHTP,
    DIV,
    MUL,
    ADD,
    SUB
    
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
	{"[0-9]{1,10}", NUM},
	{"0[xX][0-9a-fA-F]{1, 8}", HEX},
    {"-", '-'},	
	{"\\*", '*'},
	{"\\(", LEFTP},
	{"\\)", RIGHTP},
	{"\\+", ADD}
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

				printf("match regex[%d] at position %d with len %d: %.*s", i, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. 
				 * Add codes to perform some actions with this token.
				 */

				switch (rules[i].token_type)
				{
				case HEX:
				case NUM:
				{
				    tokens[nr_token].type = rules[i].token_type;
				    if (substr_len > 32){
				        memcpy((void *)tokens[nr_token].str, (void *)substr_start, 32);
				    } else {
				        memcpy((void *)tokens[nr_token].str, (void *)substr_start, substr_len);
				    }
					nr_token++;
					break;
				}
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
        if(tokens[i].type == '*' && (i == 0 || tokens[i - 1].type > 261)) {//若*前一位是运算符（>261） 
            tokens[i].type = DEREF;
        }
        if (token[i].type == '-' && (i == 0 || tokens[i - 1].type > 261)){
            tokens[i].type = NEG;
        }
    }
    
	return eval(0, nr_token - 1, success);
}
bool check_parentheses(int p, int q, bool* success)
{
    int stk[1000], top = -1, lastOutP = 0;
    for (int i = p; i <= q; i++){
        if (tokens[i].type == LEFTP)
            stk[++top] = i;
        else if (tokens[i].type == RIGHTP) {
            if (top != -1)
                lastOutP = stk[top--];
            else {//出现多出来的右括号
                *success = false;
                return false; 
            }
        }
    }
    if (lastOutP == p && tokens[p].type == LEFTP && tokens[q].type == RIGHTP)
        return true;
    else 
        return false;
}
uint32_t eval(int p, int q, bool* success)
{
    if(p > q) {
        *success = false;
        return 0;
    }
    else if(p == q) { 
        /* Single token.
         * For now this token should be a number. 
         * Return the value of the number.
         */ 
        return atoi(tokens[p].str);
    }
    else if(check_parentheses(p, q, success) == true ) {
        /* The expression is surrounded by a matched pair of parentheses. 
         * If that is the case, just throw away the parentheses.
         */
        return eval(p + 1, q - 1); 
    }
    else {
        //check返回false有两种可能：一种是表达式不合法，这时候success也会变成false，另一种是表达式合法，正常计算
        if (*success == false){
            return 0;
        }
        //下面是正常计算
/*        op = the position of dominant operator in the token expression;
        val1 = eval(p, op - 1);
        val2 = eval(op + 1, q);
        switch(op_type) {
            case '+': return val1 + val2;
            case '-': /* ... */
 /*           case '*': /* ... */
 /*           case '/': /* ... */
 /*           default: assert(0);
        
        }*/
        return 0;
    }
}

