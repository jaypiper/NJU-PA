#include <stdio.h>
#include <assert.h>

int main() {
  printf("PASS-1\n");
  
  FILE *fp = fopen("/share/texts/num", "r+");
  printf("PASS0\n");
  assert(fp);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  assert(size == 5000);
  printf("PASS1\n");

  fseek(fp, 500 * 5, SEEK_SET);
  int i, n;
  for (i = 500; i < 1000; i ++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1);
  }
  printf("PASS2\n");

  fseek(fp, 0, SEEK_SET);
  for (i = 0; i < 500; i ++) {
    fprintf(fp, "%4d\n", i + 1 + 1000);
  }
  printf("PASS3\n");

  for (i = 500; i < 1000; i ++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1);
  }
  printf("PASS4\n");

  fseek(fp, 0, SEEK_SET);
  for (i = 0; i < 500; i ++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1 + 1000);
  }

  fclose(fp);

  printf("PASS!!!\n");

  return 0;
}
