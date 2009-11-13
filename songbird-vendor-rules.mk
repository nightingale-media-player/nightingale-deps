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

# Filter things out of paths; done in two phases; normalize spaces 
# then turn remaining spaces into path separators (colons).

SB_PKG_CONFIG_PATH := $(subst $(SPACE),:,$(strip $(SB_PKG_CONFIG_PATH)))
ifneq (,$(PKG_CONFIG_PATH))
  SB_PKG_CONFIG_PATH := $(SB_PKG_CONFIG_PATH):$(PKG_CONFIG_PATH)
endif

SB_PATH := $(subst $(SPACE),:,$(strip $(SB_PATH))):$(PATH)

SB_DYLD_LIBRARY_PATH := $(subst $(SPACE),:,$(strip $(SB_DYLD_LIBRARY_PATH)))
ifneq (,$(DYLD_LIBRARY_PATH))
  SB_DYLD_LIBRARY_PATH := $(SB_DYLD_LIBRARY_PATH):$(DYLD_LIBRARY_PATH)
endif

# Only export these if we're actually building a target
BUILD_TARGET_SET = $(if \
                      $(or $(filter debug, $(MAKECMDGOALS)), \
                           $(filter release, $(MAKECMDGOALS)), \
                           $(filter regen-makefiles, $(MAKECMDGOALS))),\
                       build_requested,)

SB_RUN_CONFIGURE ?= 1

# This flag only means anything on Win32 right now, so assert that...
ifdef SB_USE_MOZCRT
   ifneq (Msys,$(SB_VENDOR_ARCH))
      $(error SB_USE_MOZCRT is only meaningful on Win32.)
   endif
endif

ifeq (Msys,$(SB_VENDOR_ARCH))
   ifeq (,$(wildcard $(WINDOWS_SDK_ROOT)))
      $(error Could not find Windows SDK: $(WINDOWS_SDK_ROOT)) 
   endif

   ifeq (,$(wildcard $(DIRECTX_SDK_ROOT)))
      $(error Could not find DirectX SDK: $(DIRECTX_SDK_ROOT)) 
   endif

   ifeq (,$(wildcard $(QUICKTIME_SDK_ROOT)))
      $(error Could not find QuickTime SDK: $(QUICKTIME_SDK_ROOT)) 
   endif

   ifeq (debug,$(SB_BUILD_TYPE))
      ifeq (1,$(SB_USE_MOZCRT))
         SB_CFLAGS += -MDd
         SB_LDFLAGS += -L$(MOZSDK_DIR)/lib -NODEFAULTLIB:msvcrt \
          -NODEFAULTLIB:msvcrtd -DEFAULTLIB:mozcrt19d
      else
         SB_CFLAGS += -MTd
      endif
      SB_CFLAGS += -DDEBUG -UNDEBUG
   endif
   ifeq (release,$(SB_BUILD_TYPE))
      ifeq (1,$(SB_USE_MOZCRT))
         SB_CFLAGS += -MD
         SB_LDFLAGS += -L$(MOZSDK_DIR)/lib -NODEFAULTLIB:msvcrt \
          -NODEFAULTLIB:msvcrtd -DEFAULTLIB:mozcrt19
      else
         SB_CFLAGS += -MT
      endif
      SB_CFLAGS += -UDEBUG -DNDEBUG
   endif
endif

# TODO: define these as a list of exportable targets and expand that, so
# we can match the printouts in -rules.mk
ifneq (,$(BUILD_TARGET_SET))
  export CC = $(SB_CC)
  export CXX = $(SB_CXX)
  export LD = $(SB_LD)
  export AR = $(SB_AR)
  export OBJDUMP = $(SB_OBJDUMP)

  export CPPFLAGS = $(SB_CPPFLAGS)
  export CFLAGS = $(SB_CFLAGS)
  export CXXFLAGS = $(SB_CXXFLAGS)
  export LDFLAGS = $(SB_LDFLAGS)
  export ACLOCAL_FLAGS = $(SB_ACLOCAL_FLAGS)
  export PKG_CONFIG_PATH = $(SB_PKG_CONFIG_PATH)
  export PATH = $(SB_PATH)
ifeq (Msys,$(SB_VENDOR_ARCH))
  export LIBS = $(SB_LIBS)
