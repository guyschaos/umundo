# try hard to find protoc-umundo-rpc
set(UMUNDO_PROTOBUF_RPC_EXECUTABLE)
set(UMUNDO_PROTOBUF_RPC_EXECUTABLE_DEP)
if (NOT CMAKE_CROSSCOMPILING)
	if (CMAKE_GENERATOR MATCHES "Xcode")
		set(UMUNDO_PROTOBUF_RPC_EXECUTABLE "${CMAKE_BINARY_DIR}/rpc/src/umundo/${CMAKE_BUILD_TYPE}/protoc-umundo-rpc")
	elseif(CMAKE_GENERATOR MATCHES "Unix Makefiles")
		set(UMUNDO_PROTOBUF_RPC_EXECUTABLE "${CMAKE_BINARY_DIR}/rpc/src/umundo/protoc-umundo-rpc")
	elseif(CMAKE_GENERATOR MATCHES "NMake.*")
		set(UMUNDO_PROTOBUF_RPC_EXECUTABLE "${CMAKE_BINARY_DIR}/rpc/src/umundo/protoc-umundo-rpc.exe")
	endif()
	set(UMUNDO_PROTOBUF_RPC_EXECUTABLE_DEP "protoc-umundo-rpc")
endif()

if (NOT UMUNDO_PROTOBUF_RPC_EXECUTABLE)
	find_program(UMUNDO_PROTOBUF_RPC_EXECUTABLE 
		NAMES protoc-umundo-rpc protoc-umundo-rpc.exe
		PATHS 
			/usr/local/bin 
			/opt/local/bin
			/usr/bin
			C:\\Program\ Files\ \(x86\)\\uMundo\\bin
		ENV UMUNDO_PROTOBUF_RPC_EXECUTABLE
	)
endif()

#########################################################################
# Redefine PROTOBUF_GENERATE_CPP macro to allow subdirectories and objc
#########################################################################

find_package(Protobuf REQUIRED)

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

	if (NOT UMUNDO_PROTOBUF_RPC_EXECUTABLE)
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
