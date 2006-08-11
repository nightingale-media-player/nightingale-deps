#!/bin/bash
export CFLAGS=-fshort-wchar && \
export CXXFLAGS=-fshort-wchar && \
./configure --prefix=${PWD}/compiled/release \
  --enable-releasemode \
  --enable-threadsafe \
  --enable-tempstore \
  --enable-cross-thread-connections \
  --disable-tcl

