#include "cpu/exec.h"
#include "cc.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
  rtl_j(decinfo.jmp_pc);

  print_asm("jmp %x", decinfo.jmp_pc);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint32_t cc = decinfo.opcode & 0xf;
  rtl_setcc(&s0, cc);
  rtl_li(&s1, 0);
  rtl_jrelop(RELOP_NE, &s0, &s1, decinfo.jmp_pc);
  //printf("sf: %d,  of: %d\n",cpu.SF,cpu.OF);
  print_asm("j%s %x", get_cc_name(cc), decinfo.jmp_pc);
}

make_EHelper(jmp_rm) {
  rtl_jr(&id_dest->val);

  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  // the target address is calculated at the decode stage
  rtl_push(pc);
  //printf("pc: %x\n",*pc);
  rtl_j(decinfo.jmp_pc);
 // printf("pc: %x\n",*pc);
  print_asm("call %x", decinfo.jmp_pc);
}

make_EHelper(ret) {
  rtl_pop(&s0);
  rtl_j(s0);
  print_asm("ret");
}

make_EHelper(ret_imm) {
  TODO();

  print_asm("ret %s", id_dest->str);
}

make_EHelper(call_rm) {
  rtl_push(pc);

  rtl_jr(&id_dest->val);

  print_asm("call *%s", id_dest->str);
}
