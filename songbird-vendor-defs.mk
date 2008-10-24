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

# Initialize variables...

SB_CONFIGURE_OPTS :=
SB_USE_SYSTEM_LIBS ?=
SB_PATH :=
SB_PKG_CONFIG_PATH :=
SB_DYLD_LIBRARY_PATH :=

SB_VENDOR_TARGET_DEP_MODULES ?= \
  liboil flac ogg vorbis \
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
   else
   ifeq (ppc,$(SB_VENDOR_SUBARCH))
      SB_TARGET_ARCH := macosx-ppc
      SB_ARCH_DETECTED := 1
   endif
   endif
endif

ifeq (Linux,$(SB_VENDOR_ARCH))
   ifeq (i686,$(SB_VENDOR_SUBARCH))
      SB_TARGET_ARCH := linux-i686
      SB_ARCH_DETECTED := 1
   else
   ifeq (x86_64,$(SB_VENDOR_SUBARCH))
      SB_TARGET_ARCH := linux-x86_64
      SB_ARCH_DETECTED := 1
   endif
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
CMAKE ?= cmake
CONFIGURE ?= ./configure
CP ?= cp
ifeq (Msys,$(SB_VENDOR_ARCH))
  CP_RECURSE_FLAGS ?= -dpr
else
  CP_RECURSE_FLAGS ?= -R
endif
FIND ?= find
LN ?= ln
MKDIR ?= mkdir -p
PYTHON ?= python
SED ?= sed
TAR ?= tar
ZIP ?= zip

SUBMAKE_CMD = $(MAKE)

DUMP_SYMS_ARGS := --vcs-info

ifeq (Darwin,$(SB_VENDOR_ARCH))
   SB_STRIP ?= strip -x -S
   DUMP_SYMS ?= $(MOZSDK_DIR)/dump_syms
   OTOOL ?= otool
   INSTALL_NAME_TOOL ?= install_name_tool

   ifeq (i386,$(SB_VENDOR_SUBARCH))
      DUMP_SYMS_ARGS += -a i386
   else
   ifeq (ppc,$(SB_VENDOR_SUBARCH))
      DUMP_SYMS_ARGS += -a ppc
   endif
   endif
else
ifeq (Linux,$(SB_VENDOR_ARCH))
   SB_STRIP ?= strip -v
   DUMP_SYMS = $(MOZSDK_DIR)/dump_syms
   INSTALL_NAME_TOOL ?= echo install_name_tool called on Linux && exit 1;
   OTOOL ?= echo otool called on Linux && exit 1;
else
ifeq (Msys,$(SB_VENDOR_ARCH))
   DUMP_SYMS = $(MOZSDK_DIR)/dump_syms.exe
   # Strip isn't needed/available on Win32; error out
   SB_STRIP ?= echo strip called on Win32 && exit 1;
   INSTALL_NAME_TOOL ?= echo install_name_tool called on Win32 && exit 1;
   OTOOL ?= echo otool called on Win32 && exit 1;

   SB_CC = cl
   SB_CXX = cl
   SB_LD = link
   SB_OBJDUMP = objdump
endif
endif
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

   CONFIGURE_TARGET = i686-pc-mingw32

   # We need these for all builds on Win32, since the system doesn't provide
   # it...
   SB_VENDOR_TARGET_DEP_MODULES += iconv glib gettext
endif

ifeq (Darwin,$(SB_VENDOR_ARCH))
  SB_CFLAGS += -fnested-functions \
               -gstabs+ \
               -isysroot $(MACOSX_SDK) \
               -D__MACOSX__ \
               -D__APPLE__

  SB_LDFLAGS += -headerpad_max_install_names -isysroot $(MACOSX_SDK) \
   -Wl,-syslibroot,$(MACOSX_SDK)

   # We need these for all builds on OSX, since the system doesn't provide them
   SB_VENDOR_TARGET_DEP_MODULES += glib gettext
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

ifeq (debug, $(SB_BUILD_TYPE))
  SB_CONFIGURE_OPTS += --enable-debug
  ifeq (Msys, $(SB_VENDOR_ARCH))
    SB_CFLAGS += -MTd -Zi
  endif
else
ifeq (release, $(SB_BUILD_TYPE))
  SB_CONFIGURE_OPTS += --disable-debug
  ifeq (Msys, $(SB_VENDOR_ARCH))
    SB_CFLAGS += -MT
  endif
else
  $(error Unknown SB_BUILD_TYPE: $(SB_BUILD_TYPE))
endif
endif

