plugin_LTLIBRARIES = libgstdshowsrcwrapper.la

libgstdshowsrcwrapper_la_SOURCES = gstdshowaudiosrc.c gstdshowsrcwrapper.c gstdshowvideosrc.c

libgstdshowsrcwrapper_la_CFLAGS = $(GST_CFLAGS) -I$(top_srcdir)/gst-libs/gst
libgstdshowsrcwrapper_la_LIBADD = $(GST_LIBS) $(top_builddir)/gst-libs/gst/dshow/libgstdshow-@GST_MAJORMINOR@.la
libgstdshowsrcwrapper_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstdshowsrcwrapper_la_LIBTOOLFLAGS = --tag=disable-static

noinst_HEADERS = gstdshowaudiosrc.h gstdshowsrcwrapper.h gstdshowvideosrc.h
