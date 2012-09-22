#!/bin/bash


export DIR="$(cd -P "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

export CC=gcc-4.7
export SB_VENDOR_BINARIES_CO_ROOT=$DIR
export SB_VENDOR_BUILD_ROOT=$DIR
