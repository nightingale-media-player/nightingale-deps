#!/usr/bin/env python

import os
import os.path
import sys
import subprocess

def winpath(unixpath):
    """ returns the windows version of a unixpath """
    if unixpath.startswith("/cygdrive/c"):
        return unixpath.replace("/cygdrive/c", "c:")
    if unixpath[0] == "/" and unixpath[2] == "/" and os.path.exists(unixpath[1] + ":"):
        return unixpath[1] + ":" + unixpath[2:]
    return unixpath

if __name__ == "__main__":
    if sys.argv[1].startswith("-cr"):
        fargs = ["lib"] + sys.argv[2:]
    else:
        fargs = ["lib"] + sys.argv[1:]

    fargs[1] = "-OUT:%s" % (fargs[1])

    print "Calling ", fargs

    sys.exit(subprocess.call(fargs))
