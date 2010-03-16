/* GStreamer
 * OSX video sink
 * Copyright (C) 2004-6 Zaheer Abbas Merali <zaheerabbas at merali dot org>
 * Copyright (C) 2007,2008,2009 Pioneers of the Inevitable <songbird@songbirdnest.com>
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
 *
 * The development of this code was made possible due to the involvement of
 * Pioneers of the Inevitable, the creators of the Songbird Music player.
 * 
 */

/**
 * SECTION:element-osxvideosink
 *
 * The OSXVideoSink renders video frames to a MacOSX window. The video output
 * must be directed to a window embedded in an existing NSApp.
 *
 * When the NSView to be embedded is created an element #GstMessage with a 
 * name of 'have-ns-view' will be created and posted on the bus. 
 * The pointer to the NSView to embed will be in the 'nsview' field of that 
 * message. The application MUST handle this message and embed the view
 * appropriately.
 */

#include "config.h"

#include "osxvideosink.h"
#include <unistd.h>
#include <pthread.h>
#import "cocoawindow.h"

GST_DEBUG_CATEGORY (gst_debug_osx_video_sink);
#define GST_CAT_DEFAULT gst_debug_osx_video_sink

static const GstElementDetails gst_osx_video_sink_details =
GST_ELEMENT_DETAILS ("OSX Video sink",
    "Sink/Video",
    "OSX native videosink",
    "Zaheer Abbas Merali <zaheerabbas at merali dot org>");

static GstStaticPadTemplate gst_osx_video_sink_sink_template_factory =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("video/x-raw-yuv, "
        "framerate = (fraction) [ 0, MAX ], "
        "width = (int) [ 1, MAX ], "
	"height = (int) [ 1, MAX ], "
#if G_BYTE_ORDER == G_BIG_ENDIAN
       "format = (fourcc) YUY2")
#else
        "format = (fourcc) UYVY")
#endif
    );

enum
{
  ARG_0,
  ARG_EMBED
};

//
// @brief Utility category entry for |NSThread|.
//
@interface NSThread (GstUtls)

//
// @returns True if the caller is on the main thread, false if not.
// @note The 10.5 SDK and greater provide a |isMainThread| method.
//
+ (BOOL)inMainThread;

@end

@implementation NSThread (GstUtls)

+ (BOOL)inMainThread
{
  return (pthread_main_np() == 1);
}

@end

//
// @brief Helper ObjC class to ensure that view objects are created and
//        released on the main thread.
//
@interface GstThreadService : NSObject
{
  NSView *mView;  // weak
}

//
// @brief Create a |GstGlView| with a given rectangle. Calling this method
//        to create the view will ensure that the view will be created on
//        the main thread.
//
- (NSView *)createGstGLViewWithRect:(NSRect)aRect;

//
// @brief Release a |GstGLView|. If the caller is not on the main thread,
//        the method will ensure that the object is released on the main thread.
//
- (void)releaseGstGLView:(GstGLView *)aView;

//
// @brief Set the video size of a |GstGLView|. If the caller is not on the main
//        thread, the method will ensure that the object is sized on the
//        main thread.
//
- (void)setGstGLViewVideoSize:(GstGLView *)aView size:(NSSize)aSize;

@end


@interface GstThreadService (Private)
- (void)_proxyCreateView:(NSString *)aStringRect;
- (void)_proxyReleaseView:(GstGLView *)aView;
- (void)_proxySetVideoSize:(NSString *)aStringSize;
@end

@implementation GstThreadService

- (NSView *)createGstGLViewWithRect:(NSRect)aRect
{
  if ([NSThread inMainThread]) {
    return [[GstGLView alloc] initWithFrame:aRect];
  }
  else {
    NSString *frameStr = NSStringFromRect(aRect);
    [self performSelectorOnMainThread:@selector(_proxyCreateView:)
                           withObject:frameStr
                        waitUntilDone:YES];
    return mView;
  }
}

