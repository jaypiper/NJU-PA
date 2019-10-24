#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  for(int i = 0; i < 8; i++){
    if(!(ref_r->ebp==cpu.ebp)) return false;
  }

  return ref_r->pc == pc;
}

void isa_difftest_attach(void) {
}
