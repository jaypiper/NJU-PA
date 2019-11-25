#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  int n = 0;
  while(*s != 0){
    n ++;
    s ++;
  }
  return n;
}

char *strcpy(char* dst,const char* src) {
  char* beg = dst;
   while(*src !=0 ){
     *dst = *src;
     dst ++;
     src ++;
   }
   return beg;
}

char* strncpy(char* dst, const char* src, size_t n) {
  assert(0);
  return NULL;
}

char* strcat(char* dst, const char* src) {
  char* beg = dst;
  while(*dst) dst++;
  while(*src){
    *dst = *src;
    dst ++;
    src ++;
  }
  *dst = 0;
  return beg;
}

int strcmp(const char* s1, const char* s2) {
  while(!(*s1 == 0 || *s2 == 0)){
    if(*s1 > *s2) return 1;
    else if(*s1 < *s2) return -1;
    s1 ++;
    s2 ++;
  }
  if(*s1 == 0 && *s2 != 0) return -1;
  else if(*s2 == 0 && *s1 != 0) return 1;
  return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    for(int i = 0; i < n; i++){
    if(*s1 > *s2) return 1;
    if(*s1 < *s2) return -1;
    s1++; s2++;
  }
  return 0;
}

void* memset(void* v,int c,size_t n) {
  char* tem = v;
  for(int i = 0; i < n; i++) {
    *tem = c;
    tem++;
  }
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  //不允许overlap??
  char* ob = out;
  const char* ib = in;
  for(int i = 0; i < n; i++){
    *ob = *ib;
    ob ++;
    ib ++;
  }
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  const char* bs1 = s1;
  const char* bs2 = s2;
  for(int i = 0; i < n; i++){
    if(*bs1 > *bs2) return 1;
    if(*bs1 < *bs2) return -1;
    bs1++; bs2++;
  }

  return 0;
}

#endif
