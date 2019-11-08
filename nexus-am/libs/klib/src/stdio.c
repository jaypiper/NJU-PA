#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  char buf[1024];
  va_list ap;
  va_start(ap,fmt);
  vsprintf(buf,fmt,ap);
  for(char* c = buf; *c; c++) _putc(*c);
  va_end(ap);
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *s;
  char *c = out;
  int d,d_num = 0;
   while(*fmt){
    if(*fmt == '%'){
      fmt++;
      d_num = 0;
      switch(*fmt){
        case 's': s = va_arg(ap,char*); 
                  while(*s){ *c = *s; s++; c++;}
                  c++;
                  break;
        case 'd': d = va_arg(ap,int); 
                  if(d == 0){
                    *c = '0';
                    c++;
                  }
                  for(int i = d; i!=0; i/=10) d_num++;
                  for(int i = d_num; i > 0; i--){
                    *(c + i - 1) = d % 10 + '0';
                    d /= 10;
                  }                 
                  c += d_num;
                  break;
        case 'x': d = va_arg(ap,int);
                  if(d == 0){
                    *c = '0';
                    c++;
                  }
                  for(int i = d; i != 0; i /= 16) d_num++;
                  for(int i = d_num; i > 0; i--){
                    if((d & 0xff) > 9) *(c + i - 1) = (d & 0xff) -10 +'a';
                    else *(c + i - 1) = (d & 0xff) + '0';
                    d /= 16;
                  }
                  c += d_num;
                  break;
        default :  break;
      }
    }
    else{
      *c = *fmt;
      c++;
    }
    *c =  0;
    fmt++;
  }
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  
  va_start(ap,fmt);
  vsprintf(out,fmt,ap);
  va_end(ap);
  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {

  return 0;
}

#endif
