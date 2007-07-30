#!/bin/sh
################################################################################
################################################################################
#
# BEGIN SONGBIRD GPL
# 
# This file is part of the Songbird web player.
#
# Copyright(c) 2007 POTI, Inc.
# http://www.songbirdnest.com
# 
# This file may be licensed under the terms of of the
# GNU General Public License Version 2 (the "GPL").
# 
# Software distributed under the License is distributed 
# on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either 
# express or implied. See the GPL for the specific language 
# governing rights and limitations.
#
# You should have received a copy of the GPL along with this 
# program. If not, go to http://www.gnu.org/licenses/gpl.html
# or write to the Free Software Foundation, Inc., 
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
# 
# END SONGBIRD GPL
#
################################################################################
#
# Script for building libgpod.
#
################################################################################
################################################################################

################################################################################
#
# Build configuration.
#
################################################################################

#
# Target configuration
#
#   tgt_name                    Name of target to build.
#

tgt_name=libgpod


#
# System configuration.
#
#   build_sys_type              Build system type.
#   tgt_arch_list               List of build target architectures.
#

sys_name=`uname`
mach_name=`uname -m`
if [ "$sys_name" = "Darwin" ]; then
    build_sys_type=Darwin
    if [ "$mach_name" = "i386" ]; then
        tgt_arch_list="macosx-i686"
    else
        tgt_arch_list=macosx-ppc
    fi
elif [ "$sys_name" = "Linux" ]; then
    build_sys_type=Linux
    if [ "$mach_name" = "x86_64" ]; then
        tgt_arch_list=linux-x86_64
    else
        tgt_arch_list=linux-i686
    fi
else
    build_sys_type=Cygwin
    tgt_arch_list=windows-i686
fi


#
# Build configuration
#
#   dep_dir                     Directory containing dependencies.
#

dep_dir=${PWD}/../..


################################################################################
#
# Build functions.
#
################################################################################

#
# build_all
#
#   This function builds all of the configured targets.
#

build_all()
{
    # Build for each of the target architectures.
    for tgt_arch in ${tgt_arch_list}
    do
        # Build release target.
        build release

        # Build debug target.
        build debug
    done
}


#
# export_append
#
#   --> export_name             Name of export variable to which to append.
#   --> args...                 Values to append to export variable.
#
#   This function appends the values specified by args... to the export variable
# specified by export_name.
#

export_append()
{
    # Get the export variable name.
    export_name=$1;
    shift

    # Append the parameters to the export variable.
    eval "${export_name}=\"\${${export_name}} $*\"";
}


#
# setup_build
#
#   --> build_tgt_arch          Target build architecture.
#
#   This function sets up a build for the target architecture specified by
# build_tgt_arch.
#

setup_build()
{
    # Read the function parameters.
    build_tgt_arch="$1"

    # Set up default environment.
    use_sys_libs=false;

    # Set up the build environment for the given target.
    case "${build_tgt_arch}" in

        linux-i686 | linux-x86_64)
            export CPPFLAGS="${CPPFLAGS} -fPIC"
            use_sys_libs=true
            ;;

        windows-i686)
            export CPPFLAGS="${CPPFLAGS} -MD -DWIN32"
            export_append                                                      \
                    "LIBGPOD_CFLAGS"                                           \
                    "-D __NO_CTYPE"                                            \
                    "-I${dep_arch_dir}/libgw32c/include/glibc"                 \
                    "-I${dep_arch_dir}/mingw/include"                          \
                    "-include"                                                 \
                        "${dep_dir}/vendor/libgpod/win32/include/libgpod_port.h"
            export_append                                                      \
                        "LIBGPOD_LIBS"                                         \
                        "${dep_arch_dir}/libgw32c/lib/libgw32c.a"              \
                        "-L${dep_arch_dir}/mingw/lib"                          \
                        "${dep_arch_dir}/mingw/lib/gcc/mingw32/3.4.2/libgcc.a" \
                        "/NODEFAULTLIB:msvcrt.Lib"                             \
                        "/IMPLIB:gpod.lib"                                     \
                        "-ladvapi32"                                           \
                        "-lgdi32"                                              \
                        "-lkernel32"                                           \
                        "-lole32"                                              \
                        "-lshell32"                                            \
                        "-luser32"                                             \
                        "-luuid"
            cl_process="${dep_arch_dir}/mozilla/release/scripts/cygwin-wrapper"
            cfg_tgt=i686-pc-mingw32
            ;;

        macosx-i686 | macosx-ppc)
            export CFLAGS="${CFLAGS} -fnested-functions"
            export LDFLAGS="${LDFLAGS} -headerpad_max_install_names"
            ;;

    esac
}


