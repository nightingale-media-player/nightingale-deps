#!/bin/bash

if [ $# != 2 ]; then
  echo "usage: xr_pack_deps.sh <windows-i686-msvc8|linux-x86_64|linux-i686|macosx-ppc|macosx-i686>"
  echo "                       <trunk|SONGBIRD_0_2|SONGBIRD_0_3|SONGBIRD_0_4>"
  exit
fi

plat=$1
tree=$2

# create destination directories
mkdir -p songbird/$tree/dependencies/$plat/mozilla/debug/
mkdir -p songbird/$tree/dependencies/$plat/xulrunner/debug/
mkdir -p songbird/$tree/dependencies/$plat/mozilla/release/
mkdir -p songbird/$tree/dependencies/$plat/xulrunner/release/

# make debug mozilla sdk
./make-mozilla-sdk.sh mozilla/ \
                      mozilla/compiled/xulrunner-debug/ \
                      songbird/$tree/dependencies/$plat/mozilla/debug/

# make debug xulrunner tarball
./make-xulrunner-tarball.sh mozilla/compiled/xulrunner-debug/dist/bin/ \
                            songbird/$tree/dependencies/$plat/xulrunner/debug/ \
                            xulrunner.tar.bz2

# make release mozilla sdk
./make-mozilla-sdk.sh mozilla/ \
                      mozilla/compiled/xulrunner-release/ \
                      songbird/$tree/dependencies/$plat/mozilla/release/

# make release xulrunner tarball
./make-xulrunner-tarball.sh mozilla/compiled/xulrunner-release/dist/bin/ \
                            songbird/$tree/dependencies/$plat/xulrunner/release/ \
                            xulrunner.tar.bz2
