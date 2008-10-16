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
#   tgt_arch                    Build target architecture.
#zzz should build for both i686 and ppc on Mac
#

sys_name=`uname`
mach_name=`uname -m`
if [ "$sys_name" = "Darwin" ]; then
    build_sys_type=Darwin
    if [ "$mach_name" = "i386" ]; then
        tgt_arch=macosx-i686
    else
        tgt_arch=macosx-ppc
    fi
elif [ "$sys_name" = "Linux" ]; then
    build_sys_type=Linux
    if [ "$mach_name" = "x86_64" ]; then
        tgt_arch=linux-x86_64
    else
        tgt_arch=linux-i686
    fi
else
    build_sys_type=Cygwin
    tgt_arch=windows-i686
fi


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

build_all()
{
    # Build release target.
    build release

    # Build debug target.
    build debug
}


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
    cp -R ../taglib ${build_dir}
    cd ${build_dir}/taglib

    # Determine the debug options.
    if [ "${build_type}" = "debug" ]; then
        debug_option=--enable-debug=yes
    else
        debug_option=--enable-debug=no
    fi

    # Build a multi-threaded library.
    #zzz msvc specific, fix it
    export CPPFLAGS="/MT"

    # Configure, build, and install.
    ./configure --prefix=${install_dir}/${tgt_arch}/${tgt_name}/${build_type}  \
                ${debug_option}                                                \
                --enable-static                                                \
                --disable-shared                                               \
                --enable-cxx-warnings=no
    make
    make install

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


