lib_LTLIBRARIES = libgpod.la

libgpod_la_SOURCES = \
	itdb.h           \
	itdb_artwork.c   \
	itdb_itunesdb.c  \
	itdb_playlist.c  \
	itdb_private.h   \
	itdb_photoalbum.c \
	itdb_track.c     \
	db-artwork-parser.c \
	db-artwork-parser.h \
	db-parse-context.c  \
	db-parse-context.h  \
	db-artwork-debug.c  \
	db-artwork-debug.h  \
	db-image-parser.c   \
	db-image-parser.h   \
	db-itunes-parser.h  \
	db-artwork-writer.c \
	ithumb-writer.c \
	itdb_device.c \
	itdb_device.h \
	itdb_endianness.h \
	glib-compat.h


libgpod_la_headers = itdb.h
libgpod_la_noinst_headers = itdb_private.h hal-common.h
libgpod_la_LDFLAGS = -version-info $(LIBGPOD_CURRENT):$(LIBGPOD_REVISION):$(LIBGPOD_AGE) \
        -no-undefined

libgpodincludedir = $(includedir)/gpod-1.0/gpod
libgpodinclude_HEADERS = $(libgpod_la_headers)

INCLUDES=$(LIBGPOD_CFLAGS)
LIBS=$(LIBGPOD_LIBS)

# Special rules for building with the MSVC linker.
if USE_MSVC_LINK

# Set up link command for MSVC link.
MSVC_libgpod_la_LINK = link /dll /machine:I386 /out:$@

# Convert LIBS variable to MSVC format.
MSVC_LIBS1 = $(LIBS)
MSVC_LIBS2 = $(patsubst -l%,%.lib,$(MSVC_LIBS1))
MSVC_LIBS = $(patsubst -L%,/libpath:%,$(MSVC_LIBS2))

# Convert libgpod_la_OBJECTS to MSVC format.
MSVC_libgpod_la_OBJECTS1 = $(libgpod_la_OBJECTS)
MSVC_libgpod_la_OBJECTS =                                                      \
                        $(patsubst %.lo,.libs/%.o,$(MSVC_libgpod_la_OBJECTS1))

# Add MSVC stub to objects.
MSVC_libgpod_la_OBJECTS += .libs/msvc_stub.msvc_o

# Build rule for MSVC stub.
.libs/msvc_stub.msvc_o: msvc_stub.c
	cl /MT /TP /RTC1 /Gm /ZI -c /Fo$@ /Tp$<

# Build MSVC dll in place of libtool la.
libgpod.la: .libs/libgpod.dll
.libs/libgpod.dll: $(libgpod_la_OBJECTS)                                       \
                   .libs/msvc_stub.msvc_o                                      \
                   $(libgpod_la_DEPENDENCIES) 
	dlltool --export-all-symbols -z .libs/libgpod.def                      \
                $(MSVC_libgpod_la_OBJECTS)
	$(MSVC_libgpod_la_LINK) /DEF:.libs/libgpod.def                         \
                           $(MSVC_libgpod_la_OBJECTS)                          \
                           $(MSVC_LIBS)

endif
