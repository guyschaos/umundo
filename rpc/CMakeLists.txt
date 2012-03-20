############################################################
# General setup
############################################################

# gather all header files
file(GLOB_RECURSE UMUNDORPC_HEADER_FILES src/*.h)
#message("UMUNDORPC_HEADER_FILES ${UMUNDORPC_HEADER_FILES}")

include(UseUMundo)

############################################################
# Select implementations depending on build target
############################################################

if(RPC_PROTOBUF)
	find_package(Protobuf REQUIRED)
	if (NOT PROTOBUF_PROTOC_EXECUTABLE)
		message(FATAL_ERROR "Protobuf required for remote procedure calls")
	endif()
	include_directories(${PROTOBUF_INCLUDE_DIRS})
	LIST(APPEND UMUNDORPC_LIBRARIES optimized ${PROTOBUF_LIBRARY})
	LIST(APPEND UMUNDORPC_LIBRARIES debug ${PROTOBUF_LIBRARY_DEBUG})

	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	file(GLOB_RECURSE PROTOBUF_INTERFACES services/protobuf/*.proto)

	UMUNDO_PROTOBUF_GENERATE_CPP_S11N(PROTO_SRCS PROTO_HDRS ${PROTOBUF_INTERFACES})
	list(APPEND UMUNDORPC_HEADER_FILES ${PROTO_HDRS})	
	list(APPEND UMUNDORPC_FILES ${PROTO_SRCS})	

	UMUNDO_PROTOBUF_GENERATE_CPP_RPC(PROTO_SRCS PROTO_HDRS ${PROTOBUF_INTERFACES})
	list(APPEND UMUNDORPC_HEADER_FILES ${PROTO_HDRS})	
	list(APPEND UMUNDORPC_FILES ${PROTO_SRCS})	

	
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
