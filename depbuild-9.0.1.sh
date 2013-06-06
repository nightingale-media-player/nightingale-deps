#!/bin/bash

ARCH="$(uname -m)"

# XUL Version
export XUL="9.0.1"

# Top level build path
export SB_VENDOR_BUILD_ROOT="$(cd "$(dirname "$0")" && pwd)"

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

cp -a "oldscripts/*" "linux-$ARCH/mozilla-$XUL/debug/scripts/"
cp -a "oldscripts/*" "linux-$ARCH/mozilla-$XUL/release/scripts/"

