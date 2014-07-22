/*
 * glib-compat.c
 * Functions copied from glib 2.10
 *
 * Copyright 2005 David Schleef <ds@schleef.org>
 */

#include "gst_private.h" /* for g_warning */
#include <glib.h>

G_BEGIN_DECLS

/* copies */
#if !GLIB_CHECK_VERSION(2,25,0)

#if defined (_MSC_VER) && !defined(_WIN64)
typedef struct _stat32 GStatBuf;
#else
typedef struct stat GStatBuf;
#endif

#endif

/* adaptations */

G_END_DECLS
