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

notice "finding executable files..."
$FIND -L "$permFixDir" -perm +0111 -not -type d -not -name '*\.so' -exec chmod 0755 {} \; -print

notice "finding non executable files..."
$FIND -L "$permFixDir" -not -perm +0111 -exec chmod 0644 {} \; -print

cd "$permFixDir"

notice "Creating mega xpt out of all .xpts..."
pushd components > /dev/null
if test -e xulrunner.xpt; then
   notice "xulrunner.xpt already exists. Skipping mega xpt creation."
else
   all_xpts=$($FIND . -name '*.xpt')
   shellExec ../xpt_link xulrunner.xpt $all_xpts
   shellExec $RM -f $all_xpts
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
