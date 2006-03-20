#!/bin/bash
./configure --prefix=$HOME \
  --enable-threadsafe \
  --enable-tempstore \
  --enable-cross-thread-connections \
  --disable-tcl

