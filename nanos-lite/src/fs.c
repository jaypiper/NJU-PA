#include "fs.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;   //类型是off_t?？   //QAQ对应fireable应该加在最后面
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, invalid_read, serial_write},
  {"/dev/fb", 0, 0, invalid_read, fb_write},
  {"/proc/dispinfo", 128, 0,dispinfo_read, invalid_write},
  {"/dev/events", 256, 0, events_read, invalid_write},
#include "files.h"      //这种包括方法也太强了！
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t ramdisk_read(void *buf, size_t offset, size_t len);

void init_fs() {
  int w = screen_width();
  int h = screen_height();
  assert(!strcmp(file_table[3].name,"/dev/fb"));
  file_table[3].size = w * h * 4;
 // printf("w: %d, h: %d, size: %d\n",w, h,file_table[3].size);
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char* pathname, int flags, int mode){
  //通过pathname进行索引，找到的话返回文件描述符，否则assertion(assert?)终止
  int idx = 0;
  for(idx = 0; idx < NR_FILES; idx++){
    if(strcmp(file_table[idx].name, pathname) == 0) break;
  }
  printf("idx: %d, %s\n", idx,pathname);
  assert(idx < NR_FILES);
  file_table[idx].open_offset = 0;
  return idx;
}
/*尝试从fd(file descroptor)中读取len个字节，放入buf开始的位置，返回读取字节数
  增加file offset（open offset),（读取字节数, 不一定所有能读到len字节)
  =W=我不想检测len是否为0*/
size_t fs_read(int fd, void *buf, size_t len){
  //printf("read fd: %d\n",fd);
  ReadFn readf = file_table[fd].read;
  size_t read_size = len;
  if(readf == NULL) {
    readf = ramdisk_read;
    if(file_table[fd].size - file_table[fd].open_offset < len)
      read_size = file_table[fd].size - file_table[fd].open_offset;
    readf(buf, file_table[fd].disk_offset + file_table[fd].open_offset, read_size);
    file_table[fd].open_offset += read_size;// 到底是加之前的还是之后的呢？QAQ好像是一样的
  }
  else if(!strcmp(file_table[fd].name, "/proc/dispinfo")||!strcmp(file_table[fd].name, "/bin/event")){
    if(file_table[fd].open_offset >= file_table[fd].size) return 0;
    if(file_table[fd].open_offset + len > file_table[fd].size)
      read_size = file_table[fd].size - file_table[fd].open_offset;
    readf(buf, file_table[fd].open_offset, read_size);
    file_table[fd].open_offset += read_size;
    // printf("%d\n",file_table[fd].open_offset);
    // printf("%s\n",buf);
    // printf("read_size: %d",read_size);
  }
  else readf(buf, 0, read_size);
  //ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, read_size);
  return read_size;
}
/* 从offset开始写入吗*/
size_t fs_write(int fd, const void *buf, size_t len){
  size_t write_size = len;
  WriteFn writef =  file_table[fd].write;
  if(file_table[fd].write == NULL){
    writef = ramdisk_write;
    if(file_table[fd].size - file_table[fd].open_offset < len)
      write_size = file_table[fd].size - file_table[fd].open_offset;
    writef(buf, file_table[fd].disk_offset + file_table[fd].open_offset, write_size);
    file_table[fd].open_offset += write_size;
    return write_size;
  }
  else return writef(buf, 0, write_size);
  // file_table[fd].open_offset += write_size;   //QAQ忘记加offset了
  // return write_size;
}
/* 允许open_offset超过文件大小,但讲义上面说不要，因此如果超过那么操作
   无效，并返回-1*/
size_t fs_lseek(int fd, size_t offset, int whence){
  size_t newoff = file_table[fd].open_offset;
  switch(whence){
    case SEEK_SET: newoff = offset; break;
    case SEEK_CUR: newoff += offset; break;
    case SEEK_END: newoff = file_table[fd].size + offset; break;
    default: return -1;
  }
  if(newoff > file_table[fd].size) return -1;
  file_table[fd].open_offset = newoff;
  return newoff;
}

int fs_close(int fd){
  //file_table[fd].open_offset = 0;
  return 0;
}