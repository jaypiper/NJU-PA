#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //printf("\nreached!\n");
  if(id_dest->width==1)id_dest->val = (int32_t)(int8_t)id_dest->val;
  else if(id_dest->width == 2)id_dest->val = (int32_t)(int16_t)id_dest->val;
  rtl_push(&id_dest -> val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&cpu.gpr[id_dest->reg]._32);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  rtl_mv(&s0,&cpu.esp);
  for(int i = 0; i < 4; i++) rtl_push(&cpu.gpr[i]._32);
  rtl_push(&s0);
  for(int i = 5; i < 8; i++) rtl_push(&cpu.gpr[i]._32);

  print_asm("pusha");
}

make_EHelper(popa) {
  for(int i = 7; i > 4; i--) rtl_pop(&cpu.gpr[i]._32);
  rtl_pop(&s0);
  for(int i = 3; i >= 0; i--) rtl_pop(&cpu.gpr[i]._32);
  //rtl_mv(&cpu.esp,&s0);

  print_asm("popa");
}

make_EHelper(leave) {
  rtl_li(&cpu.esp, cpu.ebp);
  rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decinfo.isa.is_operand_size_16) {
    rtl_mv(&s0,&cpu.eax);
    if((s0>>15)&1) rtl_li(&cpu.edx,0x0000FFFF);
    else rtl_li(&cpu.edx,0);
  }
  else {
    rtl_mv(&s0,&cpu.eax);
    if((s0>>31)&1) rtl_li(&cpu.edx,0xFFFFFFFF);
    else rtl_li(&cpu.edx,0);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
    rtl_mv(&s0,&cpu.eax);
    if((s0>>7)&1) rtl_ori(&cpu.eax,&cpu.eax,0x0000FF00);
    else rtl_andi(&cpu.eax,&cpu.eax,0xFFFF00FF);
  }
  else {
    rtl_mv(&s0,&cpu.eax);
    if((s0>>15)&1) rtl_ori(&cpu.eax,&cpu.eax,0xFFFF0000);
    else rtl_andi(&cpu.eax,&cpu.eax,0x0000FFFF);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(movs){
    cpu.edi += 1;
    cpu.esi += 1;
    //  s0 = *((uint8_t*)(intptr_t)cpu.esi);
    //  *((uint8_t*)(intptr_t)cpu.edi) = (uint8_t)s0;
    // rtl_mv(cpu.edi, &s0);
    // rtl_mv(&s1,&cpu.edi);
    
    // rtl_andi(&s0, &s0, 0xFFFFFFFFu >> 8*(4-decinfo.dest.width));  
    // rtl_andi(&s1, &s1, 0xFFFFFFFFu >> 8*(4-decinfo.dest.width));
    // rtl_lm(&s0,&s0,decinfo.dest.width);
    // rtl_sm(&s1,&s0,decinfo.dest.width);
} 

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
