
#ifndef _OJ_PROGRAM_H_
#define _OJ_PROGRAM_H_

#include <glib.h>


typedef struct _OJVariable {
  char *name;
} OJVariable;

typedef struct _OJArgument {
  OJVariable *var;
  int is_indirect;
  int is_indexed;
  OJVariable *index_var;
  int index_scale;
  int type; // remove
  int index; // remove
  int offset;
} OJArgument;

typedef struct _OJInstruction {
  int opcode;

  OJArgument args[3];
} OJInstruction;

typedef struct _OJProgram {
  OJInstruction insns[100];
  int n_insns;

  OJVariable vars[100];
  int n_vars;

  /* parsing state */
  char *s;
  char *error;

  OJInstruction *insn;

}OJProgram;

typedef struct _OJOpcode {
  char *name;
  int n_args;
} OJOpcode;


OJProgram * oj_program_new (void);
int oj_opcode_lookup (const char *s, int len);
void oj_program_parse (OJProgram *p, const char *program);
void oj_program_output_mmx (OJProgram *p);
void oj_program_free (OJProgram *program);

#endif

