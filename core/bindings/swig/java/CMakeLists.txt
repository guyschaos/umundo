# generate JNI library and create a jar
# Make from within Eclipse fails miserably with the whole thing
if(CMAKE_GENERATOR MATCHES "Eclipse.*" OR CMAKE_ECLIPSE_VERSION GREATER 0)
	message(STATUS "NOTE: Eclipse fails with dependency on *.java")
	return()
endif()

if (NOT CMAKE_CROSSCOMPILING)
	find_package(JNI)
	if(JNI_FOUND)
		include_directories(${JNI_INCLUDE_DIRS})
	else()
		message(STATUS "No JNI libraries found - not building Java wrappers")
		return()
	endif()
endif()

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
if (MSVC)
	# MSVC does not include inttypes.h but SWIG needs it
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../msvc)
endif()

SET(CMAKE_SWIG_FLAGS "")
SET(UMUNDO_JAVA_PACKAGE "org.umundo.core")
SET(UMUNDO_JAVA_DIR "org/umundo/core")

# we need ; to produce a space with the package .. weird
SET_SOURCE_FILES_PROPERTIES(umundocore.i PROPERTIES SWIG_FLAGS "-package;${UMUNDO_JAVA_PACKAGE}")
SET_SOURCE_FILES_PROPERTIES(umundocore.i PROPERTIES CPLUSPLUS ON)
SET(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}")

SWIG_ADD_MODULE(umundocoreSwig java umundocore.i)
SWIG_LINK_LIBRARIES(umundocoreSwig ${JNI_LIBRARIES})
SWIG_LINK_LIBRARIES(umundocoreSwig ${UMUNDOCORE_LIBRARIES})
INSTALL_LIBRARY(TARGETS umundocoreSwig COMPONENT librarySwig)

# build jar and copy to library output dir
FIND_PACKAGE(Java)
if(Java_JAVAC_EXECUTABLE AND CMAKE_PATCH_VERSION GREATER 5)
	include (UseJava)
	ADD_JAR(umundocoreSwigJNI ${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/*.java)
	ADD_CUSTOM_COMMAND(
	    TARGET umundocoreSwigJNI
	    POST_BUILD
	    COMMAND ${CMAKE_COMMAND} -E copy
			${CMAKE_CURRENT_BINARY_DIR}/umundocoreSwigJNI.jar
			${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
	)
	add_dependencies(umundocoreSwigJNI umundocoreSwig)
	install_files(
		FILES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/umundocoreSwigJNI.jar 
		DESTINATION share/umundo/java 
		COMPONENT librarySwig)
	set_target_properties(umundocoreSwigJNI PROPERTIES FOLDER "Bindings")
endif()
