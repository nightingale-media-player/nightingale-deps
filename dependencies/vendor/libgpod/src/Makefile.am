lib_LTLIBRARIES = libgpod.la

libgpod_la_SOURCES = 		\
	db-artwork-debug.c  	\
	db-artwork-parser.c 	\
	db-artwork-writer.c 	\
	db-image-parser.c   	\
	db-parse-context.c  	\
	itdb_artwork.c   	\
	itdb_device.c  		\
	itdb_itunesdb.c 	\
	itdb_photoalbum.c 	\
	itdb_playlist.c  	\
	itdb_sha1.c 		\
	itdb_sysinfo.c 		\
	itdb_track.c     	\
	ithumb-writer.c 	\
	pixmaps.c 		\
	sha1.c

libgpod_la_LDFLAGS = -version-info $(LIBGPOD_SO_VERSION) -no-undefined

libgpodincludedir = $(includedir)/gpod-1.0/gpod
libgpodinclude_HEADERS = itdb.h
noinst_HEADERS = 		\
	db-artwork-debug.h	\
	db-artwork-parser.h 	\
	db-image-parser.h   	\
	db-itunes-parser.h  	\
	db-parse-context.h  	\
	itdb_device.h  		\
	itdb_endianness.h 	\
	itdb_private.h   	\
	itdb_sha1.h 		\
	pixmaps.h		\
	sha1.h

INCLUDES=$(LIBGPOD_CFLAGS)
LIBS=$(LIBGPOD_LIBS)

# Special rules for building with the MSVC linker.
if USE_MSVC_LINK

# Set up link command for MSVC link.
MSVC_libgpod_la_LINK = link -dll -machine:I386 -out:$@

# Convert LIBS variable to MSVC format.
MSVC_LIBS1 = $(LIBS)
# convert "-lfoo" to "foo.lib"
MSVC_LIBS2 = $(patsubst -l%,%.lib,$(MSVC_LIBS1))
# convert "-L/path/" to "-libpath:/path/"
MSVC_LIBS3 = $(patsubst -L%,-libpath:%,$(MSVC_LIBS2))
# convert "-libpath:/X/path" to "-libpath:X:/path"
MSVC_LIBS = $(shell echo $(MSVC_LIBS3) | sed -e 's,-libpath:/\(.\)/,-libpath:\1:/,g')

# Convert libgpod_la_OBJECTS to MSVC format.
MSVC_libgpod_la_OBJECTS = $(patsubst %.lo,.libs/%.obj,$(libgpod_la_OBJECTS))

# Build MSVC dll in place of libtool la.
libgpod.la: .libs/libgpod.dll
.libs/libgpod.dll: $(libgpod_la_OBJECTS) $(libgpod_la_DEPENDENCIES) 
	$(MSVC_libgpod_la_LINK) -debug -def:libgpod.def -nologo $(MSVC_libgpod_la_OBJECTS) \
		$(MSVC_LIBS)

endif
