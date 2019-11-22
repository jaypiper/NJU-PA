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

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, invalid_read, invalid_write},
  {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"      //这种包括方法也太强了！
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t ramdisk_read(void *buf, size_t offset, size_t len);

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char* pathname, int flags, int mode){
  //通过pathname进行索引，找到的话返回文件描述符，否则assertion(assert?)终止
  int idx = 0;
  for(idx = 0; idx < NR_FILES; idx++){
    if(strcmp(file_table[idx].name, pathname) == 0) break;
  }
  assert(idx < NR_FILES);
  file_table[idx].open_offset = 0;
  return idx;
}
/*尝试从fd(file descroptor)中读取len个字节，放入buf开始的位置，返回读取字节数
  增加file offset（open offset),（读取字节数, 不一定所有能读到len字节)
  =W=我不想检测len是否为0
*/
size_t fs_read(int fd, void *buf, size_t len){
  size_t read_size = len;
  if(file_table[fd].open_offset + len > file_table[fd].size)
    read_size = file_table[fd].size - file_table[fd].open_offset;
  read_size = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, read_size);
  file_table[fd].open_offset += read_size;// 到底是加之前的还是之后的呢？
  return read_size;
}

/* 从offset开始写入吗
*/
size_t fs_write(int fd, const void *buf, size_t len){
  size_t write_size = len;
  if(file_table[fd].open_offset + len > file_table[fd].size)
    write_size = file_table[fd].size - file_table[fd].open_offset;
  write_size = ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, write_size);
  return write_size;
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
  file_table[fd].open_offset = 0;
  return 0;
}