#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ, NEQ, DIGIT, HEX, AND, OR, REGISTER

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
        int precedence;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */
        {"\\b[0-9]+\\b",DIGIT,0},                         // degit
        {"\\b0x[a-fA-F0-9]+\\b",HEX,0},                   // hex
        {"\\$[a-zA-Z]+",REGISTER,0},                      // register 
        {"	+",NOTYPE,0},                             // tab
	{" +",	NOTYPE,0},				// spaces
	{"\\+", '+',4},					// plus
        {"-",'-',4},                                      // minus
        {"\\*",'*',5},                                    // multiple
        {"/",'/',5},                                      // divide
        {"!",'!',6},                                      // not
        {"&&",AND,2},                                     // and
        {"\\|\\|",OR,1},                                  // or
	{"==", EQ,3},					// equal
        {"!=",NEQ,3},                                     // not equal
        {"\\(",'(',7},                                    // left (
        {"\\)",')',7},                                    // right )
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
        int precedence;
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
                                char *for_register = e + position + 1;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
                                        case NOTYPE: break;
                                        case REGISTER: 
                                               tokens[nr_token].type=rules[i].token_type;
                                               tokens[nr_token].precedence=rules[i].precedence;
                                               strncpy(tokens[nr_token].str,for_register,substr_len-1);
                                               tokens[nr_token].str[substr_len-1]='\0';
                                               nr_token++;
                                               break;
					default: 
                                               tokens[nr_token].type=rules[i].token_type;
                                               tokens[nr_token].precedence=rules[i].precedence;
                                               strncpy(tokens[nr_token].str,substr_start,substr_len);
                                               tokens[nr_token].str[substr_len]='\0';
                                               nr_token++;
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

bool check_parentheses(int l,int r){
        if(tokens[l].type=='('&&tokens[r].type==')'){
            int index;
            int lcnt=0,rcnt=0;
            for(index=l;index<=r;index++){
                 if(tokens[index].type=='('){lcnt++;}
                 if(tokens[index].type==')'){rcnt++;}
                 if(rcnt >  lcnt){return false;}
            }
            if(lcnt==rcnt){return true;}
        }
        return false;
}

int dominant_operator(int l,int r){
         int operator=l;
         int m,n;
         int min_precedence=8;
         for(m=l;m<=r;m++){
              if(tokens[m].type==DIGIT||tokens[m].type==HEX||tokens[m].type==REGISTER){continue;}
              int count=0;
              bool flag = true;
              for(n=m-1;n>=l;n--){
                  if(tokens[n].type=='('&&count==0){flag=false;break;}
                  if(tokens[n].type=='('){count--;}
                  if(tokens[n].type==')'){count++;}
              }
              if(!flag){continue;};
              if(tokens[m].precedence<min_precedence){min_precedence=tokens[m].precedence;operator=m;}

         }
         return operator;
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	panic("please implement me");
	return 0;
}

