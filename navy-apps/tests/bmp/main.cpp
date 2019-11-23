#include <assert.h>
#include <stdlib.h>
#include <ndl.h>
#include <stdio.h>

int main() {
  NDL_Bitmap *bmp = (NDL_Bitmap*)malloc(sizeof(NDL_Bitmap));
  printf("pass1\n");
  NDL_LoadBitmap(bmp, "/share/pictures/projectn.bmp");
  printf("pass2\n");  
  assert(bmp->pixels);
  printf("pass3\n");
  printf("%d, %d\n", bmp->w, bmp->h);
  NDL_OpenDisplay(bmp->w, bmp->h);
  printf("pass4\n");
  NDL_DrawRect(bmp->pixels, 0, 0, bmp->w, bmp->h);
  printf("pass5\n");
  NDL_Render();
  printf("pass6\n");
  NDL_CloseDisplay();
  while (1);
  return 0;
}