- (void)releaseGstGLView:(GstGLView *)aView
{
  if ([NSThread inMainThread]) {
    [aView release];
  }
  else {
    mView = aView;
    [self performSelectorOnMainThread:@selector(_proxyReleaseView:)
                           withObject:aView
                        waitUntilDone:YES];
  }
}

- (void)setGstGLViewVideoSize:(GstGLView *)aView size:(NSSize)aSize
{
  if ([NSThread inMainThread]) {
    [aView setVideoSize:aSize]; 
  }
  else {
    NSString *sizeStr = NSStringFromSize(aSize);
    mView = aView;
    [self performSelectorOnMainThread:@selector(_proxySetVideoSize:)
                           withObject:sizeStr
                        waitUntilDone:YES];
  }
}

- (void)_proxyCreateView:(NSString *)aStringRect
{
  mView = [[GstGLView alloc] initWithFrame:NSRectFromString(aStringRect)];
}

- (void)_proxyReleaseView:(GstGLView *)aView
{
  [mView release];
}

- (void)_proxySetVideoSize:(NSString *)aStringRect
{
  if (!mView) {
    return;
  }

  [mView setVideoSize:NSSizeFromString(aStringRect)];
}

@end


static GstVideoSinkClass *parent_class = NULL;

/* This function handles osx window creation */
static GstOSXWindow *
gst_osx_video_sink_osxwindow_new (GstOSXVideoSink * osxvideosink, gint width,
    gint height)
{
  NSRect rect;
  GstOSXWindow *osxwindow = NULL;
  GstStructure *s;
  GstMessage *msg;
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

  g_return_val_if_fail (GST_IS_OSX_VIDEO_SINK (osxvideosink), NULL);

  GST_DEBUG_OBJECT (osxvideosink, "Creating new OSX window");

  osxwindow = g_new0 (GstOSXWindow, 1);

  osxwindow->width = width;
  osxwindow->height = height;

  /* Allocate our GstGLView for the window, and then tell the application
   * about it (hopefully it's listening...) */
  rect.origin.x = 0.0;
  rect.origin.y = 0.0;
  rect.size.width = (float) osxwindow->width;
  rect.size.height = (float) osxwindow->height;

  // Use the special thread service class to ensure that the view is created
  // on the main thread.
  GstThreadService *gstThreadService = [[GstThreadService alloc] init];
  osxwindow->gstview = [gstThreadService createGstGLViewWithRect:rect];
  [gstThreadService release];

  s = gst_structure_new ("have-ns-view",
	   "nsview", G_TYPE_POINTER, osxwindow->gstview,
	   nil);

  msg = gst_message_new_element (GST_OBJECT (osxvideosink), s);
  gst_element_post_message (GST_ELEMENT (osxvideosink), msg);

  GST_LOG_OBJECT (osxvideosink, "'have-ns-view' message sent");

  [pool release];

  return osxwindow;
}

static void
gst_osx_video_sink_osxwindow_destroy (GstOSXVideoSink * osxvideosink)
{
  NSAutoreleasePool *pool;

  g_return_if_fail (GST_IS_OSX_VIDEO_SINK (osxvideosink));
  pool = [[NSAutoreleasePool alloc] init];

  if (osxvideosink->osxwindow) {
    // Ensure that the gstview is released on the main thread by using the
    // ObjC thread service helper class.
    GstThreadService *gstThreadService = [[GstThreadService alloc] init];
    [gstThreadService releaseGstGLView:osxvideosink->osxwindow->gstview];
    [gstThreadService release];

    g_free (osxvideosink->osxwindow);
    osxvideosink->osxwindow = NULL;
  }
  [pool release];
}

