
# This is a *first stab* at a top-level Makefile; it's broken in a lot of
# icky ways, but it's better than the way we were doing it before (multiple
# broken icky build.sh scripts to build this stuff); see bug 13004 for more
# info...

SB_VENDOR_BUILD_LOG = build-log.txt
SB_VENDOR_BUILD_ROOT = $(CURDIR)

export SB_VENDOR_BUILD_LOG
export SB_VENDOR_BUILD_ROOT

TEE = tee

SB_VENDOR_WORLD = $(addprefix sb-, sqlite gst-plugins-good gst-plugins-bad gst-plugins-ugly taglib)

SB_VENDOR_BUILD_DEPS ?= 0

world: $(SB_VENDOR_WORLD)

TIER1 := sb-gettext sb-glib sb-sqlite
TIER2 := sb-libogg sb-libvorbis sb-libtheora sb-flac sb-taglib
TIER3 := sb-gstreamer
TIER4 := sb-gst-plugins-good sb-gst-plugins-bad sb-gst-plugins-ugly

# All borked; need to separate out platform detection stuff...
ifeq (macosx,$(SB_PLATFORM))
  GLIB_DEP = sb-glib
else
  GLIB_DEP = $(NULL)
endif
ifeq (windows,$(SB_PLATFORM))
  ZLIB_DEP = sb-zlib
  TIER1 += sb-zlib
else
  ZLIB_DEP = $(NULL)
endif

GLIB_DEP = $(error Set your GLIB_DEP)

ifneq (0,$(SB_VENDOR_BUILD_DEPS))
   sb-gettext: $(NULL)
   sb-glib: sb-gettext

   sb-libogg: $(NULL)

   sb-libvorbis: sb-libogg
   sb-libtheora: sb-libogg sb-libvorbis
   sb-flac: $(NULL)

   sb-gstreamer: $(GLIB_DEP)

   sb-gst-plugins-base: sb-gstreamer sb-libogg sb-libvorbis sb-libtheora

   sb-gst-plugins-good: sb-gst-plugins-base sb-flac
   sb-gst-plugins-bad: sb-gst-plugins-base
   sb-gst-plugins-ugly: sb-gst-plugins-base

   sb-taglib: $(ZLIB_DEP)
endif

SB_VENDOR_PACKAGE_DIR = $(CURDIR)/$(subst sb-,$(NULL),$@)

sb-%:
	@echo Making vendor package $@ in $(SB_VENDOR_PACKAGE_DIR)
	$(MAKE) -C $(SB_VENDOR_PACKAGE_DIR) -f Makefile.songbird all 2>&1 | $(TEE) $(SB_VENDOR_PACKAGE_DIR)/$(SB_VENDOR_BUILD_LOG)

clean:
	rm -rf build linux-i?86 linux-x86_64 macosx-i686 windows-i686-msvc8 xulrunner-1.9.2/mozilla/compiled

distclean: clean
	rm -rf checkout
	cd xulrunner-1.9.2/mozilla && rm -f configure .mozconfig .mozconfig.mk .mozconfig.out security/manager/.nss.checkout
	find . -type f -name \*.pyc -delete

.PHONY: world clean distclean
#$(add-prefix sb-, gettext libogg liboil libtheora gstreamer gst-plugins-base gst-plugins-good gst-plugins-bad gst-plugins-ugly)
