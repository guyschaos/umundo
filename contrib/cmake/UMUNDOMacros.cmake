
##############################################################################
# Provide custom install_* macros to account for all files
##############################################################################

include(CMakeParseArguments)

function(INSTALL_HEADERS)
	set(options)
	set(oneValueArgs COMPONENT)
	set(multiValueArgs HEADERS)
	cmake_parse_arguments(INSTALL_HEADERS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	FOREACH(HEADER ${INSTALL_HEADERS_HEADERS})
#		message(STATUS "ADDING HEADER ${HEADER}")
		if (${HEADER} MATCHES "${CMAKE_BINARY_DIR}.*")
			STRING(REGEX REPLACE "${CMAKE_BINARY_DIR}" "" REL_HEADER ${HEADER})
			STRING(REGEX MATCH "[^/\\](.*)[/\\]" REL_HEADER ${REL_HEADER})
			SET(REL_HEADER "umundo/${REL_HEADER}")
#			message(STATUS "MATCHED CMAKE_BINARY_DIR -> ${REL_HEADER}")
		elseif(${HEADER} MATCHES "${PROJECT_SOURCE_DIR}.*")
			STRING(REGEX REPLACE "${PROJECT_SOURCE_DIR}" "" REL_HEADER ${HEADER})
			STRING(REGEX MATCH "umundo(.*)[/\\]" REL_HEADER ${REL_HEADER})
#			message(STATUS "MATCHED PROJECT_SOURCE_DIR -> ${REL_HEADER}")
		else()
			message(STATUS "MATCHED no known prefix: ${HEADER}")
		endif()
		STRING(REGEX MATCH "(.*)[/\\]" DIR ${REL_HEADER})
		if (NOT DIR)
			message("Refusing to add header file ${REL_HEADER} for include in uppermost directory")
		else()
#			message("ADDING ${HEADER} in include/${DIR} for ${INSTALL_HEADERS_COMPONENT}")
			INSTALL(FILES ${HEADER} DESTINATION include/${DIR} COMPONENT ${INSTALL_HEADERS_COMPONENT})
		endif()
	ENDFOREACH()
endfunction()

function(INSTALL_FILES)
	set(options)
	set(oneValueArgs COMPONENT DESTINATION)
	set(multiValueArgs FILES)
	cmake_parse_arguments(INSTALL_FILE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	install(FILES ${INSTALL_FILE_FILES} DESTINATION ${INSTALL_FILE_DESTINATION} COMPONENT ${INSTALL_FILE_COMPONENT})
endfunction()

function(INSTALL_LIBRARY)
	set(options)
	set(oneValueArgs COMPONENT)
	set(multiValueArgs TARGETS)
	cmake_parse_arguments(INSTALL_LIBRARY "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	install(TARGETS ${INSTALL_LIBRARY_TARGETS} DESTINATION lib COMPONENT ${INSTALL_LIBRARY_COMPONENT})
endfunction()

function(INSTALL_EXECUTABLE)
	set(options)
	set(oneValueArgs COMPONENT)
	set(multiValueArgs TARGETS)
	cmake_parse_arguments(INSTALL_EXECUTABLE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	install(
		TARGETS ${INSTALL_EXECUTABLE_TARGETS} 
		DESTINATION bin
		COMPONENT ${INSTALL_EXECUTABLE_COMPONENT}
		PERMISSIONS WORLD_EXECUTE OWNER_EXECUTE GROUP_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
endfunction()