/* This function resizes a GstXWindow */
static void
gst_osx_video_sink_osxwindow_resize (GstOSXVideoSink * osxvideosink,
    GstOSXWindow * osxwindow, guint width, guint height)
{
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
  g_return_if_fail (osxwindow != NULL);
  g_return_if_fail (GST_IS_OSX_VIDEO_SINK (osxvideosink));

  osxwindow->width = width;
  osxwindow->height = height;

  GST_DEBUG_OBJECT (osxvideosink, "Resizing window to (%d,%d)", width, height);

  /* Directly resize the underlying view */
  GST_DEBUG_OBJECT (osxvideosink, "Calling setVideoSize on %p", osxwindow->gstview); 

  GstThreadService *gstThreadService = [[GstThreadService alloc] init];
  [gstThreadService setGstGLViewVideoSize:osxwindow->gstview
                                     size:NSMakeSize(width, height)];
  [gstThreadService release]; 
  
  [pool release];
}

static gboolean
gst_osx_video_sink_setcaps (GstBaseSink * bsink, GstCaps * caps)
{
  GstOSXVideoSink *osxvideosink;
  GstStructure *structure;
  gboolean res, result = FALSE;
  gint video_width, video_height;

  osxvideosink = GST_OSX_VIDEO_SINK (bsink);

  GST_DEBUG_OBJECT (osxvideosink, "caps: %" GST_PTR_FORMAT, caps);

  structure = gst_caps_get_structure (caps, 0);
  res = gst_structure_get_int (structure, "width", &video_width);
  res &= gst_structure_get_int (structure, "height", &video_height);

  if (!res) {
    goto beach;
  }

  GST_DEBUG_OBJECT (osxvideosink, "our format is: %dx%d video",
      video_width, video_height);

  GST_VIDEO_SINK_WIDTH (osxvideosink) = video_width;
  GST_VIDEO_SINK_HEIGHT (osxvideosink) = video_height;

  gst_osx_video_sink_osxwindow_resize (osxvideosink, osxvideosink->osxwindow,
      video_width, video_height);
  result = TRUE;

beach:
  return result;

}

static GstStateChangeReturn
gst_osx_video_sink_change_state (GstElement * element,
    GstStateChange transition)
{
  GstOSXVideoSink *osxvideosink;
  GstStateChangeReturn ret;

  osxvideosink = GST_OSX_VIDEO_SINK (element);

  GST_DEBUG_OBJECT (osxvideosink, "%s => %s", 
		    gst_element_state_get_name(GST_STATE_TRANSITION_CURRENT (transition)),
		    gst_element_state_get_name(GST_STATE_TRANSITION_NEXT (transition)));

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      break;
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      /* Creating our window and our image */
      GST_VIDEO_SINK_WIDTH (osxvideosink) = 320;
      GST_VIDEO_SINK_HEIGHT (osxvideosink) = 240;
      osxvideosink->osxwindow =
          gst_osx_video_sink_osxwindow_new (osxvideosink,
          GST_VIDEO_SINK_WIDTH (osxvideosink),
          GST_VIDEO_SINK_HEIGHT (osxvideosink));
      break;
    default:
      break;
  }

  ret = (GST_ELEMENT_CLASS (parent_class))->change_state (element, transition);

  switch (transition) {
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      GST_VIDEO_SINK_WIDTH (osxvideosink) = 0;
      GST_VIDEO_SINK_HEIGHT (osxvideosink) = 0;
      gst_osx_video_sink_osxwindow_destroy (osxvideosink);
      break;
    case GST_STATE_CHANGE_READY_TO_NULL:
      break;
    default:
      break;
  }

  return ret;
}

static GstFlowReturn
gst_osx_video_sink_show_frame (GstBaseSink * bsink, GstBuffer * buf)
{
  GstOSXVideoSink *osxvideosink;
  guint8 *viewdata;
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

  osxvideosink = GST_OSX_VIDEO_SINK (bsink);
  viewdata = (guint8 *) [osxvideosink->osxwindow->gstview getTextureBuffer];

  GST_DEBUG ("show_frame");
  memcpy (viewdata, GST_BUFFER_DATA (buf), GST_BUFFER_SIZE (buf));
  [osxvideosink->osxwindow->gstview displayTexture];

  [pool release];

  return GST_FLOW_OK;
}

