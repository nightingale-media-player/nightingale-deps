#!/bin/sh

#
# Clean up, just in case it's not the first time
# this script is run.
#
rm -rf ./compiled
make clean

FLAGS="-fshort-wchar -DSQLITE_OMIT_LOAD_EXTENSION=1 -fPIC"
#
# Compile the release mode sqlite3 for linux-i386
#
CFLAGS=${FLAGS} \
CXXFLAGS=${FLAGS} \
ac_cv_visibility_pragma=no \
./configure --prefix=${PWD}/compiled/release-linux-i386 \
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
# Compile the debug mode sqlite3 for linux-i386
#
CFLAGS=${FLAGS} \
CXXFLAGS=${FLAGS} \
ac_cv_visibility_pragma=no \
./configure --prefix=${PWD}/compiled/debug-linux-i386 \
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

