#!/usr/bin/env bash

set -e

MKDIR=${MKDIR:-mkdir}

shellExec() {
  echo "$*";
  eval $*;
}

if [ $# != 3 ]; then
  echo "usage: xr_pack_deps.sh <linux-x86_64|linux-i686>"
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
$MKDIR -p songbird/$tree/dependencies/$plat/mozilla/$buildMode/
$MKDIR -p songbird/$tree/dependencies/$plat/xulrunner/$buildMode/

# make mozilla sdk
shellExec ./make-mozilla-sdk.sh mozilla/ \
                                mozilla/compiled/xulrunner-$buildMode/ \
                                compiled/$plat/mozilla/$buildMode/

# make xulrunner tarball
shellExec ./make-xulrunner-tarball.sh mozilla/compiled/xulrunner-$buildMode/dist/bin/ \
                                      compiled/$plat/xulrunner/$buildMode/ \
                                      xulrunner.tar.bz2
