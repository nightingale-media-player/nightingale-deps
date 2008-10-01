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

post_build:
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

build: clean_build_dir setup_build
	cd $(SB_VENDOR_BUILD_DIR) && \
   $(CONFIGURE) --prefix=$(SB_CONFIGURE_PREFIX) \
      $(SB_CONFIGURE_OPTS) \
      --disable-loadsave \
      --enable-binary-registry \
      --disable-examples \
      --disable-tests \
      --disable-trace \
      --disable-alloc-trace \
      -C
	$(MAKE) $(SB_VENDOR_MAKEFLAGS) -C $(SB_VENDOR_BUILD_DIR)
	$(MAKE) $(SB_VENDOR_MAKEFLAGS) -C $(SB_VENDOR_BUILD_DIR) install

setup_build:
ifeq (Darwin,$(SB_VENDOR_ARCH))
	export MACOSX_DEPLOYMENT_TARGET=10.4
	export DYLD_LIBRARY_PATH="$(DYLD_LIBRARY_PATH)"
endif
	export CPPFLAGS="$(CPPFLAGS)"
	export CFLAGS="$(CFLAGS)"
	export LDFLAGS="$(LDFLAGS)"
	export ACLOCAL_FLAGS="$(ACLOCAL_FLAGS)"
	export PKG_CONFIG_PATH="$(PKG_CONFIG_PATH)"
	export PATH="$(PATH)"

	# Generate, configure, build, and install.
	export NOCONFIGURE=yes

clean_build_dir:
	$(RM) -Rf $(SB_VENDOR_BUILD_DIR)
	$(MKDIR) -p $(SB_VENDOR_BUILD_DIR)
	# TODO: this kinda sucks; fix this
	$(CP) -R $(SB_TARGET_SRC_DIR)/* $(SB_VENDOR_BUILD_DIR)

.PHONY: all release debug build build_setup clean_build_dir post_build strip_build
