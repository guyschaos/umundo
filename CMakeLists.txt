#
# To cross compile, choose a toolchain file (e.g. iOS):
# build$ cmake .. -DCMAKE_TOOLCHAIN_FILE=../contrib/cmake/CrossCompile-iOS.cmake
#
cmake_minimum_required(VERSION 2.8.2)

############################################################
# General setup
############################################################

SET(BUILD_TYPE_HELP "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug, Release, RelWithDebInfo, MinSizeRel.")
IF(DEFINED CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING ${BUILD_TYPE_HELP})
ELSE()
   SET(CMAKE_BUILD_TYPE Debug CACHE STRING ${BUILD_TYPE_HELP})
ENDIF()

project(umundo)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/contrib/cmake)

OPTION(BUILD_UMUNDO_CORE "Build umundo core" ON) 
OPTION(BUILD_UMUNDO_S11N "Build umundo serialization" ON) 
OPTION(BUILD_STATIC_LIBRARIES "Build static libraries" ON) 
OPTION(BUILD_PREFER_PREBUILT "Prefer prebuilt libraries contained in the distribution" ON) 

# Enable verbose Makefiles to debug the build process itself
set(CMAKE_VERBOSE_MAKEFILE OFF)

# enable testing and actual output with tests
enable_testing()
SET(ENV{CTEST_OUTPUT_ON_FAILURE} ON)

SET(LOGLEVEL_S11N "3" CACHE STRING "Log-level for serialization - higher is more verbose")
SET(LOGLEVEL_COMMON "1" CACHE STRING "Log-level for common - higher is more verbose")
SET(LOGLEVEL_DISC "1" CACHE STRING "Log-level for discovery - higher is more verbose")
SET(LOGLEVEL_NET "1" CACHE STRING "Log-level for networking - higher is more verbose")
SET_PROPERTY(CACHE LOGLEVEL_COMMON PROPERTY STRINGS 0 1 2 3)
SET_PROPERTY(CACHE LOGLEVEL_DISC PROPERTY STRINGS 0 1 2 3)
SET_PROPERTY(CACHE LOGLEVEL_NET PROPERTY STRINGS 0 1 2 3)
SET_PROPERTY(CACHE LOGLEVEL_S11N PROPERTY STRINGS 0 1 2 3)

############################################################
# Select implementations depending on build target
############################################################

# select implementations for discovery and network connectivity
if(UNIX AND NOT APPLE AND NOT IOS AND NOT ANDROID)
	# build with avahi on all unices but macosx
	OPTION(DISC_BONJOUR "Use bonjour for discovery" OFF) 
	OPTION(DISC_AVAHI "Use avahi for discovery" ON) 
else()
	# build with bonjour everywhere else
	OPTION(DISC_BONJOUR "Use bonjour for discovery" ON) 
	OPTION(DISC_AVAHI "Use avahi for discovery" OFF) 	
endif()
if(WIN32)
	SET(THREAD_WIN32 ON)
else()
	SET(THREAD_PTHREAD ON)
endif()
OPTION(NET_ZEROMQ "Use 0MQ for networking" ON)
OPTION(S11N_PROTOBUF "Use protobuf object serialization" ON) 

if (NET_ZEROMQ)
	SET(NET_ZEROMQ_SND_HWM "1000" CACHE STRING "Maximum queue size for publishers")
	SET(NET_ZEROMQ_RCV_HWM "1000" CACHE STRING "Maximum queue size for subscribers")
endif()

############################################################
# Library location, type and linking
############################################################

# built shared or static libraries?
if(BUILD_STATIC_LIBRARIES)
	SET(BUILD_SHARED_LIBS OFF)
else()
	SET(BUILD_SHARED_LIBS ON)
endif()

# prefer static libraries for linking
if(BUILD_PREFER_PREBUILT)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
endif()

# where should libraries end up?
string(TOLOWER ${CMAKE_SYSTEM_NAME} CMAKE_SYSTEM_NAME_LC)
string(TOLOWER ${CMAKE_CXX_COMPILER_ID} CMAKE_CXX_COMPILER_ID_LC)
string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_LC)
if(CMAKE_CROSSCOMPILING)
	string(TOLOWER ${CMAKE_CROSSCOMPILING_TARGET} CMAKE_CROSSCOMPILING_TARGET_LC)
	SET(LIBRARY_OUTPUT_PATH 
		${PROJECT_SOURCE_DIR}/lib/${CMAKE_CROSSCOMPILING_TARGET_LC}-${CMAKE_SYSTEM_VERSION}/${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_CXX_COMPILER_ID_LC}/${CMAKE_BUILD_TYPE_LC})
else()
	if (CMAKE_GENERATOR MATCHES "Visual Studio.*")
		SET(LIBRARY_OUTPUT_PATH 
			${PROJECT_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME_LC}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_CXX_COMPILER_ID_LC})
	else()
		SET(LIBRARY_OUTPUT_PATH 
			${PROJECT_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME_LC}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_CXX_COMPILER_ID_LC}/${CMAKE_BUILD_TYPE_LC})
	endif()
endif()

# set root directory for prebuilt libraries
SET(CONTRIB_HEADERS ${PROJECT_SOURCE_DIR}/contrib/prebuilt/include)
SET(CONTRIB_PREBUILT ${PROJECT_SOURCE_DIR}/contrib/prebuilt)

############################################################
# Create config.h
############################################################

if (WIN32)
	LIST(APPEND UMUNDOCORE_LIBRARIES Ws2_32)
    SET(PATH_SEPERATOR "\\\\")
else()
	SET(PATH_SEPERATOR "/")
endif()

# enable config.h style compile time options
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

############################################################
# Handle sub-components
############################################################

if (BUILD_UMUNDO_CORE)
	SET(UMUNDOCORE_LIBRARIES)
	SET(UMUNDOCORE_LIBRARIES_DEBUG)
	SET(UMUNDOCORE_FILES)
	include_directories(core/src)
	add_subdirectory(core)
endif()
if (BUILD_UMUNDO_S11N)
	SET(UMUNDOS11N_LIBRARIES)
	SET(UMUNDOS11N_LIBRARIES_DEBUG)
	SET(UMUNDOS11N_FILES)
	include_directories(core/s11n)
	add_subdirectory(s11n)
endif()

############################################################
# Some concluding remarks
############################################################

message(STATUS "Libraries will end up in: " ${LIBRARY_OUTPUT_PATH})

message(STATUS "umundo.core files: ")
foreach(file ${UMUNDOCORE_FILES})
	message(STATUS "  ${file}")
endforeach()

message(STATUS "umundo.core libraries:")
foreach(file ${UMUNDOCORE_LIBRARIES})
	message(STATUS "  ${file}")
endforeach()

message(STATUS "umundo.s11n files: ")
foreach(file ${UMUNDOS11N_FILES})
	message(STATUS "  ${file}")
endforeach()

message(STATUS "umundo.s11n libraries: ")
foreach(file ${UMUNDOS11N_LIBRARIES})
	message(STATUS "  ${file}")
endforeach()
