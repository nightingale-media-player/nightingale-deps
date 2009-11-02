plugin_LTLIBRARIES = libgstdshowvideosink.la

libgstdshowvideosink_la_SOURCES = dshowvideofakesrc.cpp dshowvideosink.cpp

libgstdshowvideosink_la_CFLAGS = $(GST_CFLAGS) -I$(top_srcdir)/gst-libs/gst
libgstdshowvideosink_la_CXXFLAGS = $(GST_CFLAGS) $(GST_PLUGINS_BASE_CFLAGS) \
   -I$(top_srcdir)/gst-libs/gst \
   -I$(WINDOWS_SDK_DIR)/Samples/Multimedia/DirectShow/BaseClasses \
   -DUNICODE -D_UNICODE
libgstdshowvideosink_la_LIBADD = $(GST_LIBS) \
    $(GST_PLUGINS_BASE_LIBS) \
    -lgstinterfaces-$(GST_MAJORMINOR) \
    -lgstbase-$(GST_MAJORMINOR) \
   -lstrmbase -lwinmm -lstrmiids \
   -lole32 -luser32 -ladvapi32 -lgdi32 -loleaut32
libgstdshowvideosink_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS) \
   -L$(DIRECTSHOW_LIB_DIR) \
   -L$(WINDOWS_SDK_DIR)/Lib

noinst_HEADERS = dshowvideofakesrc.h dshowvideosink.h

