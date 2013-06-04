#!/bin/bash

# XUL Version
export XUL="9.0.1"

# Top level build path
export SB_VENDOR_BUILD_ROOT="$(cd "$(dirname "$0")" && pwd)"


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

# build Xulrunner
make xr-all

cp -a "oldscripts/*" "linux-$ARCH/mozilla-$XUL/debug/scripts/"
cp -a "oldscripts/*" "linux-$ARCH/mozilla-$XUL/release/scripts/"

