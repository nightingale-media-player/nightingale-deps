#!/bin/sh

#
# Clean up, just in case it's not the first time
# this script is run.
#
rm -rf ./compiled
make clean

#
# Compile the release mode libogg for x86 
#
CC="gcc-4.0 -arch i386" \
CXX="g++-4.0 -arch i386" \
CFLAGS="-fshort-wchar" \
CXXFLAGS="-fshort-wchar" \
LDFLAGS="-Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk -arch i386" \
./configure --prefix=${PWD}/compiled/release-intel \
  --enable-debug=no \
  --enable-cxx-warnings=no
make && make install
make clean
rm -f config.log config.status Makefile

#
# Compile the release mode libogg for ppc
#
CC="gcc-4.0 -arch ppc" \
CXX="g++-4.0 -arch ppc" \
CFLAGS="-fshort-wchar" \
CXXFLAGS="-fshort-wchar" \
LDFLAGS="-Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk -arch ppc" \
./configure --prefix=${PWD}/compiled/release-ppc \
  --target=powerpc-apple-darwin8.0.0 \
  --enable-debug=no \
  --enable-cxx-warnings=no
make && make install
make clean
rm -f config.log config.status Makefile

#
# Package the release mode libogg into a universal binary
#
# ${PWD}/../../macosx/mozilla/release/scripts/unify \
# ${PWD}/compiled/release-ppc \
# ${PWD}/compiled/release-intel \
# ${PWD}/compiled/release-universal \
# --only-one copy

#
# Compile the debug mode libogg for x86
#
CC="gcc-4.0 -arch i386" \
CXX="g++-4.0 -arch i386" \
CFLAGS="-fshort-wchar" \
CXXFLAGS="-fshort-wchar" \
LDFLAGS="-Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk -arch i386" \
./configure --prefix=${PWD}/compiled/debug-intel \
  --target=i386-apple-darwin-8.0.0 \
  --enable-debug=yes \
  --enable-cxx-warnings=no
make && make install
make clean
rm -f config.log config.status Makefile

#
# Compile the debug mode libogg for ppc
#
CC="gcc-4.0 -arch ppc" \
CXX="g++-4.0 -arch ppc" \
CFLAGS="-fshort-wchar" \
CXXFLAGS="-fshort-wchar" \
LDFLAGS="-Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk -arch ppc" \
./configure --prefix=${PWD}/compiled/debug-ppc \
  --target=powerpc-apple-darwin8.0.0 \
  --enable-debug=yes \
  --enable-cxx-warnings=no
make && make install
make clean
rm -f config.log config.status Makefile

#
# Package the debug mode libogg into a universal binary
#
# ${PWD}/../../macosx/mozilla/release/scripts/unify \
# ${PWD}/compiled/debug-ppc \
# ${PWD}/compiled/debug-intel \
# ${PWD}/compiled/debug-universal \
# --only-one copy

