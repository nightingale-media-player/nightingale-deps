#!/usr/bin/env bash

if [ $# != 1 ]; then
  echo "usage: xr_strip_symbols.sh <windows|linux|macosx>"
  exit
fi

set -e

plat=$1

if test -n "$STRIP"; then
  strip="$STRIP"
else
  strip="strip -v"
fi

# Expect to be in the mozilla source directory.
# cd into the dist dir.
cd compiled/xulrunner-release/dist

# strip all the shared objects
find -L \
  ./bin \
  ./lib \
  ./sdk/bin \
  ./sdk/lib \
  ! -type d \
  ! -name "*LICENSE" \
  ! -name "*.a" \
  ! -name "*.aff" \
  ! -name "*.c" \
  ! -name "*.cfg" \
  ! -name "*.chk" \
  ! -name "*.cpp" \
  ! -name "*.css" \
  ! -name "*.dat" \
  ! -name "*.dic" \
  ! -name "*.dtd" \
  ! -name "*.gif" \
  ! -name "*.h" \
  ! -name "*.html" \
  ! -name "*.ico" \
  ! -name "*.idl" \
  ! -name "*.ini" \
  ! -name "*.jar" \
  ! -name "*.jpg" \
  ! -name "*.js" \
  ! -name "*.jsm" \
  ! -name "*.list" \
  ! -name "*.manifest" \
  ! -name "*.png" \
  ! -name "*.properties" \
  ! -name "*.py" \
  ! -name "*.rdf" \
  ! -name "*.reg" \
  ! -name "*.sh" \
  ! -name "*.src" \
  ! -name "*.tbl" \
  ! -name "*.txt" \
  ! -name "*.xml" \
  ! -name "*.xpm" \
  ! -name "*.xpt" \
  ! -name "*.xul" \
  -exec ${strip} {} \;

