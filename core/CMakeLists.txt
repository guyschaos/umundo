cmake_minimum_required(VERSION 2.8.2)

#
# To cross compile, choose a toolchain file (e.g. iOS):
# build$ cmake .. -DCMAKE_TOOLCHAIN_FILE=../contrib/cmake/CrossCompile-iOS.cmake
#

############################################################
# General setup
############################################################

project(uMundo)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/contrib/cmake)

# Enable verbose Makefiles to debug the build process itself
set(CMAKE_VERBOSE_MAKEFILE OFF)

# enable testing and actual output with tests
enable_testing()
SET(ENV{CTEST_OUTPUT_ON_FAILURE} ON)

# do not build shared library
SET(BUILD_SHARED_LIBS OFF)

# prefer static libraries for linking
IF(WIN32)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
ELSE(WIN32)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
ENDIF(WIN32)

# disable some warnings
if (MSVC)
	add_definitions("-DZMQ_STATIC")	
	add_definitions("-D_SCL_SECURE_NO_WARNINGS")
	add_definitions("-D_CRT_SECURE_NO_WARNINGS")
elseif (CMAKE_COMPILER_IS_GNUCXX)
	add_definitions("-Wall")	
endif()

# where should libraries end up?
string(TOLOWER ${CMAKE_SYSTEM_NAME} CMAKE_SYSTEM_NAME_LC)
string(TOLOWER ${CMAKE_CXX_COMPILER_ID} CMAKE_CXX_COMPILER_ID_LC)
if(CMAKE_CROSSCOMPILING)
	string(TOLOWER ${CMAKE_CROSSCOMPILING_TARGET} CMAKE_CROSSCOMPILING_TARGET_LC)
	SET(LIBRARY_OUTPUT_PATH 
		${PROJECT_SOURCE_DIR}/lib/${CMAKE_CROSSCOMPILING_TARGET_LC}-${CMAKE_SYSTEM_VERSION}/${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_CXX_COMPILER_ID_LC})
else()
	SET(LIBRARY_OUTPUT_PATH 
		${PROJECT_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME_LC}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_CXX_COMPILER_ID_LC})
endif()
SET(CONTRIB_HEADERS ${PROJECT_SOURCE_DIR}/contrib/prebuilt/include)
SET(CONTRIB_PREBUILT ${PROJECT_SOURCE_DIR}/contrib/prebuilt)

