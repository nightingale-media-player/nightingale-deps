# NOTE: If you've already built once successfully, then don't use this
# script to repackage! Instead go to xulrunner and 
# make -f Makefile.songbird xr-packaging

set -e

# XUL Version
XUL="6.0.2"

# Top level build path
export SB_VENDOR_BUILD_ROOT="$(cd "$(dirname "$0")" && pwd)"

# output directory
mkdir -p "linux-$(arch)"

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
  wget -O mozilla-release.hg ftp://ftp.mozilla.org/pub/mozilla.org/firefox/bundles/mozilla-release.hg
}

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

make -f Makefile.songbird

cp -a "$SB_VENDOR_BUILD_ROOT/oldscripts/"* "$SB_VENDOR_BUILD_ROOT/linux-$(arch)/mozilla-$XUL/scripts"