#
# Figure out locations of things we need (the Songbird dependencies directory,
# the tools binary directory, etc.)
#

# Is typically Makefile.songbird
SB_VENDOR_MAKEFILE := $(firstword $(MAKEFILE_LIST))

# Hardcode all this for now; this is for official building of these tools,
# so developers are likely to find this pretty painful.

SB_VENDOR_BUILD_ROOT ?= /builds/sb-deps
SB_VENDOR_BINARIES_CO_ROOT ?= $(SB_VENDOR_BUILD_ROOT)/checkout
SB_VENDOR_CHECKOUT ?= $(realpath $(CURDIR)/..)

ifeq (,$(shell test -e $(SB_VENDOR_BUILD_ROOT) && echo exists))
   $(error SB_VENDOR_BUILD_ROOT ($(SB_VENDOR_BUILD_ROOT)) does not exist...)
endif

ifeq (,$(shell test -e $(SB_VENDOR_BINARIES_CO_ROOT) && echo exists))
   $(error SB_VENDOR_BINARIES_CO_ROOT $(SB_VENDOR_BINARIES_CO_ROOT) does not exist...)
endif


SB_VENDOR_DIR ?= $(realpath $(CURDIR)/..)
SB_TARGET_SRC_DIR := $(CURDIR)

SB_VENDOR_BINARIES_DIR := $(SB_VENDOR_BUILD_ROOT)/$(SB_TARGET_ARCH)
SB_VENDOR_BINARIES_CHECKOUT := $(SB_VENDOR_BINARIES_CO_ROOT)/$(SB_TARGET_ARCH)

SB_VENDOR_BINARIES_TARGETS_FIND := $(FIND) $(SB_VENDOR_BINARIES_CHECKOUT) -maxdepth 1 -mindepth 1 -type d -not -name .svn
 
ifeq (Darwin,$(SB_VENDOR_ARCH))
  SB_VENDOR_BINARIES_TARGETS_FIND += -exec basename {} \;
else
  SB_VENDOR_BINARIES_TARGETS_FIND += -printf '%f '
endif

SB_VENDOR_BINARIES_TARGETS := $(shell $(SB_VENDOR_BINARIES_TARGETS_FIND))

ifeq (,$(shell test -e $(SB_VENDOR_BINARIES_DIR) && echo exists))
   $(error SB_VENDOR_BINARIES_DIR $(SB_VENDOR_BINARIES_DIR) does not exist...)
endif

# Where we'll build this stuff
SB_VENDOR_BUILD_DIR = $(SB_VENDOR_BUILD_ROOT)/build/$(SB_VENDOR_TARGET)/$(SB_BUILD_TYPE)

# Where we'll point configure to install it to
SB_CONFIGURE_PREFIX = $(SB_VENDOR_BINARIES_DIR)/$(SB_VENDOR_TARGET)/$(SB_BUILD_TYPE)

SB_VENDOR_TARGET_BINARY_DEPS_DIR = $(SB_VENDOR_BUILD_ROOT)/build/$(SB_VENDOR_TARGET)/build-deps

SB_VENDOR_BREAKPAD_DIR = $(SB_VENDOR_BINARIES_DIR)/$(SB_VENDOR_TARGET)/breakpad/$(SB_BUILD_TYPE)

SB_VENDOR_BREAKPAD_SYMBOL_PATH = $(SB_VENDOR_BREAKPAD_DIR)/breakpad-symbols
SB_VENDOR_BREAKPAD_ARCHIVE_PATH = $(SB_VENDOR_BREAKPAD_DIR)

SB_VENDOR_BREAKPAD_STORE_PATH := ./

# Generate syms by default; but make it easy to turn it off, if you're just
# interested in building.
SB_VENDOR_GENERATE_SYMBOLS ?= 0

# Symbols don't work on x86_64; don't even bother
ifeq (1_,$(SB_VENDOR_GENERATE_SYMBOLS)_$(filter linux-x86_64, $(SB_TARGET_ARCH)))
  SB_VENDOR_BREAKPAD_ARCHIVE = $(SB_VENDOR_BREAKPAD_DIR)/$(SB_VENDOR_TARGET)-symbols-$(SB_VENDOR_TARGET_VERSION)-$(SB_VENDOR_ARCH).zip
else
  SB_VENDOR_BREAKPAD_ARCHIVE = 
endif

# TODO: explain this gloop
define find-dep-dir
$(if $(shell test -e $1/install-$(SB_BUILD_TYPE) && echo exists),$(strip $1)/install-$(SB_BUILD_TYPE),$(strip $1)/$(SB_BUILD_TYPE))
endef

