#!/usr/bin/env python

import os
import os.path
import sys
import subprocess

def winpath(unixpath):
	""" returns the windows version of a unixpath """
	if unixpath.startswith("/cygdrive/c"):
		return unixpath.replace("/cygdrive/c", "c:")
	return unixpath

if __name__ == "__main__":
	fargs = sys.argv[:]
	fargs[0] = "cl"
	# go over all arguments
	# transform '-o output' into '-Fo:output'
	for i in sys.argv:
		print "\t", i
	if "-o" in sys.argv:
		pos = sys.argv.index("-o")
		outname = sys.argv[pos+1]
		# copy over the beginning
		fargs = sys.argv[:pos]
		# use proper argument dependening on outname
		if outname.endswith(".obj") or outname.endswith(".lo"):
			fargs.append("-Fo%s" % outname)
		elif outname.endswith(".exe"):
			fargs.append("-Fe%s" % outname)
		fargs.extend(sys.argv[pos+2:])
		if outname.endswith(".dll"):
			# we're linking, last options are linker options
			fargs.append("-out:%s" % outname)
			# let's create the .def file from outname's/.dll/.def/'
			#expname = outname.rsplit('-', 1)[0] + ".exp"
			#expname = outname.replace(".dll", ".exp")
			#print "Checking for existence of export file %s" % expname
			#if os.path.isfile(expname):
			if False:
				print "We have an export file, making a .def file for msvc linker"
				defname = outname.replace(".dll", ".def")
				expfile = file(expname, "r")
				deffile = file(defname, "w")
				# write "EXPORTS" to file, then copy all the contents of the expfile
				deffile.write("EXPORTS\n")
				while 1:
					something = expfile.read()
					if not something:
						break
					deffile.write(something)
				expfile.close()
				deffile.close()
				# append it as -def:defname
				fargs.append("-def:%s" % defname)

			# now let's convert -L<PATH> to -LIBPATH:<PATH>
			# and -l<libname> to <libname>.lib
			# and put them at the end (linker options)
		
		if outname.endswith(".dll") or outname.endswith(".exe"):
			cargs = []
			linkerargs = os.getenv("LDFLAGS", "").split()
			if outname.endswith(".exe"):
				linkerargs.append("-link")
			for i in range(len(fargs)):
				if fargs[i].startswith("-L") and not fargs[i].startswith("-LIBPATH"):
					libpath = fargs[i][2:]
					# make sure the path is a windows path
					linkerargs.append("-LIBPATH:%s" % winpath(libpath))
				elif fargs[i].startswith("-l") and not fargs[i] == "-link":
					libname = fargs[i][2:]
					linkerargs.append("%s.lib" % libname)
				else:
					cargs.append(fargs[i])
			fargs = cargs
			fargs.extend(linkerargs)

	else:
		fargs = sys.argv[:]

	# ok, and an extra round to make sur all include paths are valid windows path
	#for i in range(len(fargs)):
	#	if fargs[i].startswith("-I/"):
	#		fargs[i] = "-I%s" % winpath(fargs[i][2:])
	
	fargs[0] = "cl"
	print "Calling ", fargs

	sys.exit(subprocess.call(fargs))
