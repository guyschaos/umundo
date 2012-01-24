#!/bin/bash

#
# build ZeroMQ for android
#

# exit on error
set -e

ME=`basename $0`
TARGET_DEVICE="arm-linux-androideabi"
DEST_DIR="${PWD}/../../prebuilt/zeromq/android/${TARGET_DEVICE}"

if [ ! -f src/zmq.cpp ]; then
	echo
	echo "Cannot find src/zmq.cpp"
	echo "Run script from within zeroMQ directory:"
	echo "zeromq-3.1.0$ ../../${ME}"
	echo
	exit
fi
mkdir -p ${DEST_DIR} &> /dev/null

# set ANDROID_NDK to default if environment variable not set
if [ -n "${ANDROID_NDK:-x}" ]; then
	ANDROID_NDK="/home/sradomski/Documents/android-dev/android-ndk-r7"
fi

if [ ! -d ${ANDROID_NDK} ]; then
	echo
	echo "Cannot find android-ndk"
	echo "Set environment variable prior to calling this script:"
	echo "ANDROID_NDK=\"/path/to/android/ndk-root\" ../../${ME}"
	echo
	exit	
fi

# download updated config.guess and config.sub
cd config
wget http://git.savannah.gnu.org/cgit/config.git/plain/config.guess -O config.guess
wget http://git.savannah.gnu.org/cgit/config.git/plain/config.sub -O config.sub
cd ..

# if [ -f Makefile ]; then
# 	make clean
# fi

# select and verify toolchain
ANDROID_SYS_ROOT="${ANDROID_NDK}/standalone"

if [ ! -f ${ANDROID_SYS_ROOT}/bin/${TARGET_DEVICE}-gcc ]; then
	echo
	echo "Cannot find compiler at ${ANDROID_SYS_ROOT}/bin/${TARGET_DEVICE}-gcc"
	echo
	exit;
fi

if [ ! -d ${ANDROID_SYS_ROOT}/lib ]; then
	echo
	echo "Cannot find sytem libraries at ${ANDROID_SYS_ROOT}/lib"
	echo
	exit;
fi

./configure \
CPP="cpp" \
CXXCPP="cpp" \
CC="${ANDROID_SYS_ROOT}/bin/${TARGET_DEVICE}-gcc" \
CXX="${ANDROID_SYS_ROOT}/bin/${TARGET_DEVICE}-g++" \
LD="${ANDROID_SYS_ROOT}/bin/${TARGET_DEVICE}-ld" \
LDFLAGS="-L${ANDROID_SYS_ROOT}/lib -static" \
CPPFLAGS="-I${ANDROID_SYS_ROOT}/include -static" \
AR="${ANDROID_SYS_ROOT}/bin/${TARGET_DEVICE}-ar" \
AS="${ANDROID_SYS_ROOT}/bin/${TARGET_DEVICE}-as" \
LIBTOOL="${ANDROID_SYS_ROOT}/bin/${TARGET_DEVICE}-libtool" \
STRIP="${ANDROID_SYS_ROOT}/bin/${TARGET_DEVICE}-strip" \
RANLIB="${ANDROID_SYS_ROOT}/bin/${TARGET_DEVICE}-ranlib" \
--disable-dependency-tracking \
--target=arm-linux-androideabi \
--host=arm-linux-androideabi \
--prefix=${DEST_DIR}

make install

# tidy up
rm -rf ${DEST_DIR}/include
rm -rf ${DEST_DIR}/share
rm -rf ${DEST_DIR}/lib/pkgconfig
mv ${DEST_DIR}/lib/* ${DEST_DIR}
rm -rf ${DEST_DIR}/lib

echo
echo "Installed static libraries in ${DEST_DIR}"
echo