#!/bin/bash

ARCH="$(uname -m)"

# XUL Version
export XUL="15.0.1"

export DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export SB_VENDOR_BINARIES_CO_ROOT=$DIR
export SB_VENDOR_BUILD_ROOT=$DIR

# rm -rf build
# mkdir build
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

        cd "xulrunner"
        # fix for kernels > 3.X on versions of xul without security setup for them
        if [ ! -f mozilla/security/coreconf/Linux$(uname -r|sed -e 's/\-.*//'|grep -o "[0-9]\.[0-9]").mk ]; then
            ln -s $(pwd)/mozilla/security/coreconf/Linux2.6.mk $(pwd)/mozilla/security/coreconf/Linux$(uname -r|sed -e 's/\-.*//'|grep -o "[0-9]\.[0-9]").mk
        fi
        cd ../

        echo -e "Building xulrunner...\n"
        make -C xulrunner xr-all

        if [ ! -d "linux-$ARCH/mozilla-$XUL/debug" ] ; then 
            cd "linux-$ARCH/mozilla-$XUL"
            mkdir debug
            mv bin frozen idl include lib scripts debug
            cd ../../
        fi

        if [ ! -d "linux-$ARCH/xulrunner-$XUL/debug" ] ; then
            cd "linux-$ARCH/xulrunner-$XUL"
            mkdir debug
            mv xulrunner.tar.bz debug
            cd ../../
        fi
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
            mkdir -p "checkout/macosx-i686"
        fi

        echo -e "Building sqlite..."
        make CC=gcc CXX=g++ -C sqlite -f Makefile.songbird
        echo -e "Building taglib..."        
        make CC=gcc CXX=g++ -C taglib -f Makefile.songbird

        echo -e "Building xulrunner and crossing our fingers..."
        make CC=gcc-4.2 CXX=g++-4.2 -C xulrunner -f Makefile.songbird xr-all
        echo "Done!"
    ;;

    msys*)
        if [ ! -d "windows-i686-msvc10" ]; then
            mkdir -p "windows-i686-msvc10"
        fi
        if [ ! -d "checkout/windows-i686-msvc10" ]; then
            mkdir -p "checkout/windows-i686-msvc10"
        fi

        echo -e "Building xulrunner...\n\n"
        make -C xulrunner xr-all

        # echo -e "\nBuilding glib...\n\n"
        # make -C glib -f Makefile.songbird

        # echo -e "\nBuilding gettext...\n\n"
        # make -C gettext -f Makefile.songbird

        # echo -e "\nBuilding flac...\n\n"
        # make -C flac -f Makefile.songbird
    ;;

    *)
        echo "Lazy buildscript for your OS coming soon."
    ;;
esac


