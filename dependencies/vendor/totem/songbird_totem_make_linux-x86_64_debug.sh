#!/bin/bash
DEPS="`pwd`/../../linux-x86_64"
TMP_PKGCONFIG=/tmp/totem-pkgconfig

mkdir -p $TMP_PKGCONFIG
sed -e "s,@DEPS@,$DEPS,g" < pkgconfig/xulrunner-xpcom.pc.in  > $TMP_PKGCONFIG/xulrunner-xpcom.pc
sed -e "s,@DEPS@,$DEPS,g" < pkgconfig/xulrunner-plugin.pc.in > $TMP_PKGCONFIG/xulrunner-plugin.pc

PKG_CONFIG_PATH=$TMP_PKGCONFIG \
ac_cv_path_MOZILLA_XPIDL=$DEPS/mozilla/debug/bin/xpidl \
./configure \
  --enable-gstreamer=0.10 \
  --enable-mozilla=yes \
  --with-mozilla=xulrunner \
  --enable-debug

make

cp src/libtotem_mozilla.xpt $DEPS/plugins/totem/debug/components
cp src/.libs/libtotem_mozilla.so $DEPS/plugins/totem/debug/plugins
cp src/totem-mozilla-viewer $DEPS/plugins/totem/debug/bin

