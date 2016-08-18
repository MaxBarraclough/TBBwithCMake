#!/bin/bash

## Dirty hard-coded BASH script to pass the correct flags to CMake when configuring on my system.
## We use wildcards to (hopefully) be insensitive to TBB version.
## Note that we *must* expand all paths before passing them to CMake

## dirname command will expand path and give us the path of containing directory
m="$(dirname /home/mb/Downloads/tbb*/README)" ## sanity check: ensure tbb dir contains README file

## Use arrays to cause paths to be expanded.
## Storing in a scalar variable won't have the same effect: path remains unexpanded.
dArr=(/home/mb/Downloads/tbb*/build/linux_intel64_gcc*debug/libtbb_debug.so) ## array now holds expanded path
d="${dArr[0]}"

rArr=(/home/mb/Downloads/tbb*/build/linux_intel64*release/libtbb.so) ## array now holds expanded path
r="${rArr[0]}"
## the release .so file has no _release suffix

cmake -G"Unix Makefiles"          \
"-DTBB_ROOT_DIR=${m}"             \
"-DTBB_tbb_LIBRARY_DEBUG=${d}"    \
"-DTBB_tbb_LIBRARY_RELEASE=${r}"  \
-DCMAKE_BUILD_TYPE=Debug

# -DCMAKE_BUILD_TYPE=Release
# -DCMAKE_BUILD_TYPE=RelWithDebInfo
# -DCMAKE_BUILD_TYPE=MinSizeRel

## see https://cmake.org/cmake/help/v3.0/variable/CMAKE_BUILD_TYPE.html

