#!/bin/bash

TOPDIR=$(pwd)

# Setup bundled build deps
echo "cd $TOPDIR/packages/win32"
cd $TOPDIR/packages/win32


# Precompiled things
echo "Extracting packages"
unzip bison-2.4.1.zip
unzip cmake-2.8.12.2-win32-x86.zip
mkdir flex-2.5.33 regex-0.12 binutils-2.24-1
tar -j -x -f flex-2.5.33-MSYS-1.0.11-1.tar.bz2 -C flex-2.5.33
tar -j -x -f regex-0.12-MSYS-1.0.11-1.tar.bz2 -C regex-0.12
tar -j -x -f binutils-2.24-1-mingw32-bin.tar.bz2 -C binutils-2.24-1

echo "Setting up packages"
mkdir /c/ng-deps
mv bison-2.4.1 /c/ng-deps/bison-2.4.1
mv cmake-2.8.12.2-win32-x86/cmake-2.8.12.2-win32-x86 /c/ng-deps/cmake-2.8.12.2
mv flex-2.5.33 /c/ng-deps/flex-2.5.33
mv regex-0.12 /c/ng-deps/regex-0.12
mv binutils-2.24-1 /c/ng-deps/binutils-2.24-1

echo "Cleaing up"
rmdir cmake-2.8.12.2-win32-x86


# Need to build libffi for glib
echo "Extracting libffi"
tar -x -f libffi-3.0.13.tar.gz

echo "Building libffi"
cd libffi-3.0.13
./configure --prefix=/local && make && make install
cd ..

# Need to build libtool to prevent linking problems
echo "Extracting libtool"
tar -x -f libtool-2.4.2.tar.gz

echo "Building libtool"
cd libtool-2.4.2
./configure --prefix=/local && make && make install
cd ..

echo "Cleaing up"
rm -rf libffi-3.0.13
rm -rf libtool-2.4.2

echo "cd $TOPDIR"
cd $TOPDIR

echo "Done"
