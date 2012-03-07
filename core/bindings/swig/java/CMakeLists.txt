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

if (CMAKE_GENERATOR MATCHES "Visual Studio.*" OR CMAKE_GENERATOR MATCHES "NMake.*")
	SWIG_ADD_MODULE(umundocoreSwig java umundocore.i)
	SWIG_LINK_LIBRARIES(umundocoreSwig ${JNI_LIBRARIES})
	SWIG_LINK_LIBRARIES(umundocoreSwig ${UMUNDOCORE_LIBRARIES})
else()
	if (CMAKE_BUILD_TYPE MATCHES "Debug")
		SWIG_ADD_MODULE(umundocoreSwig_d java umundocore.i)
		SWIG_LINK_LIBRARIES(umundocoreSwig_d
		${JNI_LIBRARIES}
		${UMUNDOCORE_LIBRARIES})
	else()
		SWIG_ADD_MODULE(umundocoreSwig java umundocore.i)
		SWIG_LINK_LIBRARIES(umundocoreSwig
		${JNI_LIBRARIES}
		${UMUNDOCORE_LIBRARIES})
	endif()
endif()

# message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
# message(STATUS "CMAKE_ECLIPSE_VERSION: ${CMAKE_ECLIPSE_VERSION}")
FIND_PACKAGE(Java)
# use java started 
if(Java_JAVAC_EXECUTABLE AND CMAKE_PATCH_VERSION GREATER 5)
	if(CMAKE_GENERATOR MATCHES "Visual Studio.*" OR CMAKE_GENERATOR MATCHES "NMake.*")
		# using java to build jars gives the following error in MSVS
		# Error	179	error MSB6001: Invalid command line switch for "cmd.exe". Illegal characters in path.	C:\Program Files (x86)\MSBuild\Microsoft.Cpp\v4.0\Microsoft.CppCommon.targets	151	6	umundocoreSwigJNI
			# ADD_CUSTOM_COMMAND(
			# 		    TARGET umundocoreSwigJNI
			# 		    POST_BUILD
			# 		    COMMAND ${CMAKE_COMMAND} -E copy
			# 	${CMAKE_CURRENT_BINARY_DIR}\\\\umundocoreSwigJNI.jar
			# 	${LIBRARY_OUTPUT_PATH}
			# )
		message(STATUS "NOTE: we cannot build the umundocoreSwigJNI.jar in windows, please use one from the other platforms or build it by hand!")
	else()
		include (UseJava)
		ADD_JAR(umundocoreSwigJNI ${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/*.java)
		ADD_CUSTOM_COMMAND(
		    TARGET umundocoreSwigJNI
		    POST_BUILD
		    COMMAND ${CMAKE_COMMAND} -E copy
				${CMAKE_CURRENT_BINARY_DIR}/umundocoreSwigJNI.jar
				${LIBRARY_OUTPUT_PATH}
		)
		if (CMAKE_BUILD_TYPE MATCHES "Debug")
			add_dependencies(umundocoreSwigJNI umundocoreSwig_d)
		else()
			add_dependencies(umundocoreSwigJNI umundocoreSwig)
		endif()
	endif()
else()
	message("Could not find javac or cmake not >= 2.8.6, not building jar")
endif()