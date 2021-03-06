############################################################
# Gather libraries and header files
############################################################

# include directories for our header files
include_directories(src)

# gather all header files
file(GLOB_RECURSE UMUNDORPC_HEADER_FILES src/umundo/*.h)

include(UseUMundo)

###########################################
# gather implementation agnostic files
###########################################

file(GLOB RPC_FILES src/umundo/rpc/*.cpp)
list(APPEND UMUNDORPC_FILES  ${RPC_FILES})

###########################################
# Protocol Buffers
###########################################

if (RPC_PROTOBUF)
	find_package(Protobuf REQUIRED)
	LIST(APPEND UMUNDORPC_LIBRARIES optimized ${PROTOBUF_LIBRARY})
	LIST(APPEND UMUNDORPC_LIBRARIES debug ${PROTOBUF_LIBRARY_DEBUG})
	include_directories(${PROTOBUF_INCLUDE_DIRS})

 	if (NOT CMAKE_CROSSCOMPILING)
		add_executable(protoc-umundo-cpp-rpc src/umundo/protoc-rpc/ServiceGeneratorCPP.cpp)
		target_link_libraries(protoc-umundo-cpp-rpc ${PROTOBUF_PROTOC_LIBRARIES} ${PROTOBUF_LIBRARIES})
		# this is weird - on debian 64bit we need to link against pthread for 'pthread_once'
		if (THREAD_PTHREAD AND 64BIT_HOST) 
			target_link_libraries(protoc-umundo-cpp-rpc pthread)
		endif()
		set_target_properties(protoc-umundo-cpp-rpc PROPERTIES FOLDER "Tools")

		install(
			TARGETS protoc-umundo-cpp-rpc
			DESTINATION bin
			PERMISSIONS WORLD_EXECUTE OWNER_EXECUTE GROUP_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
			COMPONENT tools 
		)

		add_executable(protoc-umundo-java-rpc src/umundo/protoc-rpc/ServiceGeneratorJava.cpp)
		target_link_libraries(protoc-umundo-java-rpc ${PROTOBUF_PROTOC_LIBRARIES} ${PROTOBUF_LIBRARIES})
		if (THREAD_PTHREAD AND 64BIT_HOST) 
			target_link_libraries(protoc-umundo-cpp-rpc pthread)
		endif()
		set_target_properties(protoc-umundo-java-rpc PROPERTIES FOLDER "Tools")

		install(
			TARGETS protoc-umundo-java-rpc
			DESTINATION bin
			PERMISSIONS WORLD_EXECUTE OWNER_EXECUTE GROUP_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
			COMPONENT tools 
		)

	endif()
	
	# process .proto files
	if (NOT PROTOBUF_PROTOC_EXECUTABLE)
		message(FATAL_ERROR "protoc binary required for serialization")
	endif()

	file(GLOB_RECURSE PROTOBUF_INTERFACES services/protobuf/*.proto)
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	UMUNDO_PROTOBUF_GENERATE_CPP_S11N(PROTO_SRCS PROTO_HDRS ${PROTOBUF_INTERFACES})
	list(APPEND UMUNDORPC_HEADER_FILES ${PROTO_HDRS})	
	list(APPEND UMUNDORPC_FILES ${PROTO_SRCS})	

	UMUNDO_PROTOBUF_GENERATE_CPP_RPC(PROTO_SRCS PROTO_HDRS ${PROTOBUF_INTERFACES})
	list(APPEND UMUNDORPC_HEADER_FILES ${PROTO_HDRS})	
	list(APPEND UMUNDORPC_FILES ${PROTO_SRCS})	
endif()

LIST(APPEND UMUNDORPC_LIBRARIES umundorpc)

############################################################
# setup umundorpc library
############################################################

LIST(SORT UMUNDORPC_HEADER_FILES)

add_library(umundorpc ${UMUNDORPC_FILES})
INSTALL_HEADERS(HEADERS ${UMUNDORPC_HEADER_FILES} COMPONENT headers)
INSTALL_LIBRARY(TARGETS umundorpc COMPONENT libraryRPC)
set_target_properties(umundorpc PROPERTIES FOLDER "Libraries")

# ok to have in IDEs, but do not install
LIST(APPEND UMUNDORPC_HEADER_FILES ${CMAKE_CURRENT_BINARY_DIR}/../umundo/config.h)

############################################################
# Miscellaneous
############################################################

set(UMUNDORPC_FILES "${UMUNDORPC_FILES}" PARENT_SCOPE)
set(UMUNDORPC_LIBRARIES "${UMUNDORPC_LIBRARIES}" PARENT_SCOPE)
set(UMUNDORPC_HEADER_FILES "${UMUNDORPC_HEADER_FILES}" PARENT_SCOPE)

