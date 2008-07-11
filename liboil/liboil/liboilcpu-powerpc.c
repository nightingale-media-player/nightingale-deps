/*
 * LIBOIL - Library of Optimized Inner Loops
 * Copyright (c) 2003,2004 David A. Schleef <ds@schleef.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <liboil/liboilfunction.h>
#include <liboil/liboildebug.h>
#include <liboil/liboilcpu.h>
#include <liboil/liboilfault.h>
#include <liboil/liboilutils.h>

//#include <unistd.h>
//#include <fcntl.h>
//#include <stdlib.h>
//#include <string.h>
//#include <stdio.h>
//#include <setjmp.h>
//#include <signal.h>
//#include <sys/time.h>
//#include <time.h>

#if defined(__FreeBSD__)
#include <sys/types.h>
#include <sys/sysctl.h>
#endif


/***** powerpc *****/

static unsigned long
oil_profile_stamp_tb(void)
{
  unsigned long ts;
  __asm__ __volatile__("mftb %0\n" : "=r" (ts));
  return ts;
}

static void
test_altivec (void * ignored)
{
  asm volatile ("vor v0, v0, v0\n");
}

#if defined(__FreeBSD__)
void
oil_check_altivec_sysctl (void)
{
  int ret, av;
  size_t len;

  len = sizeof(enabled);
  ret = sysctlbyname("hw.altivec", &av, &len, NULL, 0);
  if (!ret && av) {
    oil_cpu_flags |= OIL_IMPL_FLAG_ALTIVEC;
  }
}
#endif

void
oil_check_altivec_fault (void)
{
  oil_fault_check_enable ();
  if (oil_fault_check_try(test_altivec, NULL)) {
    OIL_DEBUG ("cpu flag altivec");
    oil_cpu_flags |= OIL_IMPL_FLAG_ALTIVEC;
  }
  oil_fault_check_disable ();
}

void
oil_cpu_detect_arch(void)
{
#if defined(__FreeBSD__)
  oil_check_altivec_sysctl();
#else
  oil_check_altivec_fault();
#endif

  _oil_profile_stamp = oil_profile_stamp_tb;
}



