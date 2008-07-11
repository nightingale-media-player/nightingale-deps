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
#include <liboil/liboilfault.h>

//#include <unistd.h>
//#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
//#include <stdio.h>
#include <setjmp.h>
#include <signal.h>


static jmp_buf jump_env;
#ifdef HAVE_SIGACTION
static struct sigaction action;
static struct sigaction oldaction;
#else
static void * oldhandler;
#endif
static int in_try_block;
static int enable_level;

static void
illegal_instruction_handler (int num)
{
  if (in_try_block) {
#ifdef HAVE_SIGSETJMP
#if 0
    /* alternate method of siglongjmp() */
    sigset_t set;
    sigemptyset (&set);
    sigaddset (&set, SIGILL);
    sigprocmask (SIG_UNBLOCK, &set, NULL);
    longjmp (jump_env, 1);
#else
    siglongjmp (jump_env, 1);
#endif
#else
    longjmp (jump_env, 1);
#endif
  } else {
    abort ();
  }
}

/**
 * oil_fault_check_enable:
 *
 * Enables fault checking mode.  This function may be called multiple times.
 * Each call to this function must be paired with a corresponding call
 * to oil_fault_check_disable().
 *
 * This function sets a signal handler for SIGILL.
 */
void
oil_fault_check_enable (void)
{
  if (enable_level == 0) {
#ifdef HAVE_SIGACTION
    memset (&action, 0, sizeof(action));
    action.sa_handler = &illegal_instruction_handler;
    sigaction (SIGILL, &action, &oldaction);
#else
    oldhandler = signal (SIGILL, illegal_instruction_handler);
#endif
    in_try_block = 0;
    OIL_INFO("enabling SIGILL handler.  Make sure to continue past "
        "any SIGILL signals caught by gdb.");
  }
  enable_level++;
}

/**
 * oil_fault_check_try:
 * @func: the function to attempt
 * @priv: a value to pass to the function
 *
 * Calls to this
 * function must be preceded by a call to oil_fault_check_enable()
 * to enable fault checking mode.  This function sets up recovery
 * information and then calls the function @func with the parameter
 * @priv.  If @func or any other functions it calls attempt to execute
 * an illegal instruction, the exception will be caught and recovered from.
 *
 * Returns: 1 if the function was executed sucessfully, 0 if the
 * function attempted to execute an illegal instruction.
 */
int
oil_fault_check_try (void (*func) (void *), void *priv)
{
  int ret;

  in_try_block = 1;
#ifdef HAVE_SIGSETJMP
  ret = sigsetjmp (jump_env, 1);
#else
  ret = setjmp (jump_env);
#endif
  if (!ret) {
    func (priv);
  }
  in_try_block = 0;

  return (ret == 0);
}

/**
 * oil_fault_check_disable:
 *
 * Disables fault checking mode.  See oil_fault_check_enable()
 * for details.
 */
void
oil_fault_check_disable (void)
{
  enable_level--;
  if (enable_level == 0) {
#ifdef HAVE_SIGACTION
    sigaction (SIGILL, &oldaction, NULL);
#else
    signal (SIGILL, oldhandler);
#endif
    OIL_INFO("disabling SIGILL handler");
  }
}

