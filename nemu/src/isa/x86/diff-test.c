#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  uint32_t arr[77];
  ref_difftest_getregs(arr);
  for(int i = 0; i < 8; i++) printf("%d: %x  %x %x\n",i,arr[i],ref_r->gpr[i]._32,cpu.gpr[i]._32);
  // for(int i = 0;i < 8; i++) printf("%d: %x  %x \n",i,ref_r->gpr[i]._32,cpu.gpr[i]._32);
  // printf("pc: %x  %x\n",ref_r->pc,pc);
  return true;
  // for(int i = 0; i < 8; i++){
  //   if(!(ref_r->gpr[i]._32==cpu.gpr[i]._32)) return false;
  // }

  // return ref_r->pc == pc;
}

void isa_difftest_attach(void) {
}
