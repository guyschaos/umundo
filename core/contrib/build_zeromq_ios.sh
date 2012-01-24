#!/bin/bash

#
# build ZeroMQ for iOS and iOS simulator
#

SDK_VER="5.0"
DEST_DIR="../prebuilt/zeromq/ios/${SDK_VER}"
if [ ! -f configure ]; then
	echo "Cannot find ./configure"
	echo "Run from within zeroMQ directory"
	exit
fi
mkdir -p ${DEST_DIR} &> /dev/null

#
# Build for Device
#

SYSROOT="/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS${SDK_VER}.sdk"

make clean
./configure \
CPP="cpp" \
CXXCPP="cpp" \
CXX=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/g++ \
CC=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/gcc \
LD=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/ld\ -r \
CFLAGS="-O -isysroot ${SYSROOT} -arch armv7 -arch armv6" \
CXXFLAGS="-O -isysroot ${SYSROOT} -arch armv7 -arch armv6" \
--disable-dependency-tracking \
--host=arm-apple-darwin10 \
LDFLAGS="-isysroot ${SYSROOT} -arch armv7 -arch armv6" \
AR=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/ar \
AS=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/as \
LIBTOOL=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/libtool \
STRIP=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/strip \
RANLIB=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/ranlib

make -j2

if [ ! -f ./src/.libs/libzmq.a ]; then
	echo "Something went wrong while building for iOS device"
	echo "Cannot find src/.libs/libzmq.a"
	exit
fi

mkdir ${DEST_DIR}/ios
cp ./src/.libs/libzmq.a ${DEST_DIR}/ios/

#
# Simulator
#

SYSROOT="/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator${SDK_VER}.sdk"

make clean
./configure \
CXX=/Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/g++ \
CC=/Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/gcc \
LD=/Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/ld\ -r \
CFLAGS="-O -isysroot ${SYSROOT} -arch i386" \
CXXFLAGS="-O -isysroot ${SYSROOT} -arch i386" \
--disable-dependency-tracking \
LDFLAGS="-isysroot  ${SYSROOT} -arch i386" \
AR=/Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/ar \
AS=/Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/as \
LIBTOOL=/Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/libtool \
STRIP=/Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/strip \
RANLIB=/Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/ranlib

make -j2

if [ ! -f ./src/.libs/libzmq.a ]; then
	echo "Something went wrong while building for iOS device"
	echo "Cannot find src/.libs/libzmq.a"
	exit
fi

mkdir ${DEST_DIR}/ios-sim
cp ./src/.libs/libzmq.a ${DEST_DIR}/ios-sim

#
# create universal library
#
lipo -info ${DEST_DIR}/ios-sim/libzmq.a
lipo -info ${DEST_DIR}/ios/libzmq.a
lipo -create ${DEST_DIR}/ios-sim/libzmq.a ${DEST_DIR}/ios/libzmq.a -output ${DEST_DIR}/libzmq.a
echo "Built universal library in: ${DEST_DIR}/libzmq.a"
lipo -info ${DEST_DIR}/libzmq.a
