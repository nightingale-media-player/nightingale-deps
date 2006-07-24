#!/bin/sh

#
# Clean up, just in case it's not the first time
# this script is run.
#
rm -rf ./compiled
make clean

#
# Compile the debug mode libogg for linux 
#
CFLAGS="-fshort-wchar" \
CXXFLAGS="-fshort-wchar" \
./configure --prefix=${PWD}/compiled/release-linux \
  --enable-debug=yes \
  --enable-cxx-warnings=no
make && make install
make clean
rm -f config.log config.status Makefile

#
# Compile the release mode libogg for linux 
#
CFLAGS="-fshort-wchar" \
CXXFLAGS="-fshort-wchar" \
./configure --prefix=${PWD}/compiled/debug-linux \
  --enable-debug=no \
  --enable-cxx-warnings=no
make && make install
make clean
rm -f config.log config.status Makefile

