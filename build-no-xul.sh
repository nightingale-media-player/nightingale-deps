#!/bin/bash

ARCH="$(uname -m)"

# XUL Version
export XUL="9.0.1"

export DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export SB_VENDOR_BINARIES_CO_ROOT=$DIR
export SB_VENDOR_BUILD_ROOT=$DIR

if [ ! -d "build" ]; then
    mkdir build
fi

case $OSTYPE in
    linux*)
        if [ ! -d "linux-$(uname -m)" ]; then
            mkdir -p "linux-$(uname -m)"
        fi
        if [ ! -d "checkout/linux-$(uname -m)" ]; then
            mkdir -p "checkout/linux-$(uname -m)"
        fi

        echo -e "Building libtool...\n"
        make -C libtool -f Makefile.nightingale

        echo -e "Building libiconv...\n"
        make -C libiconv -f Makefile.nightingale

        echo -e "Building libjpeg-turbo...\n"
        make -C libjpeg-turbo -f Makefile.nightingale

        ### TIER 1 ###
        echo -e "Building gettext...\n"
        make -C gettext -f Makefile.nightingale

        echo -e "Building glib...\n"
        make -C glib -f Makefile.nightingale

        echo -e "Building sqlite...\n"
        make -C sqlite -f Makefile.nightingale

        ### TIER 2 ###
        echo -e "Building libogg...\n"
        make -C libogg -f Makefile.nightingale

        echo -e "Building libvorbis...\n"
        make -C libvorbis -f Makefile.nightingale

        echo -e "Building libtheora...\n"
        make -C libtheora -f Makefile.nightingale

        echo -e "Building FLAC...\n"
        make -C flac -f Makefile.nightingale

        echo -e "Building taglib...\n"
        make -C taglib -f Makefile.nightingale

        ### TIER 3 ###
        echo -e "Building gstreamer...\n"
        make -C gstreamer -f Makefile.nightingale

        ### TIER 4 ###
        echo -e "Building gst plugins...\n"
        make -C gst-plugins-base -f Makefile.nightingale
        make -C gst-plugins-good -f Makefile.nightingale
        make -C gst-plugins-bad -f Makefile.nightingale
        make -C gst-plugins-ugly -f Makefile.nightingale
    ;;

    # darwin*)
    #     # on OSX, we want 32 bit builds
    #     arch_flags="-m32 -arch i386"
    #     export CFLAGS="$arch_flags"
    #     export CXXFLAGS="$arch_flags"
    #     export CPPFLAGS="$arch_flags"
    #     export LDFLAGS="$arch_flags"
    #     export OBJCFLAGS="$arch_flags"

    #     if [ ! -d "macosx-i686" ]; then
    #         mkdir -p "macosx-i686"
    #         mkdir -p "checkout/macosx-i686"
    #     fi

    #     echo -e "Building gstreamer bits..."
    #     make CC=gcc-4.2 CXX=g++-4.2 -C gstreamer -f Makefile.nightingale
    #     echo -e "Building gst plugins...here's hoping these all build!"
    #     make CC=gcc-4.2 CXX=g++-4.2 -C gst-plugins-base -f Makefile.nightingale
    #     make CC=gcc-4.2 CXX=g++-4.2 -C gst-plugins-good -f Makefile.nightingale
    #     make CC=gcc-4.2 CXX=g++-4.2 -C gst-plugins-bad -f Makefile.nightingale
    #     make CC=gcc-4.2 CXX=g++-4.2 -C gst-plugins-ugly -f Makefile.nightingale
    #     echo "Done!"
    # ;;

    msys*)
        if [ ! -d "windows-i686-msvc10" ]; then
            mkdir -p "windows-i686-msvc10"
        fi
        if [ ! -d "checkout/windows-i686-msvc10" ]; then
            mkdir -p "checkout/windows-i686-msvc10"
        fi

        # echo -e "Building glib...\n"
        # make -C glib -f Makefile.nightingale

        echo -e "Building gettext...\n"
        make -C gettext -f Makefile.nightingale

        echo -e "Building FLAC...\n"
        make -C flac -f Makefile.nightingale

        # echo -e "Building libjpeg-turbo...\n"
        # make -C libjpeg-turbo -f Makefile.nightingale

        # echo -e "Building libogg...\n"
        # make -C libogg -f Makefile.nightingale

        # echo -e "Building libtheora...\n"
        # make -C libtheora -f Makefile.nightingale

        # echo -e "Building libtool...\n"
        # make -C libtool -f Makefile.nightingale

        # echo -e "Building libvorbis...\n"
        # make -C libvorbis -f Makefile.nightingale

        # echo -e "Building sqlite...\n"
        # make -C sqlite -f Makefile.nightingale

        # echo -e "Building taglib...\n"
        # make -C taglib -f Makefile.nightingale

        # echo -e "Building gstreamer...\n"
        # make -C gstreamer -f Makefile.nightingale

        # echo -e "Building gst plugins...\n"
        # make -C gst-plugins-base -f Makefile.nightingale
        # make -C gst-plugins-good -f Makefile.nightingale
        # make -C gst-plugins-bad -f Makefile.nightingale
        # make -C gst-plugins-ugly -f Makefile.nightingale
    ;;

    *)
        echo "Lazy buildscript for your OS coming soon."
    ;;

esac
