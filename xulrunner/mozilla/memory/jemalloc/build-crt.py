#!/usr/bin/env python

import os, sys

if 'MAKEFLAGS' in os.environ:
  del os.environ['MAKEFLAGS']
os.chdir(sys.argv[1])

os.environ['MOZ_MEMORY'] = "1"

if ('MOZ_DEBUG' in os.environ and os.environ['MOZ_DEBUG'] == "1"):
        makeRv = os.system('nmake xdll_ xmt')
else:
        makeRv = os.system('nmake dll_ mt')

sys.exit(makeRv)
