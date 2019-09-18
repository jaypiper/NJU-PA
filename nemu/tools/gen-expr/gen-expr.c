#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
// this should be enough
#define ran(x) (srand(time(NULL)) % x)
#define rand_op() (srand(time(NULL)) % 4)
static char buf[65536];
static int buf_position = 0;
static uint32_t gen_rand_num(){
  return srand(time(NULL));
}
static char gen_rand_op(){
  int op_rand = rand_op();
  switch(op_rand){
    case 0: return '+'; break;
    case 1: return '-'; break;
    case 2: return '*'; break;
    default: return '/'; break;
  }
}
static inline void gen_rand_expr() {
  buf[0] = '\0';
  switch(ran(3)){
    case 0: uint32_t a = gen_rand_num();
            uint32_t b = a, bit_num = 0;
            while(b != 0){ b /= 10; bit_num ++; }
            for(int i = bit_num - 1; i >= 0; i --){ 
              buf[buf_position + i] = (a % 10) + '0';
              a /= 10;
              buf_position ++;
            }
            break;
    case 1: buf[buf_position] = '(';   buf_position ++;
            gen_rand_expr();
            buf[buf_position] = '(';   buf_position ++
            break;
    default: gen_rand_expr(); gen_rand_op(); gen_rand_expr();
  }
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
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
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

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
