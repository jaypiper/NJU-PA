#ifndef __ARCH_H__
#define __ARCH_H__

struct _Context {
  struct _AddressSpace *as;  //还没有完全确定as位置
  uintptr_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  int irq;
  uintptr_t eip;
  uintptr_t cs;
  uintptr_t eflags;
 
};

#define GPR1 eax
#define GPR2 eip
#define GPR3 eip
#define GPR4 eip
#define GPRx eip

#endif
