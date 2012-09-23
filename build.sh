#!/bin/bash


export DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

export CC=gcc-4.7
export SB_VENDOR_BINARIES_CO_ROOT=$DIR
export SB_VENDOR_BUILD_ROOT=$DIR
export CXXFLAGS="-fpermissive"
export CFLAGS=$CXXFLAGS
export CCFLAGS=$CXXFLAGS

# Currently, we build both debug and release, which takes a lot longer,
# especially on xulrunner... TODO: make it optional to build debug

case $OSTYPE in
	# linux is easy, as all it requires right now is xulrunner, sqlite, and taglib
	# we'll get to a point where this is unnecessary on linux altogether in the future
	linux*)
	
		if [ ! -d "linux-$(uname -m)" ]; then
			mkdir -p "linux-$(uname -m)"
			mkdir -p "checkout/linux-$(uname -m)"
			mkdir "build"
		fi

		cd "xulrunner"
		if [ ! -d "xulrunner/mozilla" ]; then
			svn co "http://publicsvn.songbirdnest.com/vendor/trunk/xulrunner-${XUL}/mozilla/"
		fi

		# fix for kernels > 3.X on versions of xul without security setup for them
		if [ ! -f mozilla/security/coreconf/Linux$(uname -r|sed -e 's/\-.*//'|grep -o "[0-9]\.[0-9]").mk ]; then
			ln -s $(pwd)/mozilla/security/coreconf/Linux2.6.mk $(pwd)/mozilla/security/coreconf/Linux$(uname -r|sed -e 's/\-.*//'|grep -o "[0-9]\.[0-9]").mk
		fi
		
		cd ../
		echo -e "Building sqlite...\n"
		cd sqlite && make -f Makefile.songbird
		cd ../
		
		echo -e "Building taglib...\n"
		cd taglib && make -f Makefile.songbird
		cd ../
		
		echo -e "Building xulrunner 1.9.2...\n"
		cd xulrunner-1.9.2 && make -f Makefile.songbird
		make -f 
		cd ../
		
		echo -e "Done! Provided there were no errors, you can \nfind your deps in the linux-$(uname -m) directory. Copy or link it into [nightingale build directory]/dependencies and you're ready to build!\n"
	;;
	*)
		echo "Lazy buildscript for your OS coming soon."
	;;
esac
