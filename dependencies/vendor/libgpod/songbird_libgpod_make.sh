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
#   tgt_src_dir                 Target source directory.
#   tools_dir                   Songbird tools directory.
#

dep_dir=${PWD}/../..
tgt_src_dir=${dep_dir}/vendor/${tgt_name}
tools_dir=${dep_dir}/../tools


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

    # Get the target source version.
    tgt_ver=`svn info ${tgt_src_dir} | grep ^Revision: | sed 's/^Revision: //'`

    # Set up default environment.
    use_sys_libs=false;

    # Set up the mozilla directory structure.
    mozsdk_dir=${dep_arch_dir}/mozilla/${build_type}
    mozsdk_bin_dir=${mozsdk_dir}/bin
    mozsdk_scripts_dir=${mozsdk_dir}/scripts

    # Set up tool defs.
    python=python

    # Set up the build environment for the given target.
    case "${build_tgt_arch}" in

        linux-i686 | linux-x86_64)
            export CPPFLAGS="${CPPFLAGS} -fPIC"
            export CPPFLAGS="${CPPFLAGS} -freorder-blocks"
            export CPPFLAGS="${CPPFLAGS} -fno-reorder-functions -gstabs+"
            use_sys_libs=true
            ;;

        windows-i686)
            export CPPFLAGS="${CPPFLAGS} -MD -DWIN32 -gstabs+"
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

            # Determine MSVC version.
            tmp='s|.* \([0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\).*|\1|p'
            _MSVC_VER_FILTER=${tmp}

            CC_VERSION=`cl -v 2>&1 | sed -ne "$_MSVC_VER_FILTER"`
            _CC_MAJOR_VERSION=`echo ${CC_VERSION} | awk -F\. '{ print $1 }'`
            _CC_MINOR_VERSION=`echo ${CC_VERSION} | awk -F\. '{ print $2 }'`
            _MSC_VER=${_CC_MAJOR_VERSION}${_CC_MINOR_VERSION}
            ;;

        macosx-i686 | macosx-ppc)
            export CFLAGS="${CFLAGS} -fnested-functions"
            export CFLAGS="${CFLAGS} -gstabs+"
            export LDFLAGS="${LDFLAGS} -headerpad_max_install_names"
            ;;

    esac
}


#
# buildsymbols
#
#   This function builds the breakpad symbol files for the target.
#

buildsymbols()
{
    # Set up breakpad file structure.
    breakpad_dir=${dep_arch_dir}/${tgt_name}/breakpad/${build_type}
    breakpad_symbol_path=${breakpad_dir}/breakpad-symbols
    breakpad_archive_path=${breakpad_dir}
    tmp=${breakpad_dir}/${tgt_name}-symbols-${tgt_ver}-${tgt_arch}.zip
    breakpad_archive_filepath=${tmp}
    breakpad_store_path=./

    # Set up breakpad options.
    breakpad_sym_store_args=--vcs-info
    if [ "$sys_name" = "Darwin" ]; then
        if [ "$mach_name" = "i386" ]; then
            breakpad_sym_store_args="${breakpad_sym_store_args} -a i386"
        else
            breakpad_sym_store_args="${breakpad_sym_store_args} -a ppc"
        fi
        dump_syms_bin=${mozsdk_bin_dir}/dump_syms
    elif [ "$sys_name" = "Linux" ]; then
        dump_syms_bin=${mozsdk_bin_dir}/dump_syms
    else
        if [ "${_MSC_VER}" = "1400" ]; then
            dump_syms_bin=${mozsdk_bin_dir}/dump_syms.exe
        else
            dump_syms_bin=${tools_dir}/win32/breakpad/dump_syms-vc71.exe
        fi
    fi

    # Generate the breakpad symbols.
    mkdir -p ${breakpad_symbol_path}
    ${python} ${mozsdk_scripts_dir}/symbolstore.py                             \
              ${breakpad_sym_store_args}                                       \
              -s ${dep_arch_dir}/${tgt_name}/${build_type}/lib                 \
              ${dump_syms_bin}                                                 \
              ${breakpad_symbol_path}                                          \
              ${breakpad_store_path}                                           \
              > ${breakpad_symbol_path}/${tgt_name}-symbols.txt

    # Package symbols into a zip file.
    cd ${breakpad_symbol_path}
    zip -r9D ${breakpad_archive_filepath} .
}


