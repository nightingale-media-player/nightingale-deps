#!/bin/sh

# Test build script for windows i686
# Build environment : Cygwin
# compiler used : msvc8
# linker used : link.exe

tgt_name=gstreamer
tgt_arch=windows-i686-msvc8

dep_dir=${PWD}/../..
tgt_src_dir=${dep_dir}/vendor/${tgt_name}
tools_dir=${dep_dir}/../tools

export_append()
{
    # Get the export variable name.
    export_name=$1;
    shift

    # Append the parameters to the export variable.
    eval "${export_name}=\"\${${export_name}} $*\"";
}

export CC="cl"
export CXX="cl"
export LD="link"

# Determine MSVC version.
tmp='s|.* \([0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\).*|\1|p'
_MSVC_VER_FILTER=${tmp}

CC_VERSION=`cl -v 2>&1 | sed -ne "$_MSVC_VER_FILTER"`
_CC_MAJOR_VERSION=`echo ${CC_VERSION} | awk -F\. '{ print $1 }'`
_CC_MINOR_VERSION=`echo ${CC_VERSION} | awk -F\. '{ print $2 }'`
export _MSC_VER=${_CC_MAJOR_VERSION}${_CC_MINOR_VERSION}

start_dir=${PWD}
dep_arch_dir=${dep_dir}/${tgt_arch}
windep_arch_dir=$()

# PATH for libraries/binaries on windows
export PATH="$PATH:${dep_arch_dir}/glib/debug/bin"
export PATH="$PATH:${dep_arch_dir}/gettext/debug/bin"
export PATH="$PATH:${dep_arch_dir}/libiconv/debug/bin"

# libtool
export PATH="${dep_arch_dir}/../windows-i686/libtool/release/bin:${PATH}"
export ACLOCAL_FLAGS="$ACLOCAL_FLAGS -I ${dep_arch_dir}/../windows-i686/libtool/release/share/aclocal"
export ACLOCAL_FLAGS=`${dep_arch_dir}/mozilla/release/scripts/cygwin-wrapper echo ${ACLOCAL_FLAGS}`

export PKG_CONFIG_PATH="${dep_arch_dir}/glib/debug/lib/pkgconfig"

# SETUP CFLAGS HERE
export GLIB_CFLAGS="-I${dep_arch_dir}/glib/debug/include/glib-2.0 -I${dep_arch_dir}/glib/debug/lib/glib-2.0/include"
export GLIB_CFLAGS=`${dep_arch_dir}/mozilla/release/scripts/cygwin-wrapper echo ${GLIB_CFLAGS}`
export GLIB_ONLY_CFLAGS=$GLIB_CFLAGS
export GLIB_LIBS="-Wl,${dep_arch_dir}/glib/debug/lib/glib-2.0.lib -Wl,${dep_arch_dir}/glib/debug/lib/gmodule-2.0.lib -Wl,${dep_arch_dir}/glib/debug/lib/gobject-2.0.lib -Wl,${dep_arch_dir}/glib/debug/lib/gthread-2.0.lib -Wl,-Zi"
#export GLIB_LIBS="-L${dep_arch_dir}/glib/debug/lib -lgobject-2.0 -lgthread-2.0 -lgmodule-2.0 -lglib-2.0 -lintl -liconv"
export GLIB_LIBS=`${dep_arch_dir}/mozilla/release/scripts/cygwin-wrapper echo ${GLIB_LIBS}`
export GLIB_ONLY_LIBS=$GLIB_LIBS

export LDFLAGS="-L${dep_arch_dir}/glib/debug/lib"

# global CFLAGS
export CFLAGS="${CFLAGS} -MTd -D_MSC_VER=${_MSC_VER} -DWIN32 -D__NO_CTYPE -D_CRT_SECURE_NO_WARNINGS -DHAVE_WIN32 -D_WINDOWS -wd4820 -wd4668 -wd4100 -wd4706 -wd4127 -wd4255 -wd4710 -wd4055"
export CFLAGS=`${dep_arch_dir}/mozilla/release/scripts/cygwin-wrapper\
    echo ${CFLAGS}`

# debugging flags
export CFLAGS="${CFLAGS} -Zi"

# flag to tell bison we don't have unistd
export CFLAGS="${CFLAGS} -DYY_NO_UNISTD_H"

build_dir=${dep_arch_dir}/${tgt_name}/build
rm -Rf ${build_dir}
mkdir -p ${build_dir}
cp -R ../gstreamer ${build_dir}
cd ${build_dir}/gstreamer

# Set up the target configuration options.
if test -n "${cfg_tgt}"; then
    cfg_opts="${cfg_opts} --target=${cfg_tgt}"
    cfg_opts="${cfg_opts} --host=${cfg_tgt}"
fi

./configure --prefix=${dep_arch_dir}/${tgt_name}/${build_type}  \
    ${cfg_opts}							\
    --disable-loadsave						\
    --enable-binary-registry --disable-examples			\
    --disable-trace --disable-alloc-trace			\
    -C								\
    --target=i686-pc-mingw32 --host=i686-pc-mingw32
make && make install

cd ${start_dir}
