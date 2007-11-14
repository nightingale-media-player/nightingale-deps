/*******************************************************************************
 *******************************************************************************
 *
 * Portability header for libgpod.
 *
 *******************************************************************************
 ******************************************************************************/

/*
 * Take errno definition from mingw errno.h.  The libgw32c errno.h overrides the
 * mingw errno.h.  However, errno is not provided by the libgw32c library.
 */

int* _errno(void);
#define errno (*_errno())


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