#
# build
#
#   --> build_type              Type of build (release or debug).
#
#   This function runs the configured build of the type specified by build_type.
#

build()
{
    # Read the function parameters.
    build_type="$1"

    # Get starting directory.
    start_dir=${PWD}

    # Initialize some variables.
    export LIBGPOD_LIBS=
    export LIBGPOD_CFLAGS=
    export CPPFLAGS=
    export CFLAGS=
    export LDFLAGS=

    # Get the target architecture depedencies directory.
    dep_arch_dir=${dep_dir}/${tgt_arch}

    # Set up the build environment.
    setup_build ${tgt_arch}

    # If not using system libraries, set up build options for local versions.
    if ! $use_sys_libs; then
        # Set up gettext build options.
        export_append "LIBGPOD_LIBS"                                           \
                      "-L${dep_arch_dir}/gettext/${build_type}/lib"            \
                      "-lintl"
        export_append "LIBGPOD_CFLAGS"                                         \
                      "-I${dep_arch_dir}/gettext/${build_type}/include"

        # Set up glib build options.
        export_append "LIBGPOD_LIBS"                                           \
                      "-L${dep_arch_dir}/glib/${build_type}/lib"               \
                      "-lglib-2.0"                                             \
                      "-lgobject-2.0"
        export_append                                                          \
                    "LIBGPOD_CFLAGS"                                           \
                    "-I${dep_arch_dir}/glib/${build_type}/include/glib-2.0"    \
                    "-I${dep_arch_dir}/glib/${build_type}/lib/glib-2.0/include"

        # Set up iconv build options.
        export_append "LIBGPOD_LIBS"                                           \
                      "-L${dep_arch_dir}/libiconv/${build_type}/lib"           \
                      "-liconv"
        export_append "LIBGPOD_CFLAGS"                                         \
                      "-I${dep_arch_dir}/libiconv/${build_type}/include"
    fi

    # Apply command line options pre-processing.
    if test -n "${cl_process}"; then
        export LIBGPOD_CFLAGS=`${cl_process} echo ${LIBGPOD_CFLAGS}`
        export LIBGPOD_LIBS=`${cl_process} echo ${LIBGPOD_LIBS}`
    fi

    # Set up to build within a clean build directory.
    build_dir=${dep_arch_dir}/${tgt_name}/build
    rm -Rf ${build_dir}
    mkdir -p ${build_dir}
    cp -R ../${tgt_name} ${build_dir}
    cd ${build_dir}/${tgt_name}

    # Determine the debug options.
    if [ "${build_type}" = "debug" ]; then
        cfg_opts="${cfg_opts} --enable-debug=yes"
    else
        cfg_opts="${cfg_opts} --enable-debug=no"
    fi

    # Set up the target configuration options.
    if test -n "${cfg_tgt}"; then
        cfg_opts="${cfg_opts} --target=${cfg_tgt}"
        cfg_opts="${cfg_opts} --host=${cfg_tgt}"
    fi

    # Configure, build, and install.
    ./configure --prefix=${dep_arch_dir}/${tgt_name}/${build_type}             \
                ${cfg_opts}                                                    \
                --disable-libsuffix                                            \
                --disable-gdk-pixbuf                                           \
                --disable-dependency-tracking                                  \
                --disable-static                                               \
                --enable-cxx-warnings=no
    make && make install

    # Rename import library on windows.
    if [ "${build_tgt_arch}" = "windows-i686" ]; then
        cp ${build_dir}/${tgt_name}/src/gpod.lib                               \
           ${dep_arch_dir}/${tgt_name}/${build_type}/lib/gpod.lib
        cp ${build_dir}/${tgt_name}/src/.libs/libgpod.dll                      \
           ${dep_arch_dir}/${tgt_name}/${build_type}/lib/libgpod.dll
    fi

    # Move back to starting directory.
    cd ${start_dir}

    # Clean up build directory.
    rm -Rf ${build_dir}
}


################################################################################
#
# Build script.
#
################################################################################

# Build everything.
build_all


