#!/bin/bash


TOPDIR=$(pwd)

cd packages/macosx

# autoconf-2.69
echo "Setting up autoconf-2.69"
tar xf autoconf-2.69.tar.gz
cd autoconf-2.69
./configure
make
sudo make install
cd ..

# automake-1.13.4
echo "Setting up automake-1.13.4"
tar xf automake-1.13.4.tar.gz
cd automake-1.13.4
./configure
make
sudo make install
cd ..

# automake-1.14.1
echo "Setting up automake-1.14.1"
tar xf automake-1.14.1.tar.gz
cd automake-1.14.1
./configure
make
sudo make install
cd ..

# cmake-2.8.12.2
echo "Setting up cmake-2.8.12.2"
tar xf cmake-2.8.12.2.tar.gz
cd cmake-2.8.12.2
./configure
make
sudo make install
cd ..

# make sure we get these packages, and before the system ones
echo "Setting up paths"
echo "export PATH=/usr/local/bin:$PATH" >> ~/.bash_profile
source ~/.bash_profile

cd $(TOPDIR)
