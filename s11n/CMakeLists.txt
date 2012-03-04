############################################################
# General setup
############################################################

# gather all header files
file(GLOB_RECURSE UMUNDOS11N_HEADER_FILES src/*.h)

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

   	list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}${REL_PATH}${FIL_WE}.pb.cc")
   	list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}${REL_PATH}${FIL_WE}.pb.h")

  endforeach()

  set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()


############################################################
# Select implementations depending on build target
############################################################

if(S11N_PROTOBUF)
	find_package(Protobuf REQUIRED)
	if (NOT PROTOBUF_PROTOC_EXECUTABLE)
		message(FATAL_ERROR "Protobuf required for serialization")
	endif()
	include_directories(${PROTOBUF_INCLUDE_DIRS})
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	file(GLOB_RECURSE PROTOBUF_INTERFACES interfaces/protobuf/*.proto)

	UMUNDO_PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${PROTOBUF_INTERFACES})

	# message(STATUS "PROTO_SRCS ${PROTO_SRCS}")
	# message(STATUS "PROTO_HDRS ${PROTO_HDRS}")

	list(APPEND UMUNDOS11N_HEADER_FILES ${PROTO_HDRS})	
	list(APPEND UMUNDOS11N_FILES ${PROTO_SRCS})	

	LIST(APPEND UMUNDOS11N_LIBRARIES optimized ${PROTOBUF_LIBRARY})
	LIST(APPEND UMUNDOS11N_LIBRARIES debug ${PROTOBUF_LIBRARY_DEBUG})
	
endif(S11N_PROTOBUF)

############################################################
# Gather libraries and header files
############################################################

# include directories for our header files
include_directories(./src)

FIND_PATH(Boost_INCLUDE_DIR boost/version.hpp
	PATHS ../contrib/prebuilt/include
)
include_directories(${Boost_INCLUDE_DIR})

add_subdirectory(src/umundo)           # gather UMUNDOS11N_FILES and add target umundo library

# add objective-c bindings on mac and ios
if (APPLE OR IOS)
	file(GLOB_RECURSE UMUNDOS11N_OBJC_FILES src/umundo-objc/s11n/*.mm)
	list(APPEND UMUNDOS11N_FILES ${UMUNDOS11N_OBJC_FILES})
	file(GLOB_RECURSE UMUNDOS11N_OBJC_HEADER_FILES src/umundo-objc/s11n/*.h)
	list(APPEND UMUNDOS11N_HEADER_FILES ${UMUNDOS11N_OBJC_HEADER_FILES})
	if (IOSSIM)
	foreach(UMUNDOS11N_OBJC_FILE ${UMUNDOS11N_OBJC_FILES})
		set_source_files_properties(${UMUNDOS11N_OBJC_FILE} PROPERTIES COMPILE_FLAGS "-fobjc-abi-version=2 -fobjc-legacy-dispatch")
	endforeach()
	endif()
	include_directories(${PROJECT_SOURCE_DIR}/core/bindings/)
endif()

# library name suffix depending on build type
if (CMAKE_GENERATOR MATCHES "Visual Studio.*")
	add_library(umundoserial
		${UMUNDOS11N_FILES}
		${UMUNDOS11N_HEADER_FILES}
	)
	LIST(APPEND UMUNDOS11N_LIBRARIES umundoserial)
else()
	if (CMAKE_BUILD_TYPE MATCHES "Debug")
		add_library(umundoserial_d
			${UMUNDOS11N_FILES}
			${UMUNDOS11N_HEADER_FILES}
		)
		LIST(APPEND UMUNDOS11N_LIBRARIES debug umundoserial_d)
	else()
		add_library(umundoserial
			${UMUNDOS11N_FILES}
			${UMUNDOS11N_HEADER_FILES}
		)
		LIST(APPEND UMUNDOS11N_LIBRARIES umundoserial)
	endif()
endif()

if (UNIX AND NOT APPLE)
	# we need to duplicate the libraries on debian - is this a cmake bug?
	LIST(APPEND UMUNDOS11N_LIBRARIES ${UMUNDOS11N_LIBRARIES})
endif()

add_subdirectory(test)          # build tests

INSTALL_HEADERS_WITH_DIRECTORY("UMUNDOS11N_HEADER_FILES" headerS11N)

set(UMUNDOS11N_FILES "${UMUNDOS11N_FILES}" PARENT_SCOPE)
set(UMUNDOS11N_LIBRARIES "${UMUNDOS11N_LIBRARIES}" PARENT_SCOPE)
set(UMUNDOS11N_LIBRARIES_DEBUG "${UMUNDOS11N_LIBRARIES_DEBUG}" PARENT_SCOPE)
