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

#
# Operating system detection
#

SB_VENDOR_ARCH := $(shell uname)
SB_VENDOR_SUBARCH := $(shell uname -m)
SB_VENDOR_OS := $(shell uname -o)

SB_ARCH_DETECTED := 0

ifeq (Darwin,$(SB_VENDOR_ARCH))
   SB_VENDOR_ARCH_MAC := 1
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
   SB_VENDOR_ARCH_LINUX := 1
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

ifeq (0,$(SB_ARCH_DETECTED))
   $(error Unsupported vendor build environment ($(SB_VENDOR_ARCH), $(SB_VENDOR_SUBARCH), $(SB_VENDOR_OS))) 
endif

#
# System tools we rely on for the vendor build process; allow them to be
# overridden
#

PYTHON ?= python
CONFIGURE ?= ./configure
RM ?= rm
CP ?= cp
MKDIR ?= mkdir
FIND ?= find
ZIP ?= zip

DUMP_SYMS_ARGS := --vcs-info

ifeq (Darwin,$(SB_VENDOR_ARCH))
   STRIP ?= strip -x -S
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
ifeq (Linux, $(SB_VENDOR_ARCH))
   STRIP ?= strip -v
   DUMP_SYMS = $(MOZSDK_DIR)/dump_syms
   INSTALL_NAME_TOOL ?= echo install_name_tool called on Linux && exit 1;
   OTOOL ?= echo otool called on Linux && exit 1;
else
ifeq (Msys, $(SB_VENDOR_ARCH))
   DUMP_SYMS = $(MOZSDK_DIR)/dump_syms.exe
   # Strip isn't needed/available on Win32; error out
   STRIP ?= echo strip called on Win32 && exit 1;
   INSTALL_NAME_TOOL ?= echo install_name_tool called on Win32 && exit 1;
   OTOOL ?= echo otool called on Win32 && exit 1;
endif
endif
endif

#
# On certain platforms, we need some extra information/flag munging
#

ifeq (Msys,$(SB_VENDOR_ARCH))
   _MSVC_VER_FILTER='s|.* \([0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\).*|\1|p'
   CC_VERSION= $(shell cl -v 2>&1 | sed -ne "$(_MSVC_VER_FILTER)")
   _CC_MAJOR_VERSION=$(shell echo ${CC_VERSION} | awk -F\. '{ print $1 }')
   _CC_MINOR_VERSION=$(shell echo ${CC_VERSION} | awk -F\. '{ print $2 }')
   _MSC_VER=$(_CC_MAJOR_VERSION)$(_CC_MINOR_VERSION)

   CFLAGS +=-D_MSC_VER=$(_MSC_VER) -DWIN32 -D__NO_CTYPE \
   -D_CRT_SECURE_NO_WARNINGS -DHAVE_WIN32 -D_WINDOWS \
	-wd4820 -wd4668 -wd4100 -wd4706 -wd4127 -wd4255 -wd4710 -wd4055

   CONFIGURE_TARGET = i686-pc-mingw32
endif

ifeq (Darwin,$(SB_VENDOR_ARCH))
  CFLAGS += -fnested-functions \
            -gstabs+ \
				-D__MACOSX__

  LDFLAGS += -headerpad_max_install_names
  DYLD_LIBRARY_PATH += /opt/local/lib:/usr/lib
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
  SB_CONFIGURE_OPTS += --host=$(CONFIGURE_TARGET)
endif

ifeq (debug, $(SB_BUILD_TYPE))
  SB_CONFIGURE_OPTS += --enable-debug=yes
  ifeq (Msys, $(SB_VENDOR_ARCH))
    CFLAGS += -MTd -Zi
  endif
else
ifeq (release, $(SB_BUILD_TYPE))
  SB_CONFIGURE_OPTS += --enable-debug=no
  ifeq (Msys, $(SB_VENDOR_ARCH))
    CFLAGS += -MT
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