/* Buffer management */



/* =========================================== */
/*                                             */
/*              Init & Class init              */
/*                                             */
/* =========================================== */

static void
gst_osx_video_sink_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstOSXVideoSink *osxvideosink;

  g_return_if_fail (GST_IS_OSX_VIDEO_SINK (object));

  osxvideosink = GST_OSX_VIDEO_SINK (object);

  switch (prop_id) {
    case ARG_EMBED:
      /* Ignore, just here for backwards compatibility */
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_osx_video_sink_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstOSXVideoSink *osxvideosink;

  g_return_if_fail (GST_IS_OSX_VIDEO_SINK (object));

  osxvideosink = GST_OSX_VIDEO_SINK (object);

  switch (prop_id) {
    case ARG_EMBED:
      g_value_set_boolean (value, TRUE);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_osx_video_sink_init (GstOSXVideoSink * osxvideosink)
{
  osxvideosink->osxwindow = NULL;
}

static void
gst_osx_video_sink_base_init (gpointer g_class)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (g_class);

  gst_element_class_set_details (element_class, &gst_osx_video_sink_details);

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&gst_osx_video_sink_sink_template_factory));
}

static void
gst_osx_video_sink_class_init (GstOSXVideoSinkClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstBaseSinkClass *gstbasesink_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;
  gstbasesink_class = (GstBaseSinkClass *) klass;


  parent_class = g_type_class_ref (GST_TYPE_VIDEO_SINK);

  gobject_class->set_property = gst_osx_video_sink_set_property;
  gobject_class->get_property = gst_osx_video_sink_get_property;

  gstbasesink_class->set_caps = gst_osx_video_sink_setcaps;
  gstbasesink_class->preroll = gst_osx_video_sink_show_frame;
  gstbasesink_class->render = gst_osx_video_sink_show_frame;
  gstelement_class->change_state = gst_osx_video_sink_change_state;

  /**
   * GstOSXVideoSink:embed
   *
   * Set to #TRUE if you are embedding the video window in an application.
   *
   **/

  g_object_class_install_property (gobject_class, ARG_EMBED,
      g_param_spec_boolean ("embed", "embed", "For ABI compatiblity only, do not use",
          FALSE, G_PARAM_READWRITE));
}

/* ============================================================= */
/*                                                               */
/*                       Public Methods                          */
/*                                                               */
/* ============================================================= */

/* =========================================== */
/*                                             */
/*          Object typing & Creation           */
/*                                             */
/* =========================================== */

GType
gst_osx_video_sink_get_type (void)
{
  static GType osxvideosink_type = 0;

  if (!osxvideosink_type) {
    static const GTypeInfo osxvideosink_info = {
      sizeof (GstOSXVideoSinkClass),
      gst_osx_video_sink_base_init,
      NULL,
      (GClassInitFunc) gst_osx_video_sink_class_init,
      NULL,
      NULL,
      sizeof (GstOSXVideoSink),
      0,
      (GInstanceInitFunc) gst_osx_video_sink_init,
    };

    osxvideosink_type = g_type_register_static (GST_TYPE_VIDEO_SINK,
        "GstOSXVideoSink", &osxvideosink_info, 0);

  }

  return osxvideosink_type;
}

static gboolean
plugin_init (GstPlugin * plugin)
{

  if (!gst_element_register (plugin, "osxvideosink",
          GST_RANK_PRIMARY, GST_TYPE_OSX_VIDEO_SINK))
    return FALSE;

  GST_DEBUG_CATEGORY_INIT (gst_debug_osx_video_sink, "osxvideosink", 0,
      "osxvideosink element");

  return TRUE;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "osxvideo",
    "OSX native video output plugin",
    plugin_init, VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
