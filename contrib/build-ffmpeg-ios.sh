#!/bin/bash

#
# build ZeroMQ for iOS and iOS simulator
#

# exit on error
set -e

ME=`basename $0`
SDK_VER="5.0"
#DEST_DIR="${PWD}/../../prebuilt/zeromq/ios/${SDK_VER}"

if [ ! -f ffmpeg.c ]; then
	echo
	echo "Cannot find ffmpeg.c"
	echo "Run script from within ffmpeg directory:"
	echo "ffmpeg$ ../../${ME}"
	echo
	exit
fi
#mkdir -p ${DEST_DIR} &> /dev/null

#
# Build for Device
#

SYSROOT="/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS${SDK_VER}.sdk"

if [ ! -d ${SYSROOT} ]; then
	echo
	echo "Cannot find iOS developer tools."
	echo
	exit	
fi

# if [ -f Makefile ]; then
# 	make clean
# fi

#mkdir -p ${DEST_DIR}/ios &> /dev/null
#CXX=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/g++ \
#CC=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/gcc \
#LD=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/ld\ -r \

./configure \
CFLAGS="-O -isysroot ${SYSROOT} -arch armv7 -arch armv6" \
CXXFLAGS="-O -isysroot ${SYSROOT} -arch armv7 -arch armv6" \
--disable-dependency-tracking \
--host=arm-apple-darwin10 \
--disable-doc 
--disable-ffmpeg 
--disable-ffplay 
--disable-ffserver 
--enable-cross-compile
LDFLAGS="-isysroot ${SYSROOT} -arch armv7 -arch armv6" \
AR=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/ar \
AS=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/as \
LIBTOOL=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/libtool \
STRIP=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/strip \
RANLIB=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/ranlib
#--prefix=${DEST_DIR}/ios

make -j2 install
exit

#
# Simulator
#

SYSROOT="/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator${SDK_VER}.sdk"

if [ -f Makefile ]; then
	make clean
fi

mkdir -p ${DEST_DIR}/ios-sim &> /dev/null

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
RANLIB=/Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/ranlib \
--prefix=${DEST_DIR}/ios-sim

make -j2 install

# tidy up
rm -rf ${DEST_DIR}/ios/include
rm -rf ${DEST_DIR}/ios/share
rm -rf ${DEST_DIR}/ios/lib/pkgconfig
mv ${DEST_DIR}/ios/lib/* ${DEST_DIR}/ios
rm -rf ${DEST_DIR}/ios/lib

rm -rf ${DEST_DIR}/ios-sim/include
rm -rf ${DEST_DIR}/ios-sim/share
rm -rf ${DEST_DIR}/ios-sim/lib/pkgconfig
mv ${DEST_DIR}/ios-sim/lib/* ${DEST_DIR}/ios-sim
rm -rf ${DEST_DIR}/ios-sim/lib


#
# create universal library
#
lipo -info ${DEST_DIR}/ios-sim/libzmq.a
lipo -info ${DEST_DIR}/ios/libzmq.a
lipo -create ${DEST_DIR}/ios-sim/libzmq.a ${DEST_DIR}/ios/libzmq.a -output ${DEST_DIR}/libzmq.a
echo "Built universal library in: ${DEST_DIR}/libzmq.a"
lipo -info ${DEST_DIR}/libzmq.a
