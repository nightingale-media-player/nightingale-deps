plugin_LTLIBRARIES = libgstid3demux.la

libgstid3demux_la_SOURCES = gstid3demux.c
libgstid3demux_la_CFLAGS = $(GST_PLUGINS_BASE_CFLAGS) $(GST_BASE_CFLAGS) $(GST_CFLAGS) 
libgstid3demux_la_LIBADD = $(GST_PLUGINS_BASE_LIBS) -lgsttag-@GST_MAJORMINOR@ \
	-lgstpbutils-@GST_MAJORMINOR@ $(GST_BASE_LIBS)
libgstid3demux_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstid3demux_la_LIBTOOLFLAGS = --tag=disable-static

noinst_HEADERS = gstid3demux.h
