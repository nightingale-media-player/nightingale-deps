# output directory
if [ ! -d linux\-`arch` ]; then
    mkdir linux\-`arch`
fi

# build directory
if [ ! -d build ] ; then
    mkdir build
fi

# build path ...just because we're lazy 
# and don't want to rewrite this whole build system
export SB_VENDOR_BUILD_ROOT=`pwd`

# change this if we do a version bump
cd xulrunner-6.0.2
make -f Makefile.songbird
