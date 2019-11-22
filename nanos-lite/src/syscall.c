#include "common.h"
#include "syscall.h"

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_exit: _halt(a[2]); break;
    case SYS_yield: _yield(); break;
    case SYS_write: 
      if(a[3] == 0 || (a[1] != 1 && a[1] != 2)) {
        c->GPRx = 0;  //x是返回值吗呜呜呜
        break;
      }
      c->GPRx = 1;
      char* out_ = (char*)a[2];
      for(int i = 0; i  < a[3]; i++) {
        _putc(*out_);
        out_++;
      }
      break;

    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
