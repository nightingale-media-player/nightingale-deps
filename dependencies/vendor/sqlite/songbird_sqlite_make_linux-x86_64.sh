#!/bin/sh

#
# Clean up, just in case it's not the first time
# this script is run.
#
rm -rf ./compiled
make clean

#
# Compile the release mode sqlite3 for linux-x86_64
#
CFLAGS="-fshort-wchar -fPIC" \
CXXFLAGS=-fshort-wchar \
ac_cv_visibility_pragma=no \
./configure --prefix=${PWD}/compiled/release-linux-x86_64 \
  --enable-releasemode \
  --enable-threadsafe \
  --enable-tempstore \
  --enable-cross-thread-connections \
  --disable-tcl \
  LIBS=-ldl
make && make install
make clean
rm -f config.log config.status Makefile

#
# Compile the debug mode sqlite3 for linux-x86_64
#
CFLAGS="-fshort-wchar -fPIC" \
CXXFLAGS=-fshort-wchar \
ac_cv_visibility_pragma=no \
./configure --prefix=${PWD}/compiled/debug-linux-x86_64 \
  --enable-releasemode \
  --enable-threadsafe \
  --enable-tempstore \
  --enable-cross-thread-connections \
  --disable-tcl \
  --enable-debug \
  LIBS=-ldl
make && make install
make clean
rm -f config.log config.status Makefile

