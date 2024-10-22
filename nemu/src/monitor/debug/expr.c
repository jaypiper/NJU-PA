#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>


enum {
  TK_NOTYPE = 256, NUMBER = 512, TK_EQ = 257, TK_SOE = 258, TK_AND = 259, HEX = 260, REG = 261, NEG = 262, ADDRESS = 263,
  TK_NOTEQ = 264,
  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"0x(0|1|2|3|4|5|6|7|8|9|a|b|c|d|e|f|A|B|C|D|E|F)+", HEX},
  {"\\$([A-Z]|[a-z]|[0-9])+", REG},
  {"(0|1|2|3|4|5|6|7|8|9)+", NUMBER},
  {"\\(", '('},
  {"\\)", ')'},
  {"\\*", '*'},
  {"/", '/'},
  {"-", '-'},
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"!=", TK_NOTEQ},
  {"<=", TK_SOE},             
  {"&&", TK_AND}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};
uint32_t isa_reg_str2val(char*,bool*);

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[512] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
  while (e[position] != '\0' && e[position] != 10) {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        /*Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);*/
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
	      tokens[nr_token].type = rules[i].token_type;
	      for(int j=0;j<substr_len;j++){
	        tokens[nr_token].str[j] = substr_start[j];
          if(j < 31) tokens[nr_token].str[j+1] = '\0';
	      }
        nr_token++;
        //printf("remember the token.str size is just 32, deal with it!\n");
        //下面先注释掉，后面记得实现
        // switch (rules[i].token_type) {
        //   default: printf("hello, there still left somthing to do\n");//TODO();
        // }

        break;
      }
    }
    
    if (i == NR_REGEX) {
      //tokens debug使用
      //printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


static uint32_t compute_num(uint32_t i){
  uint32_t num = 0;
  /*十进制数*/
  if(tokens[i].type == NUMBER || tokens[i].type == NEG){
      for(int j = 0; j < 32 && tokens[i].str[j] != 0; j++){
          num *= 10;
          num += tokens[i].str[j]-'0';
      }
  
      if(tokens[i].type == NEG) num = 0 - num;
  
  }
  /*十进制/十六进制地址*/
  else if(tokens[i].type == ADDRESS){
      if(tokens[i].str[1] == 'x'){
          for(int j = 2; j < 32 && tokens[i].str[j] != 0; j++){
              num *= 16;
	            if(tokens[i].str[j] <= '9') num += tokens[i].str[j] - '0';
	            else if(tokens[i].str[j] <= 'Z') num += tokens[i].str[j] - 'A' + 10;
	            else if(tokens[i].str[j] <= 'z') num += tokens[i].str[j] - 'a' + 10;
	        }
      }

      else{
          for( int j = 0; j < 32 && tokens[i].str[j] != 0; j++){
		          num *= 10;
		          num += tokens[i].str[j] - '0';
	        }
      }
     
      num = paddr_read(num,4);

  }
  /*十六进制数*/
  else if(tokens[i].type == HEX){
      for(int j = 2; j < 32 && tokens[i].str[j] != 0; j++){
          num *= 16;
          if(tokens[i].str[j] <= '9') num += tokens[i].str[j] - '0';
          else if(tokens[i].str[j] <= 'Z') num += tokens[i].str[j] - 'A' + 10;
          else if(tokens[i].str[j] <= 'z') num += tokens[i].str[j] - 'a' + 10;
      } 
  }
  /*寄存器 $eax...*/
  else if(tokens[i].type == REG){
    bool judge = false;
    num = isa_reg_str2val(&tokens[i].str[1],&judge);
  }
 
  return num;
}


static uint32_t eval(int beg, int end){
  //++iter;
  //printf("at beg: %d, end: %d\n",beg,end); //测试代码
  //if(iter>=20)return 0;
  if(beg > end)return 0;
  
  /* 处理负号 */
  int pre_type = 0; //1表示数字，0表示符号
  for(int i = beg; i <= end; i++){
    if(tokens[i].type == '-' && pre_type == 0){
        tokens[i++].type = TK_NOTYPE;
        while(tokens[i].type == TK_NOTYPE) i++;
	      tokens[i].type = NEG;
        }
    /* 处理解引用 */
    else if(tokens[i].type == '*' && pre_type == 0){
        tokens[i++].type = TK_NOTYPE;
        while(tokens[i].type == TK_NOTYPE) i++;
	      tokens[i].type = ADDRESS;
    }
    if(tokens[i].type == NUMBER || tokens[i].type == NEG || tokens[i].type == ADDRESS || tokens[i].type == REG || tokens[i].type == HEX) pre_type = 1;
    else if(tokens[i].type != TK_NOTYPE) pre_type = 0;
    
  }
 
	/*计算*/
  if(tokens[beg].type == TK_NOTYPE) return eval(beg+1,end);
  if(tokens[end].type == TK_NOTYPE) return eval(beg,end-1);
  if(beg == end) return compute_num(beg);

  int in_par_num = 0;  //当前括号的层数
  int main_op = 0; //主运算符位置
  for(int i = beg; i <= end; i++){
    if(tokens[i].type == '(') in_par_num++;
    else if(tokens[i].type == ')') in_par_num--;
    else if((tokens[i].type == '+' || tokens[i].type == '-') && in_par_num == 0) main_op = i;
    else if((tokens[i].type == '*' || tokens[i].type == '/') && in_par_num ==0 && tokens[main_op].type != '+' && tokens[main_op].type != '-') main_op = i;
    else if((tokens[i].type == TK_AND || tokens[i].type == TK_EQ || tokens[i].type == TK_NOTEQ || tokens[i].type == TK_SOE)&& in_par_num == 0 && tokens[main_op].type != '+' && tokens[main_op].type != '-' && tokens[main_op].type != '*' && tokens[main_op].type != '/') main_op = i;
  }
  if(main_op == 0 && tokens[beg].type == '(' && tokens[end].type == ')')return eval(beg+1,end-1);
  uint32_t val1 = eval(beg, main_op-1);
  uint32_t val2 = eval(main_op + 1, end);
  //printf("%d %d",val1,val2);
  //处理二元运算符。 一元运算符全部压缩到运算对象中
  switch(tokens[main_op].type){
    case '+': return val1 + val2;
    case '-': return val1 - val2;
    case '*': return val1 * val2;
    case '/': return val1 / val2;
    case TK_NOTEQ:  return (val1 != val2);
    case TK_SOE: return (val1 <= val2);
    case TK_EQ:   return (val1 == val2);
    case TK_AND:  return (val1 && val2); 
    default: printf("wrong at token_num: %d, token_type: %d\n",main_op,tokens[main_op].type); return 0;
  }
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  *success = true;
  return eval(0,nr_token-1);
  /* TODO: Insert codes to evaluate the expression. */
  
  //TODO();

}
/*不要bool指针版*/
uint32_t expr_val(char* e){
  make_token(e);
  return eval(0,nr_token-1);
}

uint32_t atoui(char* str){
  uint32_t num = 0;
  while(*str){
    num += *str - '0';
    num *= 10;
  }
  return num;
}
