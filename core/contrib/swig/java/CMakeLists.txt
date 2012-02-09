# generate JNI library and create a jar

find_package(JNI)
if(JNI_FOUND)
	include_directories(${JNI_INCLUDE_DIRS})
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

if (CMAKE_GENERATOR MATCHES "Visual Studio.*")
	SWIG_ADD_MODULE(umundocoreSwig java umundocore.i)
	SWIG_LINK_LIBRARIES(umundocoreSwig ${JNI_LIBRARIES})
	foreach(LIB ${UMUNDOCORE_LIBRARIES})
		SWIG_LINK_LIBRARIES(umundocoreSwig general ${LIB})
	endforeach()
	foreach(LIB ${UMUNDOCORE_LIBRARIES_DEBUG})
		SWIG_LINK_LIBRARIES(umundocoreSwig debug ${LIB})
	endforeach()
else()
	if (CMAKE_BUILD_TYPE MATCHES "Debug")
		SWIG_ADD_MODULE(umundocoreSwig_d java umundocore.i)
		SWIG_LINK_LIBRARIES(umundocoreSwig_d
		${JNI_LIBRARIES}
		${UMUNDOCORE_LIBRARIES_DEBUG})
	else()
		SWIG_ADD_MODULE(umundocoreSwig java umundocore.i)
		SWIG_LINK_LIBRARIES(umundocoreSwig
		${JNI_LIBRARIES}
		${UMUNDOCORE_LIBRARIES})
	endif()
endif()

FIND_PACKAGE(Java)
# use java started 
if(Java_JAVAC_EXECUTABLE AND CMAKE_PATCH_VERSION GREATER 5)
	include (UseJava)
	ADD_JAR(umundocoreSwigJNI ${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/*.java)

	ADD_CUSTOM_COMMAND(
	    TARGET umundocoreSwigJNI
	    POST_BUILD
	    COMMAND ${CMAKE_COMMAND} -E copy
			${CMAKE_CURRENT_BINARY_DIR}/umundocoreSwigJNI.jar
			${LIBRARY_OUTPUT_PATH}
	)
	if (CMAKE_BUILD_TYPE MATCHES "Debug" AND NOT CMAKE_GENERATOR MATCHES "Visual Studio.*")
		add_dependencies(umundocoreSwigJNI umundocoreSwig_d)
	else()
		add_dependencies(umundocoreSwigJNI umundocoreSwig)
	endif()
else()
	message("Could not find javac or cmake not >= 2.8.6, not building jar")
endif()