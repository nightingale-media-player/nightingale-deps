#!/bin/bash

ARCH="$(uname -m)"

# XUL Version
export XUL="9.0.1"

export DIR="$(cd "$(dirname "$0")" && pwd)"
export SB_VENDOR_BINARIES_CO_ROOT=$DIR
export SB_VENDOR_BUILD_ROOT=$DIR
export SB_TARGET_ARCH="linux-i686"

mkdir -p "checkout/linux-$ARCH"

# output directory
mkdir -p "linux-$ARCH"

rm -rf build
mkdir build

cd xulrunner
# fix for kernels > 3.X on versions of xul without security setup for them
case $OSTYPE in
    linux*)
        if [ ! -f mozilla/security/coreconf/Linux$(uname -r|sed -e 's/\-.*//'|grep -o "[0-9]\.[0-9]").mk ]; then
            ln -s $(pwd)/mozilla/security/coreconf/Linux2.6.mk $(pwd)/mozilla/security/coreconf/Linux$(uname -r|sed -e 's/\-.*//'|grep -o "[0-9]\.[0-9]").mk
        fi
    ;;
    *)
        # weee! xulrunner already has security coreconf for our kernel!
    ;;
esac
cd ../

echo -e "Building xulrunner...\n"
make -C xulrunner xr-all

echo -e "Building sqlite...\n"
make -C sqlite -f Makefile.songbird

echo -e "Building taglib...\n"
make -C taglib -f Makefile.songbird

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
