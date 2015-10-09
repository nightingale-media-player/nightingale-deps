#!/bin/bash

set -e

# build-dependencies on Debian-based systems:
# automake autoconf2.13 libtool cmake unzip zip libasound2-dev libbz2-dev libcairo2-dev
# libevent-dev libfreetype6-dev libgnomevfs2-dev libgtk2.0-dev libharfbuzz-dev libhunspell-dev
# libidl-dev libjpeg-dev libnotify-dev libnspr4-dev libnss3-dev libpixman-1-dev libpng12-dev
# libsqlite3-dev libxext-dev libxt-dev zlib1g-dev

export DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export SB_VENDOR_BINARIES_CO_ROOT=$DIR
export SB_VENDOR_BUILD_ROOT=$DIR

export LINT_BUILD=1

if [ ! -d "build" ]; then
    mkdir build
fi

case $OSTYPE in
    linux*)
        if [ "$LINT_BUILD" -eq "1" ] ; then
            # hardening flags
            export CFLAGS="-fstack-protector --param=ssp-buffer-size=4 -D_FORTIFY_SOURCE=2"
            export CXXFLAGS="-fstack-protector --param=ssp-buffer-size=4 -D_FORTIFY_SOURCE=2"
            export LDFLAGS="-Wl,-z,now -Wl,-z,relro"
        fi

        export SB_CFLAGS=$CFLAGS
        export SB_CCFLAGS=$CFLAGS
        export SB_CXXFLAGS=$CXXFLAGS

        export SB_LDFLAGS=$LDFLAGS

        if [ -n "$DIST_NAME_BINARIES_DIR" ] ; then
            if [ "$DIST_NAME_BINARIES_DIR" -eq "1" ] && \
               [ ! -d "dist/linux-$(uname -m)" ] ; then
                mkdir -p "dist/linux-$(uname -m)"
            fi
        fi
        if [ ! -d "linux-$(uname -m)" ]; then
            mkdir -p "linux-$(uname -m)"
        fi
        if [ ! -d "checkout/linux-$(uname -m)" ]; then
            mkdir -p "checkout/linux-$(uname -m)"
        fi

        echo -e "Building sqlite...\n"
        make -C sqlite -f Makefile.songbird
        strip --strip-all "linux-$(uname -m)/sqlite/release/bin/sqlite3"
        strip --strip-debug "linux-$(uname -m)/sqlite/release/lib/libsqlite3.a"

        echo -e "Building taglib...\n"
        make -C taglib -f Makefile.songbird
        strip --strip-debug "linux-$(uname -m)/taglib/release/lib/libtag.a"

        echo -e "Building xulrunner 1.9.2...\n"
        make -C xulrunner-1.9.2 -f Makefile.songbird xr-all

    ;;
    *)
        echo "Hardening isn't relevant for you."
    ;;
esac
