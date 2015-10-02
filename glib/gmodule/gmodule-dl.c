/* GMODULE - GLIB wrapper code for dynamic module loading
 * Copyright (C) 1998, 2000 Tim Janik
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/. 
 */

/* 
 * MT safe
 */
#include "config.h"

#include <dlfcn.h>

/* Perl includes <nlist.h> and <link.h> instead of <dlfcn.h> on some systmes? */


/* dlerror() is not implemented on all systems
 */
#ifndef	G_MODULE_HAVE_DLERROR
#  ifdef __NetBSD__
#    define dlerror()	g_strerror (errno)
#  else /* !__NetBSD__ */
/* could we rely on errno's state here? */
#    define dlerror()	"unknown dl-error"
#  endif /* !__NetBSD__ */
#endif	/* G_MODULE_HAVE_DLERROR */

/* some flags are missing on some systems, so we provide
 * harmless defaults.
 * The Perl sources say, RTLD_LAZY needs to be defined as (1),
 * at least for Solaris 1.
 *
 * Mandatory:
 * RTLD_LAZY   - resolve undefined symbols as code from the dynamic library
 *		 is executed.
 * RTLD_NOW    - resolve all undefined symbols before dlopen returns, and fail
 *		 if this cannot be done.
 * Optionally:
 * RTLD_GLOBAL - the external symbols defined in the library will be made
 *		 available to subsequently loaded libraries.
 */
#ifndef	RTLD_LAZY
#define	RTLD_LAZY	1
#endif	/* RTLD_LAZY */
#ifndef	RTLD_NOW
#define	RTLD_NOW	0
#endif	/* RTLD_NOW */
/* some systems (OSF1 V5.0) have broken RTLD_GLOBAL linkage */
#ifdef G_MODULE_BROKEN_RTLD_GLOBAL
#undef	RTLD_GLOBAL
#endif /* G_MODULE_BROKEN_RTLD_GLOBAL */
#ifndef	RTLD_GLOBAL
#define	RTLD_GLOBAL	0
#endif	/* RTLD_GLOBAL */


/* --- functions --- */
static gchar*
fetch_dlerror (gboolean replace_null)
{
  gchar *msg = dlerror ();

  /* make sure we always return an error message != NULL, if
   * expected to do so. */

  if (!msg && replace_null)
    return "unknown dl-error";

  return msg;
}

static gpointer
_g_module_open (const gchar *file_name,
		gboolean     bind_lazy,
		gboolean     bind_local)
{
  gpointer handle;
  
  handle = dlopen (file_name,
		   (bind_local ? 0 : RTLD_GLOBAL) | (bind_lazy ? RTLD_LAZY : RTLD_NOW));
  if (!handle)
    g_module_set_error (fetch_dlerror (TRUE));
  
  return handle;
}

static gpointer
_g_module_self (void)
{
  gpointer handle;
  
  /* to query symbols from the program itself, special link options
   * are required on some systems.
   */
  
#ifdef __BIONIC__
  handle = RTLD_DEFAULT;
#else
  handle = dlopen (NULL, RTLD_GLOBAL | RTLD_LAZY);
#endif
  if (!handle)
    g_module_set_error (fetch_dlerror (TRUE));
  
  return handle;
}

static void
_g_module_close (gpointer handle,
		 gboolean is_unref)
{
  /* Intentionally not dlclose()ing because it is safer to leave the library
   * loaded in memory than to close it and possibly leave dangling pointers
   * to things like atexit handlers, atfork handlers, blocks, etc.
   *
   * See https://trac.macports.org/ticket/45309 for an example from when
   * a library used by a module added a new dependency which had an
   * initializer which added a child atfork handler.  The result is that
   * after closing the module, the system had a dangling pointer for the
   * atfork handler which would at best crash on the child side of fork()
   * and at worst lead to arbitray code execution of whatever happened to be
   * at that location in memory at a later time in the process.
   */
}

static gpointer
_g_module_symbol (gpointer     handle,
		  const gchar *symbol_name)
{
  gpointer p;
  gchar *msg;

  fetch_dlerror (FALSE);
  p = dlsym (handle, symbol_name);
  msg = fetch_dlerror (FALSE);
  if (msg)
    g_module_set_error (msg);
  
  return p;
}

static gchar*
_g_module_build_path (const gchar *directory,
		      const gchar *module_name)
{
  if (directory && *directory) {
    if (strncmp (module_name, "lib", 3) == 0)
      return g_strconcat (directory, "/", module_name, NULL);
    else
      return g_strconcat (directory, "/lib", module_name, "." G_MODULE_SUFFIX, NULL);
  } else if (strncmp (module_name, "lib", 3) == 0)
    return g_strdup (module_name);
  else
    return g_strconcat ("lib", module_name, "." G_MODULE_SUFFIX, NULL);
}