# gather all header files
file(GLOB_RECURSE HEADER_FILES src/*.h)

############################################################
# Select implementations depending on build target
############################################################

# select implementations for discovery and network connectivity
if(UNIX AND NOT APPLE AND NOT IOS)
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


SET(LOGLEVEL_COMMON "1" CACHE STRING "Log-level for common - higher is more verbose")
SET(LOGLEVEL_DISC "1" CACHE STRING "Log-level for discovery - higher is more verbose")
SET(LOGLEVEL_NET "1" CACHE STRING "Log-level for networking - higher is more verbose")
SET_PROPERTY(CACHE LOGLEVEL_COMMON PROPERTY STRINGS 0 1 2 3)
SET_PROPERTY(CACHE LOGLEVEL_DISC PROPERTY STRINGS 0 1 2 3)
SET_PROPERTY(CACHE LOGLEVEL_NET PROPERTY STRINGS 0 1 2 3)

############################################################
# Gather libraries and header files
############################################################

if(NET_ZEROMQ)
	find_package(ZeroMQ REQUIRED)
	LIST(APPEND UMUNDO_LIBRARIES ${ZeroMQ_LIBRARY})
	LIST(APPEND UMUNDO_LIBRARIES_DEBUG ${ZeroMQ_LIBRARY_DEBUG})
	include_directories(${ZeroMQ_INCLUDE_DIR})
endif()

if(DISC_BONJOUR AND NOT APPLE AND NOT IOS)
	find_package(Bonjour REQUIRED)
	LIST(APPEND UMUNDO_LIBRARIES ${Bonjour_LIBRARY})
	LIST(APPEND UMUNDO_LIBRARIES_DEBUG ${Bonjour_LIBRARY})
	include_directories(${Bonjour_INCLUDE_DIR})
endif()

if(DISC_AVAHI)
	find_package(Avahi REQUIRED)
	LIST(APPEND UMUNDO_LIBRARIES ${Avahi_LIBRARY})
	LIST(APPEND UMUNDO_LIBRARIES_DEBUG ${Avahi_LIBRARY})
	include_directories(${Avahi_INCLUDE_DIR})
endif()

# if(CMAKE_CROSSCOMPILING AND IOS)
# 	find_package(BoostiOS REQUIRED)
# 	include_directories(${Boost_INCLUDE_DIR})
# else()
# 	find_package(Boost REQUIRED)
# 	include_directories(${Boost_INCLUDE_DIRS})
# 	mark_as_advanced(Boost_DIR)
# endif()

FIND_PATH(Boost_INCLUDE_DIR boost/version.hpp
	PATHS contrib/prebuilt/include
)
include_directories(${Boost_INCLUDE_DIR})

if (WIN32)
	LIST(APPEND UMUNDO_LIBRARIES Ws2_32)
    SET(PATH_SEPERATOR "\\\\")
else()
	SET(PATH_SEPERATOR "/")
endif()

# enable config.h style compile time options
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)
LIST(APPEND HEADER_FILES ${CMAKE_CURRENT_BINARY_DIR}/src/config.h)
LIST(SORT HEADER_FILES)

# include directories for our header files
include_directories(./src)
include_directories(./test)

add_subdirectory(src)           # gather UMUNDO_FILES and add target umundo library
add_subdirectory(test)          # build tests

############################################################
# Copy required header files to include/
############################################################

# remove old include dir
file(REMOVE_RECURSE ${PROJECT_SOURCE_DIR}/include/)

# copy headers to include
file(WRITE ${PROJECT_SOURCE_DIR}/include/umundo.h "/** auto-generated list of all headers */\n\n")
foreach(HEADER_FILE ${HEADER_FILES})

	# get relative path from project source dir
	STRING(REGEX REPLACE "${PROJECT_SOURCE_DIR}/src/" "" REL_HEADER_FILE "${HEADER_FILE}")	
	STRING(REGEX REPLACE "(.*)[/^](.*.h)" "\\1" REL_HEADER_PATH "${REL_HEADER_FILE}")
	if (NOT ${REL_HEADER_PATH} MATCHES ".*/.*")

		# copy only one directory deep, rest is implementation specific
		if (${REL_HEADER_FILE} STREQUAL "config.h")
			file(COPY ${HEADER_FILE} DESTINATION ${PROJECT_SOURCE_DIR}/include/)
		else()
			file(COPY ${HEADER_FILE} DESTINATION ${PROJECT_SOURCE_DIR}/include/${REL_HEADER_PATH})
		endif()
		file(APPEND ${PROJECT_SOURCE_DIR}/include/umundo.h "#include \"${REL_HEADER_FILE}\"\n")
	else()

		# copy implementation specific headers
		if (DISC_AVAHI AND REL_HEADER_PATH MATCHES ".*/avahi")
			file(COPY ${HEADER_FILE} DESTINATION ${PROJECT_SOURCE_DIR}/include/${REL_HEADER_PATH})
			file(APPEND ${PROJECT_SOURCE_DIR}/include/umundo.h "#include \"${REL_HEADER_FILE}\"\n")
		endif()
		if (DISC_BONJOUR AND REL_HEADER_PATH MATCHES ".*/bonjour")
			file(COPY ${HEADER_FILE} DESTINATION ${PROJECT_SOURCE_DIR}/include/${REL_HEADER_PATH})
			file(APPEND ${PROJECT_SOURCE_DIR}/include/umundo.h "#include \"${REL_HEADER_FILE}\"\n")
		endif()
		if (NET_ZEROMQ AND REL_HEADER_PATH MATCHES ".*/zeromq")
			file(COPY ${HEADER_FILE} DESTINATION ${PROJECT_SOURCE_DIR}/include/${REL_HEADER_PATH})
			file(APPEND ${PROJECT_SOURCE_DIR}/include/umundo.h "#include \"${REL_HEADER_FILE}\"\n")
		endif()
	endif()
endforeach()

############################################################
# Generate wrappers for other languages
############################################################

if (NOT CMAKE_CROSSCOMPILING)
	add_subdirectory(contrib/swig)  # build umundo JNI library
endif()

############################################################
# Miscellaneous
############################################################

MARK_AS_ADVANCED(CMAKE_OSX_ARCHITECTURES CMAKE_OSX_DEPLOYMENT_TARGET CMAKE_OSX_SYSROOT)

