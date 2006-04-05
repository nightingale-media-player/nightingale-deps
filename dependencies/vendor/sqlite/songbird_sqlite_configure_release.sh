#!/bin/bash
export CFLAGS=-fshort-wchar && \
export CXXFLAGS=-fshort-wchar && \
./configure --prefix=$HOME/_built_sqlite3/release \
  --enable-releasemode \
  --enable-threadsafe \
  --enable-tempstore \
  --enable-cross-thread-connections \
  --disable-tcl

