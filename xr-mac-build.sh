#!/bin/bash

set -e

export DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export SB_VENDOR_BINARIES_CO_ROOT=$DIR
export SB_VENDOR_BUILD_ROOT=$DIR

# Currently, we build both debug and release, which takes a lot longer,
# especially on xulrunner... TODO: make it optional to build debug

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

        # echo -e "Building gettext..."
        # make CC=gcc CXX=g++ -C gettext -f Makefile.songbird
        # echo -e "Building glib..."
        # make CC=gcc CXX=g++ -C glib -f Makefile.songbird
        # echo -e "Building libidl..."
        # make CC=gcc CXX=g++ -C libIDL -f Makefile.songbird

        echo -e "Building xulrunner and crossing our fingers..."
        make CC=gcc-4.2 CXX=g++-4.2 -C xulrunner-1.9.2 -f Makefile.songbird xr-all

    ;;
    *)
        echo "This script isn't for you"
    ;;
esac
