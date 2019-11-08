#include "rtl/rtl.h"
#include "isa/mmu.h"
void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  union {
    GateDesc gd;
    struct{
      uint32_t lo, hi;
    };
  } gate;
  vaddr_t addr = cpu.IDTR._addr + 8*NO;
  //printf("addr: %x\n",addr);
  gate.lo = vaddr_read(addr,4);
  gate.hi = vaddr_read(addr+4,4);
  if(gate.gd.present){
    rtl_push(&cpu.eflags);
    rtl_push(&cpu.CS);
    rtl_push(&ret_addr);
    rtl_j((uint32_t)(gate.gd.offset_31_16 << 16) | gate.gd.offset_15_0);
  }
  

}

bool isa_query_intr(void) { 
  return false;
}
