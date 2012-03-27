# from (http://www.cmake.org/cmake/help/cmake-2-8-docs.html#command:file):
#
# We do not recommend using GLOB to collect a list of source files from your source tree. 
# If no CMakeLists.txt file changes when a source is added or removed then the generated 
# build system cannot know when to ask CMake to regenerate.)
#
# - keep in mind

# build a list of all source files one directory beneath

file(GLOB UTIL_FILES util/*.cpp)
list(APPEND UMUNDOUTIL_FILES  ${UTIL_FILES})

set(UMUNDOUTIL_FILES "${UMUNDOUTIL_FILES}" PARENT_SCOPE)
