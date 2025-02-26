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
size_t fs_lseek(int fd, size_t offset, int whence);
void loader_read(int idx,void* vaddr,uint32_t filesz,uint32_t memsz);

static uintptr_t loader(PCB *pcb, const char *filename) {
  //判断是否为elf：我没写QAQ
  int idx = fs_open(filename, 0, 0);
  //printf("idx:%d\n",idx);
  Elf_Ehdr elf;
  fs_read(idx, &elf, sizeof(Elf_Ehdr));
  // for(int i = 0; i < sizeof(Elf_Ehdr)/4; i++) printf("%x \n",*((uint32_t*)&elf+i));
  // printf("\n");
  for(int i = 0; i < elf.e_phnum; i++){
    fs_lseek(idx, elf.e_phoff + i * elf.e_phentsize, 0);
    Elf_Phdr ent; 
    fs_read(idx, &ent, elf.e_phentsize);
    // for(int i = 0; i < elf.e_phentsize/4; i++) printf("%x \n",*((uint32_t*)&ent+i));
    // printf("\n");
    if(ent.p_type == PT_LOAD){
     // uint8_t  buf[ent.p_memsz+1]; //需要+1吗
      fs_lseek(idx, ent.p_offset, 0);
      loader_read(idx,(void*)ent.p_vaddr,ent.p_filesz,ent.p_memsz);
      // fs_read(idx, buf, ent.p_filesz);
      // for(int j = ent.p_filesz; j <= ent.p_memsz; j++) buf[j] = 0;
      // memcpy((void*)ent.p_vaddr, buf , ent.p_memsz);
     
    }
  }
  fs_close(idx);
  return elf.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  //printf("\nentry: %d %x\n", entry, entry);

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
