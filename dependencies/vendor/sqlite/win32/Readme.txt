To update to a new version of sqlite for windows:

1. Check in new source code to sqlite/src.
2. Do the following for both release and debug modes:
  1. Run configure (with appropriate --enable/--disable flags, look at the mac
     or linux scripts). You'll probably need to 'export LD=link' before you
     start.
  2. Run plain 'make' to generate all the files you'll need. Don't run 'make
     install'.
  3. Copy these files into the appropriate folder in sqlite/src/generated:
       - keywordhash.h
       - opcodes.c
       - opcodes.h
       - parse.c
       - parse.h
       - sqlite3.h
3. Open the sqlite solution. Make sure your current configuration is set to
   'Default' (that will make a debug version and a release version).
4. Rebuild the solution.
5. Copy these files to the appropriate folder in
   vendor-binaries/windows-i686/sqlite/[debug|release]/lib:
     - sqlite.lib
     - sqlite.pdb
6. Copy these files to the appropriate folder in
   vendor-binaries/windows-i686/sqlite/[debug|release]/include:
     - sqlite3.h


If you ever change any of the --enable/--disable flags then you will need to
make changes to the defines saved in the vc projects.