
#include "config.h"

#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ojprogram.h"

const char *program =
"load r0, s1\n"
"load r1, s2\n"
"add r2, r0, r1\n"
"store d1, r2\n";

int
main (int argc, char *argv[])
{
  OJProgram *p;

  p = oj_program_new ();
  oj_program_parse (p, program);
  oj_program_output_mmx (p);

  return 0;
}


