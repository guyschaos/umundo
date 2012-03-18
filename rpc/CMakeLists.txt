############################################################
# General setup
############################################################

# gather all header files
file(GLOB_RECURSE UMUNDORPC_HEADER_FILES src/*.h)

#########################################################################
# Redefine PROTOBUF_GENERATE_CPP macro to allow subdirectories and objc
#########################################################################

function(UMUNDO_PROTOBUF_GENERATE_CPP SRCS HDRS)
  if(NOT ARGN)
    message(SEND_ERROR "Error: PROTOBUF_GENERATE_CPP() called without any proto files")
    return()
  endif(NOT ARGN)

  if(PROTOBUF_GENERATE_CPP_APPEND_PATH)
    # Create an include path for each file specified
    foreach(FIL ${ARGN})
      get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
      get_filename_component(ABS_PATH ${ABS_FIL} PATH)
      list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
      if(${_contains_already} EQUAL -1)
          list(APPEND _protobuf_include_path -I ${ABS_PATH})
      endif()
    endforeach()
  else()
    set(_protobuf_include_path -I ${CMAKE_CURRENT_SOURCE_DIR})
  endif()

  set(${SRCS})
  set(${HDRS})
  foreach(FIL ${ARGN})
    get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
    get_filename_component(FIL_WE ${FIL} NAME_WE)
	get_filename_component(FIL_EXT ${FIL} NAME)

    STRING(REPLACE ${CMAKE_CURRENT_SOURCE_DIR} "" WO_PREFIX ${FIL})
    STRING(REPLACE ${FIL_EXT} "" REL_PATH ${WO_PREFIX})

	file( MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}${REL_PATH}" )

    add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}${REL_PATH}${FIL_WE}.pb.cc"
             "${CMAKE_CURRENT_BINARY_DIR}${REL_PATH}${FIL_WE}.pb.h"
      COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
      ARGS --cpp_out ${CMAKE_CURRENT_BINARY_DIR}${REL_PATH} ${_protobuf_include_path} ${ABS_FIL}
      DEPENDS ${ABS_FIL}
      COMMENT "Running C++ protocol buffer compiler on ${FIL}"
      VERBATIM )

	# try hard to find protoc-umundo-rpc
	set(UMUNDO_PROTOBUF_RPC_EXECUTABLE)
	set(UMUNDO_PROTOBUF_RPC_EXECUTABLE_DEP)
	if (NOT CMAKE_CROSSCOMPILING)
		if (CMAKE_GENERATOR MATCHES "Xcode")
			set(UMUNDO_PROTOBUF_RPC_EXECUTABLE "${CMAKE_CURRENT_BINARY_DIR}/src/umundo/${CMAKE_BUILD_TYPE}/protoc-umundo-rpc")
		elseif(CMAKE_GENERATOR MATCHES "Unix Makefiles")
			set(UMUNDO_PROTOBUF_RPC_EXECUTABLE "${CMAKE_CURRENT_BINARY_DIR}/src/umundo/protoc-umundo-rpc")
		endif()
		set(UMUNDO_PROTOBUF_RPC_EXECUTABLE_DEP "protoc-umundo-rpc")
	endif()

	if (NOT UMUNDO_PROTOBUF_RPC_EXECUTABLE)
		find_program(UMUNDO_PROTOBUF_RPC_EXECUTABLE 
			NAMES protoc-umundo-rpc protoc-umundo-rpc.exe
		)
	endif()

	if (UMUNDO_PROTOBUF_RPC_EXECUTABLE MATCHES UMUNDO_PROTOBUF_RPC_EXECUTABLE-NOTFOUND)
		message(FATAL_ERROR "Could not find protoc-umundo-rpc")
		RETURN()
	endif()

    add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}${REL_PATH}${FIL_WE}.rpc.pb.cc"
             "${CMAKE_CURRENT_BINARY_DIR}${REL_PATH}${FIL_WE}.rpc.pb.h"
      COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
      ARGS --plugin=protoc-gen-cpp_rpc=${UMUNDO_PROTOBUF_RPC_EXECUTABLE} --cpp_rpc_out ${CMAKE_CURRENT_BINARY_DIR}${REL_PATH} ${_protobuf_include_path} ${ABS_FIL}
      DEPENDS ${ABS_FIL} ${UMUNDO_PROTOBUF_RPC_EXECUTABLE_DEP}
      COMMENT "Running C++ RPC protocol buffer compiler on ${FIL}"
      VERBATIM )

   	list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}${REL_PATH}${FIL_WE}.pb.cc")
   	list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}${REL_PATH}${FIL_WE}.pb.h")
   	list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}${REL_PATH}${FIL_WE}.rpc.pb.cc")
   	list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}${REL_PATH}${FIL_WE}.rpc.pb.h")

  endforeach()

  set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()


############################################################
# Select implementations depending on build target
############################################################

if(RPC_PROTOBUF)
	find_package(Protobuf REQUIRED)
	if (NOT PROTOBUF_PROTOC_EXECUTABLE)
		message(FATAL_ERROR "Protobuf required for serialization")
	endif()
	include_directories(${PROTOBUF_INCLUDE_DIRS})
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	file(GLOB_RECURSE PROTOBUF_INTERFACES services/protobuf/*.proto)

	UMUNDO_PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${PROTOBUF_INTERFACES})

	message(STATUS "PROTO_SRCS ${PROTO_SRCS}")
	message(STATUS "PROTO_HDRS ${PROTO_HDRS}")

	list(APPEND UMUNDORPC_HEADER_FILES ${PROTO_HDRS})	
	list(APPEND UMUNDORPC_FILES ${PROTO_SRCS})	

	LIST(APPEND UMUNDORPC_LIBRARIES optimized ${PROTOBUF_LIBRARY})
	LIST(APPEND UMUNDORPC_LIBRARIES debug ${PROTOBUF_LIBRARY_DEBUG})
	
endif(RPC_PROTOBUF)

############################################################
# Gather libraries and header files
############################################################

# include directories for our header files
#include_directories(/src)

FIND_PATH(Boost_INCLUDE_DIR boost/version.hpp
	PATHS ../contrib/prebuilt/include
)
include_directories(${Boost_INCLUDE_DIR})

add_subdirectory(src/umundo)           # gather UMUNDORPC_FILES and add target umundo library

# add objective-c bindings on mac and ios
if (APPLE OR IOS)
	file(GLOB_RECURSE UMUNDORPC_OBJC_FILES src/umundo-objc/rpc/*.mm)
	list(APPEND UMUNDORPC_FILES ${UMUNDORPC_OBJC_FILES})
	file(GLOB_RECURSE UMUNDORPC_OBJC_HEADER_FILES src/umundo-objc/rpc/*.h)
	list(APPEND UMUNDORPC_HEADER_FILES ${UMUNDORPC_OBJC_HEADER_FILES})
	if (IOSSIM)
	foreach(UMUNDORPC_OBJC_FILE ${UMUNDORPC_OBJC_FILES})
		set_source_files_properties(${UMUNDORPC_OBJC_FILE} PROPERTIES COMPILE_FLAGS "-fobjc-abi-version=2 -fobjc-legacy-dispatch")
	endforeach()
	endif()
	include_directories(${PROJECT_SOURCE_DIR}/core/bindings/)
endif()

# library name suffix depending on build type
if (CMAKE_GENERATOR MATCHES "Visual Studio.*")
	add_library(umundorpc
		${UMUNDORPC_FILES}
		${UMUNDORPC_HEADER_FILES}
	)
	LIST(APPEND UMUNDORPC_LIBRARIES umundorpc)
	set_target_properties(umundorpc PROPERTIES FOLDER "Libraries")
else()
	if (CMAKE_BUILD_TYPE MATCHES "Debug")
		add_library(umundorpc_d
			${UMUNDORPC_FILES}
			${UMUNDORPC_HEADER_FILES}
		)
		LIST(APPEND UMUNDORPC_LIBRARIES debug umundorpc_d)
		set_target_properties(umundorpc_d PROPERTIES FOLDER "Libraries")
	else()
		add_library(umundorpc
			${UMUNDORPC_FILES}
			${UMUNDORPC_HEADER_FILES}
		)
		LIST(APPEND UMUNDORPC_LIBRARIES umundorpc)
		set_target_properties(umundorpc PROPERTIES FOLDER "Libraries")
	endif()
endif()

if (UNIX AND NOT APPLE)
	# we need to duplicate the libraries on debian - is this a cmake bug?
	LIST(APPEND UMUNDORPC_LIBRARIES ${UMUNDORPC_LIBRARIES})
endif()

add_subdirectory(test)          # build tests

INSTALL_HEADERS_WITH_DIRECTORY("UMUNDORPC_HEADER_FILES" headerRPC)

set(UMUNDORPC_FILES "${UMUNDORPC_FILES}" PARENT_SCOPE)
set(UMUNDORPC_LIBRARIES "${UMUNDORPC_LIBRARIES}" PARENT_SCOPE)
set(UMUNDORPC_LIBRARIES_DEBUG "${UMUNDORPC_LIBRARIES_DEBUG}" PARENT_SCOPE)
