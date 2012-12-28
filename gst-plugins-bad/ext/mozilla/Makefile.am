plugin_LTLIBRARIES = libgstmozilla.la

libgstmozilla_la_SOURCES = mozillaplugin.cpp mozillasrc.cpp mozillasink.cpp
libgstmozilla_la_CXXFLAGS = $(GST_PLUGINS_BASE_CFLAGS) $(GST_CFLAGS) \
							   $(MOZILLA_CFLAGS) -fshort-wchar

#-I/usr/include/xulrunner-1.9.0.1/unstable -I/usr/include/nspr 
libgstmozilla_la_LIBADD = \
	$(GST_LIBS) \
	$(GST_BASE_LIBS) \
	$(GST_PLUGINS_BASE_LIBS) \
	-lgsttag-$(GST_MAJORMINOR) \
	$(GST_LIBS) \
	$(MOZILLA_LIBS)

#	-L/usr/lib/xulrunner-devel-1.9.0.1/lib -lxpcomglue_s -lxul -lxpcom -lplds4 -lplc4 -lnspr4 -lpthread -ldl

libgstmozilla_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
