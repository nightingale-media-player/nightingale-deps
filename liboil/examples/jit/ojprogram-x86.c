
#include "config.h"

#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ojprogram.h"


void
emit (unsigned int x)
{
  g_print ("%02x ", x);
}

void
emit_modrm (OJArgument *reg, OJArgument *regmem)
{
  int bits_mod;
  int bits_rm;
  int bits_reg;

  if (regmem->is_indirect) {
    if (regmem->offset == 0) {
      bits_mod = 0;
      bits_rm = regmem->index;
      bits_reg = reg->index;
      emit ((bits_reg << 5) | (bits_mod << 3) | (bits_rm));
    } else {
      if (regmem->offset <= 127 && regmem->offset >= -128) {
        bits_mod = 1;
        bits_rm = regmem->index;
        bits_reg = reg->index;
        emit ((bits_reg << 5) | (bits_mod << 3) | (bits_rm));
        emit (regmem->offset);
      } else {
        bits_mod = 2;
        bits_rm = regmem->index;
        bits_reg = reg->index;
        emit ((bits_reg << 5) | (bits_mod << 3) | (bits_rm));
        emit ((regmem->offset>>0) & 0xff);
        emit ((regmem->offset>>8) & 0xff);
        emit ((regmem->offset>>16) & 0xff);
        emit ((regmem->offset>>24) & 0xff);
      }
    }
  } else {
    bits_mod = 3;
    bits_rm = regmem->index;
    bits_reg = reg->index;
    emit ((bits_reg << 5) | (bits_mod << 3) | (bits_rm));
  }

}

void
oj_program_output_mmx (OJProgram *program) 
{
  int i;
  char *dest_regs[] = { "error", "eax", "error", "error" };
  char *src_regs[] = { "error", "ecx", "edx", "error" };
  char *r_regs[] = { "mm0", "mm1", "mm2", "mm3" };
  OJInstruction *insn;

  g_print("  push %%ebp\n");
  g_print("  movl %%esp, %%ebp\n");
  g_print("  movl 0x8(%%ebp), %%%s\n", dest_regs[1]);
  g_print("  movl 0xc(%%ebp), %%%s\n", src_regs[1]);
  g_print("  movl 0x10(%%ebp), %%%s\n", src_regs[2]);

  for(i=0;i<program->n_insns;i++){
    insn = program->insns + i;
    switch (insn->opcode) {
      case 0:
        g_print ("  movq %d(%%%s), %%%s\n",
            insn->args[1].offset,
            src_regs[insn->args[1].index],
            r_regs[insn->args[0].index]);
        emit (0x0f);
        emit (0x6f);
        emit_modrm (insn->args + 0, insn->args + 1);
        g_print ("\n");
        break;
      case 1:
        g_print ("  movq %%%s, %d(%%%s)\n",
            r_regs[insn->args[1].index],
            insn->args[0].offset,
            dest_regs[insn->args[0].index]);
        emit (0x0f);
        emit (0x7f);
        emit_modrm (insn->args + 1, insn->args + 0);
        g_print ("\n");
        break;
      case 2:
        g_print ("  movq %%%s, %%%s\n",
            r_regs[insn->args[2].index],
            r_regs[insn->args[0].index]);
        emit (0x0f);
        emit (0x6f);
        emit_modrm (insn->args + 0, insn->args + 1);
        g_print ("\n");
        g_print ("  paddw %%%s, %%%s\n",
            r_regs[insn->args[1].index],
            r_regs[insn->args[0].index]);
        emit (0x0f);
        emit (0xfd);
        emit_modrm (insn->args + 0, insn->args + 1);
        g_print ("\n");
        break;
    }
  }

  g_print("  emms\n");
  g_print("  pop %%ebp\n");
  g_print("  ret\n");
}

