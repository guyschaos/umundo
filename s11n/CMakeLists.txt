############################################################
# General setup
############################################################

# gather all header files
file(GLOB_RECURSE UMUNDOS11N_HEADER_FILES src/*.h)

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
	PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${PROTOBUF_INTERFACES})

	list(APPEND UMUNDOS11N_HEADER_FILES ${PROTO_HDRS})	
	list(APPEND UMUNDOS11N_FILES ${PROTO_SRCS})	

	LIST(APPEND UMUNDOS11N_LIBRARIES ${PROTOBUF_LIBRARY})
	LIST(APPEND UMUNDOS11N_LIBRARIES_DEBUG ${PROTOBUF_LIBRARY_DEBUG})
	
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

add_subdirectory(src)           # gather UMUNDOS11N_FILES and add target umundo library

add_library(umundoserial
	${UMUNDOS11N_FILES}
	${UMUNDOS11N_HEADER_FILES}
)
LIST(APPEND UMUNDOS11N_LIBRARIES umundoserial)

add_subdirectory(test)          # build tests

set(UMUNDOS11N_FILES "${UMUNDOS11N_FILES}" PARENT_SCOPE)
set(UMUNDOS11N_LIBRARIES "${UMUNDOS11N_LIBRARIES}" PARENT_SCOPE)
set(UMUNDOS11N_LIBRARIES_DEBUG "${UMUNDOS11N_LIBRARIES_DEBUG}" PARENT_SCOPE)
