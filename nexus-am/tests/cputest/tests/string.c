#include "trap.h"
#include<stdio.h>

char *s[] = {
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
	", World!\n",
	"Hello, World!\n",
	"#####"
};

char str1[] = "Hello";
char str[20];
int strcmp_new(const char* src, const char* dst)
{
    int ret = 0;
    while( !(ret = *(unsigned char*)src - *(unsigned char*)dst) && *dst)
    {
        src ++;
        dst ++;
    }
    if( ret < 0) ret = -1;
    else if(ret > 0) ret = 1;
    return ret;
}


int main() {
	nemu_assert(strcmp_new(s[0], s[2]) == 0);
	nemu_assert(strcmp_new(s[0], s[1]) == -1);
	nemu_assert(strcmp_new(s[0] + 1, s[1] + 1) == -1);
	nemu_assert(strcmp_new(s[0] + 2, s[1] + 2) == -1);
	nemu_assert(strcmp_new(s[0] + 3, s[1] + 3) == -1);


//	nemu_assert(strcmp_new( strcat(strcpy(str, str1), s[3]), s[4]) == 0);
       
	nemu_assert(memcmp(memset(str, '#', 5), s[5], 5) == 0);

	return 0;
}
