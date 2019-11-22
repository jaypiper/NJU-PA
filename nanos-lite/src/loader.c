#include "proc.h"
#include <elf.h>
#include <stdlib.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#define PBEGIN 0x3000000;
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
int fs_open(const char* pathname, int flags, int mode);
int fs_close(int fd);

static uintptr_t loader(PCB *pcb, const char *filename) {
  //判断是否为elf：我没写QAQ
  int idx = fs_open(filename, 0, 0);

  printf("idx:%d\n",idx);
  Elf_Ehdr elf;
  fs_read(idx, &elf, sizeof(Elf_Ehdr));
  for(int i = 0; i < elf.e_phnum; i++){
    Elf_Phdr ent; 
    fs_read(idx, &ent, elf.e_phentsize);

    if(ent.p_type == PT_LOAD){
      uint8_t  buf[ent.p_memsz+1]; //需要+1吗
      fs_read(idx, buf, ent.p_filesz);
      for(int j = ent.p_filesz; j <= ent.p_memsz; j++) buf[j] = 0;
      memcpy((void*)ent.p_vaddr, buf , ent.p_memsz);
     
    }
  }
  fs_close(idx);
  return elf.e_entry;
}
void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
 
  Log("Jump to entry = %x", entry);

  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
