#!/bin/bash

ARCH="$(uname -m)"

# XUL Version
export XUL="15.0.1"

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

        echo -e "Building xulrunner...\n"
        make -C xulrunner xr-all
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
    ;;

    *)
        echo "Lazy buildscript for your OS coming soon."
    ;;
esac
