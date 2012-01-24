# generate JNI library and create a jar

find_package(JNI)
if(JNI_FOUND)
	include_directories(${JNI_INCLUDE_DIRS})
endif()

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(CMAKE_SWIG_FLAGS "")
SET(UMUNDO_JAVA_PACKAGE "org.umundo")
SET(UMUNDO_JAVA_DIR "org/umundo")

# we need ; to produce a space with the package .. weird
SET_SOURCE_FILES_PROPERTIES(umundo.i PROPERTIES SWIG_FLAGS "-package;${UMUNDO_JAVA_PACKAGE}")
SET_SOURCE_FILES_PROPERTIES(umundo.i PROPERTIES CPLUSPLUS ON)
SET(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}")

SWIG_ADD_MODULE(umundoSwig java umundo.i)

# build and link the libOpenALPlayer JNI library
SWIG_LINK_LIBRARIES(umundoSwig
	${PTHREADS_LIBRARY}
	${JNI_LIBRARIES}
	${UMUNDO_LIBRARIES} 
	umundo
)

FIND_PACKAGE(Java)
# use java started 
if(Java_JAVAC_EXECUTABLE AND CMAKE_PATCH_VERSION GREATER 5)
	include (UseJava)
	ADD_JAR(umundoSwigJNI ${CMAKE_CURRENT_BINARY_DIR}/${UMUNDO_JAVA_DIR}/*.java)

	ADD_CUSTOM_COMMAND(
	    TARGET umundoSwigJNI
	    POST_BUILD
	    COMMAND ${CMAKE_COMMAND} -E copy
			${CMAKE_CURRENT_BINARY_DIR}/umundoSwigJNI.jar
			${LIBRARY_OUTPUT_PATH}
	)
	add_dependencies(umundoSwigJNI umundoSwig)

else()
	message("Could not find javac or cmake not >= 2.8.6, not building jar")
endif()