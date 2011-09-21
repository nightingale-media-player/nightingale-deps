# NOTE: If you've already built once successfully, then don't use this
# script to repackage! Instead go to xulrunner and 
# make -f Makefile.songbird xr-packaging

# XUL Version
XUL="6.0.2"

# Top level build path
export SB_VENDOR_BUILD_ROOT=`pwd`

# xul source directory
if [ ! -d "xulrunner/mozilla" ] ; then
    mkdir "xulrunner/mozilla"
fi

# output directory
if [ ! -d linux\-`arch` ]; then
    mkdir linux\-`arch`
fi

# get the source files if this is a fresh build or new version
if [ ! -f "xulrunner/xulrunner-$XUL.source.tar.bz2" ] ; then
	cd "xulrunner"
	
	# remove any old source archives, if we're updating
	rm "xulrunner-*.source.tar.bz2" &> /dev/null
	
	wget "https://ftp.mozilla.org/pub/mozilla.org/xulrunner/releases/$XUL/source/xulrunner-$XUL.source.tar.bz2"
	
	tar xvf "xulrunner-$XUL.source.tar.bz2"
	
	# just in case the directory exists already
	rm -rf mozilla
	
	mv "mozilla-release" mozilla
else
	cd "xulrunner"
	
	rm -rf mozilla
	
	tar xvf "xulrunner-$XUL.source.tar.bz2"
	
	mv "mozilla-release" mozilla

fi

make -f Makefile.songbird

cp -a $SB_VENDOR_BUILD_ROOT/oldscripts/* $SB_VENDOR_BUILD_ROOT/linux\-`arch`/mozilla\-$XUL/scripts
