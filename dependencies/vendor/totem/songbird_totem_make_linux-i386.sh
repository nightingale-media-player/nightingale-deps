#!/bin/bash

# target architecture
export SB_ARCH=linux

# debug or release
export SB_CONFIGURATION=debug

make clean
rm -f config.log config.status Makefile

. songbird_totem_make_linux-common.sh

make clean
rm -f config.log config.status Makefile

export SB_CONFIGURATION=release

. songbird_totem_make_linux-common.sh

make clean
rm -f config.log config.status Makefile

