cmake_minimum_required(VERSION 2.8)

############################################################
# General setup
############################################################

project(umundoserial)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../contrib/cmake)

# Enable verbose Makefiles to debug the build process itself
set(CMAKE_VERBOSE_MAKEFILE ON)

# enable testing and actual output with tests
enable_testing()
SET(ENV{CTEST_OUTPUT_ON_FAILURE} ON)

# do not build shared library
SET(BUILD_SHARED_LIBS OFF)

# prefer static libraries for linking
IF(WIN32)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
ELSE()
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
ENDIF()

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

# set root directory for prebuilt libraries
SET(CONTRIB_HEADERS ${PROJECT_SOURCE_DIR}/../contrib/prebuilt/include)
SET(CONTRIB_PREBUILT ${PROJECT_SOURCE_DIR}/../contrib/prebuilt)

# gather all header files
file(GLOB_RECURSE HEADER_FILES src/*.h)

############################################################
# Select implementations depending on build target
############################################################

OPTION(S11N_PROTOBUF "Use bonjour for discovery" ON) 

if(S11N_PROTOBUF)
	find_package(Protobuf REQUIRED)
	if (NOT PROTOBUF_PROTOC_EXECUTABLE)
		message(FATAL_ERROR "Protobuf required for serialization")
	endif()
	include_directories(${PROTOBUF_INCLUDE_DIRS})
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	file(GLOB_RECURSE PROTOBUF_INTERFACES interfaces/protobuf/*.proto)
	PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${PROTOBUF_INTERFACES})

	list(APPEND HEADER_FILES ${PROTO_HDRS})	
	list(APPEND UMUNDO_FILES ${PROTO_SRCS})	

	LIST(APPEND UMUNDO_LIBRARIES ${PROTOBUF_LIBRARIES})
	LIST(APPEND UMUNDO_LIBRARIES_DEBUG ${PROTOBUF_LIBRARIES})
	
endif(S11N_PROTOBUF)

# this is a hack - we need to include libzmq.a into libumundocore.a (see libutils.cmake)
find_package(ZeroMQ REQUIRED)
LIST(APPEND UMUNDO_LIBRARIES ${ZeroMQ_LIBRARY})
LIST(APPEND UMUNDO_LIBRARIES_DEBUG ${ZeroMQ_LIBRARY_DEBUG})
include_directories(${ZeroMQ_INCLUDE_DIR})

include_directories(../../contrib/prebuilt/include)


SET(LOGLEVEL_S11N "3" CACHE STRING "Log-level for serialization - higher is more verbose")
SET_PROPERTY(CACHE LOGLEVEL_S11N PROPERTY STRINGS 0 1 2 3)

############################################################
# Gather libraries and header files
############################################################

find_package(UMUNDOCORE REQUIRED)
include_directories(${UMUNDOCORE_INCLUDE_DIR})
LIST(APPEND UMUNDO_LIBRARIES ${UMUNDOCORE_LIBRARY})
LIST(APPEND UMUNDO_LIBRARIES_DEBUG ${UMUNDOCORE_LIBRARY_DEBUG})

# enable config.h style compile time options
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)
LIST(APPEND HEADER_FILES ${CMAKE_CURRENT_BINARY_DIR}/src/config.h)
LIST(SORT HEADER_FILES)

# include directories for our header files
include_directories(./src)

add_subdirectory(src)           # gather UMUNDO_FILES and add target umundo library
add_subdirectory(test)          # build tests

LIST(APPEND UMUNDO_LIBRARIES umundoserial)
LIST(APPEND UMUNDO_LIBRARIES_DEBUG umundoserial)

add_library(umundoserial
 	${UMUNDO_FILES}
 	${HEADER_FILES}
)
