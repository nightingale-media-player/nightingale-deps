plugin_LTLIBRARIES = libgstqtaudiowrapper.la libgstqtvideowrapper.la

# sources used to compile this plug-in
libgstqtaudiowrapper_la_SOURCES = \
	qtaudiowrapper.c		\
	qtutils.c		\
	codecmapping.c		\
	audiodecoders.c

# flags used to compile this plugin
# add other _CFLAGS and _LIBS as needed
libgstqtaudiowrapper_la_CFLAGS = $(GST_CFLAGS) $(GST_BASE_CFLAGS) $(QUICKTIME_CFLAGS)
libgstqtaudiowrapper_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS)
libgstqtaudiowrapper_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS) $(QUICKTIME_LDFLAGS)

libgstqtvideowrapper_la_SOURCES = \
	qtvideowrapper.c		\
	qtutils.c		\
	codecmapping.c		\
	videodecoders.c		\
	imagedescription.c

# flags used to compile this plugin
# add other _CFLAGS and _LIBS as needed
libgstqtvideowrapper_la_CFLAGS = $(GST_CFLAGS) $(GST_BASE_CFLAGS) $(QUICKTIME_CFLAGS)
libgstqtvideowrapper_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS)
libgstqtvideowrapper_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS) $(QUICKTIME_LDFLAGS)

# headers we need but don't want installed
noinst_HEADERS = 		\
	codecmapping.h		\
	qtaudiowrapper.h		\
	qtvideowrapper.h		\
	qtutils.h		\
	imagedescription.h
