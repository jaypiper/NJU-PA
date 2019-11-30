#include "fs.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t fbsync_write(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
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
  {"stdout", 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, invalid_read, serial_write},
  {"/dev/fb", 0, 0, invalid_read, fb_write},
  {"/proc/dispinfo", 128, 0, dispinfo_read, invalid_write},
  {"/dev/events", 256, 0, events_read, invalid_write},
  {"/dev/fbsync",0 , 0, invalid_read, fbsync_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  //file_table[NR_FB].size = screen_width() * screen_height() * sizeof(uint32_t);
  assert(!strcmp(file_table[3].name,"/dev/fb"));
  int screen_height();
  int screen_width();
  file_table[3].size=screen_height()*screen_width()*(32/8);
  Log("Initializing filesystem... %d files loaded.", NR_FILES);
}

size_t fs_write(int fd, const void *buf, size_t len) {
  /*size_t write_size = len;
  WriteFn writef = file_table[fd].write;
  if (file_table[fd].write == NULL) writef = (WriteFn)ramdisk_write;
  if ((file_table[fd].size - file_table[fd].open_offset) <= len){
    write_size = file_table[fd].size - file_table[fd].open_offset;
  } 
  write_size = writef(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  file_table[fd].open_offset += write_size;
  return write_size;
  */
  /*
  WriteFn write = file_table[fd].write == NULL ? (WriteFn) ramdisk_write : file_table[fd].write;
  if (file_table[fd].size == 0) {
    return write(buf, 0, len);
  } else {
    if (file_table[fd].open_offset >= file_table[fd].size) {
      return 0; // Fail
    } else {
      if (file_table[fd].open_offset + len > file_table[fd].size) {
        len = file_table[fd].size - file_table[fd].open_offset;
      }
      size_t offset = file_table[fd].disk_offset + file_table[fd].open_offset;
      size_t delta = write(buf, offset, len);
      file_table[fd].open_offset += delta;
      return delta;
    }
  }
  */   
  /*Finfo *File = &file_table[fd];
  if (File->write != NULL){
    size_t reallen = File->write(buf, File->open_offset, len);
    File->open_offset += reallen;
    return reallen;
  }
  if (File->open_offset + len > File->size){
    len = File->size - File->open_offset;
  }
  File->open_offset += ramdisk_write(buf, File->disk_offset + File->open_offset, len);
  return len;
  */
    if(file_table[fd].write == NULL){
        if(file_table[fd].open_offset + len > file_table[fd].size)
            len = file_table[fd].size - file_table[fd].open_offset;
        ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
    }
    else file_table[fd].write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
    file_table[fd].open_offset += len;   
    //Log("write success!");
    return len;
}


size_t fs_read(int fd, void *buf, size_t len){
  /*size_t read_size = len;
  ReadFn readf = file_table[fd].read;
  if (file_table[fd].read == NULL) readf = (ReadFn)ramdisk_read;
  if ((file_table[fd].size - file_table[fd].open_offset) <= len){
    read_size = file_table[fd].size - file_table[fd].open_offset;
  }
  read_size = readf(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  file_table[fd].open_offset += read_size;
  return read_size;
  */
  
  /*
  ReadFn read = file_table[fd].read == NULL ? (ReadFn)ramdisk_read : file_table[fd].read;
  if (file_table[fd].size == 0) {
    return read(buf, 0, len);
  } else {
    if (file_table[fd].open_offset >= file_table[fd].size) {
      return 0; // EOF
    } else {
      if (file_table[fd].open_offset + len > file_table[fd].size) {
        len = file_table[fd].size - file_table[fd].open_offset;
      }
      size_t offset = file_table[fd].disk_offset + file_table[fd].open_offset;
      size_t delta = read(buf, offset, len);
      file_table[fd].open_offset += delta;
      return delta;
    }
  }
  */
  /*Finfo *File = &file_table[fd];
  if (File->read != NULL){
    size_t reallen = File->read(buf, File->open_offset, len);
    File->open_offset += reallen;
    return reallen;
  }
  if (File->open_offset + len > File->size){
    len = File->size - File->open_offset;
  }
  File->open_offset += ramdisk_read(buf, File->disk_offset + File->open_offset, len);
  return len;
  */
  int ret = 0;
	//Log("fs_read: fd = %d, name = %s, offset = %d, len = %d", fd, file_table[fd].name, file_table[fd].open_offset, len);
	if(file_table[fd].open_offset + len > file_table[fd].size){
		len = file_table[fd].size - file_table[fd].open_offset;
    }

	if(file_table[fd].read == NULL){
  	ret = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
	}
	else ret = file_table[fd].read(buf, file_table[fd].open_offset, len);
  file_table[fd].open_offset += ret;
  //Log("read_size = %d, filename = %s\n", ret, file_table[fd].name);
  return ret;
  
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  assert(fd >= 0 && fd < NR_FILES);
  size_t old_offset = file_table[fd].open_offset;
  switch (whence) {
    case SEEK_SET:
      file_table[fd].open_offset = offset;
      break;
    case SEEK_CUR:
      file_table[fd].open_offset = file_table[fd].open_offset + offset;
      break;
    case SEEK_END:
      file_table[fd].open_offset = file_table[fd].size + offset;
      break;
    default:
      assert(0);
  }
  if(file_table[fd].open_offset > file_table[fd].size){
    file_table[fd].open_offset = old_offset;
    return -1;
  }
  //printf("open_offset = %d\n", file_table[fd].open_offset);
  return file_table[fd].open_offset;
}

int fs_open(const char *pathname, int flags, int mode){
  //Log("Used open function.");
  for (int i = 0; i < NR_FILES; ++i) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      file_table[i].open_offset = 0;
      return i;
    }
  }
  panic("Invalid pathname %s", pathname);
  return -1;
}

int fs_close(int fd){
  //printf("Successfully close!\n");
  return 0;
}

void loader_read(int idx,void* vaddr,uint32_t filesz,uint32_t memsz){
      ramdisk_read(vaddr, file_table[idx].disk_offset + file_table[idx].open_offset, filesz);
      for(int j = filesz; j <= memsz; j++) *((uint8_t*)vaddr + j) = 0;
}
