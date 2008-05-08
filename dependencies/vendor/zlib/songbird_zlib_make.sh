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
# Script for building zlib.
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

tgt_name=zlib


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
        if [ "$mach_name" = "x86_64" ]; then
            tgt_arch_list=linux-x86_64
        else
            tgt_arch_list=linux-i686
        fi
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
#   install_dir                 Directory in which to install build targets.
#

install_dir=${PWD}/../..


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
        # Set up the build environment.
        setup_build ${tgt_arch}

        # Build release target.
        build release

        # Build debug target.
        build debug
    done
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

    # Set up the build environment for the given target.
    case "${build_tgt_arch}" in

        linux-i686 | linux-x86_64)
            CPPFLAGS="-fPIC"
            ;;

        solaris-*)
            CC=cc
            CXX=CC
            ;;

        windows-i686)
            CPPFLAGS="-MT"
            ;;

        macosx-ppc)
            cfg_tgt=i686-apple-darwin8.0.0
            CFLAGS="-arch ppc"
            CXXFLAGS="-arch ppc"
            LDFLAGS="-Wl,-arch,ppc"
            ;;

    esac

    export CPPFLAGS CC CXX CFLAGS LDFLAGS
}


#
# build_gcc
#
#   This function builds the target using the gcc tool chain.
#

build_gcc()
{
    # Configure, build, and install.
    ./configure --prefix=${install_dir}/${tgt_arch}/${tgt_name}/${build_type}  \
                ${cfg_opts}
    make && make install
}


#
# build_msvc
#
#   This function builds the target using the Microsoft Visual C tool chain.
#

build_msvc()
{
    # Get the target install directory.
    tgt_install_dir=${install_dir}/${tgt_arch}/${tgt_name}/${build_type}

    # Make the target.
    nmake -f win32/Makefile.msc

    # Install the target.
    mkdir -p ${tgt_install_dir}/include
    mkdir -p ${tgt_install_dir}/lib
    cp ${build_dir}/zlib/zconf.h ${tgt_install_dir}/include
    cp ${build_dir}/zlib/zlib.h ${tgt_install_dir}/include
    cp ${build_dir}/zlib/zlib.lib ${tgt_install_dir}/lib
}


#
# build
#
#   --> build_type              Type of build (release or debug).
#
#   This function runs the configured build of the type specified by build_type.
#zzz add debug support
#

build()
{
    # Read the function parameters.
    build_type="$1"

    # Get starting directory.
    start_dir=${PWD}

    # Set up to build within a clean build directory.
    build_dir=${install_dir}/${tgt_arch}/${tgt_name}/build
    rm -Rf ${build_dir}
    mkdir -p ${build_dir}
    cp -R ../zlib ${build_dir}
    cd ${build_dir}/zlib

    # Configure, build, and install.
    if [ "${tgt_arch}" = "windows-i686" ]; then
        build_msvc
    else
        build_gcc
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


