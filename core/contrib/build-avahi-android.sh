#!/bin/bash

#
# build Avahi for android
#

# exit on error
set -e

ME=`basename $0`
TARGET_DEVICE="arm-linux-androideabi"
DEST_DIR="${PWD}/../../prebuilt/avahi/android/${TARGET_DEVICE}"

if [ ! -d avahi-core ]; then
	echo
	echo "Cannot find avahi-core"
	echo "Run script from within avahi directory:"
	echo "avahi-0.6.30$ ../../${ME}"
	echo
	exit
fi
mkdir -p ${DEST_DIR} &> /dev/null

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

if [ -f Makefile ]; then
	make clean
fi

# select and verify toolchain
ANDROID_SYS_ROOT="${ANDROID_NDK_ROOT}/standalone"

if [ ! -f ${ANDROID_SYS_ROOT}/bin/${TARGET_DEVICE}-gcc ]; then
	echo
	echo "Cannot find compiler at ${ANDROID_SYS_ROOT}/bin/${TARGET_DEVICE}-gcc"
	echo
	exit;
fi

if [ ! -d ${ANDROID_SYS_ROOT}/lib ]; then
	echo
	echo "Cannot find android sytem libraries at ${ANDROID_SYS_ROOT}/lib"
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
CPPFLAGS="-I${ANDROID_SYS_ROOT}/include -static " \
AR="${ANDROID_SYS_ROOT}/bin/${TARGET_DEVICE}-ar" \
AS="${ANDROID_SYS_ROOT}/bin/${TARGET_DEVICE}-as" \
LIBTOOL="${ANDROID_SYS_ROOT}/bin/${TARGET_DEVICE}-libtool" \
STRIP="${ANDROID_SYS_ROOT}/bin/${TARGET_DEVICE}-strip" \
RANLIB="${ANDROID_SYS_ROOT}/bin/${TARGET_DEVICE}-ranlib" \
--disable-silent-rules \
--host=arm-linux-androideabi \
--prefix=${DEST_DIR}

make -j2 install

# # tidy up
# rm -rf ${DEST_DIR}/include
# rm -rf ${DEST_DIR}/share
# rm -rf ${DEST_DIR}/lib/pkgconfig
# mv ${DEST_DIR}/lib/* ${DEST_DIR}
# rm -rf ${DEST_DIR}/lib
# 
# echo
# echo "Installed static libraries in ${DEST_DIR}"
# echo