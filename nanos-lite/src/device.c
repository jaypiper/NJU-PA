#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  const char* out = buf;
  for(int i = 0; i < len; i++, out++) _putc(*out);
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  return 0;
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {

  if(len + offset > sizeof(dispinfo)) len = sizeof(dispinfo) - offset;
  memcpy(buf, dispinfo+offset, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  printf("len: %d, offset: %d", len, offset);
  len /= 4;
  int x = offset % screen_width();
  int y = offset / screen_width();
  
  for(int beg = 0; beg < len; ){
    if(len - beg > screen_width() - x){
      draw_rect((uint32_t*) buf, x, y, screen_width()-x, 1);
      x = 0; y++;
      beg += screen_width() - x;
    }
    else{
      draw_rect((uint32_t*)buf, x, y, len - beg, 1);
      x += len - beg;
      beg = len;
    }
    // draw_rect((uint32_t*)disp, x, y, 1, 1);
    // offset ++;
    // disp += 4;
  }
  return 0;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();
  sprintf(dispinfo, "WIDTH: %d\nHEIGHT: %d\n", screen_width(), screen_height());
  //printf("%s\n",dispinfo);
  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}