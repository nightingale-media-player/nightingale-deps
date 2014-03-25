#!/bin/bash

ARCH="$(uname -m)"

# XUL Version
export XUL="9.0.1"

export DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export SB_VENDOR_BINARIES_CO_ROOT=$DIR
export SB_VENDOR_BUILD_ROOT=$DIR

# rm -rf build checkout
# mkdir build

# if [ ! -d "build" ]; then
#     mkdir build
# fi

case $OSTYPE in
    msys*)
        if [ ! -d "windows-i686-msvc10" ]; then
            mkdir -p "windows-i686-msvc10"
        fi
        if [ ! -d "checkout/windows-i686-msvc10" ]; then
            mkdir -p "checkout/windows-i686-msvc10"
        fi

        export DIRECTX_SDK_ROOT="/c/Program Files (x86)/Microsoft DirectX SDK (June 2010)"
        export WINDOWS_SDK_ROOT="/c/Program Files (x86)/Microsoft SDKs/Windows/v7.0A"

        # echo -e "Building libtool..."
        # make -C libtool -f Makefile.nightingale

        # echo -e "Building libiconv..."
        # make -C libiconv -f Makefile.nightingale

        # echo -e "Building libjpeg-turbo...\n"
        # make -C libjpeg-turbo -f Makefile.nightingale

        # echo -e "Building zlib...\n"
        # make -C zlib -f Makefile.nightingale

        ### TIER 1 ###
        # echo -e "Building gettext..."
        # make -C gettext -f Makefile.nightingale

        # echo -e "Building glib..."
        # make -C glib -f Makefile.nightingale

        # echo -e "Building sqlite...\n"
        # make -C sqlite -f Makefile.nightingale

        ### TIER 2 ###
        # echo -e "Building libogg...\n"
        # make -C libogg -f Makefile.nightingale

        # echo -e "Building libvorbis...\n"
        # make -C libvorbis -f Makefile.nightingale

        # echo -e "Building libtheora...\n"
        # make -C libtheora -f Makefile.nightingale

        # echo -e "Building FLAC...\n"
        # make -C flac -f Makefile.nightingale
        # make -C flac-1.2.1 -f Makefile.nightingale

        # echo -e "Building taglib...\n"
        # make -C taglib -f Makefile.nightingale

        ### TIER 3 ###
        # echo -e "Building gstreamer bits..."
        # make -C gstreamer -f Makefile.nightingale

        ### TIER 4 ###
        # echo -e "Building gst plugins...here's hoping these all build!"
        # make -C gst-plugins-base -f Makefile.nightingale
        # make -C gst-plugins-good -f Makefile.nightingale
        # make -C gst-plugins-bad -f Makefile.nightingale
        # make -C gst-plugins-ugly -f Makefile.nightingale

    ;;

    *)
        echo "Windows only build script..."
    ;;
esac