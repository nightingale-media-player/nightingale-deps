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
        # make -C libtool -f Makefile.songbird

        ### TIER 1 ###
        # echo -e "Building gettext..."
        # make -C gettext -f Makefile.songbird

        # echo -e "Building glib..."
        # make -C glib -f Makefile.songbird

        # echo -e "Building sqlite...\n"
        # make -C sqlite -f Makefile.songbird

        # echo -e "Building zlib...\n"
        # make -C zlib -f Makefile.songbird

        ### TIER 2 ###
        # echo -e "Building libogg...\n"
        # make -C libogg -f Makefile.songbird

        # echo -e "Building libvorbis...\n"
        # make -C libvorbis -f Makefile.songbird

        # echo -e "Building libtheora...\n"
        # make -C libtheora -f Makefile.songbird

        # echo -e "Building FLAC...\n"
        # make -C flac -f Makefile.songbird

        # echo -e "Building taglib...\n"
        # make -C taglib -f Makefile.songbird

        ### TIER 3 ###
        # echo -e "Building gstreamer bits..."
        # make -C gstreamer -f Makefile.songbird

        ### TIER 4 ###
        # echo -e "Building gst plugins...here's hoping these all build!"
        # make -C gst-plugins-base -f Makefile.songbird
        # make -C gst-plugins-good -f Makefile.songbird
        # make -C gst-plugins-bad -f Makefile.songbird
        # make -C gst-plugins-ugly -f Makefile.songbird

        ### TIER 5? ###
        # echo -e "Building libjpeg-turbo...\n"
        # make -C libjpeg-turbo -f Makefile.songbird

    ;;

    *)
        echo "Windows only build script..."
    ;;
esac