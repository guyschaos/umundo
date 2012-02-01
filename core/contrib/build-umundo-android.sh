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


# prepare for ndk-build
mkdir -p build/ndk-build/jni
cd build/ndk-build/jni/
# copy source files
cp -R ../../../../src/* .
# copy boost headers
cp -R ../../../prebuilt/include/boost .
# copy prepared config.h
cp ../../android/src/config.h .
# copy java jni wrapper
cp ../../android/contrib/swig/java/umundoJAVA_wrap.* .
sed -e 's/\.\.\/\.\.\/\.\.\/\.\.\/src\///g' umundoJAVA_wrap.cxx > umundoJAVA_wrap.cpp

#copy libraries
cp ../../../prebuilt/zeromq/android/arm-linux-androideabi/libzmq.a .
cp ../../../prebuilt/bonjour/android/arm-linux-androideabi/libmDnssdEmbed.a .
cp ../../../../lib/android-14/arm/gnu/libumundo.a .

echo 'LOCAL_PATH := $(call my-dir)
LOCAL_CPP_FEATURES      := rtti features

include $(CLEAR_VARS)
LOCAL_MODULE    := libumundo
LOCAL_SRC_FILES := libumundo.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libzmq
LOCAL_SRC_FILES := libzmq.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libmDnssdEmbed
LOCAL_SRC_FILES := libmDnssdEmbed.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
# Here we give our module name and source file(s)
LOCAL_MODULE    := libumundoSwig
LOCAL_SRC_FILES := umundoJAVA_wrap.cpp
LOCAL_STATIC_LIBRARIES := libumundo libzmq libmDnssdEmbed
include $(BUILD_SHARED_LIBRARY)
' > Android.mk

echo 'APP_STL := gnustl_static
APP_ABI := armeabi 
APP_PLATFORM := android-14
' > Application.mk

${ANDROID_NDK_ROOT}/ndk-build

cp ../libs/armeabi/libumundoSwig.so ../../../../lib/android-14/arm/gnu/

cd ../..