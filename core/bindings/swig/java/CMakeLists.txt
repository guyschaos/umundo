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

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
if (MSVC)
	# MSVC does not include inttypes.h but SWIG needs it
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../msvc)
endif()

SET(CMAKE_SWIG_FLAGS "")
SET(UMUNDO_JAVA_PACKAGE "org.umundo.core")
SET(UMUNDO_JAVA_DIR "org/umundo/core")

# we need ; to produce a space with the package .. weird
SET_SOURCE_FILES_PROPERTIES(umundocore.i PROPERTIES SWIG_FLAGS "-package;${UMUNDO_JAVA_PACKAGE}")
SET_SOURCE_FILES_PROPERTIES(umundocore.i PROPERTIES CPLUSPLUS ON)
SET(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}")

SWIG_ADD_MODULE(umundocoreSwig java umundocore.i)
foreach(JNI_LIBRARY ${JNI_LIBRARIES})
	if (NOT ${JNI_LIBRARY} MATCHES ".*jawt.*")
		message("Linking with ${JNI_LIBRARY}")
		SWIG_LINK_LIBRARIES(umundocoreSwig ${JNI_LIBRARY})
	endif()
endforeach()
set_target_properties(umundocoreSwig PROPERTIES FOLDER "Bindings")

# I am not sure why we need it twice, but we do at least on android
SWIG_LINK_LIBRARIES(umundocoreSwig ${UMUNDOCORE_LIBRARIES})
SWIG_LINK_LIBRARIES(umundocoreSwig ${UMUNDOCORE_LIBRARIES})

INSTALL_LIBRARY(TARGETS umundocoreSwig COMPONENT librarySwig)

# build jar and copy to library output dir
# FIND_PACKAGE(Java)
# if(Java_JAVAC_EXECUTABLE AND CMAKE_PATCH_VERSION GREATER 5)
# 	include (UseJava)
# 	if(WIN32)
# 		# adding java files per wildcard does not work on windows
# 		# see also: http://www.cmake.org/Bug/view.php?id=13110
# 		ADD_JAR(umundocoreSwigJNI 
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/Connectable.java
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/EndPoint.java
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/Greeter.java
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/Message.java
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/Node.java
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/NodeStub.java
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/Publisher.java
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/PublisherSet.java
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/PublisherStub.java
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/Receiver.java
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/SWIGTYPE_p_p_char.java
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/StringVector.java
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/SubcriberSet.java
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/Subscriber.java
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/umundocoreCPP.java
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/umundocoreCPPConstants.java
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/umundocoreCPPJNI.java
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/imaxdiv_t.java
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/SubcriberSet.java
# 			${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/SubcriberSet.java
# 		)
# 	else()
# 		ADD_JAR(umundocoreSwigJNI ${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/*.java)
# 	endif()
# 	ADD_CUSTOM_COMMAND(
# 	    TARGET umundocoreSwigJNI
# 	    POST_BUILD
# 	    COMMAND ${CMAKE_COMMAND} -E copy
# 			${CMAKE_CURRENT_BINARY_DIR}/umundocoreSwigJNI.jar
# 			${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
# 	)
# 	add_dependencies(umundocoreSwigJNI umundocoreSwig)
# 	install_files(
# 		FILES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/umundocoreSwigJNI.jar 
# 		DESTINATION share/umundo/java 
# 		COMPONENT librarySwig)
# 	set_target_properties(umundocoreSwigJNI PROPERTIES FOLDER "Bindings")
# endif()