# Mozilla SDK settings
MOZSDK_DIR = $(call find-dep-dir, $(SB_VENDOR_BINARIES_DIR)/mozilla)
MOZSDK_BIN_DIR = $(MOZSDK_DIR)/bin
MOZSDK_SCRIPTS_DIR = $(MOZSDK_DIR)/scripts
MOZ_XR_DIR = $(call find-dep-dir, $(SB_VENDOR_BINARIES_DIR)/xulrunner)

# Supporting SDK paths on Win32
ifeq (Msys,$(SB_VENDOR_ARCH))
  DIRECTX_SDK_ROOT ?= /d/ms-sdks/directx-aug08
  WINDOWS_SDK_ROOT ?= /d/ms-sdks/Windows/v6.0
  QUICKTIME_SDK_ROOT ?= /d/ms-sdks/QuickTimeSDK
endif

ifeq (Darwin,$(SB_VENDOR_ARCH))
  MACOSX_SDK ?= /Developer/SDKs/MacOSX10.4u.sdk
endif

#
# Dependent library section; this is where we define and point the build system
# at our copies of various tools that are checked in.
#

define enable-sb-lib
$(filter $1, $(filter-out $(SB_VENDOR_TARGET), $(SB_VENDOR_TARGET_DEP_MODULES)))
endef

#
# GNU Gettext 
#
ifneq (,$(call enable-sb-lib, gettext))
  $(info Enabling Songbird vendor lib: gettext)
  SB_GETTEXT_DIR = $(call find-dep-dir, $(SB_VENDOR_BINARIES_DIR)/gettext)
  SB_LDFLAGS += -L$(SB_GETTEXT_DIR)/lib -lintl
  SB_CFLAGS += -I$(SB_GETTEXT_DIR)/include
  SB_CPPFLAGS += -I$(SB_GETTEXT_DIR)/include
  SB_PATH += $(SB_GETTEXT_DIR)/bin

  ifeq (Darwin,$(SB_VENDOR_ARCH))
    #SB_LDFLAGS += -Wl,-dylib_file -Wl,libintl.dylib:$(SB_GETTEXT_DIR)/lib/libintl.dylib
    SB_DYLD_LIBRARY_PATH += $(SB_GETTEXT_DIR)/lib
  endif
endif

#
# GNU iconv
#
ifneq (,$(call enable-sb-lib, iconv))
  $(info Enabling Songbird vendor lib: iconv)
  SB_ICONV_DIR := $(call find-dep-dir, $(SB_VENDOR_BINARIES_DIR)/libiconv)
  SB_LDFLAGS += -L$(SB_ICONV_DIR)/lib -liconv
  SB_CFLAGS += -I$(SB_ICONV_DIR)/include
  SB_PATH += $(SB_ICONV_DIR)/bin

  ifeq (Darwin,$(SB_VENDOR_ARCH))
    LDFLAGS += -Wl,-dylib_file -Wl,libiconv.dylib:$(SB_ICONV_DIR)/lib/libiconv.dylib
  endif
endif

#
# Glib
# 
ifneq (,$(call enable-sb-lib, glib))
  $(info Enabling Songbird vendor lib: glib)
  SB_GLIB_DIR := $(call find-dep-dir, $(SB_VENDOR_BINARIES_DIR)/glib)
  SB_PATH += $(SB_GLIB_DIR)/bin
  SB_PKG_CONFIG_PATH += $(SB_GLIB_DIR)/lib/pkgconfig

  GLIB_PARTS := glib gobject gmodule gthread

  ifeq (Darwin,$(SB_VENDOR_ARCH))
    LDFLAGS += $(foreach GLIB_PART, $(GLIB_PARTS), -Wl,-dylib_file -Wl,libgobject-2.0.dylib:$(SB_GLIB_DIR)/lib/lib$(GLIB_PART)-2.0.dylib)
    DYLD_LIBRARY_PATH += $(SB_GLIB_DIR)/lib
  endif
endif

#
# GNU libtool
#
ifneq (,$(call enable-sb-lib, libtool))
  $(info Enabling Songbird vendor lib: libtool)
  SB_PATH += $(SB_VENDOR_BINARIES_DIR)/libtool/release/bin
  ACLOCAL_FLAGS += -I $(SB_VENDOR_BINARIES_DIR)/libtool/release/share/aclocal
endif

