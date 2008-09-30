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
   ifeq (i386,$(SB_VENDOR_SUBARCH))
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
   $(error Unsupported vendor build environment ($(SB_VENDOR_ARCH), $(SB_VENDOR_SUBARCH), $SV_VENDOR_OS)) 
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

#
# On certain platforms, we need some extra information/flag munging
#

ifeq (Msys,$(SB_VENDOR_ARCH))
   _MSVC_VER_FILTER='s|.* \([0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\).*|\1|p'
   CC_VERSION= $(shell cl -v 2>&1 | sed -ne "$(_MSVC_VER_FILTER)")
   _CC_MAJOR_VERSION=$(shell echo ${CC_VERSION} | awk -F\. '{ print $1 }')
   _CC_MINOR_VERSION=$(shell echo ${CC_VERSION} | awk -F\. '{ print $2 }')
   _MSC_VER=$(_CC_MAJOR_VERSION)$(_CC_MINOR_VERSION)
endif

ifeq (Darwin,$(SB_VENDOR_ARCH))
  CFLAGS += -fnested-functions \
            -gstabs+
  LDFLAGS += -headerpad_max_install_names
  DYLD_LIBRARY_PATH += :/opt/local/lib:/usr/lib
endif

$(warn MAKECMDGOALS is $(MAKECMDGOALS))

ifeq (debug,$(MAKECMDGOALS))
   SB_BUILD_TYPE := debug
else
ifeq (release,$(MAKECMDGOALS))
   SB_BUILD_TYPE := release
endif
endif

#
# Figure out locations of things we need (the Songbird dependencies directory,
# the tools binary directory, etc.)
#

# Is typically Makefile.songbird
SB_VENDOR_MAKEFILE := $(firstword $(MAKEFILE_LIST))

SB_DEPENDENCIES_DIR ?= $(realpath $(CURDIR)/../..)
SB_TOOLS_DIR := $(realpath $(SB_DEPENDENCIES_DIR)/../tools)

SB_TARGET_SRC_DIR := $(CURDIR)

# TODO - check to see this dir exists
SB_VENDOR_BINARIES_DIR := $(SB_DEPENDENCIES_DIR)/$(SB_TARGET_ARCH)

# Where we'll build this stuff
SB_VENDOR_BUILD_DIR = $(SB_VENDOR_BINARIES_DIR)/$(SB_VENDOR_TARGET)/build-$(SB_BUILD_TYPE)

# Where we'll point configure to install it to
SB_CONFIGURE_PREFIX = $(SB_VENDOR_BINARIES_DIR)/$(SB_VENDOR_TARGET)/$(SB_BUILD_TYPE)

SB_CONFIGURE_OPTS :=
SB_USE_SYSTEM_LIBS ?=

# Mozilla SDK settings
MOZSDK_DIR = $(SB_VENDOR_BINARIES_DIR)/mozilla/$(SB_BUILD_TYPE)
MOZSDK_BIN_DIR = $(MOZSDK_DIR)/bin
MOZSDK_SCRIPTS_DIR = $(MOZSDK_DIR)/scripts

# Gettext build options
SB_GETTEXT_DIR = $(SB_VENDOR_BINARIES_DIR)/gettext/$(SB_BUILD_TYPE)
LIBS += -L$(SB_GETTEXT_DIR)/lib -lintl
CFLAGS += -I$(SB_GETTEXT_DIR)/include
PATH := $(SB_GETTEXT_DIR)/bin:$(PATH)

ifeq (Darwin,$(SB_VENDOR_ARCH))
  LDFLAGS += -Wl,-dylib_file -Wl,libintl.dylib:$(SB_GETTEXT)/lib/libintl.dylib
  DYLD_LIBRARY_PATH := $(SB_GETTEXT_DIR)/lib:$(DYLD_LIBRARY_PATH)
endif

# Set up iconv build options.
SB_ICONV_DIR := $(SB_VENDOR_BINARIES_DIR)/iconv/$(SB_BUILD_TYPE)
LIBS += -L$(SB_ICONV_DIR)/lib -liconv
CFLAGS += -I$(SB_ICONV_DIR)/include
PATH := $(SB_ICONV_DIR)/bin:$(PATH)

ifeq (Darwin,$(SB_VENDOR_ARCH))
  LDFLAGS += -Wl,-dylib_file -Wl,libiconv.dylib:$(SB_ICONV_DIR)/lib/libiconv.dylib
endif

# Set up glib build options.
SB_GLIB_DIR := $(SB_VENDOR_BINARIES_DIR)/glib/$(SB_BUILD_TYPE)
PATH := $(SB_GLIB_DIR)/bin:$(PATH)
PKG_CONFIG_PATH := $(SB_GLIB_DIR)/lib/pkgconfig:$(PKG_CONFIG_PATH)

GLIB_PARTS := glib gobject gmodule gthread

ifeq (Darwin,$(SB_VENDOR_ARCH))
  LDFLAGS += $(foreach GLIB_PART, $(GLIB_PARTS), -Wl,-dylib_file -Wl,libgobject-2.0.dylib:$(SB_GLIB_DIR)/lib/lib$(GLIB_PART)-2.0.dylib)
  DYLD_LIBRARY_PATH := $(SB_GLIB_DIR)/lib:$(DYLD_LIBRARY_PATH)
endif

# Set up libtool.
PATH := $(SB_VENDOR_BINARIES_DIR)/libtool/release/bin:$(PATH)
ACLOCAL_FLAGS += -I $(SB_VENDOR_BINARIES_DIR)/libtool/release/share/aclocal

# add win32 specific flags
ifeq (Msys, $(SB_VENDOR_ARCH))
  CFLAGS +=-D_MSC_VER=$(_MSC_VER) -DWIN32 -D__NO_CTYPE \
   -D_CRT_SECURE_NO_WARNINGS -DHAVE_WIN32 -D_WINDOWS \
	-wd4820 -wd4668 -wd4100 -wd4706 -wd4127 -wd4255 -wd4710 -wd4055
endif

# Set up the target configuration options.
ifneq (,$(CONFIGURE_TARGET))
  SB_CONFIGURE_OPTS += --target=$(CONFIGURE_TARGET)
  SB_CONFIGURE_OPTS += --host=$(CONFIGURE_TARGET)
endif

SB_VENDOR_MAKEFLAGS ?=

ifeq (debug, $(SB_BUILD_TYPE))
  SB_CONFIGURE_OPTS += --enable-debug=yes
  ifeq (Msys, $(SB_VENDOR_ARCH))
    CFLAGS += -MTd -Zi
  endif
else
  # Default to release mode...
  SB_CONFIGURE_OPTS += --enable-debug=no
  ifeq (Msys, $(SB_VENDOR_ARCH))
    CFLAGS += -MT
  endif
endif
