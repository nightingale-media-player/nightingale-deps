#!/usr/bin/env bash

set -e

CHMOD=${CHMOD:-chmod}
CP=${CP:-cp}
FIND=${FIND:-find}
GREP=${GREP:-grep}
RM=${RM:-rm}
TAR=${TAR:-tar}

notice() {
  echo "$*" 1>&2
}

shellExec() {
  echo "$*";
  eval $*; 
}

if [ $# != 3 ]; then
  notice "usage: make-xulrunner-tarball.sh [mozilla-bin-dir] [songbird-tarball-dest-dir] [target-tarball-name]"
  exit 1
fi

srcdir="$1"
srcDirname="$(dirname "$srcdir")"
srcBasename="$(basename "$srcdir")"
bindir="$(cd "$srcDirname" 2>/dev/null && pwd || echo "$srcDirname")/$srcBasename"

destdir="$2"
destDirname="$(dirname "$destdir")"
destBasename="$(basename "$destdir")"
tarballdir="$(cd "$destDirname" 2>/dev/null && pwd || echo "$destDirname")/$destBasename"

tarballname="$3"

tarball="$tarballdir/$tarballname"

permFixDir="$srcDirname/$srcBasename"
notice "Fixing permissions in $permFixDir..."
$FIND "$permFixDir" -perm +0111 -exec chmod 0755 {} \;
$FIND "$permFixDir" -not -perm +0111 -exec chmod 0644 {} \;

cd "$permFixDir"

notice "Creating mega xpt out of all .xpts..."
pushd components > /dev/null
if test -e xulrunner.xpt; then
  notice "xulrunner.xpt already exists. Skipping mega xpt creation."
else
  all_xpts=$($FIND . -name '*.xpt')
  [ -z "${PYTHON}" ] && PYTHON="$(which python2 2>/dev/null || which python)"
  shellExec $PYTHON ../../sdk/bin/xpt.py link xulrunner.xpt $all_xpts
  shellExec $RM -f $all_xpts
fi
# Before 2.0, xpts didn't need to be registered in manifests, but they do now.
# All of the xpts are linked and removed, but the 'mega' xpt isn't registered
# in the manifest, while all the ones just removed are listed still.
# So, overwrite the manifest containing the old list with our new, single, one.
if test -z "$(cat interfaces.manifest | $GREP xulrunner.xpt)" ; then
  $CP interfaces.manifest interfaces.manifest.bak
  echo -e 'interfaces xulrunner.xpt\n' > interfaces.manifest
fi
popd > /dev/null

EXTRA_TAR_FLAGS=""
# Ensure there are no .pdbs in the xulrunner release tarballs...
if test -n "$(echo "$destBasename" | $GREP release)"; then
  EXTRA_TAR_FLAGS="--exclude='*.pdb'"
fi

notice "creating tarball in dest..."

TARBALL_FILES=$(echo *)

shellExec $TAR -cjvh --owner=0 --group=0 --exclude='CVS' --exclude='.hg' --exclude='.svn' $EXTRA_TAR_FLAGS --numeric-owner -p -f "$tarball" $TARBALL_FILES

notice "done."
