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
# Script for building gst-plugins-base
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

tgt_name=gst-plugins-base


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
    sys_name="Windows"
    build_sys_type=Windows
    _MSVC_VER_FILTER='s|.* \([0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\).*|\1|p'
    CC_VERSION=`cl -v 2>&1 | sed -ne "$_MSVC_VER_FILTER"`
    _CC_MAJOR_VERSION=`echo ${CC_VERSION} | awk -F\. '{ print $1 }'`
    _CC_MINOR_VERSION=`echo ${CC_VERSION} | awk -F\. '{ print $2 }'`
    export _MSC_VER=${_CC_MAJOR_VERSION}${_CC_MINOR_VERSION}

    case "$CC_VERSION" in
        13.*)
            tgt_arch_list=windows-i686
            ;;
        *)
            tgt_arch_list=windows-i686-msvc8
            ;;
    esac
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
            ;;

        windows-i686-msvc8)
            # Determine MSVC version.
            tmp='s|.* \([0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\).*|\1|p'
            _MSVC_VER_FILTER=${tmp}

            # Set up to use the MSVC linker and compiler
            export LD=link
	    export CC=cl
	    export CXX=cl
	    export OBJDUMP=objdump

	    # cygpath tool
            cl_process="${dep_arch_dir}/mozilla/release/scripts/cygwin-wrapper"

	    # target
            cfg_tgt=i686-pc-mingw32

            ;;

        macosx-i686 | macosx-ppc)
            export CFLAGS="${CFLAGS} -fnested-functions -D__MACOSX__"
            export CFLAGS="${CFLAGS} -gstabs+"
            export LDFLAGS="${LDFLAGS} -headerpad_max_install_names"
	    export MACOSX_DEPLOYMENT_TARGET="10.4"
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

# remap_library_name name
#
# Remap the library name 'libgst${name}-0.10.dylib' to not refer to the build directory
remap_library_name()
{
  name="$1"
  fullname="libgst${name}-0.10.dylib"

  install_name_tool -id "$fullname" "${dep_arch_dir}/${tgt_name}/${build_type}/lib/${fullname}"
}

# remap_lib_dep_names name depname1 [...]
#
# Remap the dependent library names 'libgst${depname}-0.10.dylib' to not refer to the build directory
remap_lib_dep_names()
{
  libname="$1"
  libfullname="libgst${libname}-0.10.dylib"
  shift

  for dep in $@
  do
      depname="libgst${dep}-0.10"
      install_name_tool -change "${dep_arch_dir}/${tgt_name}/${build_type}/lib/${depname}.0.dylib" "${depname}.dylib" \
              "${dep_arch_dir}/${tgt_name}/${build_type}/lib/${libfullname}"
  done
}

