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
  
force_add_extensions=0

if [ $1 = -h ]; then
  print_usage
  notice ""
  notice "The contents of DIRECTORY will be stored in ARCHIVE."
  notice ""
  notice "Options:"
  notice "  -h  show this help text"
  notice "  -e  treat extensions as not-optional, and always add them"
  notice ""
  exit 1
elif [ $1 = -e ]; then
  force_add_extensions=1
  shift
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

for ((i=0; $i<$num_files; i=$i+1)); do
  f="${files[$i]}"

  notice "processing $f"

  # This regex for extensions comes from common.sh:39
  if [ "$force_add_extensions" = "1" -a $(echo "$f" | grep -c 'extensions/.*/') = "1" ]; then
    make_add_instruction "$f" "$force_add_extensions" >> $manifest
  else
    make_add_instruction "$f" >> $manifest
  fi

  dir=$(dirname "$f")
  mkdir -p "$workdir/$dir"
  $BZIP2 -cz9 "$targetdir/$f" > "$workdir/$f"
  copy_perm "$targetdir/$f" "$workdir/$f"

  targetfiles="$targetfiles \"$f\""
done

# Append remove instructions for any dead files.
append_remove_instructions "$targetdir" >> $manifest

$BZIP2 -z9 "$manifest" && mv -f "$manifest.bz2" "$manifest"

eval "$MAR -C \"$workdir\" -c output.mar $targetfiles"
mv -f "$workdir/output.mar" "$archive"

# cleanup
rm -fr "$workdir"
