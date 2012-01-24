# To cross compile for iOS simulator:
# build$ cmake .. -DCMAKE_TOOLCHAIN_FILE=../contrib/cmake/CrossCompile-Android.cmake

SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 7)
SET(CMAKE_SYSTEM_PROCESSOR armeabi-v7a)

SET(CMAKE_CROSSCOMPILING_TARGET android)
SET(ANDROID ON)

SET(NDK_ROOT "/home/sradomski/Documents/android-dev/android-ndk-r7")
SET(PLATFORM_ROOT "${NDK_ROOT}/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/arm-linux-androideabi")
set(NDK_SYSROOT "${NDK_ROOT}/platforms/android-14/arch-arm/" )

# ./toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/arm-linux-androideabi/bin/gcc
# ./toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/arm-linux-androideabi/bin/g++
# ./toolchains/x86-4.4.3/prebuilt/linux-x86/i686-android-linux/bin/gcc
# ./toolchains/x86-4.4.3/prebuilt/linux-x86/i686-android-linux/bin/g++
# ./toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/lib/gcc/arm-linux-androideabi/4.4.3/include

set (CMAKE_C_COMPILER   "${PLATFORM_ROOT}/bin/gcc")
set (CMAKE_CXX_COMPILER "${PLATFORM_ROOT}/bin/g++")
set (CMAKE_AR "${PLATFORM_ROOT}/bin/ar")
set (CMAKE_LINKER "${PLATFORM_ROOT}/bin/ld")
set (CMAKE_NM "${PLATFORM_ROOT}/bin/nm")
set (CMAKE_OBJCOPY "${PLATFORM_ROOT}/bin/objcopy")
set (CMAKE_OBJDUMP "${PLATFORM_ROOT}/bin/objdump")
set (CMAKE_STRIP "${PLATFORM_ROOT}/bin/strip")
set (CMAKE_RANLIB "${PLATFORM_ROOT}/bin/ranlib")

# force compiler and linker flags
set(CMAKE_CXX_FLAGS "-fPIC -Wno-psabi -fsigned-char --sysroot=\"${NDK_SYSROOT}\"")
set(CMAKE_C_FLAGS "-fPIC -Wno-psabi -fsigned-char --sysroot=\"${NDK_SYSROOT}\"")

# ios headers
INCLUDE_DIRECTORIES(SYSTEM "${SDKROOT}/usr/include")

# ios libraries
LINK_DIRECTORIES("${SDKROOT}/usr/lib/system")
LINK_DIRECTORIES("${SDKROOT}/usr/lib")

SET (CMAKE_FIND_ROOT_PATH "${SDKROOT}")
SET (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
SET (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
SET (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
