# from (http://www.cmake.org/cmake/help/cmake-2-8-docs.html#command:file):
#
# We do not recommend using GLOB to collect a list of source files from your source tree. 
# If no CMakeLists.txt file changes when a source is added or removed then the generated 
# build system cannot know when to ask CMake to regenerate.)
#
# - keep in mind

# build a list of all source files one directory beneath

file(GLOB S11N_FILES *.cpp)
list(APPEND UMUNDO_FILES  ${S11N_FILES})

if (S11N_PROTOBUF)
	file(GLOB_RECURSE S11N_PROTOBUF_FILES protobuf/*.cpp)
	list(APPEND UMUNDO_FILES ${S11N_PROTOBUF_FILES})
endif(S11N_PROTOBUF)

set(UMUNDO_FILES "${UMUNDO_FILES}" PARENT_SCOPE)
