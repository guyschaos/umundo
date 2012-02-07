# generate JNI library and create a jar

find_package(JNI)
if(JNI_FOUND)
	include_directories(${JNI_INCLUDE_DIRS})
endif()

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(CMAKE_SWIG_FLAGS "")
SET(UMUNDO_JAVA_PACKAGE "org.umundo.core")
SET(UMUNDO_JAVA_DIR "org/umundo/core")

# we need ; to produce a space with the package .. weird
SET_SOURCE_FILES_PROPERTIES(umundocore.i PROPERTIES SWIG_FLAGS "-package;${UMUNDO_JAVA_PACKAGE}")
SET_SOURCE_FILES_PROPERTIES(umundocore.i PROPERTIES CPLUSPLUS ON)
SET(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}")

SWIG_ADD_MODULE(umundocoreSwig java umundocore.i)

# build and link the umundoCore JNI library
SWIG_LINK_LIBRARIES(umundocoreSwig
	${JNI_LIBRARIES}
	${UMUNDOCORE_LIBRARIES} 
)

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
	add_dependencies(umundocoreSwigJNI umundocoreSwig)

else()
	message("Could not find javac or cmake not >= 2.8.6, not building jar")
endif()