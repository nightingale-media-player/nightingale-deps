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
#   tgt_arch_list               List of build target architectures.
#

sys_name=`uname`
mach_name=`uname -m`
if [ "$sys_name" = "Darwin" ]; then
    build_sys_type=Darwin
    if [ "$mach_name" = "i386" ]; then
        tgt_arch_list="macosx-i686 macosx-ppc"
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
            export CPPFLAGS="-fPIC"
            ;;

        windows-i686)
            export CPPFLAGS="-MT"
            export CC="cl"
            export CXX="cl"
            export LD="link"
            ;;

        macosx-ppc)
            cfg_tgt=i686-apple-darwin8.0.0
            export CFLAGS="-arch ppc"
            export CXXFLAGS="-arch ppc"
            export LDFLAGS="-Wl,-arch,ppc"
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

    # Set up internal taglib include paths and build defs.
    #XXXeps taglib makefiles should do this.
    export CPPFLAGS="${CPPFLAGS} -I${dep_arch_dir}/taglib/build/taglib/taglib"
    export CPPFLAGS="${CPPFLAGS} -DMAKE_TAGLIB_LIB"

    # Set up zlib build options.
    export LDFLAGS="${LDFLAGS} -L${dep_arch_dir}/zlib/${build_type}/lib"
    export CPPFLAGS="${CPPFLAGS} -I${dep_arch_dir}/zlib/${build_type}/include"
    if [ "${tgt_arch}" = "windows-i686" ]; then
        export LDFLAGS=`${dep_arch_dir}/mozilla/release/scripts/cygwin-wrapper \
                        echo ${LDFLAGS}`
        export CPPFLAGS=`${dep_arch_dir}/mozilla/release/scripts/cygwin-wrapper\
                         echo ${CPPFLAGS}`
    fi

    # Set up to build within a clean build directory.
    build_dir=${dep_arch_dir}/${tgt_name}/build
    rm -Rf ${build_dir}
    mkdir -p ${build_dir}
    cp -R ../taglib ${build_dir}
    cd ${build_dir}/taglib

    # Determine the debug options.
    if [ "${build_type}" = "debug" ]; then
        cfg_opts="${cfg_opts} --enable-debug=yes"
    else
        cfg_opts="${cfg_opts} --enable-debug=no"
    fi

    # Enable use of zlib.
    export ac_cv_header_zlib_h=yes

    # Set up the target configuration options.
    if test -n "${cfg_tgt}"; then
        cfg_opts="${cfg_opts} --target=${cfg_tgt}"
        cfg_opts="${cfg_opts} --host=${cfg_tgt}"
    fi

    # Make the source distribution.
    make -f Makefile.cvs

    # Configure, build, and install.
    ./configure --prefix=${dep_arch_dir}/${tgt_name}/${build_type}             \
                ${cfg_opts}                                                    \
                --disable-libsuffix                                            \
                --enable-static                                                \
                --disable-shared                                               \
                --enable-cxx-warnings=no
    make && make install

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