#
# strip
#
#   --> strip_dir               Directory containing the compiled files to
#                               strip.
#
#   This function strips symbol information from the compiled files residing in
# the directory specified by strip_dir.
#

strip()
{
    # Get the directory in which to strip.
    strip_dir=$1;
    shift

    # Set up the strip options.
    if [ "$sys_name" = "Darwin" ]; then
        strip="strip -x -S"
    elif [ "$sys_name" = "Linux" ]; then
        strip="strip -v"
    else
        # Strip not needed for Windows.
        return
    fi

    # Strip all of the compiled files.
    find -L                                                                    \
         ${strip_dir}                                                          \
         ! -type d                                                             \
         -and \(    -name "*.dylib"                                            \
                -or -name "*.so"                                               \
                -or -name "*.dll"                                              \
                -or -name "*.lib"                                              \
              \)                                                               \
         -and -exec ${strip} {} \;
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
        tgt_dep_dir="${dep_arch_dir}/gettext/${build_type}"
        export_append "LIBGPOD_LIBS" "-L${tgt_dep_dir}/lib" "-lintl"
        export_append "LIBGPOD_CFLAGS" "-I${tgt_dep_dir}/include"
        if [ "$sys_name" = "Darwin" ]; then
            export_append "LD_FLAGS"                                           \
                          "-dylib_file"                                        \
                          "libintl.dylib:${tgt_dep_dir}/lib/libintl.dylib"
            export DYLD_LIBRARY_PATH="${DYLD_LIBRARY_PATH}:${tgt_dep_dir}/lib"
        fi

        # Set up glib build options.
        tgt_dep_dir="${dep_arch_dir}/glib/${build_type}"
        export_append "LIBGPOD_LIBS"                                           \
                      "-L${tgt_dep_dir}/lib"                                   \
                      "-lglib-2.0"                                             \
                      "-lgobject-2.0"
        export_append "LIBGPOD_CFLAGS"                                         \
                      "-I${tgt_dep_dir}/include/glib-2.0"                      \
                      "-I${tgt_dep_dir}/lib/glib-2.0/include"
        if [ "$sys_name" = "Darwin" ]; then
            export_append                                                      \
                        "LDFLAGS"                                              \
                        "-dylib_file"                                          \
                        "libglib-2.0.dylib:${tgt_dep_dir}/lib/libglib-2.0.dylib"
            export_append                                                      \
                "LDFLAGS"                                                      \
                "-dylib_file"                                                  \
                "libgobject-2.0.dylib:${tgt_dep_dir}/lib/libgobject-2.0.dylib"
            export DYLD_LIBRARY_PATH="${DYLD_LIBRARY_PATH}:${tgt_dep_dir}/lib"
        fi

        # Set up iconv build options.
        tgt_dep_dir="${dep_arch_dir}/libiconv/${build_type}"
        export_append "LIBGPOD_LIBS" "-L${tgt_dep_dir}/lib" "-liconv"
        export_append "LIBGPOD_CFLAGS" "-I${tgt_dep_dir}/include"
        if [ "$sys_name" = "Darwin" ]; then
            export_append "LD_FLAGS"                                           \
                          "-dylib_file"                                        \
                          "libiconv.dylib:${tgt_dep_dir}/lib/libiconv.dylib"
            export DYLD_LIBRARY_PATH="${DYLD_LIBRARY_PATH}:${tgt_dep_dir}/lib"
        fi
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

    # Post-process libraries on Mac.
    if [ "$sys_name" = "Darwin" ]; then
        install_name_tool                                                      \
            -id libgpod.dylib                                                  \
            ${dep_arch_dir}/${tgt_name}/${build_type}/lib/libgpod.dylib
    fi

    # Build the symbols.
    buildsymbols

    # Strip symbols from compiled files.
    strip ${dep_arch_dir}/${tgt_name}/${build_type}

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


