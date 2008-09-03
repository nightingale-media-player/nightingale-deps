plugin_LTLIBRARIES = libgstacmenc.la

libgstacmenc_la_SOURCES = acmenc.c
libgstacmenc_la_CFLAGS = $(GST_CFLAGS) $(GST_BASE_CFLAGS) \
	$(GST_PLUGINS_BASE_CFLAGS)
libgstacmenc_la_LIBADD = $(GST_BASE_LIBS) $(GST_PLUGINS_BASE_LIBS) \
	-lgstriff-$(GST_MAJORMINOR) -lmsacm32
libgstacmenc_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS) $(DIRECTSOUND_LDFLAGS)

