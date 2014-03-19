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
    darwin*)
        # on OSX, we want 32 bit builds
        arch_flags="-m32 -arch i386"
        export CFLAGS="$arch_flags"
        export CXXFLAGS="$arch_flags"
        export CPPFLAGS="$arch_flags"
        export LDFLAGS="$arch_flags"
        export OBJCFLAGS="$arch_flags"

        if [ ! -d "macosx-i686" ]; then
            mkdir -p "macosx-i686"
        fi
        if [ ! -d "checkout/macosx-i686" ]; then
            mkdir -p "checkout/macosx-i686"
        fi


        #####

        # echo -e "Building libtool...\n"
        # make CC=gcc-4.2 CXX=g++-4.2 -C libtool -f Makefile.nightingale

        #####

        # echo -e "Building libiconv...\n"
        # make CC=gcc-4.2 CXX=g++-4.2 -C libiconv -f Makefile.nightingale

        # echo -e "Building gettext...\n"
        # make CC=gcc-4.2 CXX=g++-4.2 -C gettext -f Makefile.nightingale

        # echo -e "Building glib...\n"
        # make CC=gcc-4.2 CXX=g++-4.2 -C glib -f Makefile.nightingale

        # echo -e "Building libIDL...\n"
        # make CC=gcc-4.2 CXX=g++-4.2 -C libIDL -f Makefile.nightingale

        # echo -e "Building sqlite...\n"
        # make CC=gcc-4.2 CXX=g++-4.2 -C sqlite -f Makefile.nightingale

        #####

        # echo -e "Building libjpeg-turbo...\n"
        # make CC=gcc-4.2 CXX=g++-4.2 -C libjpeg-turbo -f Makefile.nightingale

        #####

        # echo -e "Building libogg...\n"
        # make CC=gcc-4.2 CXX=g++-4.2 -C libogg -f Makefile.nightingale

        # echo -e "Building libvorbis...\n"
        # make CC=gcc-4.2 CXX=g++-4.2 -C libvorbis -f Makefile.nightingale

        # echo -e "Building libtheora...\n"
        # make CC=gcc-4.2 CXX=g++-4.2 -C libtheora -f Makefile.nightingale

        # echo -e "Building flac...\n"
        # make CC=gcc-4.2 CXX=g++-4.2 -C flac -f Makefile.nightingale

        # echo -e "Building taglib...\n"
        # make CC=cc-4.2 CXX=c++-4.2 -C taglib -f Makefile.nightingale # CC and CXX aren't really needed here

        #####

        # echo -e "Building gstreamer...\n"
        # make CC=gcc-4.2 CXX=g++-4.2 -C gstreamer -f Makefile.nightingale

        #####

        # echo -e "Building gst-plugins-base...\n"
        # make CC=gcc-4.2 CXX=g++-4.2 -C gst-plugins-base -f Makefile.nightingale

        # echo -e "Building gst-plugins-good...\n"
        # make CC=gcc-4.2 CXX=g++-4.2 -C gst-plugins-good -f Makefile.nightingale

        # echo -e "Building gst-plugins-bad...\n"
        # make CC=gcc-4.2 CXX=g++-4.2 -C gst-plugins-bad -f Makefile.nightingale

        # echo -e "Building gst-plugins-ugly...\n"
        # make CC=gcc-4.2 CXX=g++-4.2 -C gst-plugins-ugly -f Makefile.nightingale

        #####

        # echo -e "Building xulrunner...\n"
        # make -C xulrunner xr-all

        #####

    ;;

    *)
        echo "Mac only build script!\n"
    ;;

esac

