#!/bin/bash

# exit on error
set -e

ME=`basename $0`

. find-android-ndk.sh > /dev/null

mkdir -p build/android
cd build/android
cmake ../../../ -DCMAKE_TOOLCHAIN_FILE=../../cmake/CrossCompile-Android.cmake 
make -j
cd ../..

if [ ! -f ../lib/iOS-5.0/arm/gnu/libumundo.a ]; then
	echo
	echo "Could not find ../lib/iOS-5.0/arm/gnu/libumundo.a"
	echo "Something went wrong with building"
	exit
fi
