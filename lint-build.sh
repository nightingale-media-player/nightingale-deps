#!/bin/bash

set -e

export DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export SB_VENDOR_BINARIES_CO_ROOT=$DIR
export SB_VENDOR_BUILD_ROOT=$DIR

if [ ! -d "build" ]; then
    mkdir build
fi

case $OSTYPE in
    linux*)
        # hardening flags
        export CFLAGS="-fstack-protector --param=ssp-buffer-size=4"
        export CXXFLAGS="-D_FORTIFY_SOURCE=2"
        export LDFLAGS="-Wl,-z,now -Wl,-z,relro"

        export SB_CFLAGS=$CFLAGS
        export SB_CCFLAGS=$CFLAGS
        export SB_CXXFLAGS=$CXXFLAGS
        export SB_CPPFLAGS=$CXXFLAGS

        export SB_LDFLAGS=$LDFLAGS

        if [ ! -d "linux-$(uname -m)" ]; then
            mkdir -p "linux-$(uname -m)"
        fi
        if [ ! -d "checkout/linux-$(uname -m)" ]; then
            mkdir -p "checkout/linux-$(uname -m)"
        fi

        cd "xulrunner-1.9.2"
        # fix for kernels > 3.X on versions of xul without security setup for them
        if [ ! -f mozilla/security/coreconf/Linux$(uname -r|sed -e 's/\-.*//'|grep -o "[0-9]\.[0-9]{1,2}").mk ]; then
            ln -s $(pwd)/mozilla/security/coreconf/Linux2.6.mk $(pwd)/mozilla/security/coreconf/Linux$(uname -r|sed -e 's/\-.*//'|grep -o "[0-9]\.[0-9]{1,2}").mk
        fi
        cd ../

        # echo -e "Building xulrunner 1.9.2...\n"
        # make -C xulrunner-1.9.2 -f Makefile.songbird xr-all
        # make -C xulrunner-1.9.2 -f Makefile.songbird xr-build-debug
        # make -C xulrunner-1.9.2 -f Makefile.songbird xr-build-release
        # make -C xulrunner-1.9.2 -f Makefile.songbird xr-symbols
        # make -C xulrunner-1.9.2 -f Makefile.songbird xr-packaging

        # echo -e "Building sqlite...\n"
        # make -C sqlite -f Makefile.songbird

        # echo -e "Building taglib...\n"
        # make -C taglib -f Makefile.songbird

    ;;
    *)
        echo "Hardening isn't relevant for you."
    ;;
esac
