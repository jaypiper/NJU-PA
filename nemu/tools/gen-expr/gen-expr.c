#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
// this should be enough

static char buf[65536];
static uint32_t buf_position = 0;
static uint32_t gen_rand_num(){

  return rand()%100;
}
static char gen_rand_op(){

  switch(rand() % 4){
    case 0: return '+'; break;
    case 1: return '-'; break;
    case 2: return '*'; break;
    default: return '/'; break;
  }
}
static void space(){
  uint32_t sp_num = rand() % 5;
  for(int i = 0; i < sp_num; i++)buf[buf_position++] = ' ';
  return;
}
static inline void gen_rand_expr() {
  
  //printf("position: %d\n",buf_position);
  uint32_t a = gen_rand_num();
  uint32_t b = a, bit_num = 0;
 
  switch(rand() % 4){
    case 0: //分配两种情况使得表达式长度收敛
    case 1: space();
            while(b != 0){ b /= 10; bit_num ++; }
            for(int i = bit_num - 1; i >= 0; i --){ 
              buf[buf_position + i] = (a % 10) + '0';
              a /= 10;              
            }
            if(bit_num == 0) buf[buf_position++]='0';
            //printf("case 0\n");
            buf_position += bit_num;
            space();
            break;
    case 2: space();
            buf[buf_position++] = '(';  
            gen_rand_expr();
            buf[buf_position++] = ')';   
            space();
            break;
    default:  gen_rand_expr(); buf[buf_position++] = gen_rand_op(); gen_rand_expr(); 
  }
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\\n\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  
  int i;
  for (i = 0; i < loop; i ++) {
    buf[0] = '\0';
    buf_position = 0;
    gen_rand_expr();
    buf[buf_position] = '\0';
    sprintf(code_buf, code_format, buf);
    //printf("%s\n",buf);
    
    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
