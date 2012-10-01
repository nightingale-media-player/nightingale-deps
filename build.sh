#TODO: put the finished archive in {os}_arch/{release|debug}/xulrunner.tar.bz2 rather than just in the file root!

# NOTE: If you've already built once successfully, then don't use this
# script to repackage! Instead go to xulrunner and 
# make -f Makefile.songbird xr-packaging

set -e

# Use HG or TARBALL to fetch the sources?
# Really, just use whichever suits you or is fastest
USE="TARBALL"

# XUL Version
export XUL="15.0.1"

# Top level build path
export SB_VENDOR_BUILD_ROOT="$(cd "$(dirname "$0")" && pwd)"

# output directory
mkdir -p "linux-$(uname -m)"

cd "xulrunner"
# just in case the directory exists already
#rm -rf mozilla

get_bundle() {
  [ -f mozilla-release.hg ] && {
    [ -f mozilla-release.old.hg ] && {
      rm mozilla-release.old.hg
    }
    mv mozilla-release.hg mozilla-release.old.hg
  }
  # if you have axel or aria2c installed, i'd suggest using it here to speed up the download
  wget -O mozilla-release.hg ftp://ftp.mozilla.org/pub/mozilla.org/firefox/bundles/mozilla-release.hg
}

use_hg() {
  [ -d mozilla ] || {
    # mozilla directory does not exist; get the source code
    mkdir -p mozilla
    hg --cwd mozilla init
    # make sure we have a bundle available
    [ -f mozilla-release.hg ] || get_bundle
    # unbundle
    hg --cwd mozilla unbundle ../mozilla-release.hg
    # make sure we have the release we want
    hg --cwd mozilla tags | grep -F "FIREFOX_${XUL//./_}_RELEASE" || {
      # perhaps the bundle was too old; grab a new one
      get_bundle
      hg --cwd mozilla unbundle ../mozilla-release.hg
    }
    # remember to checkout
    hg co "FIREFOX_${XUL//./_}_RELEASE"
  }
}

use_tarball() {
	if [ ! -f "xulrunner-$XUL.source.tar.bz2" ] ; then	
	    # if you have axel or aria2c installed, i'd suggest using it here to speed up the download	
		wget "https://ftp.mozilla.org/pub/mozilla.org/xulrunner/releases/$XUL/source/xulrunner-$XUL.source.tar.bz2"
		tar xvf "xulrunner-$XUL.source.tar.bz2"
		# just in case the directory exists already
		rm -rf mozilla
		mv "mozilla-release" mozilla
	else
		rm -rf mozilla
		tar xvf "xulrunner-$XUL.source.tar.bz2"
		mv "mozilla-release" mozilla
	fi
}

if [ $USE = "HG" ] ; then
	use_hg
elif [ $USE = "TARBALL" ] ; then
	use_tarball
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

cp -a "$SB_VENDOR_BUILD_ROOT/oldscripts/"* "$SB_VENDOR_BUILD_ROOT/linux-$(arch)/mozilla-$XUL/scripts"
