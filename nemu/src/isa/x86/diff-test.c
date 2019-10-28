#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  //  for(int i = 0;i < 8; i++) printf("%d: %x  %x \n",i,ref_r->gpr[i]._32,cpu.gpr[i]._32);
  //  printf("pc: %x  %x\n",ref_r->pc,pc);
  // return true;
  for(int i = 0; i < 8; i++){
    if(!(ref_r->gpr[i]._32==cpu.gpr[i]._32)) return false;
  }
  if(!(ref_r->CF == cpu.CF)) return false;
  if(!(ref_r->ZF == cpu.ZF)) return false;
  if(!(ref_r->SF == cpu.SF)) return false;
  if(!(ref_r->OF == cpu.OF)) return false;

  return true;
   //return ref_r->pc == pc;
}

void isa_difftest_attach(void) {
}
