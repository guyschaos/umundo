#!/bin/bash

#
# build all of umundo for iOS and iOS simulator
#

# exit on error
set -e

ME=`basename $0`
DIR="$( cd "$( dirname "$0" )" && pwd )"
SDK_VER="5.0"
CWD=`pwd`
BUILD_DIR="/tmp/build-umundo-ios"

mkdir -p ${BUILD_DIR} &> /dev/null

if [[ -z $1 || $1 = "Debug" ]] ; then
cd ${BUILD_DIR}
rm -rf * && cmake ${DIR}/../ -DCMAKE_TOOLCHAIN_FILE=${DIR}/cmake/CrossCompile-iOS-Sim.cmake -DCMAKE_BUILD_TYPE=Debug && make VERBOSE=1 -j2
rm -rf * && cmake ${DIR}/../ -DCMAKE_TOOLCHAIN_FILE=${DIR}/cmake/CrossCompile-iOS.cmake -DCMAKE_BUILD_TYPE=Debug && make VERBOSE=1 -j2

# build universal libraries for debug
cd ${DIR}
lipo -create -output ../lib/ios-${SDK_VER}/libumundocore_d.ios.a \
../lib/ios-${SDK_VER}/arm/gnu/Debug/libumundocore_d.a \
../lib/ios-${SDK_VER}/i386/gnu/Debug/libumundocore_d.a

lipo -create -output ../lib/ios-${SDK_VER}/libumundoserial_d.ios.a \
../lib/ios-${SDK_VER}/arm/gnu/Debug/libumundoserial_d.a \
../lib/ios-${SDK_VER}/i386/gnu/Debug/libumundoserial_d.a

fi

if [[ -z $1 || $1 = "Release" ]] ; then

cd ${BUILD_DIR}
rm -rf * && cmake ${DIR}/../ -DCMAKE_TOOLCHAIN_FILE=${DIR}/cmake/CrossCompile-iOS-Sim.cmake -DCMAKE_BUILD_TYPE=Release && make -j2
rm -rf * && cmake ${DIR}/../ -DCMAKE_TOOLCHAIN_FILE=${DIR}/cmake/CrossCompile-iOS.cmake -DCMAKE_BUILD_TYPE=Release && make -j2

# build universal libraries for release
cd ${DIR}
lipo -create -output ../lib/ios-${SDK_VER}/libumundocore.ios.a \
../lib/ios-${SDK_VER}/arm/gnu/Release/libumundocore.a \
../lib/ios-${SDK_VER}/i386/gnu/Release/libumundocore.a

lipo -create -output ../lib/ios-${SDK_VER}/libumundoserial.ios.a \
../lib/ios-${SDK_VER}/arm/gnu/Release/libumundoserial.a \
../lib/ios-${SDK_VER}/i386/gnu/Release/libumundoserial.a
fi


