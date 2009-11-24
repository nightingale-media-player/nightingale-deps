#!/bin/sh
# Run this to generate all the initial makefiles, etc.

# aclocal
if test -f acinclude.m4; then rm acinclude.m4; fi

libtoolize --copy --force
aclocal -I common/m4 -I . $ACLOCAL_FLAGS
autoheader

autoconf

automake --add-missing --copy

