#include <stdio.h>

int main() {
  FILE *fp = fopen("/dev/events", "r");
  int time = 0;
  printf("Start to receive events...\n");
  while (1) {
    char buf[256];
    char *p = buf, ch;
    while ((ch = fgetc(fp)) != -1) {
      *p ++ = ch;
      if(ch == '\n') {
        *p = '\0';
        break;
      }
    }
    int is_time = buf[0] == 't';
    //printf("eventbuf: %d\n",is_time);
    time += is_time;
    if (!is_time) {
      printf("receive event: %s\n", buf);
    }
    else if (time % 1024 == 0) {
      printf("receive time event for the %dth time: %s\n", time, buf);
    }
  }

  fclose(fp);
  return 0;
}

