#!/bin/bash

# XUL Version
export XUL="9.0.1"

# Top level build path
export SB_VENDOR_BUILD_ROOT="$(cd ../"$(dirname "$0")" && pwd)"

# rebuild
make xr-packaging
