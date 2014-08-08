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

def fix_link_args(fargs):
	cargs = []
	linkerargs = []
	fargs.extend(os.getenv("LDFLAGS", "").split())
	for i in range(len(fargs)):
		if fargs[i].startswith("-L") and not \
				fargs[i].startswith("-LIBPATH"):
			libpath = fargs[i][2:]
			# make sure the path is a windows path
			linkerargs.append("-LIBPATH:%s" % winpath(libpath))
		elif fargs[i].startswith("-l") and not fargs[i] == "-link":
			libname = fargs[i][2:]
			if libname == "z":
				libname = "zlib"
			linkerargs.append("%s.lib" % libname)
		elif fargs[i].startswith("-NODEFAULTLIB") or \
				fargs[i].startswith("-DEFAULTLIB") or \
				fargs[i].startswith("-LIBPATH"):
			linkerargs.append(fargs[i])
		elif fargs[i].startswith("-Wl,-DLL,-IMPLIB:"):
			linkerargs.append("-DLL")
			linkerargs.append("-IMPLIB:%s" % fargs[i][17:])
		elif fargs[i].startswith("-Wl,--export-all-symbols"):
			# no-op
			linkerargs.append("")
		else:
			cargs.append(fargs[i])
	resultargs = ["cl"] + cargs[1:] + ["-link"] + linkerargs
	
	# strip -rpath
	while "-rpath" in resultargs:
		pos = resultargs.index("-rpath")
		resultargs[pos:pos+2] = []
	# strip -version-info
	while "-version-info" in resultargs:
		pos = resultargs.index("-version-info")
		resultargs[pos:pos+2] = []
	return resultargs

if __name__ == "__main__":
	fargs = ["cl"] + sys.argv[1:]
	# go over all arguments
	# transform '-o output' into '-Fo:output'
	for i in sys.argv:
		print "\t", i
	fargs = fix_link_args(["cl"] + sys.argv[1:])
	if "-o" in fargs:
		pos = fargs.index("-o")
		outname = fargs[pos+1]
		# use proper argument dependening on outname
		if outname.endswith(".exe"):
			fargs[pos:pos+2] = ["-Fe%s" % outname]
		elif outname.endswith(".la"):
			fargs[0] = "lib"
			fargs[pos:pos+2] = ["-out:%s" % outname]
		else:
			fargs[pos:pos+2] = ["-Fo%s" % outname]
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
        if "-g" in fargs:
                fargs[fargs.index("-g")] = "-Zi"

	# ok, and an extra round to make sur all include paths are valid windows path
	#for i in range(len(fargs)):
	#	if fargs[i].startswith("-I/"):
	#		fargs[i] = "-I%s" % winpath(fargs[i][2:])
	
	print "Calling ", fargs

	sys.exit(subprocess.call(fargs))
