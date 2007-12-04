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
 *   Rename functions that use structure arguments containing time fields so
 * that the 32 bit time field function variants are used.
 *  On Vista, calling stat would pass a structure with 32-bit time fields, but
 * the CRT stat would fill it with 64-bit time fields, resulting in memory
 * corruption.  Normally, this would be handled transparently or by defining
 * _USE_32BIT_TIME_T.  However, it's not properly handled with the mixed mingw
 * compile and MSVC linking.
 */

#define stat _stat
#define fstat _fstat


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


