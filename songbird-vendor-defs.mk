################################################################################
################################################################################
#
# BEGIN SONGBIRD GPL
# 
# This file is part of the Songbird web player.
#
# Copyright(c) 2007 POTI, Inc.
# http://www.songbirdnest.com
# 
# This file may be licensed under the terms of of the
# GNU General Public License Version 2 (the "GPL").
# 
# Software distributed under the License is distributed 
# on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either 
# express or implied. See the GPL for the specific language 
# governing rights and limitations.
#
# You should have received a copy of the GPL along with this 
# program. If not, go to http://www.gnu.org/licenses/gpl.html
# or write to the Free Software Foundation, Inc., 
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
# 
# END SONGBIRD GPL
#
################################################################################
#
# Makefile definitions for Songbird vendor repository
#
################################################################################
################################################################################

# TODO
# -- function to make sure paths can be prepended/appended to, and spaces
#  are removed, ":"'s are added, etc.

COMMA := ,
EMPTY :=
SPACE := $(EMPTY) $(EMPTY)

## vendor-autodefs.mk

# This is a weird function that will merely echos a command and then calls it; it's
# useful for bash shell loops that we don't want to echo the entire loop for, but 
# we still want make-like behavior (print the command, then run it)
define sh_make
echo $1; $1
endef

# Initialize variables...

SB_CONFIGURE_OPTS :=
SB_USE_SYSTEM_LIBS ?=
SB_PATH :=
SB_PKG_CONFIG_PATH :=
SB_DYLD_LIBRARY_PATH :=

SB_VENDOR_TARGET_DEP_MODULES ?= \
 flac jpeg ogg theora vorbis \
 gstreamer gst-plugins-base

#
# Operating system detection
#

SB_VENDOR_ARCH := $(shell uname 2>&1)
SB_VENDOR_SUBARCH := $(shell uname -m 2>&1)
SB_VENDOR_OS := $(shell uname -o 2>&1)

SB_ARCH_DETECTED := 0

ifeq (Darwin,$(SB_VENDOR_ARCH))
   ifeq (i386,$(SB_VENDOR_SUBARCH))
      SB_TARGET_ARCH := macosx-i686
      SB_ARCH_DETECTED := 1
   endif
   ifeq (x86_64,$(SB_VENDOR_SUBARCH))
      SB_TARGET_ARCH := macosx-i686
      SB_ARCH_DETECTED := 1
   endif
   ifeq (ppc,$(SB_VENDOR_SUBARCH))
      SB_TARGET_ARCH := macosx-ppc
      SB_ARCH_DETECTED := 1
   endif
endif

ifeq (SunOS,$(SB_VENDOR_ARCH))
   ifeq (i86pc,$(SB_VENDOR_SUBARCH))
      SB_TARGET_ARCH := solaris-i386
      SB_ARCH_DETECTED := 1
   endif
   ifeq (x86_64,$(SB_VENDOR_SUBARCH))
      SB_TARGET_ARCH := solaris-x86_64
      SB_ARCH_DETECTED := 1
   endif
   ifneq (,$(filter sun4u sun4v,$(SB_VENDOR_SUBARCH)))
      SB_TARGET_ARCH := solaris-sparc
      SB_ARCH_DETECTED := 1
   endif
endif

ifeq (Linux,$(SB_VENDOR_ARCH))
   ifeq (i686,$(SB_VENDOR_SUBARCH))
      SB_TARGET_ARCH := linux-i686
      SB_ARCH_DETECTED := 1
   endif
   ifeq (x86_64,$(SB_VENDOR_SUBARCH))
      SB_TARGET_ARCH := linux-x86_64
      SB_ARCH_DETECTED := 1
   endif
   ifeq (ppc,$(SB_VENDOR_SUBARCH))
      SB_TARGET_ARCH := linux-ppc
      SB_ARCH_DETECTED := 1
   endif
   ifeq (ppc64,$(SB_VENDOR_SUBARCH))
      SB_TARGET_ARCH := linux-ppc64
      SB_ARCH_DETECTED := 1
   endif
