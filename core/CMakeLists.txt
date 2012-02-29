############################################################
# General setup
############################################################

# gather all header files
file(GLOB_RECURSE UMUNDOCORE_HEADER_FILES src/*.h)

############################################################
# Gather libraries and header files
############################################################

if(NET_ZEROMQ)
	find_package(ZeroMQ REQUIRED)
	LIST(APPEND UMUNDOCORE_LIBRARIES optimized ${ZeroMQ_LIBRARY})
	LIST(APPEND UMUNDOCORE_LIBRARIES debug ${ZeroMQ_LIBRARY_DEBUG})
	include_directories(${ZeroMQ_INCLUDE_DIR})
endif()

if(DISC_BONJOUR AND NOT APPLE AND NOT IOS)
	find_package(Bonjour REQUIRED)
	LIST(APPEND UMUNDOCORE_LIBRARIES ${Bonjour_LIBRARY})
endif()

if(DISC_AVAHI)
	find_package(Avahi REQUIRED)
	LIST(APPEND UMUNDOCORE_LIBRARIES ${Avahi_LIBRARIES})
	include_directories(${Avahi_INCLUDE_DIR})
endif()

if(THREAD_PTHREAD AND NOT ANDROID)
	LIST(APPEND UMUNDOCORE_LIBRARIES "pthread")
endif()

FIND_PATH(Boost_INCLUDE_DIR boost/version.hpp
	PATHS /usr/include
)
include_directories(${Boost_INCLUDE_DIR})

if (ANDROID)
	LIST(APPEND UMUNDOCORE_LIBRARIES log)
endif()

if (UNIX AND NOT APPLE AND NOT ANDROID)
	LIST(APPEND UMUNDOCORE_LIBRARIES rt)
endif()
if (WIN32)
	LIST(APPEND UMUNDOCORE_LIBRARIES Ws2_32)
endif()

LIST(SORT UMUNDOCORE_HEADER_FILES)

# include directories for our header files
include_directories(./src)
include_directories(./test)

add_subdirectory(src/umundo)    # gather UMUNDOCORE_FILES and add target umundo library

# add objective-c bindings on mac and ios
if (APPLE OR IOS)
	file(GLOB_RECURSE UMUNDOCORE_OBJC_FILES ${PROJECT_SOURCE_DIR}/contrib/bindings/objc/umundo-objc/core/*.mm)
	list(APPEND UMUNDOCORE_FILES ${UMUNDOCORE_OBJC_FILES})
	file(GLOB_RECURSE UMUNDOCORE_OBJC_HEADER_FILES ${PROJECT_SOURCE_DIR}/contrib/bindings/objc/umundo-objc/core/*.h)
	list(APPEND UMUNDOCORE_HEADER_FILES ${UMUNDOCORE_OBJC_HEADER_FILES})
	include_directories(${PROJECT_SOURCE_DIR}/contrib/bindings/objc/)
endif()

# library name suffix depending on build type
if (CMAKE_GENERATOR MATCHES "Visual Studio.*")
	add_library(umundocore
		${UMUNDOCORE_FILES}
		${UMUNDOCORE_HEADER_FILES}
	)
	LIST(APPEND UMUNDOCORE_LIBRARIES umundocore)
else()
	if (CMAKE_BUILD_TYPE MATCHES "Debug")
		add_library(umundocore_d
			${UMUNDOCORE_FILES}
			${UMUNDOCORE_HEADER_FILES}
		)
		LIST(APPEND UMUNDOCORE_LIBRARIES debug umundocore_d)
	else()
		add_library(umundocore
			${UMUNDOCORE_FILES}
			${UMUNDOCORE_HEADER_FILES}
		)
		LIST(APPEND UMUNDOCORE_LIBRARIES umundocore)
	endif()
endif()

INSTALL_HEADERS_WITH_DIRECTORY("UMUNDOCORE_HEADER_FILES" headerCore)

# ok to have in IDEs, but do not install
LIST(APPEND UMUNDOCORE_HEADER_FILES ${CMAKE_CURRENT_BINARY_DIR}/../umundo/config.h)

if (UNIX AND NOT APPLE)
	# we need to duplicate the libraries on debian - is this a cmake bug?
	LIST(APPEND UMUNDOCORE_LIBRARIES ${UMUNDOCORE_LIBRARIES})
endif()

if (NOT CMAKE_CROSSCOMPILING)
	add_subdirectory(test)          # build tests
	add_subdirectory(docs)          # build documentation
	add_subdirectory(tools)         # command-line tools
endif()

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
set(UMUNDOCORE_HEADER_FILES "${UMUNDOCORE_HEADER_FILES}" PARENT_SCOPE)


