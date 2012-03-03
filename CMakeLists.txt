#
# To cross compile, choose a toolchain file (e.g. iOS):
# build$ cmake .. -DCMAKE_TOOLCHAIN_FILE=../contrib/cmake/CrossCompile-iOS.cmake
#
cmake_minimum_required(VERSION 2.8.5)

# build type has to be set first
SET(BUILD_TYPE_HELP "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug, Release, RelWithDebInfo, MinSizeRel.")
IF(DEFINED CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING ${BUILD_TYPE_HELP})
ELSE()
   SET(CMAKE_BUILD_TYPE Debug CACHE STRING ${BUILD_TYPE_HELP})
ENDIF()

project(umundo)

# lower case version of system name and compiler for library paths
string(TOLOWER ${CMAKE_SYSTEM_NAME} CMAKE_SYSTEM_NAME_LC)
string(TOLOWER ${CMAKE_CXX_COMPILER_ID} CMAKE_CXX_COMPILER_ID_LC)

############################################################
# Search paths for cross compiling and prebuilds
############################################################

#
# Setting the CMAKE_FIND_ROOT_PATH to a list of directories will cause all CMake modules
# to look in these directories in addition to the system search paths:
# http://www.vtk.org/Wiki/CMake_Cross_Compiling#Searching_and_finding_external_software
#

# search root for includes
LIST(APPEND CMAKE_FIND_ROOT_PATH "${PROJECT_SOURCE_DIR}/contrib/prebuilt")

# find libraries
if(CMAKE_CROSSCOMPILING)
	SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE "ONLY")
	if (IOS)
		SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY "ONLY")
		LIST(APPEND CMAKE_FIND_ROOT_PATH "${PROJECT_SOURCE_DIR}/contrib/prebuilt/ios/${CMAKE_SYSTEM_VERSION}")
	elseif (ANDROID)
		SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY "BOTH") # we need both to find jni
		SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM "BOTH")
		SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE "BOTH")
		LIST(APPEND CMAKE_FIND_ROOT_PATH "${PROJECT_SOURCE_DIR}/contrib/prebuilt/android/${ANDROID_NDK_ABI_EXT}")
		LIST(APPEND CMAKE_FIND_ROOT_PATH "/usr/local/bin" "/") # this is needed to find swig
	endif ()
else ()
	LIST(APPEND CMAKE_FIND_ROOT_PATH "${PROJECT_SOURCE_DIR}/contrib/prebuilt/${CMAKE_SYSTEM_NAME_LC}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_CXX_COMPILER_ID_LC}")
endif()

message(STATUS "CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH}")

############################################################
# Macros we need later on for installation
############################################################

# Preserve directory structure for header files in distribution packages
# adapted with authors permission from http://www.semipol.de/archives/251
MACRO(INSTALL_HEADERS_WITH_DIRECTORY HEADER_LIST COMPONENT)
	FOREACH(HEADER ${${HEADER_LIST}})
#		message(STATUS "ADDING HEADER ${HEADER}")
		if (${HEADER} MATCHES "${CMAKE_BINARY_DIR}.*")
			STRING(REGEX REPLACE "${CMAKE_BINARY_DIR}" "" REL_HEADER ${HEADER})
			STRING(REGEX MATCH "[^/\\](.*)[/\\]" REL_HEADER ${REL_HEADER})
			SET(REL_HEADER "umundo/${REL_HEADER}")
#			message(STATUS "MATCHED CMAKE_BINARY_DIR -> ${REL_HEADER}")
		elseif(${HEADER} MATCHES "${PROJECT_SOURCE_DIR}.*")
			STRING(REGEX REPLACE "${PROJECT_SOURCE_DIR}" "" REL_HEADER ${HEADER})
			STRING(REGEX MATCH "umundo(.*)[/\\]" REL_HEADER ${REL_HEADER})
#			message(STATUS "MATCHED PROJECT_SOURCE_DIR -> ${REL_HEADER}")
		endif()
		STRING(REGEX MATCH "(.*)[/\\]" DIR ${REL_HEADER})
		if (NOT DIR)
			message(STATUS "Refusing to add header file ${REL_HEADER} for include in uppermost directory")
		else()
			INSTALL(FILES ${HEADER} DESTINATION include/${DIR} COMPONENT ${COMPONENT})
		endif()
	ENDFOREACH()
