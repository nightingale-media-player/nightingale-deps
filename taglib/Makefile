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
# Makefile for building Songbird vendor: taglib
#
################################################################################
################################################################################

SB_VENDOR_TARGET_DEP_MODULES :=

SB_VENDOR_TARGET := taglib
SB_VENDOR_TARGET_VERSION := 1.6.1

include $(CURDIR)/../songbird-vendor-defs.mk

# Set compiler flags.
ifeq (Linux,$(SB_VENDOR_ARCH))
  CMAKE_C_FLAGS := -fPIC -fno-stack-protector
  CMAKE_CXX_FLAGS := -fPIC -fno-stack-protector
endif

SB_ZLIB_DIR := $(SB_VENDOR_BINARIES_DIR)/zlib/$(SB_BUILD_TYPE)

ifeq (Msys,$(SB_VENDOR_ARCH))
  # On Windows, we need to use the mozilla CRT because we need to link this
  # into an XPCOM component; otherwise, we end up using a mix of static and
  # dynamic CRTs and symbols end up missing.  This also means we need to
  # match the MSVCP80 (C++ standard library) configuration too - and Mozilla
  # builds with -D_STATIC_CPPLIB (statically linked C++ library).
  SB_USE_MOZCRT = 1
  CMAKE_C_FLAGS := -Zc:wchar_t- -Zi -EHsc -D_STATIC_CPPLIB
  CMAKE_CXX_FLAGS := $(CMAKE_C_FLAGS)

  CMAKE_C_FLAGS_DEBUG := $(CMAKE_C_FLAGS) -MDd
  CMAKE_C_FLAGS_RELEASE := $(CMAKE_C_FLAGS) -MD
  CMAKE_CXX_FLAGS_DEBUG := $(CMAKE_CXX_FLAGS) -MDd
  CMAKE_CXX_FLAGS_RELEASE := $(CMAKE_CXX_FLAGS) -MD

  CMAKE_ZLIB_FLAGS := -DZLIB_INCLUDE_DIR:PATH="$(SB_ZLIB_DIR)/include" \
   -DZLIB_LIBRARY:FILEPATH="$(SB_ZLIB_DIR)/lib/zlib.lib"
else
   ifeq (SunOS,$(SB_VENDOR_ARCH))
      CMAKE_C_FLAGS_DEBUG += -g
      CMAKE_CXX_FLAGS_DEBUG += -g
   else
      CMAKE_C_FLAGS_DEBUG += -g -gstabs+
      CMAKE_CXX_FLAGS_DEBUG += -g -gstabs+
      CMAKE_C_FLAGS_RELEASE += -gstabs+
      CMAKE_CXX_FLAGS_RELEASE += -gstabs+
   endif
endif

# This makefile fixes OS X 10.6, by forcing a 10.4u SDK / GCC4.0 build
ifeq (Darwin,$(SB_VENDOR_ARCH))
  CMAKE_ARGS += -DCMAKE_OSX_SYSROOT:PATH=/Developer/SDKs/MacOSX10.4u.sdk
  CMAKE_ARGS += -DCMAKE_CXX_COMPILER:PATH=/usr/bin/c++-4.0
endif

# Build a static taglib.
CMAKE_ARGS += -DENABLE_STATIC=ON
CMAKE_ARGS += -DWITH_ASF=ON
CMAKE_ARGS += -DWITH_MP4=ON

ifeq (debug,$(SB_BUILD_TYPE))
  CMAKE_ARGS += -DCMAKE_BUILD_TYPE=Debug
endif
ifeq (release,$(SB_BUILD_TYPE))
  CMAKE_ARGS += -DCMAKE_BUILD_TYPE=Release
endif

# Set makefile type.
ifeq (Msys,$(SB_VENDOR_ARCH))
  CMAKE_MAKEFILE_TYPE := NMake Makefiles

  # Used by -rules; defaults to $(MAKE), BUT NOT FOR Win32 CMAKE!!
  SUBMAKE_CMD = $(CURDIR)/../call-nmake.py
else
  CMAKE_MAKEFILE_TYPE := Unix Makefiles
endif

SB_RUN_CONFIGURE := 0

include $(CURDIR)/../songbird-vendor-rules.mk

module_setup_build:
	cd $(SB_VENDOR_BUILD_DIR) && $(CMAKE) $(CMAKE_ARGS) \
          -DCMAKE_C_FLAGS="$(CMAKE_C_FLAGS)" \
          -DCMAKE_CXX_FLAGS="$(CMAKE_CXX_FLAGS)" \
          -DCMAKE_C_FLAGS_DEBUG="$(CMAKE_C_FLAGS_DEBUG)" \
          -DCMAKE_CXX_FLAGS_DEBUG="$(CMAKE_CXX_FLAGS_DEBUG)" \
          -DCMAKE_C_FLAGS_RELEASE="$(CMAKE_C_FLAGS_RELEASE)" \
          -DCMAKE_CXX_FLAGS_RELEASE="$(CMAKE_CXX_FLAGS_RELEASE)" \
          -DCMAKE_VERBOSE_MAKEFILE=1 \
          -DCMAKE_INSTALL_PREFIX=$(SB_CONFIGURE_PREFIX) \
          -DCMAKE_DEBUG_POSTFIX= \
          $(CMAKE_ZLIB_FLAGS) \
          -G "$(CMAKE_MAKEFILE_TYPE)"

module_post_build:
	@echo Removing \(unused\) taglib C-bindings...
	$(FIND) $(SB_CONFIGURE_PREFIX) -name '*_c.*' -exec rm -v {} \;
