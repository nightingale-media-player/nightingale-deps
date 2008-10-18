#!/usr/bin/env python

#
# It turns out that make and nmake don't play well together (seemingly because
# gmake exports MAKEFLAGS and other things to the environment that nmake barfs
# on), so this is a simple python script to scrub the environment and call
# nmake in a way it likes. We also add support for the -C option, which nmake
# is clueless to, but which our vendor make framework uses.
#
# This code is based on an idea from Mozilla;
#    see mozilla/memory/jemalloc/build-crt.py
#

from optparse import OptionParser
import os, sys

def main(argv):
   o = OptionParser()
   o.add_option('-C', '--chdir', dest='runningDir', default=os.getcwd(),
                help="Directory to chdir into first; like gmake's -C")

   (options, args) = o.parse_args()

   nmakeCmd = 'nmake'
   if 'NMAKE' in os.environ:
     nmakeCmd = os.environ['NMAKE']

   if 'MAKEFLAGS' in os.environ:
      del os.environ['MAKEFLAGS']

   os.chdir(options.runningDir)
   return os.execvp(nmakeCmd, (nmakeCmd,) + tuple(args))

if __name__ == '__main__':
   sys.exit(main(sys.argv[1:]))
