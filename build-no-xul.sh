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
            mkdir -p "checkout/linux-$(uname -m)"
        fi

        echo -e "Building FLAC...\n"
        make -C flac -f Makefile.songbird

        echo -e "Building libjpeg-turbo...\n"
        make -C libjpeg-turbo -f Makefile.songbird

        echo -e "Building libogg...\n"
        make -C libogg -f Makefile.songbird

        echo -e "Building libtheora...\n"
        make -C libtheora -f Makefile.songbird

        echo -e "Building libtool...\n"
        make -C libtool -f Makefile.songbird

        echo -e "Building libvorbis...\n"
        make -C libvorbis -f Makefile.songbird

        echo -e "Building sqlite...\n"
        make -C sqlite -f Makefile.songbird

        echo -e "Building taglib...\n"
        make -C taglib -f Makefile.songbird

        echo -e "Building gstreamer...\n"
        make -C gstreamer -f Makefile.songbird

        echo -e "Building gst plugins...\n"
        make -C gst-plugins-base -f Makefile.songbird
        make -C gst-plugins-good -f Makefile.songbird
        make -C gst-plugins-bad -f Makefile.songbird
        make -C gst-plugins-ugly -f Makefile.songbird
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
    #     make CC=gcc-4.2 CXX=g++-4.2 -C gstreamer -f Makefile.songbird
    #     echo -e "Building gst plugins...here's hoping these all build!"
    #     make CC=gcc-4.2 CXX=g++-4.2 -C gst-plugins-base -f Makefile.songbird
    #     make CC=gcc-4.2 CXX=g++-4.2 -C gst-plugins-good -f Makefile.songbird
    #     make CC=gcc-4.2 CXX=g++-4.2 -C gst-plugins-bad -f Makefile.songbird
    #     make CC=gcc-4.2 CXX=g++-4.2 -C gst-plugins-ugly -f Makefile.songbird
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
        # make -C glib -f Makefile.songbird

        echo -e "Building gettext...\n"
        make -C gettext -f Makefile.songbird

        echo -e "Building FLAC...\n"
        make -C flac -f Makefile.songbird

        # echo -e "Building libjpeg-turbo...\n"
        # make -C libjpeg-turbo -f Makefile.songbird

        # echo -e "Building libogg...\n"
        # make -C libogg -f Makefile.songbird

        # echo -e "Building libtheora...\n"
        # make -C libtheora -f Makefile.songbird

        # echo -e "Building libtool...\n"
        # make -C libtool -f Makefile.songbird

        # echo -e "Building libvorbis...\n"
        # make -C libvorbis -f Makefile.songbird

        # echo -e "Building sqlite...\n"
        # make -C sqlite -f Makefile.songbird

        # echo -e "Building taglib...\n"
        # make -C taglib -f Makefile.songbird

        # echo -e "Building gstreamer...\n"
        # make -C gstreamer -f Makefile.songbird

        # echo -e "Building gst plugins...\n"
        # make -C gst-plugins-base -f Makefile.songbird
        # make -C gst-plugins-good -f Makefile.songbird
        # make -C gst-plugins-bad -f Makefile.songbird
        # make -C gst-plugins-ugly -f Makefile.songbird
    ;;

    *)
        echo "Lazy buildscript for your OS coming soon."
    ;;

esac