#
# liboil
#
ifneq (,$(call enable-sb-lib, liboil))
  $(info Enabling Songbird vendor lib: liboil)
  SB_LIBOIL_DIR = $(call find-dep-dir, $(SB_VENDOR_BINARIES_DIR)/liboil)
  SB_LIBOIL_LIBS = -L$(SB_LIBOIL_DIR)/lib -loil-0.3
  SB_LIBOIL_CFLAGS = -I$(SB_LIBOIL_DIR)/include/liboil-0.3
  SB_PKG_CONFIG_PATH += $(SB_LIBOIL_DIR)/lib/pkgconfig

  ifeq (Msys, $(SB_VENDOR_ARCH))
     SB_PATH += $(SB_LIBOIL_DIR)/bin
     ifeq (debug, $(SB_BUILD_TYPE))
        SB_LIBOIL_LIBS += -Wl,-Zi
     endif
  endif
endif

#
# gstreamer
#
ifneq (,$(call enable-sb-lib, gstreamer))
  $(info Enabling Songbird vendor lib: gstreamer)
  SB_GSTREAMER_DIR = $(call find-dep-dir, $(SB_VENDOR_BINARIES_DIR)/gstreamer)
  SB_PATH += $(SB_GSTREAMER_DIR)/bin
  SB_PKG_CONFIG_PATH += $(SB_GSTREAMER_DIR)/lib/pkgconfig
  
  # A list of basic dylibs on mac that need to be fixed up across all the
  # gstreamer modules; these are built by various parts of gstreamer and 
  # base
  GST_BASE_DYLIBS = audio cdda fft interfaces netbuffer pbutils riff \
    rtp rtsp sdp tag video
endif

#
# gstreamer-plugins-base
#
ifneq (,$(call enable-sb-lib, gst-plugins-base))
  $(info Enabling Songbird vendor lib: gst-plugins-base)
  SB_GST_PLUGINS_BASE_DIR = $(call find-dep-dir, $(SB_VENDOR_BINARIES_DIR)/gst-plugins-base)
  SB_PATH += $(SB_GST_PLUGINS_BASE_DIR)/bin
  SB_PKG_CONFIG_PATH += $(SB_GST_PLUGINS_BASE_DIR)/lib/pkgconfig
endif

#
# libogg
#
ifneq (,$(call enable-sb-lib, ogg))
  $(info Enabling Songbird vendor lib: ogg)
  SB_LIBOGG_DIR = $(call find-dep-dir, $(SB_VENDOR_BINARIES_DIR)/libogg)
  SB_OGG_LIBS = -L$(SB_LIBOGG_DIR)/lib
  ifeq (Msys_debug,$(SB_VENDOR_ARCH)_$(SB_BUILD_TYPE))
    SB_OGG_LIBS += -logg_d
  else
     SB_OGG_LIBS += -logg
  endif
  SB_OGG_CFLAGS := -I$(SB_LIBOGG_DIR)/include
  SB_PKG_CONFIG_PATH += $(SB_LIBOGG_DIR)/lib/pkgconfig

  ifeq (Msys, $(SB_VENDOR_ARCH))
     SB_PATH += $(SB_LIBOGG_DIR)/bin
     ifeq (debug, $(SB_BUILD_TYPE))
       SB_LIBOGG_LIBS += -Wl,-Zi
     endif
  endif
endif

#
# libvorbis
#
ifneq (,$(call enable-sb-lib, vorbis))
  $(info Enabling Songbird vendor lib: vorbis)
  SB_LIBVORBIS_DIR = $(call find-dep-dir, $(SB_VENDOR_BINARIES_DIR)/libvorbis)
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

#
# libFLAC
#
ifneq (,$(call enable-sb-lib, flac))
  $(info Enabling Songbird vendor lib: flac)
  SB_LIBFLAC_DIR = $(call find-dep-dir, $(SB_VENDOR_BINARIES_DIR)/flac)
  SB_LDFLAGS += -L$(SB_LIBFLAC_DIR)/lib
  ifeq (Msys,$(SB_VENDOR_ARCH))
    SB_FLAC_LIBS += -lFLAC-8
    SB_PATH += $(SB_LIBFLAC_DIR)/bin
    ifeq (debug,$(SB_BUILD_TYPE))
      SB_FLAC_LIBS += -Wl,-Zi
    endif
  endif
  SB_CPPFLAGS += -I$(SB_LIBFLAC_DIR)/include
  SB_PKG_CONFIG_PATH += $(SB_LIBFLAC_DIR)/lib/pkgconfig
endif
