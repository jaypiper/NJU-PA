#include <assert.h>
#include <stdlib.h>
#include <ndl.h>
#include <stdio.h>

int main() {
  printf("reach-0");
  NDL_Bitmap *bmp = (NDL_Bitmap*)malloc(sizeof(NDL_Bitmap));
  printf("reach-1");
  NDL_LoadBitmap(bmp, "/share/pictures/projectn.bmp");
  printf("reach-2");
  assert(bmp->pixels);
  printf("reach-3");
  NDL_OpenDisplay(bmp->w, bmp->h);
  printf("reach-4");
  NDL_DrawRect(bmp->pixels, 0, 0, bmp->w, bmp->h);
  printf("reach-5");
  NDL_Render();
  printf("reach-6");
  NDL_CloseDisplay();
  while (1);
  return 0;
}
