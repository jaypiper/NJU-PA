#include "cpu/exec.h"

void raise_intr(uint32_t,vaddr_t);

make_EHelper(lidt) {
  cpu.IDTR._sz = vaddr_read(id_dest->addr,2);
  cpu.IDTR._addr = vaddr_read(id_dest->addr + 2,4);
  if(decinfo.isa.is_operand_size_16)
    cpu.IDTR._addr &= 0x00FFFFFF;
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

  difftest_skip_ref();
}

make_EHelper(int) {
  printf("%x\n",id_dest->val);
  raise_intr(id_dest->val,decinfo.seq_pc);  
  //注意使用intr.c中函数进行实现

  print_asm("int %s", id_dest->str);

  difftest_skip_dut(1, 2);
}

make_EHelper(iret) {
  rtl_pop(&s0);
  rtl_pop(&cpu.CS);
  rtl_pop(&cpu.eflags);
  rtl_j(s0);

  print_asm("iret");
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

make_EHelper(in) {
  switch(id_dest->width){
    case 1: rtl_li(&s0, pio_read_b(id_src->val)); break;
    case 2: rtl_li(&s0, pio_read_w(id_src->val)); break;
    case 4: rtl_li(&s0, pio_read_l(id_src->val)); break;
    default: assert(0);
  }
  operand_write(id_dest,&s0);

  print_asm_template2(in);
}

make_EHelper(out) {
  switch(id_src->width){
    case 1: pio_write_b(id_dest->val, id_src->val); break;
    case 2: pio_write_w(id_dest->val, id_src->val); break;
    case 4: pio_write_l(id_dest->val, id_src->val); break;
    default: assert(0);
  }

  print_asm_template2(out);
}
