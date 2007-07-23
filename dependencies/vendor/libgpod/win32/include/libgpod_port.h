/*******************************************************************************
 *******************************************************************************
 *
 * BEGIN WINDJAY GPL
 *
 * Copyright (c) 2006-2007 Erik Staats.
 * http://www.windjay.com
 *
 * This file may be licensed under the terms of of the
 * GNU General Public License Version 2 (the "GPL").
 *
 * Software distributed under the License is distributed
 * on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either 
 * express or implied. See the GPL for the specific language 
 * governing rights and limitations.
 *
 * You should have received a copy of the GPL along with this 
 * program. If not, go to http://www.gnu.org/licenses/gpl.html
 * or write to the Free Software Foundation, Inc., 
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * 
 * END WINDJAY GPL
 *
 *******************************************************************************
 *
 * Portability header for libgpod.
 *
 *******************************************************************************
 ******************************************************************************/

/*
 * Map errno to _errno.
 *
 *   Using mingw and libgw32c, errno is used but not available.  Not sure why.
 * Should figure out what's going on and do a real fix.
 */

extern int _errno;
#define errno _errno


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


