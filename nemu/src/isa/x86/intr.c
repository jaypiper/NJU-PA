#include "rtl/rtl.h"

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push(&cpu.eflags);
  rtl_push(&cpu.CS);
  rtl_push(&ret_addr);
  rtl_j((vaddr_read(cpu.IDTR._addr + 8*NO +4,4) & 0xFFFF0000) 
      + (vaddr_read(cpu.IDTR._addr + 8*NO, 4) & 0x0000FFFF));

}

bool isa_query_intr(void) { 
  return false;
}