ENDMACRO()

############################################################
# General setup
############################################################

# where to find the cmake modules we distribute
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/contrib/cmake)

# some compiler flags
if(CMAKE_COMPILER_IS_GNUCXX)
	# best practices from scott meyers
#	add_definitions(-Weffc++)
	# all warnings and as errors
	add_definitions(-Werror)
	add_definitions(-Wall)
	# swig will throw a warning with optimization otherwise
	add_definitions(-fno-strict-aliasing)
elseif (MSVC)
	add_definitions("-DZMQ_STATIC")	
	add_definitions("-D_SCL_SECURE_NO_WARNINGS")
	add_definitions("-D_CRT_SECURE_NO_WARNINGS")
endif()

OPTION(BUILD_UMUNDO_CORE "Build umundo core" ON) 
if (CMAKE_CROSSCOMPILING AND ANDROID)
	OPTION(BUILD_UMUNDO_S11N "Build umundo serialization" OFF) 
else()
	OPTION(BUILD_UMUNDO_S11N "Build umundo serialization" ON) 
endif()

OPTION(BUILD_STATIC_LIBRARIES "Build static libraries" ON) 
if (CMAKE_CROSSCOMPILING)
	OPTION(BUILD_PREFER_STATIC_LIBRARIES "Prefer static libraries when resolving dependencies" ON)
else()
	OPTION(BUILD_PREFER_STATIC_LIBRARIES "Prefer static libraries when resolving dependencies" OFF)
endif()

# Enable verbose Makefiles to debug the build process itself
set(CMAKE_VERBOSE_MAKEFILE OFF)

# enable testing and actual output with tests
enable_testing()
SET(ENV{CTEST_OUTPUT_ON_FAILURE} ON)

if (CMAKE_BUILD_TYPE MATCHES Release)
	SET(LOGLEVEL_S11N "1" CACHE STRING "Log-level for serialization - higher is more verbose" FORCE)
	SET(LOGLEVEL_COMMON "1" CACHE STRING "Log-level for common - higher is more verbose" FORCE)
	SET(LOGLEVEL_DISC "1" CACHE STRING "Log-level for discovery - higher is more verbose" FORCE)
	SET(LOGLEVEL_NET "1" CACHE STRING "Log-level for networking - higher is more verbose" FORCE)
else()
	SET(LOGLEVEL_S11N "4" CACHE STRING "Log-level for serialization - higher is more verbose" FORCE)
	SET(LOGLEVEL_COMMON "4" CACHE STRING "Log-level for common - higher is more verbose" FORCE)
	SET(LOGLEVEL_DISC "4" CACHE STRING "Log-level for discovery - higher is more verbose" FORCE)
	SET(LOGLEVEL_NET "4" CACHE STRING "Log-level for networking - higher is more verbose" FORCE)
endif()

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
	OPTION(DISC_BONJOUR_EMBED "Embed mDNS discovery service" OFF) 
	OPTION(DISC_AVAHI "Use avahi for discovery" ON) 
else()
	# build with bonjour everywhere else
	OPTION(DISC_BONJOUR "Use bonjour for discovery" ON) 
	OPTION(DISC_AVAHI "Use avahi for discovery" OFF) 	
	if(CMAKE_CROSSCOMPILING AND ANDROID)
		OPTION(DISC_BONJOUR_EMBED "Embed mDNS discovery service" ON)
	else()
		OPTION(DISC_BONJOUR_EMBED "Embed mDNS discovery service" OFF) 
	endif()
endif()

# CMake does not allow explicit dependencies
if (DISC_BONJOUR_EMBED AND NOT DISC_BONJOUR)
	message("Cannot use embedded bonjour without bonjour - DISC_BONJOUR!")
	SET(DISC_BONJOUR CACHE ON FORCE)
endif()
if (DISC_BONJOUR AND DISC_AVAHI)
	message("Cannot use both avahi and bonjour for discovery - disable one!")
endif()
if (NOT DISC_AVAHI AND NOT DISC_BONJOUR)
	message("No discovery implementation choosen - enable either DISC_BONJOUR or DISC_AVAHI!")
