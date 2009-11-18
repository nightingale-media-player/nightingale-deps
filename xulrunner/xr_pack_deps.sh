#!/bin/bash

if [ $# != 3 ]; then
  echo "usage: xr_pack_deps.sh <windows-i686-msvc8|linux-x86_64|linux-i686|macosx-ppc|macosx-i686>"
  echo "                       <trunk|SONGBIRD_0_2|SONGBIRD_0_3|SONGBIRD_0_4>"
  echo "                       <release|debug>"
  exit
fi

plat=$1
tree=$2
buildMode=$3

if [ "$3" != "debug" ] && [ "$3" != "release" ]; then
  echo "Invalid build mode; must be 'debug' or 'release'."
  exit -1
fi

# create destination directories
mkdir -p songbird/$tree/dependencies/$plat/mozilla/$buildMode/
mkdir -p songbird/$tree/dependencies/$plat/xulrunner/$buildMode/

# make mozilla sdk
./make-mozilla-sdk.sh mozilla/ \
                      mozilla/compiled/xulrunner-$buildMode/ \
                      songbird/$tree/dependencies/$plat/mozilla/$buildMode/

# make xulrunner tarball
./make-xulrunner-tarball.sh mozilla/compiled/xulrunner-$buildMode/dist/bin/ \
                            songbird/$tree/dependencies/$plat/xulrunner/$buildMode/ \
                            xulrunner.tar.bz2
