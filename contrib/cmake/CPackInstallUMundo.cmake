# see: http://www.vtk.org/Wiki/CMake:CPackConfiguration

#
# Throughout the source, we used INSTALL_HEADERS_WITH_DIRECTORY to 
# prepare header files for installation. Now add the actual libraries 
# and invoke the packager
# 

# gather and rename libraries

file(GLOB_RECURSE PLATFORM_DEBUG_LIBS 
	${LIBRARY_ROOT_PATH}/Debug/*.a
	${LIBRARY_ROOT_PATH}/Debug/*.so
	${LIBRARY_ROOT_PATH}/Debug/*.lib
	${LIBRARY_ROOT_PATH}/Debug/*.jar
	${LIBRARY_ROOT_PATH}/Debug/*.dylib
	${LIBRARY_ROOT_PATH}/Debug/*.dll
	${LIBRARY_ROOT_PATH}/Debug/*.pdb
	${LIBRARY_ROOT_PATH}/Debug/*.exp
	${LIBRARY_ROOT_PATH}/Debug/*.ilk
	${LIBRARY_ROOT_PATH}/Debug/*.jnilib
)

file(GLOB_RECURSE PLATFORM_RELEASE_LIBS 
	${LIBRARY_ROOT_PATH}/Release/*.a
	${LIBRARY_ROOT_PATH}/Release/*.so
	${LIBRARY_ROOT_PATH}/Release/*.lib
	${LIBRARY_ROOT_PATH}/Release/*.jar
	${LIBRARY_ROOT_PATH}/Release/*.dylib
	${LIBRARY_ROOT_PATH}/Release/*.dll
	${LIBRARY_ROOT_PATH}/Release/*.exp
	${LIBRARY_ROOT_PATH}/Release/*.jnilib
)

foreach(PLATFORM_DEBUG_LIB ${PLATFORM_DEBUG_LIBS})
	if (PLATFORM_DEBUG_LIB MATCHES ".*umundocoreSwig.*")
		install(FILES ${PLATFORM_DEBUG_LIB} DESTINATION share/umundo COMPONENT libraryDebugSwig)
		message(STATUS "PACKAGE DEBUG SWIG ${PLATFORM_DEBUG_LIB}")
	elseif (PLATFORM_DEBUG_LIB MATCHES ".*umundoserial.*")
		install(FILES ${PLATFORM_DEBUG_LIB} DESTINATION lib COMPONENT libraryDebugS11N)
		message(STATUS "PACKAGE DEBUG SERIAL ${PLATFORM_DEBUG_LIB}")
	elseif (PLATFORM_DEBUG_LIB MATCHES ".*umundocore.*")
		install(FILES ${PLATFORM_DEBUG_LIB} DESTINATION lib COMPONENT libraryDebugCore)
		message(STATUS "PACKAGE DEBUG CORE ${PLATFORM_DEBUG_LIB}")
	else()
		message(STATUS "PACKAGE DEBUG UNK ${PLATFORM_DEBUG_LIB} - not packaging")
	endif()
endforeach()

foreach(PLATFORM_RELEASE_LIB ${PLATFORM_RELEASE_LIBS})
	if (PLATFORM_RELEASE_LIB MATCHES ".*umundocoreSwig.*")
		install(FILES ${PLATFORM_RELEASE_LIB} DESTINATION share/umundo COMPONENT librarySwig)
		message(STATUS "PACKAGE RELEASE SWIG ${PLATFORM_RELEASE_LIB}")
	elseif (PLATFORM_RELEASE_LIB MATCHES ".*umundoserial.*")
		install(FILES ${PLATFORM_RELEASE_LIB} DESTINATION lib COMPONENT libraryS11N)
		message(STATUS "PACKAGE RELEASE SERIAL ${PLATFORM_RELEASE_LIB}")
	elseif (PLATFORM_RELEASE_LIB MATCHES ".*umundocore.*")
		install(FILES ${PLATFORM_RELEASE_LIB} DESTINATION lib COMPONENT libraryCore)
		message(STATUS "PACKAGE RELEASE CORE ${PLATFORM_RELEASE_LIB}")
	else()
		message(STATUS "PACKAGE RELEASE UNK ${PLATFORM_RELEASE_LIB} - not packaging")	
	endif()
endforeach()

if (UNIX)
	if (APPLE)
		set(CPACK_GENERATOR "PackageMaker;TGZ")
	else()
		set(CPACK_GENERATOR "DEB;RPM;TGZ")
	endif()
	set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
endif()
if (WIN32)
	set(CPACK_GENERATOR "NSIS;ZIP")
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "uMundo")
endif()

set(CPACK_PACKAGE_NAME "uMundo")
set(CPACK_PACKAGE_VENDOR "Telecooperation Group - TU Darmstadt")
set(CPACK_PACKAGE_CONTACT "radomski@tk.informatik.tu-darmstadt.de")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "uMundo - publish/subscribe since 2012")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/installer/description.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/installer/license.txt")

set(CPACK_PACKAGE_VERSION "0.0.1")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "1")

###
# Configuration for NSIS installer on Win32
#
# pairs of executables and labels for start menu
#CPACK_PACKAGE_EXECUTABLES
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "umundo.telecooperation.tu-darmstadt")
if (WIN32)
	set(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}\\\\installer\\\\nsis\\\\umundo-logo.bmp")
else()
	set(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/installer/nsis/umundo-logo.bmp")
endif()

###
# Configuration for PackageMaker on MacOSX
#
set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/installer/packageMaker/readme.txt")
set(CPACK_RESOURCE_FILE_WELCOME "${PROJECT_SOURCE_DIR}/installer/packageMaker/welcome.txt")

###
# Configuration for debian packages
#
set(CPACK_DEBIAN_PACKAGE_NAME "umundo")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libavahi-client3")
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "swig2.0, protobuf-compiler")

###
# Configuration for RPM packages
#
set(CPACK_RPM_PACKAGE_NAME "umundo")
set(CPACK_RPM_PACKAGE_LICENSE "BSD")

###
# Gather all components
#
set(CPACK_COMPONENTS_ALL 
	tools
	samples
	
	librarySwig
	libraryDebugSwig
	
	libraryCore
	headerCore
	libraryDebugCore
	 
	libraryS11N
	headerS11N
	libraryDebugS11N
)

###
# Description of components 
#
set(CPACK_COMPONENT_SAMPLES_DISPLAY_NAME "Sample Applications")
set(CPACK_COMPONENT_SAMPLES_DESCRIPTION 
  "Sample applications with source-code, illustrating the API and usage of the library.")

set(CPACK_COMPONENT_TOOLS_DISPLAY_NAME "Command-line Tools")
set(CPACK_COMPONENT_TOOLS_DESCRIPTION "Command-line tools to debug and monitor a umundo network.")

set(CPACK_COMPONENT_LIBRARYSWIG_DISPLAY_NAME "umundo.core JNI")
set(CPACK_COMPONENT_LIBRARYSWIG_DESCRIPTION "umundo.core library wrapped for Java per native interfaces. This will install the actual library and the JAR archive.")
set(CPACK_COMPONENT_LIBRARYDEBUGSWIG_DISPLAY_NAME "umundo.core JNI (Debug)")
set(CPACK_COMPONENT_LIBRARYDEBUGSWIG_DESCRIPTION "Debug version of umundo.core library wrapped for Java per native interfaces. This will install the actual library and the JAR archive.")

set(CPACK_COMPONENT_LIBRARYCORE_DISPLAY_NAME "umundo.core Library")
set(CPACK_COMPONENT_LIBRARYCORE_DESCRIPTION "Static library libumundocore with the basic pub/sub implementation and discovery.")
set(CPACK_COMPONENT_HEADERCORE_DISPLAY_NAME "umundo.core C++ Headers")
set(CPACK_COMPONENT_HEADERCORE_DESCRIPTION "C++ header files for umundo.core")
set(CPACK_COMPONENT_LIBRARYDEBUGCORE_DISPLAY_NAME "umundo.core Library (Debug)")
set(CPACK_COMPONENT_LIBRARYDEBUGCORE_DESCRIPTION "Debug version of static library libumundocore with the basic pub/sub implementation and discovery.")

set(CPACK_COMPONENT_LIBRARYS11N_DISPLAY_NAME "umundo.s11n Library")
set(CPACK_COMPONENT_LIBRARYS11N_DESCRIPTION "Static library libumundoserial with typed pub/sub and object serialization.")
set(CPACK_COMPONENT_HEADERS11N_DISPLAY_NAME "umundo.s11n C++ Headers")
set(CPACK_COMPONENT_HEADERS11N_DESCRIPTION "C++ header files for umundo.s11n")
set(CPACK_COMPONENT_LIBRARYDEBUGS11N_DISPLAY_NAME "umundo.s11n Library")
set(CPACK_COMPONENT_LIBRARYDEBUGS11N_DESCRIPTION "Static library libumundoserial with typed pub/sub and object serialization.")

###
# Interdependencies
#
set(CPACK_COMPONENT_LIBRARYS11N_DEPENDS libraryCore)
set(CPACK_COMPONENT_LIBRARYDEBUGS11N_DEPENDS libraryDebugCore)
set(CPACK_COMPONENT_HEADERS11N_DEPENDS headerCore)

###
# Grouping
#
set(CPACK_COMPONENT_SAMPLES_GROUP "Applications")
set(CPACK_COMPONENT_TOOLS_GROUP "Applications")

set(CPACK_COMPONENT_LIBRARYSWIG_GROUP "Development")
set(CPACK_COMPONENT_LIBRARYDEBUGSWIG_GROUP "Development")

set(CPACK_COMPONENT_LIBRARYCORE_GROUP "Development")
set(CPACK_COMPONENT_HEADERCORE_GROUP "Development")
set(CPACK_COMPONENT_LIBRARYDEBUGCORE_GROUP "Development")

set(CPACK_COMPONENT_LIBRARYS11N_GROUP "Development")
set(CPACK_COMPONENT_HEADERS11N_GROUP "Development")
set(CPACK_COMPONENT_LIBRARYDEBUGS11N_GROUP "Development")

# set(CPACK_COMPONENT_GROUP_APPLICATIONS_DESCRIPTION
#   "All of the tools you'll ever need to develop software")
