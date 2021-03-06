############################################################
# Gather libraries and header files
############################################################

# include directories for our header files
include_directories(src)

# gather all header files
file(GLOB_RECURSE UMUNDOS11N_HEADER_FILES src/umundo/*.h)

include(UseUMundo)

if (WIN32)
	set(PC_PCRE_LIBDIR "C:/Program Files/GnuWin32/lib")
	set(PC_PCRE_INCLUDEDIR "C:/Program Files/GnuWin32/include")
endif()

find_package(PCRE REQUIRED)
include_directories(${PCRE_INCLUDE_DIR})
LIST(APPEND UMUNDOS11N_LIBRARIES ${PCRE_LIBRARIES})

###########################################
# gather implementation agnostic files
###########################################

file(GLOB S11N_FILES src/umundo/s11n/*.cpp)
list(APPEND UMUNDOS11N_FILES  ${S11N_FILES})

###########################################
# Protocol Buffers
###########################################

if (S11N_PROTOBUF)
	find_package(Protobuf REQUIRED)
	LIST(APPEND UMUNDOS11N_LIBRARIES optimized ${PROTOBUF_LIBRARY})
	LIST(APPEND UMUNDOS11N_LIBRARIES debug ${PROTOBUF_LIBRARY_DEBUG})
	include_directories(${PROTOBUF_INCLUDE_DIRS})

	file(GLOB_RECURSE S11N_PROTOBUF_FILES src/umundo/s11n/protobuf/*.cpp)
	list(APPEND UMUNDOS11N_FILES  ${S11N_PROTOBUF_FILES})

	if (NOT PROTOBUF_PROTOC_EXECUTABLE)
		message(FATAL_ERROR "protoc binary required for serialization")
	endif()

	# process .proto files
	file(GLOB_RECURSE PROTOBUF_INTERFACES interfaces/protobuf/*.proto)
	include_directories(${CMAKE_CURRENT_BINARY_DIR})
	
	UMUNDO_PROTOBUF_GENERATE_CPP_S11N(PROTO_SRCS PROTO_HDRS ${PROTOBUF_INTERFACES})
	# list(APPEND UMUNDOS11N_HEADER_FILES ${PROTO_HDRS})	
	# list(APPEND UMUNDOS11N_FILES ${PROTO_SRCS})	
	
endif(S11N_PROTOBUF)

############################################################
# bindings
############################################################

if (APPLE OR IOS)
	file(GLOB_RECURSE UMUNDOS11N_OBJC_FILES src/umundo-objc/s11n/*.mm)
	file(GLOB_RECURSE UMUNDOS11N_OBJC_HEADER_FILES src/umundo-objc/s11n/*.h)
	list(APPEND UMUNDOS11N_HEADER_FILES ${UMUNDOS11N_OBJC_HEADER_FILES})
	list(APPEND UMUNDOS11N_FILES ${UMUNDOS11N_OBJC_FILES})
	if (IOSSIM)
		foreach(UMUNDOS11N_OBJC_FILE ${UMUNDOS11N_OBJC_FILES})
			set_source_files_properties(${UMUNDOS11N_OBJC_FILE} PROPERTIES COMPILE_FLAGS "-fobjc-abi-version=2 -fobjc-legacy-dispatch")
		endforeach()
	endif()
	include_directories(${PROJECT_SOURCE_DIR}/core/bindings/)
endif()

LIST(APPEND UMUNDOS11N_LIBRARIES umundoserial)

############################################################
# setup umundoserial library
############################################################

LIST(SORT UMUNDOS11N_HEADER_FILES)

add_library(umundoserial ${UMUNDOS11N_FILES})
INSTALL_HEADERS(HEADERS ${UMUNDOS11N_HEADER_FILES} COMPONENT headers)
INSTALL_LIBRARY(TARGETS umundoserial COMPONENT libraryS11N)
set_target_properties(umundoserial PROPERTIES FOLDER "Libraries")

# ok to have in IDEs, but do not install
LIST(APPEND UMUNDOS11N_HEADER_FILES ${CMAKE_CURRENT_BINARY_DIR}/../umundo/config.h)

############################################################
# Miscellaneous
############################################################

set(UMUNDOS11N_FILES "${UMUNDOS11N_FILES}" PARENT_SCOPE)
set(UMUNDOS11N_LIBRARIES "${UMUNDOS11N_LIBRARIES}" PARENT_SCOPE)
set(UMUNDOS11N_HEADER_FILES "${UMUNDOS11N_HEADER_FILES}" PARENT_SCOPE)

