cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)

project(intl)

set(HAVE_NEWLOCALE 0)
set(HAVE_POSIX_PRINTF 0)
set(HAVE_SNPRINTF 0)
set(HAVE_ASPRINTF 0)
set(HAVE_WPRINTF 0)

configure_file(libgnuintl.h.in libgnuintl.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

set(INTL_SRCS
  bindtextdom.c
  dcgettext.c
  dgettext.c
  gettext.c
  finddomain.c
  hash-string.c
  loadmsgcat.c
  localealias.c
  textdomain.c
  l10nflist.c
  explodename.c
  dcigettext.c
  dcngettext.c
  dngettext.c
  ngettext.c
  plural-exp.c
  localcharset.c
  threadlib.c
  lock.c
  relocatable.c
  langprefs.c
  localename.c
  log.c
  printf.c
  setlocale.c
  version.c
  osdep.c
  intl-compat.c
  libintl.rc
  plural.c
  intl.def
)

add_definitions(-DLOCALE_ALIAS_PATH="")
add_definitions(-DLOCALEDIR="")
add_definitions(-DIN_LIBINTL)
#add_definitions(-DBUILDING_DLL)
#add_definitions(-DHAVE_VISIBILITY)

add_library(intl ${LIBRARY_TYPE} ${INTL_SRCS})
target_link_libraries(intl ${ICONV_LIBRARIES})
if(MSVC)
set_target_properties(intl PROPERTIES OUTPUT_NAME "intl")
endif(MSVC)

if(WINCE)
    target_link_libraries(intl ${WCECOMPAT_LIBRARIES})
endif(WINCE)

install(TARGETS intl RUNTIME DESTINATION bin
                        LIBRARY DESTINATION lib
                        ARCHIVE DESTINATION lib)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libgnuintl.h DESTINATION include RENAME libintl.h)
