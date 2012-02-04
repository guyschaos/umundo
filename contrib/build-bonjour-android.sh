#!/bin/bash

# exit on error
set -e

ME=`basename $0`
TARGET_DEVICE="arm-linux-androideabi"
DEST_DIR="${PWD}/../../prebuilt/bonjour/android/${TARGET_DEVICE}"

if [ ! -d mDNSPosix ]; then
	echo
	echo "Cannot find mDNSPosix"
	echo "Run script from within mDNSResponder directory:"
	echo "mDNSResponder-333.10$ ../../${ME}"
	echo
	exit
fi
mkdir -p ${DEST_DIR} &> /dev/null

. ../../find-android-ndk.sh

if [ -f dummy.c ]; then
	rm ./dummy.c
else
	patch -p1 < ../mDNSResponder-333.10.android.patch
fi

echo '#include "mDNSEmbeddedAPI.h"' >> dummy.c
echo 'mDNSexport mDNS mDNSStorage;' >> dummy.c
echo 'mDNSexport const char ProgramName[] = "umundo";' >> dummy.c


mDNSEmbedded=( mDNSShared/dnssd_clientshim.c mDNSPosix/mDNSPosix.c mDNSCore/mDNS.c mDNSCore/DNSCommon.c mDNSShared/mDNSDebug.c \
	mDNSShared/GenLinkedList.c mDNSCore/uDNS.c mDNSShared/PlatformCommon.c mDNSPosix/mDNSUNP.c mDNSCore/DNSDigest.c dummy.c )

ANDROID_BIN_PREFIX=${ANDROID_NDK_ROOT}/toolchains/arm-linux-androideabi-4.4.3/prebuilt/darwin-x86/bin/arm-linux-androideabi
AR=${ANDROID_BIN_PREFIX}-ar
CC=${ANDROID_BIN_PREFIX}-gcc
LD=${ANDROID_BIN_PREFIX}-ld
SYSROOT=${ANDROID_NDK_ROOT}/platforms/android-14/arch-arm

CC_FLAGS="\
 -ImDNSCore -ImDNSShared -fwrapv -W -Wall -DPID_FILE=\"/var/run/mdnsd.pid\" \
 -DMDNS_UDS_SERVERPATH=\"/var/run/mdnsd\" -DTARGET_OS_ANDROID -DNOT_HAVE_SA_LEN -DUSES_NETLINK -fpic\
 -Wdeclaration-after-statement -Os -DMDNS_DEBUGMSGS=0"

if [ -d build ]; then
	rm -rf build
fi

mkdir -p build/mDNSPosix &> /dev/null
mkdir -p build/mDNSCore &> /dev/null
mkdir -p build/mDNSShared &> /dev/null
mkdir -p build/prod &> /dev/null

# compile all the files
OBJS=""
for file in ${mDNSEmbedded[@]}; do
	if [ ! -f build/$file.o ]; then
		echo ${CC} --sysroot=${SYSROOT} ${CC_FLAGS} $file -o build/$file.o
		${CC} --sysroot=${SYSROOT} ${CC_FLAGS} -c $file -o build/$file.o
		OBJS="$OBJS build/$file.o"
	fi
done

echo ${CC} --sysroot=${SYSROOT} -shared -o build/prod/libmDNSEmbedded.so $OBJS
${CC} --sysroot=${SYSROOT} -shared -o build/prod/libmDNSEmbedded.so $OBJS
echo ${AR} rvs build/prod/libmDNSEmbedded.a $OBJS
${AR} rvs build/prod/libmDNSEmbedded.a $OBJS

cp build/prod/* ${DEST_DIR}