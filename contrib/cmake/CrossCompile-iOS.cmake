# To cross compile for iPhone:
# build$ cmake .. -DCMAKE_TOOLCHAIN_FILE=../contrib/cmake/CrossCompile-iOS.cmake

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 5.0)
SET(CMAKE_SYSTEM_PROCESSOR arm)
SET(CMAKE_OSX_ARCHITECTURES "armv6" "armv7")

SET(ARCHS "-arch armv6 -arch armv7")
SET(CMAKE_CROSSCOMPILING_TARGET IOS)
SET(IOS ON)

execute_process(COMMAND xcode-select -print-path
    OUTPUT_VARIABLE XCODE_SELECT OUTPUT_STRIP_TRAILING_WHITESPACE)

if(EXISTS ${XCODE_SELECT})
	SET(DEVROOT "${XCODE_SELECT}/Platforms/iPhoneOS.platform/Developer")
	if (NOT EXISTS "${DEVROOT}/SDKs/iPhoneOS${CMAKE_SYSTEM_VERSION}.sdk")
		# specified SDK version does not exist, use last one
		file(GLOB INSTALLED_SDKS ${DEVROOT}/SDKs/*)
		list(SORT INSTALLED_SDKS)
		list(REVERSE INSTALLED_SDKS)
		list(GET INSTALLED_SDKS 0 LATEST_SDK)
		string(REGEX MATCH "[0-9]\\.[0-9]" CMAKE_SYSTEM_VERSION ${LATEST_SDK})
	endif()
else()
	SET(DEVROOT "/Developer/Platforms/iPhoneOS.platform/Developer")
endif()

SET(SDKROOT "${DEVROOT}/SDKs/iPhoneOS${CMAKE_SYSTEM_VERSION}.sdk")
SET(CMAKE_OSX_SYSROOT "${SDKROOT}")

SET (CMAKE_C_COMPILER "${DEVROOT}/usr/bin/gcc")
SET (CMAKE_CXX_COMPILER "${DEVROOT}/usr/bin/g++")

# force compiler and linker flags
SET(CMAKE_C_LINK_FLAGS ${ARCHS})
SET(CMAKE_CXX_LINK_FLAGS ${ARCHS})
# SET(CMAKE_C_FLAGS ${ARCHS}) # C_FLAGS wont stick, use ADD_DEFINITIONS instead
# SET(CMAKE_CXX_FLAGS ${ARCHS})
ADD_DEFINITIONS(${ARCHS})
ADD_DEFINITIONS("--sysroot=${SDKROOT}")

# ios headers
INCLUDE_DIRECTORIES(SYSTEM "${SDKROOT}/usr/include")

# ios libraries
LINK_DIRECTORIES("${SDKROOT}/usr/lib/system")
LINK_DIRECTORIES("${SDKROOT}/usr/lib")

SET (CMAKE_FIND_ROOT_PATH "${SDKROOT}")
SET (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
SET (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
SET (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
