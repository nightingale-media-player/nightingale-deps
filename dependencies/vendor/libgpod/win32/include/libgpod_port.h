/*******************************************************************************
 *******************************************************************************
 *
 * Portability header for libgpod.
 *
 *******************************************************************************
 ******************************************************************************/

/*
 * Remap gettext macros.
 *
 *   gettext is not properly set up.  To avoid crashes in ligbpod when using
 * strings, the gettext macros are redefined.
 */

#include <config.h>
#include <glib/gi18n-lib.h>

#undef _
#define _(String) String
#undef Q_
#define Q_(String) String

#undef fprintf
#undef vfprintf
#undef printf
#undef vprintf
#undef sprintf
#undef vsprintf


/* implement sync() the way libgw32c does */
#define sync() _flushall()
