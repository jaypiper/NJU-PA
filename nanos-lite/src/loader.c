#include "proc.h"
#include <elf.h>
//#include <stdlib.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#define PBEGIN 0x3000000;
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  //判断是否为elf：我没写QAQ
  Elf_Ehdr elf;
  ramdisk_read(&elf, 0, sizeof(Elf_Ehdr));
  printf("\n total: %d", elf.e_phnum);
  for(int i = 0; i < elf.e_phnum; i++){
    printf("\n%d done\n", i);
    Elf_Phdr ent; 
    //uint32_t type;
    ramdisk_read(&ent, elf.e_phoff + i * elf.e_phentsize, elf.e_phentsize);
    if(ent.p_type == PT_LOAD){
      uint8_t * buf[ent.p_memsz]; //需要+1吗
      
      ramdisk_read(buf, ent.p_offset, ent.p_filesz);
      for(int j = ent.p_filesz; j < ent.p_memsz; j++) buf[j] = 0;
      ramdisk_write(buf, ent.p_offset, ent.p_memsz);
     
    }
    printf("\n%d done\n", i);
  }

  return PBEGIN;
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
