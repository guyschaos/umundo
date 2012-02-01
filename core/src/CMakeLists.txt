# from (http://www.cmake.org/cmake/help/cmake-2-8-docs.html#command:file):
#
# We do not recommend using GLOB to collect a list of source files from your source tree. 
# If no CMakeLists.txt file changes when a source is added or removed then the generated 
# build system cannot know when to ask CMake to regenerate.)
#
# - keep in mind

# build a list of all source files one directory beneath
file(GLOB COMMON_FILES common/*.cpp)
file(GLOB CONN_FILES connection/*.cpp)
file(GLOB DISC_FILES discovery/*.cpp)
file(GLOB TREAD_FILES thread/*.cpp)

list(APPEND UMUNDO_FILES 
	${COMMON_FILES}
	${CONN_FILES}
	${DISC_FILES}
	${TREAD_FILES}
)

if (DISC_BONJOUR)
	file(GLOB_RECURSE DISC_BONJOUR_FILES discovery/bonjour/*.cpp)
	list(APPEND UMUNDO_FILES ${DISC_BONJOUR_FILES})
endif(DISC_BONJOUR)

if (DISC_AVAHI)
	file(GLOB_RECURSE DISC_AVAHI_FILES discovery/avahi/*.cpp)
	list(APPEND UMUNDO_FILES ${DISC_AVAHI_FILES})
endif(DISC_AVAHI)

if (NET_ZEROMQ)	
	file(GLOB_RECURSE NET_ZEROMQ_FILES connection/zeromq/*.cpp)
	list(APPEND UMUNDO_FILES ${NET_ZEROMQ_FILES})
endif(NET_ZEROMQ)

add_library(umundo
	${UMUNDO_FILES}
	${HEADER_FILES}
)

