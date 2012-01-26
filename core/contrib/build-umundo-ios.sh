#!/bin/bash

# exit on error
set -e

mkdir -p build/ios
cd build/ios
cmake ../../../ -DCMAKE_TOOLCHAIN_FILE=../../cmake/CrossCompile-iOS.cmake 
make -j2
cd ../..

if [ ! -f ../lib/iOS-5.0/arm/gnu/libumundo.a ]; then
	echo "Could not find ../lib/iOS-5.0/arm/gnu/libumundo.a"
	echo "Something went wrong with building"
	exit
fi

mkdir -p build/ios-sim
cd build/ios-sim
cmake ../../../ -DCMAKE_TOOLCHAIN_FILE=../../cmake/CrossCompile-iOS-Sim.cmake 
make -j2
cd ../..

if [ ! -f ../lib/iOS-5.0/i386/gnu/libumundo.a ]; then
	echo "Could not find ../lib/iOS-5.0/arm/gnu/libumundo.a"
	echo "Soething went wrong with building"
	exit
fi

lipo -create ../lib/iOS-5.0/i386/gnu/libumundo.a ../lib/iOS-5.0/arm/gnu/libumundo.a -output ../lib/iOS-5.0/libumundo.a
if [ -f ../lib/iOS-5.0/libumundo.a ]; then
	echo ""
	echo "Successfully built ../lib/iOS-5.0/libumundo.a as universal library for iOS"
	lipo -info ../lib/iOS-5.0/libumundo.a
	echo ""
else
	echo ""
	echo "Something went wrong creating the fat library"
	echo ""
fi
