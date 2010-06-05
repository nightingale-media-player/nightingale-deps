#!/bin/bash
#
# This tool generates full update packages for the update system.
# Author: Darin Fisher
#

. $(dirname "$0")/common.sh

# -----------------------------------------------------------------------------

print_usage() {
  notice "Usage: $(basename $0) [OPTIONS] ARCHIVE DIRECTORY"
}

if [ $# = 0 ]; then
  print_usage
  exit 1
fi

if [ $1 = -h ]; then
  print_usage
  notice ""
  notice "The contents of DIRECTORY will be stored in ARCHIVE."
  notice ""
  notice "Options:"
  notice "  -h  show this help text"
  notice "  -m  use a file manifest for the mar tool"
  notice ""
  exit 1
fi

use_mar_manfiest=
if [ $1 = -m ]; then
   use_mar_manfiest=1
   shift 1
fi

# -----------------------------------------------------------------------------

archive="$1"
targetdir="$2"
workdir="$targetdir.work"
manifest="$workdir/update.manifest"
targetfiles="update.manifest"

mkdir -p "$workdir"

# Generate a list of all files in the target directory.
pushd "$targetdir"
if test $? -ne 0 ; then
  exit 1
fi

list_files files

popd

> $manifest

num_files=${#files[*]}
  
if [ -n "$use_mar_manifest" ]; then
   echo "update.manifest" >> "$workdir/mar.manifest"
fi

for ((i=0; $i<$num_files; i=$i+1)); do
  f="${files[$i]}"

  notice "processing $f"

  make_add_instruction "$f" >> $manifest

  dir=$(dirname "$f")
  mkdir -p "$workdir/$dir"
  $BZIP2 -cz9 "$targetdir/$f" > "$workdir/$f"
  copy_perm "$targetdir/$f" "$workdir/$f"

  if [ -n "$use_mar_manifest" ]; then
     echo "$f" >> "$workdir/mar.manifest"
  fi

  targetfiles="$targetfiles \"$f\""
done

# Append remove instructions for any dead files.
append_remove_instructions "$targetdir" >> $manifest

$BZIP2 -z9 "$manifest" && mv -f "$manifest.bz2" "$manifest"

if [ -n "$use_mar_manifest" ]; then
   echo "$MAR -C \"$workdir\" -f mar.manifest -c output.mar"
   eval "$MAR -C \"$workdir\" -f mar.manifest -c output.mar"
else
   echo "$MAR -C \"$workdir\" -c output.mar $targetfiles"
   eval "$MAR -C \"$workdir\" -c output.mar $targetfiles"
fi

mv -f "$workdir/output.mar" "$archive"

# cleanup
rm -fr "$workdir"
