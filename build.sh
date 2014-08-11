#!/bin/bash

set -e

export DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export SB_VENDOR_BINARIES_CO_ROOT=$DIR
export SB_VENDOR_BUILD_ROOT=$DIR

# Currently, we build both debug and release, which takes a lot longer,
# especially on xulrunner... TODO: make it optional to build debug

if [ ! -d "build" ]; then
    mkdir "build"
fi

case $OSTYPE in
    # linux is easy, as all it requires right now is xulrunner, sqlite, and taglib
    # we'll get to a point where this is unnecessary on linux altogether in the future
    linux*)
        export CXXFLAGS="-fpermissive"
        export CFLAGS=$CXXFLAGS
        export CCFLAGS=$CXXFLAGS

        if [ ! -d "linux-$(uname -m)" ]; then
            mkdir -p "linux-$(uname -m)"
        fi
        if [ ! -d "checkout/linux-$(uname -m)" ]; then
            mkdir -p "checkout/linux-$(uname -m)"
        fi

        echo -e "Building xulrunner 1.9.2..."
        make -C xulrunner-1.9.2 -f Makefile.songbird xr-all

        echo -e "Building sqlite..."
        make -C sqlite -f Makefile.songbird

        echo -e "Building taglib..."
        make -C taglib -f Makefile.songbird

        echo -e "Done! Provided there were no errors, you can \nfind your deps in the linux-$(uname -m) directory. Copy or link it into [nightingale build directory]/dependencies and you're ready to build!\n"
    ;;

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

        echo -e "Building gettext..."
        make CC=gcc CXX=g++ -C gettext -f Makefile.songbird
        echo -e "Building glib..."
        make CC=gcc CXX=g++ -C glib -f Makefile.songbird
        echo -e "Building libidl..."
        make CC=gcc CXX=g++ -C libidl -f Makefile.songbird

        echo -e "Building xulrunner and crossing our fingers..."
        make CC=gcc-4.2 CXX=g++-4.2 -C xulrunner-1.9.2 -f Makefile.songbird xr-all

        echo -e "Building flac..."
        make CC=gcc CXX=g++ -C flac -f Makefile.songbird
        echo -e "Building libjpeg..."
        make CC=gcc CXX=g++ -C libjpeg -f Makefile.songbird
        echo -e "Building libogg..."
        make CC=gcc CXX=g++ -C libogg -f Makefile.songbird
        echo -e "Building libtheora..."
        make CC=gcc CXX=g++ -C libtheora -f Makefile.songbird
        echo -e "Building libtool..."
        make CC=gcc CXX=g++ -C libtool -f Makefile.songbird
        echo -e "Building libvorbis..."        
        make CC=gcc CXX=g++ -C libvorbis -f Makefile.songbird
        echo -e "Building sqlite..."
        make CC=gcc CXX=g++ -C sqlite -f Makefile.songbird
        echo -e "Building taglib..."        
        make CC=gcc CXX=g++ -C taglib -f Makefile.songbird

        echo -e "Building gstreamer bits..."
        make CC=gcc-4.2 CXX=g++-4.2 -C gstreamer -f Makefile.songbird
        echo -e "Building gst plugins...here's hoping these all build!"
        make CC=gcc-4.2 CXX=g++-4.2 -C gst-plugins-base -f Makefile.songbird
        make CC=gcc-4.2 CXX=g++-4.2 -C gst-plugins-good -f Makefile.songbird
        make CC=gcc-4.2 CXX=g++-4.2 -C gst-plugins-bad -f Makefile.songbird
        make CC=gcc-4.2 CXX=g++-4.2 -C gst-plugins-ugly -f Makefile.songbird
        echo "Done!"

    ;;

    msys*)
        if [ ! -d "windows-i686-msvc8" ]; then
            mkdir -p "windows-i686-msvc8"
        fi
        if [ ! -d "checkout/windows-i686-msvc8" ]; then
            mkdir -p "checkout/windows-i686-msvc8"
        fi

        WIN_OUTPUT_DIR="$DIR/windows-i686-msvc8"
        WIN_PB_DIR="$DIR/prebuilt/win32"

        export DIRECTX_SDK_ROOT="/c/SDKs/DXSDK_Jun08"
        export QUICKTIME_SDK_ROOT="/c/SDKs/QuickTime-SDK"
        export WINDOWS_SDK_ROOT="/c/SDKs/Windows-SDK/v6.0"


        echo -e "Extracting prebuilt libiconv package..."
        pushd . > /dev/null
        cd $WIN_PB_DIR
        unzip libiconv-1.9.1-win32.zip
        mv libiconv-1.9.1-win32 $WIN_OUTPUT_DIR/libiconv
        popd > /dev/null

        echo -e "Building gettext..."
        make -C gettext-win -f Makefile.songbird

        echo -e "Extracting prebuilt glib package..."
        pushd . > /dev/null
        cd $WIN_PB_DIR
        unzip glib-2.34.3-win32.zip
        mv glib-2.34.3-win32 $WIN_OUTPUT_DIR/glib
        popd > /dev/null

        # Need some scripts from XR to generate symbols in other deps,
        # so build XR here
        echo -e "Building xulrunner 1.9.2..."
        make -C xulrunner-1.9.2 -f Makefile.songbird xr-all

        echo -e "Building zlib..."
        make -C zlib -f Makefile.songbird
        echo -e "Building sqlite..."
        make -C sqlite -f Makefile.songbird
        echo -e "Building tablib..."
        make -C taglib -f Makefile.songbird
        echo -e "Building libjpeg-turbo..."
        make -C libjpeg-turbo -f Makefile.songbird
        echo -e "Building flac..."
        make -C flac-win -f Makefile.songbird

        echo -e "Building libogg..."
        make -C libogg -f Makefile.songbird
        echo -e "Building libvorbis..."
        make -C libvorbis -f Makefile.songbird
        echo -e "Building libtheora..."
        make -C libtheora -f Makefile.songbird

        echo -e "Extracting prebuilt bins package..."
        pushd . > /dev/null
        cd $WIN_PB_DIR
        unzip win32-bins-20140810.zip
        popd > /dev/null
        echo -e "Adding bins to PATH..."
        WIN_PB_BINS_DIR=$WIN_PB_DIR/win32-bins-20140810
        export PATH=$WIN_PB_BINS_DIR:$PATH

        echo -e "Building gstreamer..."
        make -C gstreamer -f Makefile.songbird

        echo -e "Building gst-plugins-base..."
        make -C gst-plugins-base -f Makefile.songbird
        echo -e "Building gst-plugins-good..."
        make -C gst-plugins-good -f Makefile.songbird
        echo -e "Building gst-plugins-bad..."
        make -C gst-plugins-bad -f Makefile.songbird
        echo -e "Building gst-plugins-ugly..."
        make -C gst-plugins-ugly -f Makefile.songbird

        echo "Done!"
    ;;
    *)
      echo "Lazy buildscript for your OS coming soon."
    ;;
esac
