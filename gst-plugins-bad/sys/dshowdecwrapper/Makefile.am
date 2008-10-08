plugin_LTLIBRARIES = libgstdshowdecwrapper.la

libgstdshowdecwrapper_la_SOURCES = gstdshowaudiodec.cpp gstdshowfakesrc.cpp \
	gstdshowvideodec.cpp gstdshowdecwrapper.cpp gstdshowutil.cpp

libgstdshowdecwrapper_la_CFLAGS = $(GST_CFLAGS) -I$(top_srcdir)/gst-libs/gst
libgstdshowdecwrapper_la_CXXFLAGS = $(GST_CFLAGS) -I$(top_srcdir)/gst-libs/gst \
	-I$(WINDOWS_SDK_DIR)/Samples/Multimedia/DirectShow/BaseClasses \
	-DUNICODE -D_UNICODE
libgstdshowdecwrapper_la_LIBADD = $(GST_LIBS) -lstrmbase -lwinmm \
								  -ldmoguids -lwmcodecdspuuid
libgstdshowdecwrapper_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS) \
	-L$(DIRECTSHOW_LIB_DIR) \
	-L$(WINDOWS_SDK_DIR)/Lib

noinst_HEADERS = gstdshowaudiodec.h gstdshowfakesrc.h \
				 gstdshowutil.h gstdshowvideodec.h
