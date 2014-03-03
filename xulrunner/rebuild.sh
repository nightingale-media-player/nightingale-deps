#!/bin/bash

# XUL Version
export XUL="9.0.1"

export ARCH="$(uname -m)"

# Top level build path
export SB_VENDOR_BUILD_ROOT="$(cd ../"$(dirname "$0")" && pwd)"

# rebuild
make xr-packaging

cd ..
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
