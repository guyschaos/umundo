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

add_subdirectory(src/umundo)           # gather UMUNDOS11N_FILES and add target umundo library

# library name suffix depending on build type
set(LIBRARY_NAME_BUILD_TYPE "umundoserial")
if (CMAKE_BUILD_TYPE MATCHES "Debug")
	set(LIBRARY_NAME_BUILD_TYPE "${LIBRARY_NAME_BUILD_TYPE}_d")
	LIST(APPEND UMUNDOS11N_LIBRARIES_DEBUG ${LIBRARY_NAME_BUILD_TYPE})
else()
	LIST(APPEND UMUNDOS11N_LIBRARIES ${LIBRARY_NAME_BUILD_TYPE})
endif()
# add library as target
add_library(${LIBRARY_NAME_BUILD_TYPE}
	${UMUNDOS11N_FILES}
	${UMUNDOS11N_HEADER_FILES}
)

if (UNIX AND NOT APPLE)
	# we need to duplicate the libraries on debian - is this a cmake bug?
	LIST(APPEND UMUNDOS11N_LIBRARIES ${UMUNDOS11N_LIBRARIES})
	LIST(APPEND UMUNDOS11N_LIBRARIES_DEBUG ${UMUNDOS11N_LIBRARIES_DEBUG})
endif()

add_subdirectory(test)          # build tests

INSTALL_HEADERS_WITH_DIRECTORY("UMUNDOS11N_HEADER_FILES" headerS11N)

set(UMUNDOS11N_FILES "${UMUNDOS11N_FILES}" PARENT_SCOPE)
set(UMUNDOS11N_LIBRARIES "${UMUNDOS11N_LIBRARIES}" PARENT_SCOPE)
set(UMUNDOS11N_LIBRARIES_DEBUG "${UMUNDOS11N_LIBRARIES_DEBUG}" PARENT_SCOPE)
#set(UMUNDOS11N_HEADER_FILES "${UMUNDOS11N_HEADER_FILES}" PARENT_SCOPE)
