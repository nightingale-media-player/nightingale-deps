#!/bin/bash

TOPDIR=$(pwd)

DXSDK_URL="http://download.microsoft.com/download/A/E/7/AE743F1F-632B-4809-87A9-AA1BB3458E31/DXSDK_Jun10.exe"
VS2010EXP_URL="http://go.microsoft.com/?linkid=9709969"
VS2010EXP_ISO_URL="http://download.microsoft.com/download/1/E/5/1E5F1C0A-0D5B-426A-A603-1798B951DDAE/VS2010Express1.iso"
MOZBLD_URL="https://ftp.mozilla.org/pub/mozilla.org/mozilla/libraries/win32/MozillaBuildSetup-1.8.0.exe"

HAVE_ARIA2C=0

if [ -z  $(which aria2c | grep unknown) && ! -z $(which aria2c) ] ; then
    HAVE_ARIA2C=1
fi

mkdir -p setup-build/win32 && cd setup-build/win32

# DirectX SDK (June 2010)
if [ "1" -eq "$HAVE_ARIA2C" ] ; then
    aria2c -j8 $DXSDK_URL
else 
    wget $DXSDK_URL
fi

if [ "1" -eq "$HAVE_ARIA2C" ] ; then
    aria2c -j8 $VS2010EXP_URL -o VS2010Express1.iso
else 
    wget $VS2010EXP_URL -O VS2010Express1.iso
fi

# Get MozillaBuild
echo "Downloading MozillaBuild\n"
if [ "1" -eq "$HAVE_ARIA2C" ] ; then
    aria2c -j8 $MOZBLD_URL
else 
    wget $MOZBLD_URL
fi

echo "Please install MozillaBuild-1.8, downloaded to setup-build/win32/MozillaBuildSetup-1.8.0.exe\n"

MB_FOUND=0
while [[ MB_FOUND -eq 0 ]]; do
    read -p "Press the enter key to continue once you've installed MozillaBuild\n" -n1 -s
    
    if [ -e "/c/mozilla-build-1.8/start-msvc10.bat" ] ; then
        MB_FOUND=1
    else
        echo "Couldn't find /c/mozillabuild/start-msvc10.bat!"
        echo "Are you sure you installed MozillaBuild?\n"
    fi
done


# Setup bundled build deps
echo "cd $(TOPDIR)/packages/win32\n"
cd $(TOPDIR)/packages/win32

echo "Extracting packages\n"
unzip bison-2.1-bin.zip
unzip bison-2.1-dep.zip
unzip cmake-2.8.12.2-win32-x86.zip
mkdir flex-2.5.33 regex-0.12
tar -j -x -f flex-2.5.33-MSYS-1.0.11-1.tar.bz2 -C flex-2.5.33
tar -j -x -f regex-0.12-MSYS-1.0.11-1.tar.bz2 -C regex-0.12

echo "Setting up packages\n"
mkdir /c/ng-deps
mv bison-2.1-bin /c/ng-deps/bison-2.1-bin
mv bison-2.1-deps /c/ng-deps/bison-2.1-deps
mv cmake-2.8.12.2-win32-x86/cmake-2.8.12.2-win32-x86 /c/ng-deps/cmake-2.8.12.2
mv flex-2.5.33 /c/ng-deps/flex-2.5.33
mv regex-0.12 /c/ng-deps/regex-0.12
rmdir cmake-2.8.12.2-win32-x86

# TODO: (next) add /c/ng-deps/* paths and win paths to start-msvc10.bat