endif

  export LIBOIL_CFLAGS = $(SB_LIBOIL_CFLAGS)
  export LIBOIL_LIBS = $(SB_LIBOIL_LIBS)
  export OGG_CFLAGS = $(SB_OGG_CFLAGS)
  export OGG_LIBS = $(SB_OGG_LIBS)
  export VORBIS_CFLAGS = $(SB_VORBIS_CFLAGS)
  export VORBIS_LIBS = $(SB_VORBIS_LIBS)
  export FLAC_CFLAGS = $(SB_FLAC_CFLAGS)
  export FLAC_LIBS = $(SB_FLAC_LIBS)

  ifeq (Darwin,$(SB_VENDOR_ARCH))
    export MACOSX_DEPLOYMENT_TARGET=10.4
    export DYLD_LIBRARY_PATH = $(SB_DYLD_LIBRARY_PATH)
  endif
endif

# This is for libtool and automake; we manage running configure outselves, so
# turn this off.
export NOCONFIGURE = 1

all:
ifneq (1,$(SB_VENDOR_SKIP_DEBUG_BUILD))
	$(MAKE) -f $(SB_VENDOR_MAKEFILE) debug
endif
ifneq (1,$(SB_VENDOR_SKIP_RELEASE_BUILD))
	$(MAKE) -f $(SB_VENDOR_MAKEFILE) release
endif

debug: build post_build $(SB_VENDOR_BREAKPAD_ARCHIVE) copy_symbols
ifneq (,$(SB_VENDOR_BUILD_LOG))
	-$(CP) $(SB_VENDOR_BUILD_LOG) $(SB_VENDOR_BINARIES_DIR)/$(SB_VENDOR_TARGET)
endif

release: build post_build $(SB_VENDOR_BREAKPAD_ARCHIVE) copy_symbols strip_build
ifneq (,$(SB_VENDOR_BUILD_LOG))
	-$(CP) $(SB_VENDOR_BUILD_LOG) $(SB_VENDOR_BINARIES_DIR)/$(SB_VENDOR_TARGET)
endif