SB_DEPENDENCIES_DIR ?= $(realpath $(CURDIR)/../..)

SB_TARGET_SRC_DIR := $(CURDIR)

# TODO - check to see this dir exists
SB_VENDOR_BINARIES_DIR := $(SB_DEPENDENCIES_DIR)/$(SB_TARGET_ARCH)

# Where we'll build this stuff
SB_VENDOR_BUILD_DIR = $(SB_VENDOR_BINARIES_DIR)/$(SB_VENDOR_TARGET)/build-$(SB_BUILD_TYPE)

# Where we'll point configure to install it to
SB_CONFIGURE_PREFIX = $(SB_VENDOR_BINARIES_DIR)/$(SB_VENDOR_TARGET)/$(SB_BUILD_TYPE)

SB_VENDOR_BREAKPAD_DIR = $(SB_VENDOR_BINARIES_DIR)/$(SB_VENDOR_TARGET)/breakpad/$(SB_BUILD_TYPE)

SB_VENDOR_BREAKPAD_SYMBOL_PATH = $(SB_VENDOR_BREAKPAD_DIR)/breakpad-symbols
SB_VENDOR_BREAKPAD_ARCHIVE_PATH = $(SB_VENDOR_BREAKPAD_DIR)

SB_VENDOR_BREAKPAD_STORE_PATH := ./

# Generate syms by default; but make it easy to turn it off, if you're just
# interested in building.
SB_VENDOR_GENERATE_SYMBOLS ?= 1

# Symbols don't work on x86_64; don't even bother
ifeq (1_,$(SB_VENDOR_GENERATE_SYMBOLS)_$(filter linux-x86_64, $(SB_TARGET_ARCH)))
  SB_VENDOR_BREAKPAD_ARCHIVE = $(SB_VENDOR_BREAKPAD_DIR)/$(SB_VENDOR_TARGET)-symbols-$(SB_VENDOR_TARGET_VERSION)-$(SB_VENDOR_ARCH).zip
else
  SB_VENDOR_BREAKPAD_ARCHIVE = 
endif

SB_CONFIGURE_OPTS :=
SB_USE_SYSTEM_LIBS ?=

# Mozilla SDK settings
MOZSDK_DIR = $(SB_VENDOR_BINARIES_DIR)/mozilla/$(SB_BUILD_TYPE)
MOZSDK_BIN_DIR = $(MOZSDK_DIR)/bin
MOZSDK_SCRIPTS_DIR = $(MOZSDK_DIR)/scripts

SB_PATH :=
SB_PKG_CONFIG_PATH :=
SB_DYLD_LIBRARY_PATH :=

#
# Dependent library section; this is where we define and point the build system
# at our copies of various tools that are checked in.
#

#
# GNU Gettext 
#
SB_GETTEXT_DIR = $(SB_VENDOR_BINARIES_DIR)/gettext/$(SB_BUILD_TYPE)
LIBS += -L$(SB_GETTEXT_DIR)/lib -lintl
CFLAGS += -I$(SB_GETTEXT_DIR)/include
SB_PATH += $(SB_GETTEXT_DIR)/bin

ifeq (Darwin,$(SB_VENDOR_ARCH))
  LDFLAGS += -Wl,-dylib_file -Wl,libintl.dylib:$(SB_GETTEXT)/lib/libintl.dylib
  DYLD_LIBRARY_PATH := $(SB_GETTEXT_DIR)/lib:$(DYLD_LIBRARY_PATH)
endif

#
# GNU iconv
#
SB_ICONV_DIR := $(SB_VENDOR_BINARIES_DIR)/iconv/$(SB_BUILD_TYPE)
LIBS += -L$(SB_ICONV_DIR)/lib -liconv
CFLAGS += -I$(SB_ICONV_DIR)/include
SB_PATH += $(SB_ICONV_DIR)/bin

