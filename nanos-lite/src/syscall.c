#include "common.h"
#include "syscall.h"
#include "proc.h"

size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);
int fs_open(const char* pathname, int flags, int mode);
void naive_uload(PCB *pcb, const char *filename);

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_exit: _halt(a[2]); break;
    case SYS_yield: _yield(); break;
    case SYS_write:  c -> GPRx =fs_write(a[1],(void*)a[2], a[3]); break;
    case SYS_brk: c  -> GPRx = 0; break;
    case SYS_read: c -> GPRx = fs_read(a[1],(void*)a[2], a[3]); break;
    case SYS_close: fs_close(a[1]); break;
    case SYS_lseek: c -> GPRx = fs_lseek(a[1], a[2], a[3]); break;
    case SYS_open: c -> GPRx = fs_open((char*)a[1], a[2], a[3]); break;
    case SYS_execve: naive_uload(NULL,(void*)a[1]); //第一个参数实际为pcb

    default: panic("Unhandled syscall ID = %d", a[0]);
  }
// if(a[3] == 0 || (a[1] != 1 && a[1] != 2)) {
//                     c->GPRx = 0;  //x是返回值吗呜呜呜
//                     break;
//                   }
//                   c->GPRx = 1;
//                   char* out_ = (char*)a[2];
//                   for(int i = 0; i  < a[3]; i++) {
//                     _putc(*out_);
//                     out_++;
//                   }
//                   break;
  return NULL;
}
