#!/bin/sh
################################################################################
################################################################################
#
# BEGIN SONGBIRD GPL
# 
# This file is part of the Songbird web player.
#
# Copyright(c) 2007-2008 POTI, Inc.
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
# Script for building taglib.
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

tgt_name=taglib


#
# System configuration.
#
#   build_sys_type              Build system type.
#   tgt_arch_list               List of build target architectures.
#

mach_name=`uname -m`
case `uname` in
    Darwin)
        build_sys_type=Darwin
        if [ "$mach_name" = "i386" ]; then
            tgt_arch_list="macosx-i686 macosx-ppc"
        else
            tgt_arch_list=macosx-ppc
        fi
        ;;

    Linux)
        build_sys_type=Linux
        case "$mach_name" in
            x86_64)
                tgt_arch_list=linux-x86_64
                ;;
            *86)
                tgt_arch_list=linux-i686
                ;;
            powerpc* | ppc*)
                tgt_arch_list=linux-ppc
                ;;
        esac
        ;;

    SunOS)
        build_sys_type=Solaris
        case "$mach_name" in
            x86_64)
                tgt_arch_list=solaris-x86_64
                ;;
            i86pc)
                tgt_arch_list=solaris-i386
                ;;
            *)
                tgt_arch_list=solaris-sparc
                ;;
        esac
        ;;

    *)
        build_sys_type=Cygwin
        _MSVC_VER_FILTER='s|.* \([0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\).*|\1|p'
        CC_VERSION=`cl -v 2>&1 | sed -ne "$_MSVC_VER_FILTER"`
        case "$CC_VERSION" in
            13.*)
                tgt_arch_list=windows-i686
                ;;
            *)
                tgt_arch_list=windows-i686-msvc8
                ;;
        esac
        ;;
esac

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
        setup_build ${tgt_arch} release
        build release

        # Build debug target.
        setup_build ${tgt_arch} debug
        build debug
    done
}


#
# setup_build
#
#   --> build_tgt_arch          Target build architecture.
#   --> build_type              Type of build (release or debug).
#
#   This function sets up a build for the target architecture specified by
# build_tgt_arch and of the type specified by build_type.
#

setup_build()
{
    # Read the function parameters.
    build_tgt_arch="$1"
    build_type="$2"

    # Get the target architecture depedencies directory.
    dep_arch_dir=${dep_dir}/${tgt_arch}

    # Set up the build type environment.
    case "${build_type}" in
        debug)
            CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=Debug"
            ;;

        release | *)
            CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=Release"
            ;;
    esac

    # Set up the build environment for the given target.
    case "${build_tgt_arch}" in

        linux-i686 | linux-x86_64 | linux-ppc)
            # Set library defs.
            ZLIB_LIBRARY="libz.a"
            zlib_dir=${dep_arch_dir}/zlib/${build_type}

            # Set compiler flags.
            CMAKE_C_FLAGS="${CMAKE_C_FLAGS} -fPIC -fno-stack-protector"
            CMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -fPIC -fno-stack-protector"

            # Set makefile type.
            CMAKE_MAKEFILE_TYPE="Unix Makefiles"
            CMAKE_MAKE_CMD="make"
            ;;

        solaris-*)
            # Set library defs.
            ZLIB_LIBRARY="libz.so"
            zlib_dir=/usr

            # Set compiler flags.
            CMAKE_C_FLAGS="${CMAKE_C_FLAGS} -KPIC"
            CMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -KPIC"

            # Set makefile type.
            CMAKE_MAKEFILE_TYPE="Unix Makefiles"
            CMAKE_MAKE_CMD="make"
            ;;

        windows-i686)
            ;;

        windows-i686-msvc8)
            # Set compiler flags.
            CMAKE_C_FLAGS="${CMAKE_C_FLAGS} -Zc:wchar_t- -Zi"
            CMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -Zc:wchar_t- -Zi"

            # Set to use multi-threaded libraries.  Can't set in CMAKE_C_FLAGS
            # because default flags will override.
            CMAKE_C_FLAGS_DEBUG="${CMAKE_C_FLAGS} -MTd"
            CMAKE_CXX_FLAGS_DEBUG="${CMAKE_CXX_FLAGS} -MTd"
            CMAKE_C_FLAGS_RELEASE="${CMAKE_C_FLAGS} -MT"
            CMAKE_CXX_FLAGS_RELEASE="${CMAKE_CXX_FLAGS} -MT"

            # Set library defs.
            ZLIB_LIBRARY="zlib.lib"
            zlib_dir=${dep_arch_dir}/zlib/${build_type}

            # Set makefile type.
            CMAKE_MAKEFILE_TYPE="NMake Makefiles"
            CMAKE_MAKE_CMD="nmake"
            ;;

        macosx-i686 | macosx-ppc)
            # Set library defs.
            ZLIB_LIBRARY="libz.a"
            zlib_dir=${dep_arch_dir}/zlib/${build_type}

            # Set makefile type.
            CMAKE_MAKEFILE_TYPE="Unix Makefiles"
            CMAKE_MAKE_CMD="make"
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

    # Get the target architecture depedencies directory.
    dep_arch_dir=${dep_dir}/${tgt_arch}

    # Set up to build within a clean build directory.
    build_dir=${dep_arch_dir}/${tgt_name}/build
    rm -Rf ${build_dir}
    mkdir -p ${build_dir}
    cp -R ../taglib ${build_dir}
    cd ${build_dir}/taglib

    # Make the source distribution.
    make -f Makefile.cvs

    # Build and install.
    cmake ${CMAKE_ARGS}                                                        \
          -DCMAKE_C_FLAGS="${CMAKE_C_FLAGS}"                                   \
          -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS}"                               \
          -DCMAKE_C_FLAGS_DEBUG="${CMAKE_C_FLAGS_DEBUG}"                       \
          -DCMAKE_CXX_FLAGS_DEBUG="${CMAKE_CXX_FLAGS_DEBUG}"                   \
          -DCMAKE_C_FLAGS_RELEASE="${CMAKE_C_FLAGS_RELEASE}"                   \
          -DCMAKE_CXX_FLAGS_RELEASE="${CMAKE_CXX_FLAGS_RELEASE}"               \
          -DCMAKE_VERBOSE_MAKEFILE=1                                           \
          -DCMAKE_INSTALL_PREFIX=${dep_arch_dir}/${tgt_name}/${build_type}     \
          -DCMAKE_DEBUG_POSTFIX=                                               \
          -DZLIB_INCLUDE_DIR:PATH="${zlib_dir}/include"                        \
          -DZLIB_LIBRARY:FILEPATH="${zlib_dir}/lib/${ZLIB_LIBRARY}"            \
          -G "${CMAKE_MAKEFILE_TYPE}"
    ${CMAKE_MAKE_CMD} && ${CMAKE_MAKE_CMD} install

    # Post-process libraries on Mac.
    if [ "$build_sys_type" = "Darwin" ]; then
        install_name_tool                                                      \
            -id @executable_path/libtag.dylib                                  \
            ${dep_arch_dir}/${tgt_name}/${build_type}/lib/libtag.dylib
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