endif()

if(WIN32)
	SET(THREAD_WIN32 ON)
else()
	SET(THREAD_PTHREAD ON)
endif()
OPTION(NET_ZEROMQ "Use 0MQ for networking" ON)
OPTION(S11N_PROTOBUF "Use protobuf object serialization" ON) 

if (NET_ZEROMQ)
	SET(NET_ZEROMQ_SND_HWM "10000" CACHE STRING "Maximum queue size for publishers")
	SET(NET_ZEROMQ_RCV_HWM "10000" CACHE STRING "Maximum queue size for subscribers")
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
if(BUILD_PREFER_STATIC_LIBRARIES)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
endif()

# where should libraries end up?
#
# LIBRARY_ROOT_PATH is the path up to the Debug/Release directories
# LIBRARY_OUTPUT_PATH is including the Debug/Release components, except for MSVS where it is added automatically
#
if(CMAKE_CROSSCOMPILING)
	string(TOLOWER ${CMAKE_CROSSCOMPILING_TARGET} CMAKE_CROSSCOMPILING_TARGET_LC)
	SET(LIBRARY_ROOT_PATH 
		${PROJECT_SOURCE_DIR}/lib/${CMAKE_CROSSCOMPILING_TARGET_LC}-${CMAKE_SYSTEM_VERSION}/${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_CXX_COMPILER_ID_LC})
	SET(LIBRARY_OUTPUT_PATH ${LIBRARY_ROOT_PATH}/${CMAKE_BUILD_TYPE})
else()
	if (CMAKE_GENERATOR MATCHES "Visual Studio.*")
		SET(LIBRARY_ROOT_PATH 
			${PROJECT_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME_LC}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_CXX_COMPILER_ID_LC})
		SET(LIBRARY_OUTPUT_PATH ${LIBRARY_ROOT_PATH})
	else()
		SET(LIBRARY_ROOT_PATH 
			${PROJECT_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME_LC}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_CXX_COMPILER_ID_LC})
		SET(LIBRARY_OUTPUT_PATH ${LIBRARY_ROOT_PATH}/${CMAKE_BUILD_TYPE})
	endif()
endif()

############################################################
# Create config.h
############################################################

# determine path seperator to shorten filenames in Debug.cpp
if (WIN32)
	LIST(APPEND UMUNDOCORE_LIBRARIES Ws2_32)
    SET(PATH_SEPERATOR "\\\\")
else()
	SET(PATH_SEPERATOR "/")
endif()

# enable config.h style compile time options and add as "umundo/config.h"
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/umundo/config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

############################################################
# Handle sub-components
############################################################

if (BUILD_UMUNDO_CORE)
	SET(UMUNDOCORE_LIBRARIES)
	SET(UMUNDOCORE_FILES)
	SET(UMUNDOCORE_HEADER_FILES)
	include_directories(core/src)
	add_subdirectory(core)
endif()
if (BUILD_UMUNDO_S11N)
	SET(UMUNDOS11N_LIBRARIES)
	SET(UMUNDOS11N_FILES)
	SET(UMUNDOS11N_HEADER_FILES)
	include_directories(core/s11n)
	add_subdirectory(s11n)
endif()

############################################################
# Some concluding remarks
############################################################

message(STATUS "umundo.core files: ")
foreach(file ${UMUNDOCORE_FILES})
	message(STATUS "  ${file}")
endforeach()

message(STATUS "umundo.core libraries:")
foreach(file ${UMUNDOCORE_LIBRARIES})
	message(STATUS "  ${file}")
endforeach()
# 
# message(STATUS "umundo.s11n files: ")
# foreach(file ${UMUNDOS11N_FILES})
# 	message(STATUS "  ${file}")
# endforeach()
# 
# message(STATUS "umundo.s11n libraries: ")
# foreach(file ${UMUNDOS11N_LIBRARIES})
# 	message(STATUS "  ${file}")
# endforeach()

message(STATUS "Libraries will end up in: " ${LIBRARY_OUTPUT_PATH})


############################################################
# Installation / Provide package target
############################################################

# see contrib/cmake/CPackUMundoPlatform.cmake
include(contrib/cmake/CPackUMundoPlatform.cmake)

# This must always be last!
include(CPack)
