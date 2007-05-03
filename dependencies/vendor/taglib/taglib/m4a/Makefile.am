INCLUDES = \
	-I$(top_srcdir)/taglib \
	-I$(top_srcdir)/taglib/mpeg/id3v1 \
	-I$(top_srcdir)/taglib/toolkit \
	$(all_includes)

noinst_LTLIBRARIES = libitunes.la

libitunes_la_SOURCES = mp4file.cpp \
                       mp4itunestag.cpp \
                       mp4isobox.cpp \
                       mp4isofullbox.cpp \
                       mp4skipbox.cpp \
                       mp4moovbox.cpp \
                       mp4mvhdbox.cpp \
                       mp4trakbox.cpp \
                       mp4mdiabox.cpp \
                       mp4minfbox.cpp \
                       mp4stblbox.cpp \
                       mp4stsdbox.cpp \
                       mp4sampleentry.cpp \
                       mp4audiosampleentry.cpp \
                       mp4hdlrbox.cpp \
                       mp4ilstbox.cpp \
                       boxfactory.cpp \
                       mp4fourcc.cpp \
                       mp4udtabox.cpp \
                       mp4metabox.cpp \
                       mp4tagsproxy.cpp \
                       mp4propsproxy.cpp \
                       mp4audioproperties.cpp \
                       itunesnambox.cpp \
                       itunesartbox.cpp \
                       itunesalbbox.cpp \
                       itunesgenbox.cpp \
                       itunestrknbox.cpp \
                       itunesdaybox.cpp \
                       itunescmtbox.cpp \
                       itunesgrpbox.cpp \
                       ituneswrtbox.cpp \
                       itunesdiskbox.cpp \
                       itunestmpobox.cpp \
                       itunescvrbox.cpp \
                       itunesdatabox.cpp

taglib_include_HEADERS = mp4file.h \
                         mp4itunestag.h \
                         mp4isobox.h \
                         mp4isofullbox.h \
                         mp4skipbox.h \
                         mp4moovbox.h \
                         mp4mvhdbox.h \
                         mp4trakbox.h \
                         mp4mdiabox.h \
                         mp4minfbox.h \
                         mp4stblbox.h \
                         mp4stsdbox.h \
                         mp4sampleentry.h \
                         mp4audiosampleentry.h \
                         mp4hdlrbox.h \
                         mp4ilstbox.h \
                         boxfactory.h \
                         mp4fourcc.h \
                         mp4udtabox.h \
                         mp4metabox.h \
                         mp4tagsproxy.h \
                         mp4propsproxy.h \
                         mp4audioproperties.h \
                         itunesnambox.h \
                         itunesartbox.h \
                         itunesalbbox.h \
                         itunesgenbox.h \
                         itunestrknbox.h \
                         itunesdaybox.h \
                         itunescmtbox.h \
                         itunesgrpbox.h \
                         ituneswrtbox.h \
                         itunesdiskbox.h \
                         itunestmpobox.h \
                         itunescvrbox.h \
                         itunesdatabox.h

taglib_includedir = $(includedir)/taglib

EXTRA_DIST = $(libitunes_la_SOURCES) $(taglib_include_HEADERS)