regen-makefiles: setup_environment
ifeq (linux-i686,$(SB_TARGET_ARCH))
   ifneq (,$(filter $(SB_REGEN_MAKEFILE_PKGS),$(SB_VENDOR_TARGET)))
      ifeq (,$(filter gst%,$(SB_VENDOR_TARGET)))
	      $(MKDIR) common/m4
      endif
	   $(CP) $(SB_VENDOR_BINARIES_DIR)/libtool/release/share/aclocal/* $(CURDIR)/common/m4
	   @echo This command may fail. This is apparently OK.
	   -./autogen.sh
	   @echo Attempting to fix up libtool symlinks...
	   @for l in $$(find $(CURDIR) -type l); do \
         f=$$(readlink $$l); \
         echo $(RM) $$l; \
         $(RM) $$l; \
         echo $(CP) $$f $(CURDIR)/$$(basename $$l); \
         $(CP) $$f $(CURDIR)/$$(basename $$l); \
      done
	   @echo Regenerated $(SB_VENDOR_TARGET) makefiles are ready to check in
	   @echo NOTICE: beware newly generated files which may have to be svn added:
	   @echo 
	   @$(SVN) stat | $(GREP) ^? | $(AWK) '{print $$2}' | $(GREP) -v ^common | $(GREP) -v ^m4 | $(GREP) -v autom4te.cache | $(GREP) -v autoregen.sh | $(GREP) -v stamp-h.in
   else
	   @echo This package does not require makfiles to be regenerated. Doing nothing.
   endif
else
	@echo The regen-makefiles target can only be run on linux-i686.
endif

# We strip .so's on Linux *and* Mac because libtool gets confused and on the
# mac, generates some libraries with the .so extension.
strip_build:
ifdef NO_STRIP
   $(info Skipping strip_build step...)
else
   ifneq (Msys,$(SB_VENDOR_ARCH))
      ifeq (Darwin,$(SB_VENDOR_ARCH))
	      for d in $(SB_CONFIGURE_PREFIX); do \
           $(FIND) $$d \
          -type f -name "*.dylib" \
          -exec $(STRIP) {} \; ; \
          done
      endif
	      for d in $(SB_CONFIGURE_PREFIX); do \
           $(FIND) $$d \
          -type f -name "*.so*" \
          -exec $(STRIP) {} \; ; \
          done
   endif
endif

# post_build's heroic (but byzantine) shell loop needs some explanation;
# the goal here is to run two command on .dylib's that get spit out of libtool's
# bowels on the mac:
#  1. install_name_tool -id, which seemingly gives libraries an internal name
#
#  2. install_name_tool -change, which allows you to rename references to other
#     .dylibs which contain hardcoded paths to .dylibs on a filesystem, which
#     are pretty much guaranteed to not exist on someone's machine.
#
# This shell loop uses find to find all the .dylibs in the final install
# directory after we've finished building; the first thing we do is give the
# library its name, which we obtain by basenaming it.
#
# Then, we run otool on each library to give us a list of .dylibs of *OURS*
# it depends on (that's what the grep is about); we run it through awk before
# we call basename because the output of otool includes a ":", because there's
# a string which indicates the version of the .dylib). Finally, we loop through
# these referenced libraries, and rename them within the library that we just
# ran -id on.
#
# Simple, no?
#

post_build: module_post_build
ifneq (,$(filter Darwin Linux, $(SB_VENDOR_ARCH)))
	@echo Slaying libtool .la files on real platforms...
	$(FIND) $(SB_CONFIGURE_PREFIX) -type f -name '*.la' -exec $(RM) -fv {} \;
endif
ifeq (Darwin,$(SB_VENDOR_ARCH))
ifneq (,$(SB_VENDOR_TARGET_DYLIB_FIXUPS))
	@echo On the prowl for the following .dylib and .so external references:
	@echo    $(SB_VENDOR_TARGET_DYLIB_FIXUPS)
	@echo
	@echo Fixing up .dylib -ids
	@for l in `$(FIND) $(SB_CONFIGURE_PREFIX) -type f -name "*.dylib" \
          -o -name "*.so"`; do \
          for fix in $(SB_VENDOR_TARGET_DYLIB_FIXUPS); do \
            if ! test -z `echo $$l | grep $$fix`; then \
              echo --\> $(INSTALL_NAME_TOOL) -id $${fix}.dylib $$l; \
              $(INSTALL_NAME_TOOL) -id $${fix}.dylib $$l; \
              break; \
            fi; \
          done; \
        done

	@echo Fixing up .dylib reference paths
	@for l in `$(FIND) $(SB_CONFIGURE_PREFIX) -type f -name "*.dylib" \
          -o -name "*.so"`; do \
          echo Checking $$l...; \
          for path in `$(OTOOL) -L $$l | perl -nle 'print $1 if (m#\s+($(SB_CONFIGURE_PREFIX)\S+)\s+#);'`; do \
            for fix in $(SB_VENDOR_TARGET_DYLIB_FIXUPS); do \
              if ! test -z `echo $$path | grep $$fix`; then \
                echo --\> $(INSTALL_NAME_TOOL) -change $$path $${fix}.dylib $$l; \
                $(INSTALL_NAME_TOOL) -change $$path $${fix}.dylib $$l; \
                break; \
              fi; \
            done; \
          done; \
        done
endif
endif

#
# This heroic (but byzantine) shell loop handles copying the .pdb files, 
# so it's obviously windows only.
# 
# It does this by finding all .pdb files in the breakpad directory, strips
# off the directory name and the .pdb extension, and then looks at where
# we installed the package for, in order, a file with .exe, .dll, or .lib
# with the same name. If we find one, we copy the .pdb into that direcotry.
# The ORDERING OF THE EXTENSIONS MATTERS (and was chosen purposefully); this
# is because we only copy the .pdb ONCE, so if we find a .pdb with the same
# name, we'll copy it into the directory with the .exe first.
#

copy_symbols:
ifeq (Msys,$(SB_VENDOR_ARCH))
	@echo Gathering PDBs to add to vendor-binary tree...
	for dbug in $$($(FIND) $(SB_VENDOR_BUILD_DIR) -type f \
         -iname '*.pdb' \
         -not -iregex 'vc.*.pdb'); do \
           a=$$(basename $$dbug); \
	   b=$${a/%.???/}; \
           copied=0; \
           for ext in exe dll lib; do \
              for f in $$($(FIND) $(SB_CONFIGURE_PREFIX) -type f \
               -name $$b.$$ext); do \
                    $(CP) $$dbug $$(dirname $$f); \
                    copied=1; \
                    break; \
              done; \
              if test "$$copied" = "1"; then \
                 break; \
              fi; \
           done; \
        done
endif

upload_symbols: $(SB_VENDOR_BREAKPAD_ARCHIVE)
	$(SYMBOL_UPLOADER) $(SB_VENDOR_BREAKPAD_ARCHIVE)

$(SB_VENDOR_BREAKPAD_ARCHIVE):
ifeq (Msys,$(SB_VENDOR_ARCH))
	$(MKDIR) $(SB_VENDOR_BREAKPAD_SYMBOL_PATH)
	@echo Gathering PDBs, EXEs, LIBs, and DLLs for Breakpad consumption...
	for f in $$($(FIND) $(SB_VENDOR_BUILD_DIR) \
	            -false $(foreach ext,pdb exe dll,-o -iname '*.$(ext)')); do \
	   a=$$(basename $$f); \
	   a=$${a/%.???/.pdb}; \
	   $(MKDIR) -v $(SB_VENDOR_BREAKPAD_SYMBOL_PATH)/$$a; \
	   $(CP) $$f $(SB_VENDOR_BREAKPAD_DIR)/; \
        done
endif
	$(MKDIR) $(SB_VENDOR_BREAKPAD_SYMBOL_PATH)
	$(PYTHON) $(MOZSDK_SCRIPTS_DIR)/symbolstore.py \
	   $(DUMP_SYMS_ARGS) \
	   -s $(SB_CONFIGURE_PREFIX) \
	   $(DUMP_SYMS) \
	   $(SB_VENDOR_BREAKPAD_SYMBOL_PATH) \
	   $(SB_VENDOR_BREAKPAD_DIR) \
	   $(SB_VENDOR_BREAKPAD_STORE_PATH) \
	   > $(SB_VENDOR_BREAKPAD_SYMBOL_PATH)/$(SB_VENDOR_TARGET)-$(SB_VENDOR_TARGET_VERSION)-$(SB_TARGET_ARCH)-$(SB_BUILD_TYPE)-symbol-list.txt
	$(FIND) $(SB_VENDOR_BREAKPAD_SYMBOL_PATH) -type d -exec $(CHMOD) 0755 {} \; 
	$(FIND) $(SB_VENDOR_BREAKPAD_SYMBOL_PATH) -type f -exec $(CHMOD) 0644 {} \; 
	cd $(SB_VENDOR_BREAKPAD_SYMBOL_PATH) && \
	   $(ZIP) -r9D $(SB_VENDOR_BREAKPAD_ARCHIVE) .

$(SB_VENDOR_BINARIES_DIR):
	$(MKDIR) $(SB_VENDOR_BINARIES_DIR)

setup_environment: $(SB_VENDOR_BINARIES_DIR)
	$(MKDIR) $(SB_VENDOR_BUILD_ROOT)/build
ifeq (Msys,$(SB_VENDOR_ARCH))
	$(foreach tgt, \
	  $(SB_VENDOR_BINARIES_TARGETS), \
	  $(if $(wildcard $(SB_VENDOR_BINARIES_DIR)/$(tgt)),, \
	       $(MSYS_CP) $(SB_VENDOR_BINARIES_CHECKOUT)/$(tgt) \
	             $(SB_VENDOR_BINARIES_DIR)/$(tgt) --exclude=.svn && \
	             $(MKDIR) $(SB_VENDOR_BINARIES_DIR)/$(tgt)/.msyscp ; ))
	@echo Fixing up libtools .la files for first-time use...
	$(FIND) $(SB_VENDOR_BINARIES_DIR)/ -type f -name '*.la' -exec $(SB_VENDOR_CHECKOUT)/fix-win32-libtool-la-paths.pl -f {} \;
else
	$(foreach tgt, \
	  $(SB_VENDOR_BINARIES_TARGETS), \
	  $(if $(wildcard $(SB_VENDOR_BINARIES_DIR)/$(tgt)),, \
	       $(LN) -sv $(SB_VENDOR_BINARIES_CHECKOUT)/$(tgt) \
	            $(SB_VENDOR_BINARIES_DIR); ))
endif
ifeq (Msys,$(SB_VENDOR_ARCH))
	(test -e $(SB_VENDOR_BINARIES_DIR)/$(SB_VENDOR_TARGET)/.msyscp && \
          $(RM) -rf $(SB_VENDOR_BINARIES_DIR)/$(SB_VENDOR_TARGET) && \
          $(MKDIR) $(SB_VENDOR_BINARIES_DIR)/$(SB_VENDOR_TARGET)) || true
else
	(test -h $(SB_VENDOR_BINARIES_DIR)/$(SB_VENDOR_TARGET) && \
          $(RM) -v $(SB_VENDOR_BINARIES_DIR)/$(SB_VENDOR_TARGET) && \
          $(MKDIR) $(SB_VENDOR_BINARIES_DIR)/$(SB_VENDOR_TARGET)) || true
endif

build: setup_environment clean_build_dir setup_build module_setup_build
	# We do this RUN_CONFIGURE insanity to support cmake
ifeq (1,$(SB_RUN_CONFIGURE))
	cd $(SB_VENDOR_BUILD_DIR) && \
          $(CONFIGURE) --prefix=$(SB_CONFIGURE_PREFIX) \
          $(SB_VENDOR_TARGET_CONFIGURE_OPTS) \
          $(SB_CONFIGURE_OPTS) \
          -C
endif
	# We do this submake-cmd insanity to support cmake
	$(SUBMAKE_CMD) -C $(SB_VENDOR_BUILD_DIR)
	$(SUBMAKE_CMD) -C $(SB_VENDOR_BUILD_DIR) install

$(SB_VENDOR_TARGET_BINARY_DEPS_DIR): $(SB_VENDOR_TARGET_DEPENDENT_DEBS)
	$(MKDIR) $(SB_VENDOR_TARGET_BINARY_DEPS_DIR)
	$(foreach deb, \
          $(SB_VENDOR_TARGET_DEPENDENT_DEBS), \
          cd $(SB_VENDOR_TARGET_BINARY_DEPS_DIR) && \
          $(AR) -x $(deb) && \
          $(TAR) xfvz data.tar.gz ;)

setup_build: \
 $(if $(SB_VENDOR_TARGET_DEPENDENT_DEBS), $(SB_VENDOR_TARGET_BINARY_DEPS_DIR),)
	@echo 
	@echo 
	@echo ====================================
	@echo Songbird Vendor Environment Settings
	@echo ====================================
	@echo Tools
	@echo -----
	@echo CC = "$(CC)"
	@echo CXX = "$(CXX)"
	@echo LD = "$(LD)"
	@echo OBJDUMP = "$(OBJDUMP)"
	@echo STRIP = "$(STRIP)"
	@echo
	@echo Flags
	@echo -----
	@echo CPPFLAGS = $(CPPFLAGS)
	@echo CFLAGS = $(CFLAGS)
	@echo CXXFLAGS = $(CXXFLAGS)
	@echo LDFLAGS = $(LDFLAGS)
	@echo ACLOCAL_FLAGS = $(ACLOCAL_FLAGS)
	@echo
	@echo Paths
	@echo -----
	@echo PKG_CONFIG_PATH = $(PKG_CONFIG_PATH)
	@echo PATH = $(PATH)
ifeq (Msys,$(SB_VENDOR_ARCH))
	@echo LIBS = $(LIBS)
endif
	@echo
	@echo Dependent library settings
	@echo --------------------------
	@echo LIBOIL_CFLAGS = $(LIBOIL_CFLAGS)
	@echo LIBOIL_LIBS = $(LIBOIL_LIBS)
	@echo OGG_CFLAGS = $(OGG_CFLAGS)
	@echo OGG_LIBS = $(OGG_LIBS)
	@echo FLAC_CFLAGS = $(FLAC_CFLAGS)
	@echo FLAC_LIBS = $(FLAC_LIBS)
	@echo
	@echo Platform- and Module- specific settings
	@echo ---------------------------------------
ifeq (Darwin,$(SB_VENDOR_ARCH))
	@echo MACOSX_DEPLOYMENT_TARGET = $(MACOSX_DEPLOYMENT_TARGET)
	@echo DYLD_LIBRARY_PATH = $(DYLD_LIBRARY_PATH)
endif
ifneq (,$(SB_MODULE_EXPORTS))
	@$(foreach mod_exp, \
          $(strip $(SB_MODULE_EXPORTS)), \
          echo $(mod_exp) = $$$(mod_exp) ;)
endif
	@echo
	@echo =============================
	@echo END Environment Settings DUMP
	@echo =============================
	@echo 
	@echo 
	$(MKDIR) $(SB_VENDOR_BUILD_DIR)
	# TODO: this kinda sucks; fix this; also, now it sucks with the msys
	# stuff in a cross-platform way.
	$(MSYS_CP) $(SB_TARGET_SRC_DIR) $(SB_VENDOR_BUILD_DIR) --exclude=.svn

clean_build_dir:
	$(RM) -rf $(SB_VENDOR_BUILD_DIR)
	$(RM) -rf $(SB_VENDOR_TARGET_BINARY_DEPS_DIR)
	$(RM) -rf $(SB_VENDOR_BREAKPAD_DIR)
	$(RM) -rf $(SB_CONFIGURE_PREFIX)

.PHONY: FORCE all release debug build setup_build setup_environment clean_build_dir post_build strip_build module_setup_build module_post_build copy_symbols upload_symbols regen-makefiles
