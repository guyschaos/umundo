# from (http://www.cmake.org/cmake/help/cmake-2-8-docs.html#command:file):
#
# We do not recommend using GLOB to collect a list of source files from your source tree. 
# If no CMakeLists.txt file changes when a source is added or removed then the generated 
# build system cannot know when to ask CMake to regenerate.)
#
# - keep in mind

# build a list of all source files one directory beneath

file(GLOB RPC_FILES rpc/*.cpp)
list(APPEND UMUNDORPC_FILES  ${RPC_FILES})

if (RPC_PROTOBUF AND NOT CMAKE_CROSSCOMPILING)
	file(GLOB_RECURSE RPC_PROTOBUF_FILES protoc-rpc/*.cpp)

	add_executable(protoc-umundo-rpc ${RPC_PROTOBUF_FILES})
	target_link_libraries(protoc-umundo-rpc ${PROTOBUF_PROTOC_LIBRARIES} ${PROTOBUF_LIBRARIES})
	set_target_properties(protoc-umundo-rpc PROPERTIES FOLDER "Tools")

	install(
		TARGETS protoc-umundo-rpc
		DESTINATION bin
		PERMISSIONS WORLD_EXECUTE OWNER_EXECUTE GROUP_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
		COMPONENT tools 
	)

endif()

set(UMUNDORPC_FILES "${UMUNDORPC_FILES}" PARENT_SCOPE)
