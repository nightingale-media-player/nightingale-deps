#!/bin/bash
./configure --prefix=$HOME/_built_sqlite3/release \
  --enable-releasemode \
  --enable-threadsafe \
  --enable-tempstore \
  --enable-cross-thread-connections \
  --disable-tcl

