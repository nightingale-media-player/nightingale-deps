/* Windows DMO encoder wrapper for GStreamer
 * Copyright (C) 2009 Pioneers of the Inevitable <songbird@songbirdnest.com>
 *
 * Authors: Michael Smith <msmith@songbirdnest.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>

#include "dmoenc.h"

#define GST_CAT_DEFAULT dmoenc_debug
GST_DEBUG_CATEGORY (dmoenc_debug);

static gboolean
plugin_init (GstPlugin * plugin)
{
  GST_DEBUG_CATEGORY_INIT (dmoenc_debug, "dmoenc", 0, "WM DMO Encoders");

  if (!gst_element_register (plugin, "wmadmoenc", GST_RANK_NONE,
      wmadmoenc_get_type ()))
    return FALSE;

  if (!gst_element_register (plugin, "wmvdmoenc", GST_RANK_NONE,
      wmvdmoenc_get_type ()))
    return FALSE;

  return TRUE;
}

extern "C" {

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, GST_VERSION_MINOR,
    "dmoenc",
    "Windows Media DMO Encoders", plugin_init, VERSION, "LGPL", "GStreamer",
    "http://gstreamer.net/")

}

