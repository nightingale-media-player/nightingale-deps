# NOTE: If you've already built once successfully, then don't use this
# script to repackage! Instead go to xulrunner and 
# make -f Makefile.songbird xr-packaging

set -e

# Use HG or TARBALL to fetch the sources?
# Really, just use whichever suits you or is fastest
USE="TARBALL"

# XUL Version
export XUL="1.9.2"

# Top level build path
export SB_VENDOR_BUILD_ROOT="$(cd "$(dirname "$0")" && pwd)"

# output directory
mkdir -p "linux-$(arch)"

# sqlite
cd sqlite
make
cd ../

# taglib
cd taglib
make
cd ../

cd "xulrunner"
if [ ! -f "xulrunner/mozilla" ]; then
	svn co "http://publicsvn.songbirdnest.com/vendor/trunk/xulrunner-${XUL}/mozilla/"
fi

# fix for kernels > 3.X on versions of xul without security setup for them
case $OSTYPE in
	linux*)
		if [ ! -f mozilla/security/coreconf/Linux$(uname -r|sed -e 's/\-.*//'|grep -o "[0-9]\.[0-9]").mk ]; then
			ln -s $(pwd)/mozilla/security/coreconf/Linux2.6.mk $(pwd)/mozilla/security/coreconf/Linux$(uname -r|sed -e 's/\-.*//'|grep -o "[0-9]\.[0-9]").mk
		fi
	;;
	*)
		# weee! xulrunner already has security coreconf for our kernel!
	;;
esac

# build Xulrunner
make