endif

ifeq (Msys,$(SB_VENDOR_OS))
   SB_TARGET_ARCH := windows-i686-msvc8
   SB_ARCH_DETECTED := 1
   # We redefine SB_VENDOR_ARCH here to make it more useful to us; in Msys,
   # uname returns some some long, difficult string to compare against...
   SB_VENDOR_ARCH := Msys
endif

ifneq (1,$(SB_ARCH_DETECTED))
   $(error Unsupported vendor build environment ($(SB_VENDOR_ARCH), $(SB_VENDOR_SUBARCH), $(SB_VENDOR_OS))) 
endif

#
# System tools we rely on for the vendor build process; allow them to be
# overridden
#

AWK ?= awk
AUTOCONF ?= autoconf
CHMOD ?= chmod
CMAKE ?= cmake
CONFIGURE ?= ./configure
CP ?= cp
CUT ?= cut
MSYS_CP ?= $(SB_VENDOR_CHECKOUT)/msys-recursive-cp
FIND ?= find
GREP ?= grep
LN ?= ln
MKDIR ?= mkdir -p
MV ?= mv
PYTHON ?= python
RM ?= rm
SED ?= sed
SVN ?= svn
SYMBOL_UPLOADER ?= $(MOZSDK_SCRIPTS_DIR)/upload_symbols.sh
TAR ?= tar
ZIP ?= zip

SUBMAKE_CMD = $(MAKE)

# For when new XRs land... 
# DUMP_SYMS_ARGS := --vcs-info -V
DUMP_SYMS_ARGS := --vcs-info

ifeq (Darwin,$(SB_VENDOR_ARCH))
   STRIP ?= strip -x -S
   DUMP_SYMS ?= $(MOZSDK_BIN_DIR)/dump_syms
   OTOOL ?= otool
   INSTALL_NAME_TOOL ?= install_name_tool

   SB_CFLAGS += -m32 -arch i386
   SB_CXXFLAGS += -m32 -arch i386
   SB_CPPFLAGS += -m32 -arch i386
   SB_LDFLAGS += -m32 -arch i386
   SB_OBJCFLAGS += -m32 -arch i386

   ifeq (i386,$(SB_VENDOR_SUBARCH))
      DUMP_SYMS_ARGS += -a i386
   else
   ifeq (ppc,$(SB_VENDOR_SUBARCH))
      DUMP_SYMS_ARGS += -a ppc
   endif
   endif
endif
ifeq (Linux,$(SB_VENDOR_ARCH))
   STRIP ?= strip -v
   DUMP_SYMS ?= $(MOZSDK_BIN_DIR)/dump_syms
   INSTALL_NAME_TOOL ?= echo install_name_tool called on Linux && exit 1;
   OTOOL ?= echo otool called on Linux && exit 1;
   SB_AR ?= ar
endif
ifeq (Msys,$(SB_VENDOR_ARCH))
   DUMP_SYMS ?= $(MOZSDK_BIN_DIR)/dump_syms.exe
   DUMP_SYMS_ARGS += --copy
   # Strip isn't needed/available on Win32; error out
   STRIP ?= echo strip called on Win32 && exit 1;
   INSTALL_NAME_TOOL ?= echo install_name_tool called on Win32 && exit 1;
   OTOOL ?= echo otool called on Win32 && exit 1;

   SB_CC = cl
   SB_CXX = cl
   SB_LD = link
   SB_OBJDUMP = objdump
endif

#
# On certain platforms, we need some extra information/flag munging
#