# remap_plugin_dep_names name depname1 [...]
#
# Remap the dependent library names 'libgst${depname}-0.10.dylib' to not refer to the build directory
remap_plugin_dep_names()
{
  libname="$1"
  libfullname="libgst${libname}.so"
  shift

  for dep in $@
  do
      depname="libgst${dep}-0.10"
      install_name_tool -change "${dep_arch_dir}/${tgt_name}/${build_type}/lib/${depname}.0.dylib" "${depname}.dylib" \
              "${dep_arch_dir}/${tgt_name}/${build_type}/lib/gstreamer-0.10/${libfullname}"
  done
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
    export CPPFLAGS=
    export CFLAGS=
    export LDFLAGS=
    export ACLOCAL_FLAGS=
    export LIBOIL_CFLAGS=
    export LIBOIL_LIBS=
    export OGG_CFLAGS=
    export OGG_LIBS=
    export VORBIS_CFLAGS=
    export VORBIS_LIBS=
    export PKG_CONFIG_PATH=
    export DYLD_LIBRARY_PATH=/opt/local/lib:/usr/lib

    # Get the target architecture depedencies directory.
    dep_arch_dir=${dep_dir}/${tgt_arch}

    # Set up the build environment.
    setup_build ${tgt_arch}

    # If not using system libraries, set up build options for local versions.
    if ! $use_sys_libs; then

        # Set up gettext build options.
        tgt_dep_dir="${dep_arch_dir}/gettext/${build_type}"
        export_append "LIBS" "-L${tgt_dep_dir}/lib" "-lintl"
        export_append "CFLAGS" "-I${tgt_dep_dir}/include"
	export PATH="${tgt_dep_dir}/bin:${PATH}"

        # Set up iconv build options.
        tgt_dep_dir="${dep_arch_dir}/libiconv/${build_type}"
        export_append "LIBS" "-L${tgt_dep_dir}/lib" "-liconv"
        export_append "CFLAGS" "-I${tgt_dep_dir}/include"
	export PATH="${tgt_dep_dir}/bin:${PATH}"

        # Set up glib build options.
        tgt_dep_dir="${dep_arch_dir}/glib/${build_type}"
	export PATH="${tgt_dep_dir}/bin:${PATH}"
	export PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:${tgt_dep_dir}/lib/pkgconfig"

        # Set up liboil build options.
        tgt_dep_dir="${dep_arch_dir}/liboil/${build_type}"
        export_append "LIBOIL_LIBS"                                           \
                      "-L${tgt_dep_dir}/lib"                                   \
                      "-loil-0.3"
        export_append "LIBOIL_CFLAGS"                                         \
                      "-I${tgt_dep_dir}/include/liboil-0.3"
	export PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:${tgt_dep_dir}/lib/pkgconfig"
	if [ "$sys_name" = "Windows" ]; then
	    export PATH="$PATH:${tgt_dep_dir}/bin"
	    if [ "$build_type" = "debug" ]; then
		export LIBOIL_LIBS="$LIBOIL_LIBS -Wl,-Zi"
	    fi
        fi

        # Set up gstreamer build options.
        tgt_dep_dir="${dep_arch_dir}/gstreamer/${build_type}"
	export PATH="${tgt_dep_dir}/bin:${PATH}"
	export PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:${tgt_dep_dir}/lib/pkgconfig"

        # Set up libogg build options.
        tgt_dep_dir="${dep_arch_dir}/libogg/${build_type}"
	if [ "$build_type" = "debug" -a $sys_name = "Windows" ]; then
	    # debug builds on windows use "ogg_d.dll" instead of "ogg.dll"
            export_append "OGG_LIBS"                                           \
                          "-L${tgt_dep_dir}/lib"                                   \
                          "-logg_d"
	else
            export_append "OGG_LIBS"                                           \
                          "-L${tgt_dep_dir}/lib"                                   \
                          "-logg"
	fi
        export_append "OGG_CFLAGS"                                         \
                      "-I${tgt_dep_dir}/include"
	# Our pkgconfig scripts are currently broken on OSX; the above definitions are ok though.
	if [ "$sys_name"  != "Darwin" ]; then
	    export PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:${tgt_dep_dir}/lib/pkgconfig"
	fi

	if [ "$sys_name" = "Windows" ]; then
	    export PATH="$PATH:${tgt_dep_dir}/bin"
	    if [ "$build_type" = "debug" ]; then
		export OGG_LIBS="$OGG_LIBS -Wl,-Zi"
	    fi
        fi

        # Set up libvorbis build options.
        tgt_dep_dir="${dep_arch_dir}/libvorbis/${build_type}"
        export_append "VORBIS_LIBS"                                           \
                      "-L${tgt_dep_dir}/lib"                                   \
                      "-lvorbis"				\
	    "-lvorbisenc"
	export VORBIS_LIBS="${VORBIS_LIBS} ${OGG_LIBS}"
        export_append "VORBIS_CFLAGS"                                         \
                      "-I${tgt_dep_dir}/include"
	export VORBIS_CFLAGS="${VORBIS_CFLAGS} ${OGG_CFLAGS}"
	export PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:${tgt_dep_dir}/lib/pkgconfig"
	if [ "$sys_name" = "Windows" ]; then
	    export PATH="$PATH:${tgt_dep_dir}/bin"
	    if [ "$build_type" = "debug" ]; then
		export VORBIS_LIBS="$VORBIS_LIBS -Wl,-Zi"
	    fi
        fi

    fi

    # Set up libtool.
    export PATH="${dep_arch_dir}/libtool/release/bin:${PATH}"
    export_append "ACLOCAL_FLAGS"                                              \
                  "-I ${dep_arch_dir}/libtool/release/share/aclocal"

    # add win32 specific flags
    if [ "$sys_name" = "Windows" ]; then
	export CFLAGS="${CFLAGS} -D_MSC_VER=${_MSC_VER} -DWIN32 -D__NO_CTYPE -D_CRT_SECURE_NO_WARNINGS  -DHAVE_WIN32 -D_WINDOWS -wd4820 -wd4668 -wd4100 -wd4706 -wd4127 -wd4255 -wd4710 -wd4055"
	if [ "$build_type" = "debug" ]; then
	    export CFLAGS="${CFLAGS} -MTd -Zi"
	else
	    export CFLAGS="${CFLAGS} -MT"
	fi
	export CFLAGS="${CFLAGS} -DYY_NO_UNISTD_H"
	GST_PARSE_LA="parse/libgstparse_la-lex._gst_parse_yy.lo parse/libgstparse_la-grammar.tab.lo"
    fi

    # Apply command line options pre-processing.
    if test -n "${cl_process}"; then
        export CFLAGS=`${cl_process} echo ${CFLAGS}`
        export LIBS=`${cl_process} echo ${LIBS}`
        export LIBOIL_CFLAGS=`${cl_process} echo ${LIBOIL_CFLAGS}`
        export LIBOIL_LIBS=`${cl_process} echo ${LIBOIL_LIBS}`
        export OGG_CFLAGS=`${cl_process} echo ${OGG_CFLAGS}`
        export OGG_LIBS=`${cl_process} echo ${OGG_LIBS}`
        export VORBIS_CFLAGS=`${cl_process} echo ${VORBIS_CFLAGS}`
        export VORBIS_LIBS=`${cl_process} echo ${VORBIS_LIBS}`
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

    # Generate, configure, build, and install.
    echo cfg_opts
    export NOCONFIGURE="yes"
    ./configure --prefix=${dep_arch_dir}/${tgt_name}/${build_type}             \
                ${cfg_opts}                                                    \
	--disable-examples \
	--disable-tests \
	-C
    make && make install

    # Post-process libraries on Mac.
    if [ "$sys_name" = "Darwin" ]; then
	for lib in audio cdda fft interfaces netbuffer pbutils riff rtp rtsp sdp tag video
	do
	    remap_library_name $lib
	done

	remap_lib_dep_names audio interfaces
        remap_lib_dep_names cdda tag
	remap_lib_dep_names riff interfaces tag audio

	remap_plugin_dep_names audioconvert audio interfaces
	remap_plugin_dep_names decodebin pbutils
	remap_plugin_dep_names decodebin2 pbutils
	remap_plugin_dep_names ogg riff tag audio interfaces
	remap_plugin_dep_names playbin pbutils
	remap_plugin_dep_names videoscale video
	remap_plugin_dep_names volume audio interfaces
	remap_plugin_dep_names vorbis tag audio interfaces
    fi

    # Build the symbols.
    buildsymbols

    # Strip symbols from compiled files.
    if [ "${build_type}" = "release" ]; then
	strip ${dep_arch_dir}/${tgt_name}/${build_type}
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


