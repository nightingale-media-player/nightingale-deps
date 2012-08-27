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
mkdir -p "darwin"

# on OSX, we want 32 bit builds
arch_flags="-m32 -arch i386"
export CFLAGS="$arch_flags" 
export CXXFLAGS="$arch_flags" 
export CPPFLAGS="$arch_flags"
export LDFLAGS="$arch_flags" 
export OBJCFLAGS="$arch_flags"

cd "xulrunner"
if [ ! -d "xulrunner/mozilla" ]; then
	svn co "http://publicsvn.songbirdnest.com/vendor/trunk/xulrunner-${XUL}/mozilla/"
else
	cd "xulrunner/mozilla"
	svn up
fi

# build Xulrunner
make release
