#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test) {
  rtl_and(&s0,&id_src->val, &id_dest->val);
  rtl_update_ZFSF(&s0,id_dest->width);
  rtl_zero_CF();
  rtl_zero_OF();
  print_asm_template2(test);
}

make_EHelper(and) {
  //TODO();
  rtl_li(&ir,id_src -> val);
  rtl_li(&s0,id_dest -> val);
  reg_l(id_dest->reg) = c_and(ir,s0);
  print_asm_template2(and);
}

make_EHelper(xor) {
  id_dest -> val = c_xor(id_src -> val, id_dest -> val);
  if(id_dest -> type == OP_TYPE_REG)
    reg_l(id_dest -> reg) = id_dest -> val;
  else if(id_dest -> type == OP_TYPE_MEM)
    vaddr_write(id_dest -> addr, id_dest -> val, id_dest -> width);
  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&s0,&id_dest->val,&id_src->val);
  rtl_andi(&s0,&s0,0xffffffffu >> ((4-id_dest->width)*8));
  operand_write(id_dest,&s0);
  rtl_update_ZFSF(&s0,id_dest->width);
  rtl_zero_CF();
  rtl_zero_OF();
  print_asm_template2(or);
}

make_EHelper(sar) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  TODO();

  print_asm_template1(not);
}