ifeq (Msys,$(SB_VENDOR_ARCH))
   _MSVC_VER_FILTER := s|.* \([0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\).*|\1|p
   CC_VERSION := $(shell cl -v 2>&1 | sed -ne "$(_MSVC_VER_FILTER)")
   _CC_MAJOR_VERSION := $(shell echo $(CC_VERSION) | awk -F\. '{ print $$1 }')
   _CC_MINOR_VERSION := $(shell echo $(CC_VERSION) | awk -F\. '{ print $$2 }')
   _MSC_VER := $(_CC_MAJOR_VERSION)$(_CC_MINOR_VERSION)

   SB_CFLAGS += -D_MSC_VER=$(_MSC_VER) -DWIN32 -D__NO_CTYPE \
     -D_CRT_SECURE_NO_WARNINGS -DHAVE_WIN32 -D_WINDOWS \
     -wd4820 -wd4668 -wd4100 -wd4706 -wd4127 -wd4255 -wd4710 -wd4055

   SB_CFLAGS += -Zi 
   SB_CXXFLAGS += -Zi
   SB_LDFLAGS += -DEBUG

   CONFIGURE_TARGET = i686-pc-mingw32

   # We need these for all builds on Win32, since the system doesn't provide
   # it...
   SB_VENDOR_TARGET_DEP_MODULES += iconv glib gettext
endif

ifeq (Darwin,$(SB_VENDOR_ARCH))
  SB_CFLAGS += -gstabs+ \
			   -I/opt/local/include \
               -isysroot $(MACOSX_SDK) \
               -D__MACOSX__ \
               -D__APPLE__

  SB_LDFLAGS += -headerpad_max_install_names -isysroot $(MACOSX_SDK) \
   -Wl,-syslibroot,$(MACOSX_SDK)

   # We need these for all builds on OSX, since the system doesn't provide them
   SB_VENDOR_TARGET_DEP_MODULES += iconv glib gettext
endif

ifeq (Linux,$(SB_VENDOR_ARCH))
   # We need this on linux, as the system may provide a too-old version
   SB_VENDOR_TARGET_DEP_MODULES += glib
endif

# Turn on libtool, but only for linux, and only when we're regenerating
# makefiles.
ifeq (linux-i686_regen-makefiles,$(SB_TARGET_ARCH)_$(MAKECMDGOALS))
   SB_VENDOR_TARGET_DEP_MODULES += libtool
endif

# Default to release mode...
ifeq (debug,$(MAKECMDGOALS))
   SB_BUILD_TYPE := debug
else
   SB_BUILD_TYPE := release
endif

# Set up the target configuration options.
ifneq (,$(CONFIGURE_TARGET))
   SB_CONFIGURE_OPTS += --target=$(CONFIGURE_TARGET)
   SB_CONFIGURE_OPTS += --build=$(CONFIGURE_TARGET)
endif

ifneq (,$(filter linux-i686 macosx-i686,$(SB_TARGET_ARCH)))
   SB_CFLAGS += -g -gstabs+
   SB_CXXFLAGS += -g -gstabs+
endif

ifeq (debug,$(SB_BUILD_TYPE))
   SB_CONFIGURE_OPTS += --enable-debug
   SB_BUILD_TYPE_DETECTED = 1
endif
ifeq (release,$(SB_BUILD_TYPE))
   SB_CONFIGURE_OPTS += --disable-debug
   SB_BUILD_TYPE_DETECTED = 1
endif
ifndef SB_BUILD_TYPE_DETECTED
   $(error Unknown SB_BUILD_TYPE: $(SB_BUILD_TYPE))
endif

#
# Figure out locations of things we need (the Songbird dependencies directory,
# the tools binary directory, etc.)
#

# Is typically Makefile.songbird
SB_VENDOR_MAKEFILE := $(firstword $(MAKEFILE_LIST))

ifeq (,$(SB_VENDOR_BUILD_ROOT))
   $(error Must define SB_VENDOR_BUILD_ROOT)
endif

SB_VENDOR_BINARIES_CO_ROOT ?= $(SB_VENDOR_BUILD_ROOT)/checkout
SB_VENDOR_CHECKOUT ?= $(realpath $(CURDIR)/..)

ifeq (,$(wildcard $(SB_VENDOR_BUILD_ROOT)))
   $(error SB_VENDOR_BUILD_ROOT ($(SB_VENDOR_BUILD_ROOT)) does not exist...)
endif

ifeq (,$(wildcard $(SB_VENDOR_BINARIES_CO_ROOT)))
   $(error SB_VENDOR_BINARIES_CO_ROOT $(SB_VENDOR_BINARIES_CO_ROOT) does not exist...)
endif

SB_VENDOR_DIR ?= $(realpath $(CURDIR)/..)
SB_TARGET_SRC_DIR := $(CURDIR)

ifeq (1, $(DIST_NAME_BINARIES_DIR))
  SB_VENDOR_BINARIES_DIR := $(SB_VENDOR_BUILD_ROOT)/dist/$(SB_TARGET_ARCH)
else
  SB_VENDOR_BINARIES_DIR := $(SB_VENDOR_BUILD_ROOT)/$(SB_TARGET_ARCH)
endif
SB_VENDOR_BINARIES_CHECKOUT := $(SB_VENDOR_BINARIES_CO_ROOT)/$(SB_TARGET_ARCH)

# SunOS doesn't have GNU's find, so it gets different (all shell-quoted)
# arguments
ifneq (,$(filter SunOS, $(SB_VENDOR_ARCH)))
   SB_VENDOR_BINARIES_TARGETS_FIND = $(FIND) $(SB_VENDOR_BINARIES_CHECKOUT) \( -type d -a \! -name . -prune \) -o -type d
else
   SB_VENDOR_BINARIES_TARGETS_FIND = $(FIND) $(SB_VENDOR_BINARIES_CHECKOUT) -maxdepth 1 -mindepth 1 -type d -not -name .svn
endif

ifneq (,$(filter Darwin SunOS,$(SB_VENDOR_ARCH)))
   SB_VENDOR_BINARIES_TARGETS_FIND += -exec basename {} \;
else
   SB_VENDOR_BINARIES_TARGETS_FIND += -printf '%f '
endif

SB_VENDOR_BINARIES_TARGETS := $(shell $(SB_VENDOR_BINARIES_TARGETS_FIND))

ifeq (,$(wildcard $(SB_VENDOR_BINARIES_DIR)))
   $(error SB_VENDOR_BINARIES_DIR $(SB_VENDOR_BINARIES_DIR) does not exist...)
endif

# Where we'll build this stuff
SB_VENDOR_BUILD_DIR = $(SB_VENDOR_BUILD_ROOT)/build/$(SB_VENDOR_TARGET)/$(SB_BUILD_TYPE)

# Where we'll point configure to install it to
SB_CONFIGURE_PREFIX = $(SB_VENDOR_BINARIES_DIR)/$(SB_VENDOR_TARGET)/$(SB_BUILD_TYPE)

SB_VENDOR_TARGET_BINARY_DEPS_DIR = $(SB_VENDOR_BUILD_ROOT)/build/$(SB_VENDOR_TARGET)/build-deps

SB_VENDOR_BREAKPAD_DIR = $(SB_VENDOR_BUILD_ROOT)/build/$(SB_VENDOR_TARGET)/breakpad/$(SB_BUILD_TYPE)

SB_VENDOR_BREAKPAD_SYMBOL_PATH = $(SB_VENDOR_BREAKPAD_DIR)/breakpad-symbols

ifeq (Msys,$(SB_VENDOR_ARCH))
   SB_VENDOR_BREAKPAD_STORE_PATH = $(SB_VENDOR_BREAKPAD_DIR)
else
   SB_VENDOR_BREAKPAD_STORE_PATH = $(SB_CONFIGURE_PREFIX)
endif

# Detritus autoconf dumps in the source dirs when we regenerate makefiles; 
# that we should remove when we're checking them in

AUTOCONF_GENERATED_TRASH = autoregen.sh \
                           autom4te.cache \
                           stamp-h.in \
                           m4/xsize.m4 \
                           m4/wint_t.m4 \
                           m4/wchar_t.m4 \
                           m4/ulonglong.m4 \
                           m4/size_max.m4 \
                           m4/signed.m4 \
                           m4/printf-posix.m4 \
                           m4/longlong.m4 \
                           m4/longdouble.m4 \
                           m4/isc-posix.m4 \
                           m4/inttypes.m4 \
                           m4/intmax.m4 \
                           common/m4/lt~obsolete.m4 \
                           common/m4/ltversion.m4 \
                           common/m4/ltsugar.m4 \
                           common/m4/ltoptions.m4 \
                           common/m4/ltdl.m4 \
                           common/m4/libtool.m4 \
                           common/m4/argz.m4 \
                           $(NULL)

# These are packages for which the regen-makefiles target is enabled; 
# other vendor packages don't need this step, so it's a no-op
SB_REGEN_MAKEFILE_PKGS = flac\
                         gst% \
                         libjpeg \
                         libogg \
                         libvorbis \
                         libtheora \
                         $(NULL)

# Turn on/off symbol generation; 'default' means "for release builds only,
# for all non-x86_64 platforms, generate a symbol bundle."
#
# Setting explicitly to 1 or 0 forces generating symbols or not.

SB_VENDOR_GENERATE_SYMBOLS ?= default

# Symbols don't work on certain platforms; don't even bother
SB_VENDOR_IGNORE_SYMBOLS_PLATFORMS = linux-x86_64 \
                                     solaris-i386 solaris-x86_64 solaris-sparc

ifeq (default_release_,$(SB_VENDOR_GENERATE_SYMBOLS)_$(SB_BUILD_TYPE)_$(filter $(SB_VENDOR_IGNORE_SYMBOLS_PLATFORMS), $(SB_TARGET_ARCH)))
   SB_VENDOR_GENERATE_SYMBOLS = 1
endif 

ifeq (1,$(SB_VENDOR_GENERATE_SYMBOLS))
   SB_VENDOR_BREAKPAD_ARCHIVE = $(SB_VENDOR_BREAKPAD_DIR)/sb-vendor-$(SB_VENDOR_TARGET)-$(SB_VENDOR_TARGET_VERSION)-$(SB_TARGET_ARCH)-$(SB_BUILD_TYPE)-symbols.zip
else
   SB_VENDOR_BREAKPAD_ARCHIVE = 
endif

# TODO: explain this gloop
define find-dep-dir
$(SB_VENDOR_BINARIES_DIR)/$(strip $1)/$(SB_BUILD_TYPE)
endef

# XULRunner Vendor Target dir
SB_VENDOR_XR_TARGET = xulrunner-1.9.2
SB_VENDOR_MOZSDK_TARGET = $(subst xulrunner,mozilla,$(SB_VENDOR_XR_TARGET))

# Mozilla SDK settings
MOZSDK_DIR = $(call find-dep-dir, $(SB_VENDOR_MOZSDK_TARGET))
MOZSDK_BIN_DIR = $(MOZSDK_DIR)/bin
MOZSDK_SCRIPTS_DIR = $(MOZSDK_DIR)/scripts
MOZ_XR_DIR = $(call find-dep-dir, $(SB_VENDOR_XR_TARGET))

# Supporting SDK paths on Win32
ifeq (Msys,$(SB_VENDOR_ARCH))
   DIRECTX_SDK_ROOT ?= /d/ms-sdks/directx-aug08
   WINDOWS_SDK_ROOT ?= /d/ms-sdks/Windows/v6.0
   QUICKTIME_SDK_ROOT ?= /d/ms-sdks/QuickTimeSDK
endif

ifeq (Darwin,$(SB_VENDOR_ARCH))
   MACOSX_SDK ?= /Developer/SDKs/MacOSX10.5.sdk
endif

#
# Dependent library section; this is where we define and point the build system
# at our copies of various tools that are checked in.
#

define enable-sb-lib
$(filter $1, $(filter-out $(SB_VENDOR_TARGET), $(SB_VENDOR_TARGET_DEP_MODULES)))
endef

#
# GNU libtool
#
ifneq (,$(call enable-sb-lib, libtool))
  $(info Enabling Songbird vendor lib: libtool)
  SB_LIBTOOL_DIR = $(call find-dep-dir, libtool)
  SB_PATH += $(SB_LIBTOOL_DIR)/bin
  ACLOCAL_FLAGS += -I $(SB_LIBTOOL_DIR)/share/aclocal
endif

#
# GNU Gettext 
#
ifneq (,$(call enable-sb-lib, gettext))
  ifneq ($(wildcard $(SB_VENDOR_BINARIES_DIR)/gettext/$(SB_BUILD_TYPE)),)
    $(info Enabling Songbird vendor lib: gettext)
    SB_GETTEXT_DIR = $(call find-dep-dir, gettext)
    SB_LDFLAGS += -L$(SB_GETTEXT_DIR)/lib -lintl
    SB_CFLAGS += -I$(SB_GETTEXT_DIR)/include
    SB_CPPFLAGS += -I$(SB_GETTEXT_DIR)/include
    SB_PATH += $(SB_GETTEXT_DIR)/bin

    ifeq (Darwin,$(SB_VENDOR_ARCH))
      SB_DYLD_LIBRARY_PATH += $(SB_GETTEXT_DIR)/lib
    endif
  endif
endif

#
# GNU iconv
#
ifneq (,$(call enable-sb-lib, iconv))
  ifneq ($(wildcard $(SB_VENDOR_BINARIES_DIR)/libiconv/$(SB_BUILD_TYPE)),)
    $(info Enabling Songbird vendor lib: iconv)
    SB_ICONV_DIR := $(call find-dep-dir, libiconv)
    SB_LDFLAGS += -L$(SB_ICONV_DIR)/lib -liconv
    SB_CFLAGS += -I$(SB_ICONV_DIR)/include
    SB_PATH += $(SB_ICONV_DIR)/bin

    ifeq (Darwin,$(SB_VENDOR_ARCH))
      LDFLAGS += -Wl,-dylib_file -Wl,libiconv.dylib:$(SB_ICONV_DIR)/lib/libiconv.dylib
    endif
  endif
endif

#
# Zlib
#
ifeq (Msys,$(SB_VENDOR_ARCH))
  ifneq ($(wildcard $(SB_VENDOR_BINARIES_DIR)/zlib/$(SB_BUILD_TYPE)), )
    SB_ZLIB_DIR := $(call find-dep-dir, zlib)
    SB_ZLIB_LDFLAGS := -L$(SB_ZLIB_DIR)/lib -lzlib
    SB_ZLIB_CFLAGS := -I$(SB_ZLIB_DIR)/include

    ifneq (,$(call enable-sb-lib, zlib))
      $(info Enabling Songbird vendor lib: zlib)
      SB_LDFLAGS += $(SB_ZLIB_LDFLAGS)
      SB_CFLAGS += $(SB_ZLIB_CFLAGS)
    endif

  endif
endif

endif

#
# Glib
# 
ifneq (,$(call enable-sb-lib, glib))
  ifneq ($(wildcard $(SB_VENDOR_BINARIES_DIR)/glib/$(SB_BUILD_TYPE)),)
    $(info Enabling Songbird vendor lib: glib)
    SB_GLIB_DIR := $(call find-dep-dir, glib)
    SB_PATH += $(SB_GLIB_DIR)/bin
    SB_PKG_CONFIG_PATH += $(SB_GLIB_DIR)/lib/pkgconfig

    GLIB_PARTS := glib gobject gmodule gthread

    ifeq (Darwin,$(SB_VENDOR_ARCH))
      LDFLAGS += $(foreach GLIB_PART, $(GLIB_PARTS), -Wl,-dylib_file -Wl,libgobject-2.0.dylib:$(SB_GLIB_DIR)/lib/lib$(GLIB_PART)-2.0.dylib)
      SB_DYLD_LIBRARY_PATH += $(SB_GLIB_DIR)/lib
    endif

  endif
endif

#
# libIDL
#
ifeq (Darwin,$(SB_VENDOR_ARCH))
  ifneq ($(wildcard $(SB_VENDOR_BINARIES_DIR)/libIDL/$(SB_BUILD_TYPE)),)
    ifneq (,$(call enable-sb-lib, libIDL))
      $(info Enabling Songbird vendor lib: libIDL)
      SB_LIBIDL_DIR := $(call find-dep-dir, libIDL)
      SB_PATH += $(SB_LIBIDL_DIR)/bin
      SB_PKG_CONFIG_PATH += $(SB_LIBIDL_DIR)/lib/pkgconfig
      SB_DYLD_LIBRARY_PATH += $(SB_LIBIDL_DIR)/lib
    endif
  endif
endif

#
# gstreamer
#
ifneq (,$(call enable-sb-lib, gstreamer))
  ifneq ($(wildcard $(SB_VENDOR_BINARIES_DIR)/gstreamer/$(SB_BUILD_TYPE)),)
    $(info Enabling Songbird vendor lib: gstreamer)
    SB_GSTREAMER_DIR = $(call find-dep-dir, gstreamer)
    SB_PATH += $(SB_GSTREAMER_DIR)/bin
    SB_PKG_CONFIG_PATH += $(SB_GSTREAMER_DIR)/lib/pkgconfig
  
    # A list of basic dylibs on mac that need to be fixed up across all the
    # gstreamer modules; these are built by various parts of gstreamer and 
    # base
    GST_BASE_DYLIBS = audio cdda fft interfaces netbuffer pbutils riff \
     rtp rtsp sdp tag video
	endif
endif

#
# gstreamer-plugins-base
#
ifneq (,$(call enable-sb-lib, gst-plugins-base))
  ifneq ($(wildcard $(SB_VENDOR_BINARIES_DIR)/gst-plugins-base/$(SB_BUILD_TYPE)),)
    $(info Enabling Songbird vendor lib: gst-plugins-base)
    SB_GST_PLUGINS_BASE_DIR = $(call find-dep-dir, gst-plugins-base)
    SB_PATH += $(SB_GST_PLUGINS_BASE_DIR)/bin
    SB_PKG_CONFIG_PATH += $(SB_GST_PLUGINS_BASE_DIR)/lib/pkgconfig
  endif
endif

#
# libogg
#
ifneq (,$(call enable-sb-lib, ogg))
  ifneq ($(wildcard $(SB_VENDOR_BINARIES_DIR)/libogg/$(SB_BUILD_TYPE)),)
    $(info Enabling Songbird vendor lib: ogg)
    SB_LIBOGG_DIR = $(call find-dep-dir, libogg)
    SB_OGG_LIBS = -L$(SB_LIBOGG_DIR)/lib -logg
    SB_OGG_CFLAGS := -I$(SB_LIBOGG_DIR)/include
    SB_PKG_CONFIG_PATH += $(SB_LIBOGG_DIR)/lib/pkgconfig

    ifeq (Msys, $(SB_VENDOR_ARCH))
      SB_PATH += $(SB_LIBOGG_DIR)/bin
      ifeq (debug, $(SB_BUILD_TYPE))
        SB_LIBOGG_LIBS += -Wl,-Zi
      endif
    endif

  endif
endif

#
# libtheora
#
ifneq (,$(call enable-sb-lib, theora))
  ifneq ($(wildcard $(SB_VENDOR_BINARIES_DIR)/libtheora/$(SB_BUILD_TYPE)),)
    $(info Enabling Songbird vendor lib: theora)
    SB_LIBTHEORA_DIR = $(call find-dep-dir, libtheora)
    SB_THEORA_LIBS := -L$(SB_LIBTHEORA_DIR)/lib -ltheora
    SB_THEORA_LIBS += $(SB_OGG_LIBS)
    SB_THEORA_CFLAGS = -I$(SB_LIBTHEORA_DIR)/include
    SB_THEORA_CFLAGS += $(SB_OGG_CFLAGS)
    SB_PKG_CONFIG_PATH += $(SB_LIBTHEORA_DIR)/lib/pkgconfig

    ifeq (Msys,$(SB_VENDOR_ARCH))
      SB_PATH += $(SB_LIBTHEORA_DIR)/bin
      ifeq (debug,$(SB_BUILD_TYPE))
        SB_THEORA_LIBS += -Wl,-Zi
      endif
    endif

  endif
endif

#
# libvorbis
#
ifneq (,$(call enable-sb-lib, vorbis))
  ifneq ($(wildcard $(SB_VENDOR_BINARIES_DIR)/libvorbis/$(SB_BUILD_TYPE)),)
    $(info Enabling Songbird vendor lib: vorbis)
    SB_LIBVORBIS_DIR = $(call find-dep-dir, libvorbis)
    SB_VORBIS_LIBS := -L$(SB_LIBVORBIS_DIR)/lib -lvorbis -lvorbisenc
    SB_VORBIS_LIBS += $(SB_OGG_LIBS)
    SB_VORBIS_CFLAGS = -I$(SB_LIBVORBIS_DIR)/include
    SB_VORBIS_CFLAGS += $(SB_OGG_CFLAGS)
    SB_PKG_CONFIG_PATH += $(SB_LIBVORBIS_DIR)/lib/pkgconfig

    ifeq (Msys, $(SB_VENDOR_ARCH))
      SB_PATH += $(SB_LIBVORBIS_DIR)/bin
      ifeq (debug, $(SB_BUILD_TYPE))
        SB_VORBIS_LIBS += -Wl,-Zi
      endif
    endif

  endif
endif

#
# libFLAC
#
ifneq (,$(call enable-sb-lib, flac))
  ifneq ($(wildcard $(SB_VENDOR_BINARIES_DIR)/flac/$(SB_BUILD_TYPE)),)
    $(info Enabling Songbird vendor lib: flac)
    SB_LIBFLAC_DIR = $(call find-dep-dir, flac)
    SB_LDFLAGS += -L$(SB_LIBFLAC_DIR)/lib

    SB_CPPFLAGS += -I$(SB_LIBFLAC_DIR)/include
    SB_PKG_CONFIG_PATH += $(SB_LIBFLAC_DIR)/lib/pkgconfig

    ifeq (Msys,$(SB_VENDOR_ARCH))
      SB_FLAC_LIBS += -lFLAC-8
      SB_PATH += $(SB_LIBFLAC_DIR)/bin
      ifeq (debug,$(SB_BUILD_TYPE))
        SB_FLAC_LIBS += -Wl,-Zi
      endif
    endif

  endif
endif

#
# libjpeg
#
ifneq (,$(call enable-sb-lib, jpeg))
  ifneq ($(wildcard $(SB_VENDOR_BINARIES_DIR)/libjpeg/$(SB_BUILD_TYPE)),)
    $(info Enabling Songbird vendor lib: jpeg)
    # SB_LIBJPEG_DIR = $(call find-dep-dir, libjpeg)
    SB_LIBJPEG_DIR = $(call find-dep-dir, libjpeg-turbo)
    SB_LDFLAGS += -L$(SB_LIBJPEG_DIR)/lib

    SB_CFLAGS += -I$(SB_LIBJPEG_DIR)/include

    ifeq (Msys,$(SB_VENDOR_ARCH))
      SB_JPEG_LIBS += "-ljpeg-7"
      SB_PATH += $(SB_LIBJPEG_DIR)/bin
      ifeq (debug,$(SB_BUILD_TYPE))
        SB_JPEG_LIBS += -Wl,-Zi
      endif
    endif

  endif
endif

