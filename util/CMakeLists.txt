############################################################
# General setup
############################################################

# gather all header files
file(GLOB_RECURSE UMUNDOUTIL_HEADER_FILES src/*.h)

file(GLOB_RECURSE UTIL_FILES src/umundo/util/*.cpp)
list(APPEND UMUNDOUTIL_FILES  ${UTIL_FILES})

# include directories for our header files
include_directories(src)

LIST(APPEND UMUNDOUTIL_LIBRARIES umundoutil)

############################################################
# setup umundoutil library
############################################################

include(UseUMundo)

LIST(SORT UMUNDOUTIL_HEADER_FILES)

if (S11N_PROTOBUF)
	find_package(Protobuf REQUIRED)
	LIST(APPEND UMUNDOUTIL_LIBRARIES optimized ${PROTOBUF_LIBRARY})
	LIST(APPEND UMUNDOUTIL_LIBRARIES debug ${PROTOBUF_LIBRARY_DEBUG})
	include_directories(${PROTOBUF_INCLUDE_DIRS})

	file(GLOB_RECURSE S11N_PROTOBUF_FILES src/umundo/util/protobuf/*.cpp)
	list(APPEND UMUNDOUTIL_FILES  ${S11N_PROTOBUF_FILES})

	if (NOT PROTOBUF_PROTOC_EXECUTABLE)
		message(FATAL_ERROR "protoc binary required for serialization")
	endif()

	# process .proto files
	file(GLOB_RECURSE PROTOBUF_INTERFACES src/*.proto)
	include_directories(${CMAKE_CURRENT_BINARY_DIR})
	
	UMUNDO_PROTOBUF_GENERATE_CPP_RPC(PROTO_SRCS PROTO_HDRS ${PROTOBUF_INTERFACES})
	list(APPEND UMUNDOUTIL_HEADER_FILES ${PROTO_HDRS})	
	list(APPEND UMUNDOUTIL_FILES ${PROTO_SRCS})	
	
endif(S11N_PROTOBUF)

add_library(umundoutil ${UMUNDOUTIL_FILES})
INSTALL_HEADERS(HEADERS ${UMUNDOUTIL_HEADER_FILES} COMPONENT headers)
INSTALL_LIBRARY(TARGETS umundoutil COMPONENT libraryUtil)
set_target_properties(umundoutil PROPERTIES FOLDER "Libraries")

# ok to have in IDEs, but do not install
LIST(APPEND UMUNDOUTIL_HEADER_FILES ${CMAKE_CURRENT_BINARY_DIR}/../umundo/config.h)

############################################################
# Miscellaneous
############################################################

set(UMUNDOUTIL_FILES "${UMUNDOUTIL_FILES}" PARENT_SCOPE)
set(UMUNDOUTIL_LIBRARIES "${UMUNDOUTIL_LIBRARIES}" PARENT_SCOPE)
set(UMUNDOUTIL_HEADER_FILES "${UMUNDOUTIL_HEADER_FILES}" PARENT_SCOPE)

