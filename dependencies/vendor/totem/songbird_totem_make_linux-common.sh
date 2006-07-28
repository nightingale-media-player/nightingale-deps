#!/bin/bash
echo "Building for architecture '$SB_ARCH' for configuration '$SB_CONFIGURATION'"

DEPS="`pwd`/../../$SB_ARCH"
TMP_PKGCONFIG=/tmp/totem-pkgconfig

echo "Deps dir is '$DEPS'"

mkdir -p $TMP_PKGCONFIG
sed -e "s,@DEPS@,$DEPS,g" < pkgconfig/xulrunner-xpcom.pc.in  > $TMP_PKGCONFIG/xulrunner-xpcom.pc
sed -e "s,@DEPS@,$DEPS,g" < pkgconfig/xulrunner-plugin.pc.in > $TMP_PKGCONFIG/xulrunner-plugin.pc

if [ "$SB_CONFIGURATION" == "release" ]; then

PKG_CONFIG_PATH=$TMP_PKGCONFIG \
ac_cv_path_MOZILLA_XPIDL=$DEPS/mozilla/$SB_CONFIGURATION/bin/xpidl \
./configure \
  --enable-gstreamer=0.10 \
  --enable-mozilla=yes \
  --with-mozilla=xulrunner

else

PKG_CONFIG_PATH=$TMP_PKGCONFIG \
ac_cv_path_MOZILLA_XPIDL=$DEPS/mozilla/$SB_CONFIGURATION/bin/xpidl \
./configure \
  --enable-gstreamer=0.10 \
  --enable-mozilla=yes \
  --with-mozilla=xulrunner \
  --enable-debug

fi

make
mkdir -p $DEPS/plugins/totem/$SB_CONFIGURATION/components
mkdir -p $DEPS/plugins/totem/$SB_CONFIGURATION/plugins
mkdir -p $DEPS/plugins/totem/$SB_CONFIGURATION/bin
mkdir -p $DEPS/plugins/totem/$SB_CONFIGURATION/share

cp src/libtotem_mozilla.xpt $DEPS/plugins/totem/$SB_CONFIGURATION/components
cp src/.libs/libtotem_mozilla.so $DEPS/plugins/totem/$SB_CONFIGURATION/plugins
cp src/totem-mozilla-viewer $DEPS/plugins/totem/$SB_CONFIGURATION/bin
cp data/mozilla-viewer.glade $DEPS/plugins/totem/$SB_CONFIGURATION/share

