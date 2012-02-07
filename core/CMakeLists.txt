############################################################
# General setup
############################################################

# disable some warnings
if (MSVC)
	add_definitions("-DZMQ_STATIC")	
	add_definitions("-D_SCL_SECURE_NO_WARNINGS")
	add_definitions("-D_CRT_SECURE_NO_WARNINGS")
elseif (CMAKE_COMPILER_IS_GNUCXX)
	add_definitions("-Wall")	
endif()

# gather all header files
file(GLOB_RECURSE UMUNDOCORE_HEADER_FILES src/*.h)

############################################################
# Gather libraries and header files
############################################################

if(NET_ZEROMQ)
	find_package(ZeroMQ REQUIRED)
	LIST(APPEND UMUNDOCORE_LIBRARIES ${ZeroMQ_LIBRARY})
	LIST(APPEND UMUNDOCORE_LIBRARIES_DEBUG ${ZeroMQ_LIBRARY_DEBUG})
	include_directories(${ZeroMQ_INCLUDE_DIR})
endif()

if(DISC_BONJOUR AND NOT APPLE AND NOT IOS)
	find_package(Bonjour REQUIRED)
	LIST(APPEND UMUNDOCORE_LIBRARIES ${Bonjour_LIBRARY})
	LIST(APPEND UMUNDOCORE_LIBRARIES_DEBUG ${Bonjour_LIBRARY})
	if (ANDROID)
		include_directories(${CONTRIB_HEADERS}/bonjour)
	else()
		include_directories(${Bonjour_INCLUDE_DIR})
	endif()
endif()

if(DISC_AVAHI)
	find_package(Avahi REQUIRED)
	LIST(APPEND UMUNDOCORE_LIBRARIES ${Avahi_LIBRARIES})
	LIST(APPEND UMUNDOCORE_LIBRARIES_DEBUG ${Avahi_LIBRARIES})	
	include_directories(${Avahi_INCLUDE_DIR})
endif()

if(THREAD_PTHREAD AND NOT ANDROID)
	LIST(APPEND UMUNDOCORE_LIBRARIES "pthread")
	LIST(APPEND UMUNDOCORE_LIBRARIES_DEBUG "pthread")
endif()

FIND_PATH(Boost_INCLUDE_DIR boost/version.hpp
	PATHS ../contrib/prebuilt/include
)
include_directories(${Boost_INCLUDE_DIR})

if (UNIX AND NOT APPLE AND NOT ANDROID)
	LIST(APPEND UMUNDOCORE_LIBRARIES rt)
	LIST(APPEND UMUNDOCORE_LIBRARIES_DEBUG rt)
endif()
if (WIN32)
	LIST(APPEND UMUNDOCORE_LIBRARIES Ws2_32)
	LIST(APPEND UMUNDOCORE_LIBRARIES_DEBUG Ws2_32)
endif()

LIST(APPEND UMUNDOCORE_LIBRARIES umundocore)
LIST(APPEND UMUNDOCORE_LIBRARIES_DEBUG umundocore)

LIST(APPEND UMUNDOCORE_HEADER_FILES ${CMAKE_CURRENT_BINARY_DIR}/../config.h)
LIST(SORT UMUNDOCORE_HEADER_FILES)

# include directories for our header files
include_directories(./src)
include_directories(./test)


add_subdirectory(src)           # gather UMUNDOCORE_FILES and add target umundo library
add_subdirectory(test)          # build tests
add_subdirectory(docs)          # build documentation


add_library(umundocore
	${UMUNDOCORE_FILES}
	${UMUNDOCORE_HEADER_FILES}
)

############################################################
# Generate wrappers for other languages
############################################################

if (NOT CMAKE_CROSSCOMPILING OR ANDROID)
	add_subdirectory(contrib/swig)  # build umundocore JNI library
endif()

############################################################
# Miscellaneous
############################################################

MARK_AS_ADVANCED(CMAKE_OSX_ARCHITECTURES CMAKE_OSX_DEPLOYMENT_TARGET CMAKE_OSX_SYSROOT)

set(UMUNDOCORE_FILES "${UMUNDOCORE_FILES}" PARENT_SCOPE)
set(UMUNDOCORE_LIBRARIES "${UMUNDOCORE_LIBRARIES}" PARENT_SCOPE)
set(UMUNDOCORE_LIBRARIES_DEBUG "${UMUNDOCORE_LIBRARIES_DEBUG}" PARENT_SCOPE)


