#!/bin/bash

set -e

export DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export SB_VENDOR_BINARIES_CO_ROOT=$DIR
export SB_VENDOR_BUILD_ROOT=$DIR

export LINT_BUILD=1

if [ ! -d "build" ]; then
    mkdir build
fi

case $OSTYPE in
    linux*)
        if [ "$LINT_BUILD" -eq "1" ] ; then
            # hardening flags
            export CFLAGS="-fstack-protector --param=ssp-buffer-size=4"
            export CXXFLAGS="-D_FORTIFY_SOURCE=2"
            export LDFLAGS="-Wl,-z,now -Wl,-z,relro"
        fi

        export SB_CFLAGS=$CFLAGS
        export SB_CCFLAGS=$CFLAGS
        export SB_CXXFLAGS=$CXXFLAGS
        export SB_CPPFLAGS=$CXXFLAGS

        export SB_LDFLAGS=$LDFLAGS

        if [ "$DIST_NAME_BINARIES_DIR" -eq "1" ] ; then
            if [ ! -d "dist/linux-$(uname -m)" ]; then
                mkdir -p "dist/linux-$(uname -m)"
            fi
        fi
        if [ ! -d "linux-$(uname -m)" ]; then
            mkdir -p "linux-$(uname -m)"
        fi
        if [ ! -d "checkout/linux-$(uname -m)" ]; then
            mkdir -p "checkout/linux-$(uname -m)"
        fi

        echo -e "Building xulrunner 1.9.2...\n"
        make -C xulrunner-1.9.2 -f Makefile.songbird xr-all

        echo -e "Building sqlite...\n"
        make -C sqlite -f Makefile.songbird

        echo -e "Building taglib...\n"
        make -C taglib -f Makefile.songbird

    ;;
    *)
        echo "Hardening isn't relevant for you."
    ;;
esac