plugin_LTLIBRARIES = libgstasfmux.la

libgstasfmux_la_SOURCES = asfguids.c asfmux.c asfmedia.c asfheaders.c \
                          gstasfmux.c
libgstasfmux_la_CFLAGS = \
	$(GST_BASE_CFLAGS) \
	$(GST_PLUGINS_BASE_CFLAGS) \
	$(GST_CFLAGS)

libgstasfmux_la_LIBADD = \
	$(GST_PLUGINS_BASE_LIBS) \
    -lgstpbutils-0.10 \
	$(GST_BASE_LIBS)

libgstasfmux_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstasfmux_la_LIBTOOLFLAGS = --tag=disable-static

noinst_HEADERS = asfmux.h asfguids.h gstasfmux.h asfmedia.h asfheaders.h
