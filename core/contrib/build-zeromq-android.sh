#!/bin/bash

#
# build ZeroMQ for android
#

DEST_DIR="../prebuilt/zeromq/android"
if [ ! -f configure ]; then
	echo "Cannot find ./configure"
	echo "Run from within zeroMQ directory"
	exit
fi
mkdir -p ${DEST_DIR} &> /dev/null

#
# Build for Device
#

# ./toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/arm-linux-androideabi-gcc
NDK_ROOT="/home/sradomski/Documents/android-dev/android-ndk-r7"

make clean
./configure \
CPP="cpp" \
CXXCPP="cpp" \
CC="/opt/android-toolchain/bin/arm-linux-androideabi-gcc" \
CXX="/opt/android-toolchain/bin/arm-linux-androideabi-g++" \
LD="/opt/android-toolchain/bin/arm-linux-androideabi-ld" \
LDFLAGS="-L/opt/android-toolchain/lib -static" \
CPPFLAGS="-I/opt/android-toolchain/include -static" \
AR=/opt/android-toolchain/bin/arm-linux-androideabi-ar \
AS=/opt/android-toolchain/bin/arm-linux-androideabi-as \
LIBTOOL=/opt/android-toolchain/bin/arm-linux-androideabi-libtool \
STRIP=/opt/android-toolchain/bin/arm-linux-androideabi-strip \
RANLIB=/opt/android-toolchain/bin/arm-linux-androideabi-ranlib \
--disable-dependency-tracking \
--target=arm-linux-androideabi \
--host=arm-linux-androideabi \
--prefix=/opt/android-root
