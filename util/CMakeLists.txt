############################################################
# General setup
############################################################

# gather all header files
file(GLOB_RECURSE UMUNDOUTIL_HEADER_FILES src/*.h)

FIND_PATH(Boost_INCLUDE_DIR boost/version.hpp
	PATHS ../contrib/prebuilt/include
)
include_directories(${Boost_INCLUDE_DIR})

# include directories for our header files
include_directories(src)
add_subdirectory(src/umundo)

# library name suffix depending on build type
if (CMAKE_GENERATOR MATCHES "Visual Studio.*")
	add_library(umundoutil
		${UMUNDOUTIL_FILES}
		${UMUNDOUTIL_HEADER_FILES}
	)
	LIST(APPEND UMUNDOUTIL_LIBRARIES umundoutil)
	set_target_properties(umundoutil PROPERTIES FOLDER "Libraries")
else()
	if (CMAKE_BUILD_TYPE MATCHES "Debug")
		add_library(umundoutil_d
			${UMUNDOUTIL_FILES}
			${UMUNDOUTIL_HEADER_FILES}
		)
		LIST(APPEND UMUNDOUTIL_LIBRARIES debug umundoutil_d)
		set_target_properties(umundoutil_d PROPERTIES FOLDER "Libraries")
	else()
		add_library(umundoutil
			${UMUNDOUTIL_FILES}
			${UMUNDOUTIL_HEADER_FILES}
		)
		LIST(APPEND UMUNDOUTIL_LIBRARIES umundoutil)
		set_target_properties(umundoutil PROPERTIES FOLDER "Libraries")
	endif()
endif()

if (NOT CMAKE_CROSSCOMPILING)
	add_subdirectory(test)
endif()

INSTALL_HEADERS_WITH_DIRECTORY("UMUNDOUTIL_HEADER_FILES" headerUtil)

set(UMUNDOUTIL_FILES "${UMUNDOUTIL_FILES}" PARENT_SCOPE)
set(UMUNDOUTIL_LIBRARIES "${UMUNDOUTIL_LIBRARIES}" PARENT_SCOPE)
set(UMUNDOUTIL_LIBRARIES_DEBUG "${UMUNDOUTIL_LIBRARIES_DEBUG}" PARENT_SCOPE)
