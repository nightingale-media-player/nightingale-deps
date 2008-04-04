
#include "config.h"

#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ojprogram.h"


const OJOpcode opcode_list[] = {
  { "load", 2 },
  { "store", 2 },
  { "add", 3 }
};
#define N_OPCODES 3

#define ARG_REG 0
#define ARG_SRC 1
#define ARG_DEST 2


OJProgram *
oj_program_new (void)
{
  OJProgram *p;
  p = g_malloc0(sizeof(OJProgram));
  return p;
}

int
oj_opcode_lookup (const char *s, int len)
{
  int i;
  for(i=0;i<N_OPCODES;i++){
    if (strlen (opcode_list[i].name) != len) continue;
    if (strncmp (opcode_list[i].name, s, len) == 0) {
      return i;
    }
  }

  return -1;
}

static gboolean
get_opcode (OJProgram *p)
{
  char *s;
  char *opcode;
  int opcode_len;
  int i;

  if (p->error) return FALSE;

  g_print("looking for opcode at \"%s\"\n", p->s);

  s = p->s;
  while (g_ascii_isspace (s[0])) s++;
  opcode = s;
  while (g_ascii_isalnum (s[0])) s++;
  opcode_len = s - opcode;

  if (opcode_len == 0) {
    p->error = g_strdup ("expected opcode");
    return FALSE;
  }

  p->insn->opcode = oj_opcode_lookup (opcode, opcode_len);

  for(i=0;i<N_OPCODES;i++){
    if (strlen (opcode_list[i].name) != opcode_len) continue;
    if (strncmp (opcode_list[i].name, opcode, opcode_len) == 0) {
      break;
    }
  }
  if (i == N_OPCODES) {
    p->error = g_strdup ("unknown opcode");
    return FALSE;
  }

  p->insn->opcode = i;
  p->s = s;
  return TRUE;
}

static gboolean
get_arg (OJProgram *p, int i)
{
  char *s;
  char *end;

  if (p->error) return FALSE;

  g_print("looking for arg at \"%s\"\n", p->s);

  s = p->s;
  while (g_ascii_isspace (s[0])) s++;

  switch (s[0]) {
    case 'r':
      p->insn->args[i].type = ARG_REG;
      break;
    case 's':
      p->insn->args[i].type = ARG_SRC;
      break;
    case 'd':
      p->insn->args[i].type = ARG_DEST;
      break;
    default:
      p->s = s;
      p->error = g_strdup ("expected argument");
      return FALSE;
  }
  s++;

  p->insn->args[i].index = strtoul (s, &end, 10);
  if (s == end) {
    p->s = s;
    p->error = strdup ("expected number");
    return FALSE;
  }

  s = end;

  p->s = s;
  return TRUE;
}

static gboolean
skip_comma (OJProgram *p)
{
  char *s;

  if (p->error) return FALSE;
  
  g_print("looking for comma at \"%s\"\n", p->s);

  s = p->s;
  while (g_ascii_isspace (s[0])) s++;
  if (s[0] != ',') {
    p->error = g_strdup ("expected comma");
    return FALSE;
  }
  s++;
  while (g_ascii_isspace (s[0])) s++;

  p->s = s;
  return TRUE;
}

void
oj_program_parse (OJProgram *p, const char *program)
{
  char **lines;
  char *line;
  char *s;
  int i;

  lines = g_strsplit(program, "\n", 0);

  for(i=0;lines[i];i++){
    p->insn = p->insns + p->n_insns;
    line = lines[i];

    s = line;

    g_print("looking at \"%s\"\n", s);

    while (g_ascii_isspace (s[0])) s++;
    if (s[0] == 0 || s[0] == '#') continue;

    p->s = s;
    get_opcode (p);

    get_arg (p, 0);
    if (opcode_list[p->insn->opcode].n_args >= 2) {
      skip_comma (p);
      get_arg (p, 1);
      if (opcode_list[p->insn->opcode].n_args >= 3) {
        skip_comma (p);
        get_arg (p, 2);
      }
    }

    if (p->error) {
      g_print("error on line %d: %s at \"%s\"\n", i, p->error, p->s);
      g_free(p->error);
      p->error = NULL;
    }

    p->n_insns++;
  }
  g_strfreev (lines);
}

