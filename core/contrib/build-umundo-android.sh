#!/bin/bash

# exit on error
set -e

ME=`basename $0`

if [ ! -f ${ANDROID_NDK_ROOT}/ndk-build ]; then
	# try some convinient default locations
	if [ -d /opt/android-ndk-r7 ]; then
		ANDROID_NDK_ROOT="/opt/android-ndk-r7"
	elif [ -d /Developer/Applications/android-ndk-r7 ]; then
		ANDROID_NDK_ROOT="/Developer/Applications/android-ndk-r7"
	elif [ -d /home/sradomski/Documents/android-dev/android-ndk-r7 ]; then
		ANDROID_NDK_ROOT="/home/sradomski/Documents/android-dev/android-ndk-r7"
	else
		echo
		echo "Cannot find android-ndk, call script as"
		echo "ANDROID_NDK_ROOT=\"/path/to/android/ndk\" ${ME}"
		echo
		exit
	fi
	export ANDROID_NDK_ROOT="${ANDROID_NDK_ROOT}"
fi

mkdir -p build/android
cd build/android
cmake ../../../ -DCMAKE_TOOLCHAIN_FILE=../../cmake/CrossCompile-Android.cmake 
make -j2
cd ../..

if [ ! -f ../lib/iOS-5.0/arm/gnu/libumundo.a ]; then
	echo
	echo "Could not find ../lib/iOS-5.0/arm/gnu/libumundo.a"
	echo "Something went wrong with building"
	exit
fi
