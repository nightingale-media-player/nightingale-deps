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
# Makefile rules for Songbird vendor repository
#
################################################################################
################################################################################

#
# We export these to the environment down here because if we did so in -defs.mk,
# the package file wouldn't have a chance to munge these flags itself. (Part of
# me thinks this should be a target anyway...)
#

ifeq (Darwin,$(SB_VENDOR_ARCH))
  export MACOSX_DEPLOYMENT_TARGET
  export DYLD_LIBRARY_PATH
endif
export CPPFLAGS
export CFLAGS
export LDFLAGS
export ACLOCAL_FLAGS
export PKG_CONFIG_PATH
export PATH

export LIBOIL_CFLAGS = $(SB_LIBOIL_CFLAGS)
export LIBOIL_LIBS = $(SB_LIBOIL_CFLAGS)
export OGG_CFLAGS = $(SB_OGG_CFLAGS)
export OGG_LIBS = $(SB_OGG_LIBS)
export VORBIS_CFLAGS = $(SB_VORBIS_CFLAGS)
export VORBIS_LIBS = $(SB_VORBIS_LIBS)

# Generate, configure, build, and install.
export NOCONFIGURE=yes

all:
	$(MAKE) $(MAKEFLAGS) -f $(SB_VENDOR_MAKEFILE) debug
	$(MAKE) $(MAKEFLAGS) -f $(SB_VENDOR_MAKEFILE) release

debug: build post_build $(SB_VENDOR_BREAKPAD_ARCHIVE)

release: build post_build $(SB_VENDOR_BREAKPAD_ARCHIVE) strip_build

strip_build:
ifneq (Msys,$(SB_VENDOR_ARCH))
	$(FIND) -L $(SB_VENDOR_BUILD_DIR) -not -type d \
         -name "*.dylib" \
			-or -name "*.so" \
         -exec $(STRIP) {} \;
endif

post_build: module_post_build
ifeq (Darwin, $(SB_VENDOR_ARCH))
	echo Perform scrubbing here.
endif

$(SB_VENDOR_BREAKPAD_ARCHIVE):
	$(MKDIR) -p $(SB_VENDOR_BREAKPAD_SYMBOL_PATH)
	$(PYTHON) $(MOZSDK_SCRIPTS_DIR)/symbolstore.py \
              $(DUMP_SYM_ARGS) \
              -s $(SB_CONFIGURE_PREFIX)/lib \
              $(DUMP_SYMS) \
              $(SB_VENDOR_BREAKPAD_SYMBOL_PATH) \
              $(SB_VENDOR_BREAKPAD_STORE_PATH) \
              > $(SB_VENDOR_BREAKPAD_SYMBOL_PATH)/$(SB_TARGET_NAME)-symbols.txt

	cd $(SB_VENDOR_BREAKPAD_SYMBOL_PATH) && \
    $(ZIP) -r9D $(SB_VENDOR_BREAKPAD_ARCHIVE_PATH) .

build: clean_build_dir setup_build module_setup_build
	cd $(SB_VENDOR_BUILD_DIR) && \
   $(CONFIGURE) --prefix=$(SB_CONFIGURE_PREFIX) \
      $(SB_CONFIGURE_OPTS) \
		$(SB_VENDOR_TARGET_CONFIGURE_OPTS) \
      -C
	$(MAKE) -C $(SB_VENDOR_BUILD_DIR)
	$(MAKE) -C $(SB_VENDOR_BUILD_DIR) install

# Nothing for now...
setup_build:
	echo Songbird Vendor Environment Settings
ifeq (Darwin,$(SB_VENDOR_ARCH))
  @echo MACOSX_DEPLOYMENT_TARGET=$(MACOSX_DEPLOYMENT_TARGET)
  @echo DYLD_LIBRARY_PATH=$(DYLD_LIBRARY_PATH)
endif
	@echo CPPFLAGS = $(CPPFLAGS)
	@echo CFLAGS=$(CFLAGS)
	@echo LDFLAGS=$(LDFLAGS)
	@echo ACLOCAL_FLAGS=$(ACLOCAL_FLAGS)
	@echo PKG_CONFIG_PATH=$(PKG_CONFIG_PATH)
	@echo PATH=$(PATH)
	@echo LIBOIL_CFLAGS=$(LIBOIL_CFLAGS)
	@echo LIBOIL_LIBS=$(LIBOIL_LIBS)
	@echo OGG_CFLAGS=$(OGG_CFLAGS)
	@echo OGG_LIBS=$(OGG_LIBS)

clean_build_dir:
	$(RM) -Rf $(SB_VENDOR_BUILD_DIR)
	$(MKDIR) -p $(SB_VENDOR_BUILD_DIR)
	# TODO: this kinda sucks; fix this
	$(CP) -R $(SB_TARGET_SRC_DIR)/* $(SB_VENDOR_BUILD_DIR)

.PHONY: all release debug build build_setup clean_build_dir post_build strip_build module_setup_build module_post_build
