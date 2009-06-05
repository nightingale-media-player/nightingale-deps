plugin_LTLIBRARIES = libgstdmoenc.la

libgstdmoenc_la_SOURCES = wmadmoenc.cpp
libgstdmoenc_la_CXXFLAGS = $(GST_CFLAGS) $(GST_BASE_CFLAGS) \
	$(GST_PLUGINS_BASE_CFLAGS) $(ACMENC_CFLAGS)
libgstdmoenc_la_LIBADD =     \
    $(GST_BASE_LIBS)             \
    $(GST_PLUGINS_BASE_LIBS)     \
    -lgstriff-$(GST_MAJORMINOR)  \
    -lgstaudio-$(GST_MAJORMINOR) \
    -lgsttag-$(GST_MAJORMINOR)   \
    -lmsdmo                      \
    -ldmoguids                   \
    -lwmcodecdspuuid             \
    -lstrmiids                   \
    -lole32

libgstdmoenc_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS) \
	-L$(WINDOWS_SDK_DIR)/Lib

libgstdmoenc_la_LIBTOOLFLAGS = --tag=disable-static

