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

ifeq (Linux,$(SB_VENDOR_ARCH))
   STRIP ?= strip -v
   DUMP_SYMS ?= $(MOZSDK_BIN_DIR)/dump_syms
   INSTALL_NAME_TOOL ?= echo install_name_tool called on Linux && exit 1;
   OTOOL ?= echo otool called on Linux && exit 1;
   SB_AR ?= ar
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

ifeq (,$(wildcard $(SB_VENDOR_BUILD_ROOT)))
   $(error SB_VENDOR_BUILD_ROOT ($(SB_VENDOR_BUILD_ROOT)) does not exist...)
endif

SB_VENDOR_DIR ?= $(realpath $(CURDIR)/..)
SB_TARGET_SRC_DIR := $(CURDIR)

SB_VENDOR_BINARIES_DIR := $(SB_VENDOR_BUILD_ROOT)/$(SB_TARGET_ARCH)

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
SB_REGEN_MAKEFILE_PKGS = $(NULL)

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
SB_VENDOR_XR_TARGET = xulrunner-6.0.2
SB_VENDOR_MOZSDK_TARGET = $(subst xulrunner,mozilla,$(SB_VENDOR_XR_TARGET))

# Mozilla SDK settings
MOZSDK_DIR = $(call find-dep-dir, $(SB_VENDOR_MOZSDK_TARGET))
MOZSDK_BIN_DIR = $(MOZSDK_DIR)/bin
MOZSDK_SCRIPTS_DIR = $(MOZSDK_DIR)/scripts
MOZ_XR_DIR = $(call find-dep-dir, $(SB_VENDOR_XR_TARGET))

#
# Dependent library section; this is where we define and point the build system
# at our copies of various tools that are checked in.
#

define enable-sb-lib
$(filter $1, $(filter-out $(SB_VENDOR_TARGET), $(SB_VENDOR_TARGET_DEP_MODULES)))
endef
