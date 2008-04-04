#!/usr/bin/env python

import os
import os.path
import sys
import subprocess
import tempfile

def prepare_ar_hack():
	tmpfd, tmpname = tempfile.mkstemp()
	# get the list of contained files
	args = ["ar", "t", sys.argv[2]]
	ret = subprocess.call(args, stdout=tmpfd)
	files=[x.strip() for x in open(tmpname).readlines()]
	for fname in files:
		fpath = os.path.dirname(os.path.abspath(fname))
		if not os.path.exists(fpath):
			os.makedirs(fpath)
	os.close(tmpfd)		

if __name__ == "__main__":
	if len(sys.argv) > 1:
		instruction = sys.argv[1]
		if instruction == "x":
			prepare_ar_hack()
	# execute ar	
	fargs = sys.argv[:]
	fargs[0] = "ar"
	sys.exit(subprocess.call(fargs))