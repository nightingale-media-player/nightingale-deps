#!/bin/bash

# XUL Version
export XUL="9.0.1"

# Top level build path
export SB_VENDOR_BUILD_ROOT="$(cd "$(dirname "$0")" && pwd)"


cd xulrunner

# rebuild
make xr-packaging

cp -a "oldscripts/*" "linux-$ARCH/mozilla-$XUL/debug/scripts/"
cp -a "oldscripts/*" "linux-$ARCH/mozilla-$XUL/release/scripts/"