ifeq (Darwin,$(SB_VENDOR_ARCH))
  LDFLAGS += -Wl,-dylib_file -Wl,libiconv.dylib:$(SB_ICONV_DIR)/lib/libiconv.dylib
endif

#
# Glib
# 
SB_GLIB_DIR := $(SB_VENDOR_BINARIES_DIR)/glib/$(SB_BUILD_TYPE)
SB_PATH += $(SB_GLIB_DIR)/bin
SB_PKG_CONFIG_PATH += $(SB_GLIB_DIR)/lib/pkgconfig

GLIB_PARTS := glib gobject gmodule gthread

ifeq (Darwin,$(SB_VENDOR_ARCH))
  LDFLAGS += $(foreach GLIB_PART, $(GLIB_PARTS), -Wl,-dylib_file -Wl,libgobject-2.0.dylib:$(SB_GLIB_DIR)/lib/lib$(GLIB_PART)-2.0.dylib)
  DYLD_LIBRARY_PATH := $(SB_GLIB_DIR)/lib:$(DYLD_LIBRARY_PATH)
endif

#
# GNU libtool
#
SB_PATH += $(SB_VENDOR_BINARIES_DIR)/libtool/release/bin
ACLOCAL_FLAGS += -I $(SB_VENDOR_BINARIES_DIR)/libtool/release/share/aclocal

#
# liboil
#
SB_LIBOIL_DIR := $(SB_VENDOR_BINARIES_DIR)/liboil/$(SB_BUILD_TYPE)
SB_LIBOIL_LIBS := -L$(SB_LIBOIL_DIR)/lib -loil-0.3
SB_LIBOIL_CFLAGS := -I$(SB_LIBOIL_DIR)/include/liboil-0.3
SB_PKG_CONFIG_PATH += $(SB_LIBOIL_DIR)/lib/pkgconfig

ifeq (Msys, $(SB_VENDOR_ARCH))
   SB_PATH += $(SB_LIBOIL_DIR)/bin
   ifeq (debug, $(SB_BUILD_TYPE))
      SB_LIBOIL_LIBS += -Wl,-Zi
   endif
endif

#
# gstreamer
#
SB_GSTREAMER_DIR := $(SB_VENDOR_BINARIES_DIR)/gstreamer/$(SB_BUILD_TYPE)
SB_PATH += $(SB_GSTREAMER_DIR)/bin
SB_PKG_CONFIG_PATH += $(SB_GSTREAMER_DIR)/lib/pkgconfig

#
# libogg
#
SB_LIBOGG_DIR := $(SB_VENDOR_BINARIES_DIR)/libogg/$(SB_BUILD_TYPE)
SB_OGG_LIBS := -L$(SB_LIBOGG_DIR)/lib
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

#
# libvorbis
#
SB_LIBVORBIS_DIR := $(SB_VENDOR_BINARIES_DIR)/libvorbis/$(SB_BUILD_TYPE)
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

# Filter things out of paths; done in two phases; normalize spaces 
# then turn remaining spaces into path separators (colons).

SB_PKG_CONFIG_PATH := $(subst $(SPACE),:,$(strip $(SB_PKG_CONFIG_PATH)))
ifeq (,$(PKG_CONFIG_PATH))
  PKG_CONFIG_PATH := $(SB_PKG_CONFIG_PATH)
else
  PKG_CONFIG_PATH := $(SB_PKG_CONFIG_PATH):$(PKG_CONFIG_PATH)
endif

SB_PATH := $(subst $(SPACE),:,$(strip $(SB_PATH)))
ifeq (,$(PATH))
  PATH := $(SB_PATH)
else
  PATH := $(SB_PATH):$(PATH)
endif

# broken
#DYLD_LIBRARY_PATH := $(subst $(SPACE),:,$(DYLD_LIBRARY_PATH))
#DYLD_LIBRARY_PATH := $(subst $(SPACE),$(EMPTY),$(DYLD_LIBRARY_PATH))
