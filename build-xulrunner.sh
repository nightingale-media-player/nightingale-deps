# NOTE: If you've already built once successfully, then don't use this
# script to repackage! Instead go to xulrunner-$XUL and 
# make -f Makefile.songbird xr-packaging

# XUL Version
XUL="6.0.2"

# output directory
if [ ! -d linux\-`arch` ]; then
    mkdir linux\-`arch`
fi

# build directory
if [ ! -d build ] ; then
    mkdir build
fi

# xul source directory
if [ ! -d "xulrunner-$XUL/mozilla" ] ; then
    mkdir "xulrunner-$XUL/mozilla"
fi

# get the source files if this is a fresh build or new version
if [ ! -f "xulrunner-$XUL/xulrunner-$XUL.source.tar.bz2" ] ; then
	cd "xulrunner-$XUL"
	
	wget "https://ftp.mozilla.org/pub/mozilla.org/xulrunner/releases/$XUL/source/xulrunner-$XUL.source.tar.bz2"
	
	tar xvf "xulrunner-$XUL.source.tar.bz2"
	
	# just in case the directory exists already
	rm -rf mozilla
	
	mv "mozilla-release" mozilla
	
	cd ../../
else
	cd "xulrunner-$XUL"
	
	rm -rf mozilla
	
	tar xvf "xulrunner-$XUL.source.tar.bz2"
	
	mv "mozilla-release" mozilla

fi

# build path ...just because we're lazy 
# and don't want to rewrite this whole build system
export SB_VENDOR_BUILD_ROOT=`pwd`

cd "xulrunner-$XUL"
make -f Makefile.songbird
