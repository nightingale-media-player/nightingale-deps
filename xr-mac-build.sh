#!/bin/bash

set -e

export DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export SB_VENDOR_BINARIES_CO_ROOT=$DIR
export SB_VENDOR_BUILD_ROOT=$DIR

export MACOSX_SDK="/Developer/SDKs/MacOSX10.6.sdk"

# rm -rf build
if [ ! -d build ] ; then
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
            mkdir -p "macosx-i686/mozilla-1.9.2/release/scripts"
            cp xulrunner-1.9.2/mozilla/toolkit/crashreporter/tools/symbolstore.py macosx-i686/mozilla-1.9.2/release/scripts
            mkdir -p "checkout/macosx-i686"
            if [ ! -d "build" ]; then
                mkdir "build"
            fi
        fi

        # echo -e "Building libtool..."
        # make CC=gcc-4.2 CXX=g++-4.2 -C libtool -f Makefile.songbird

        # echo -e "Building libiconv..."
        # make CC=gcc-4.2 CXX=g++-4.2 -C libiconv -f Makefile.songbird
        # echo -e "Building gettext..."
        # make CC=gcc-4.2 CXX=g++-4.2 -C gettext -f Makefile.songbird
        # echo -e "Building glib..."
        # make CC=gcc-4.2 CXX=g++-4.2 -C glib -f Makefile.songbird
        # echo -e "Building libidl..."
        # make CC=gcc-4.2 CXX=g++-4.2 -C libIDL -f Makefile.songbird

        # echo -e "Building flac..."
        # make CC=gcc-4.2 CXX=g++-4.2 -C flac -f Makefile.songbird
        # echo -e "Building libjpeg..."
        # make CC=gcc-4.2 CXX=g++-4.2 -C libjpeg -f Makefile.songbird

        # echo -e "Building libogg..."
        # make CC=gcc-4.2 CXX=g++-4.2 -C libogg -f Makefile.songbird
        # echo -e "Building libtheora..."
        # make CC=gcc-4.2 CXX=g++-4.2 -C libtheora -f Makefile.songbird
        # echo -e "Building libvorbis..."        
        # make CC=gcc-4.2 CXX=g++-4.2 -C libvorbis -f Makefile.songbird

        # echo -e "Building sqlite..."
        # make CC=gcc-4.2 CXX=g++-4.2 -C sqlite -f Makefile.songbird
        # echo -e "Building taglib..."        
        # make CC=gcc-4.2 CXX=g++-4.2 -C taglib -f Makefile.songbird

        # echo -e "Building gstreamer bits..."
        # make -C gstreamer -f Makefile.songbird

        # echo -e "Building gst plugins...here's hoping these all build!"
        # make -C gst-plugins-base -f Makefile.songbird
        # make -C gst-plugins-good -f Makefile.songbird
        # make -C gst-plugins-bad -f Makefile.songbird
        # make -C gst-plugins-ugly -f Makefile.songbird

        # echo -e "Building xulrunner and crossing our fingers..."
        # make -C xulrunner-1.9.2 -f Makefile.songbird xr-all
        ### make -C xulrunner-1.9.2 -f Makefile.songbird xr-symbols
        ### make -C xulrunner-1.9.2 -f Makefile.songbird xr-packaging
    ;;
    *)
        echo "This script isn't for you"
    ;;
esac
