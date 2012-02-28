# see: http://www.vtk.org/Wiki/CMake:CPackConfiguration

#
# Throughout the source, we used INSTALL_HEADERS_WITH_DIRECTORY to 
# prepare header files for installation. Now add the actual libraries 
# and invoke the packager
# 

########################################
# gather libraries for package
########################################

file(GLOB_RECURSE PLATFORM_LIBS 
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
	${LIBRARY_ROOT_PATH}/Release/*.a
	${LIBRARY_ROOT_PATH}/Release/*.so
	${LIBRARY_ROOT_PATH}/Release/*.lib
	${LIBRARY_ROOT_PATH}/Release/*.jar
	${LIBRARY_ROOT_PATH}/Release/*.dylib
	${LIBRARY_ROOT_PATH}/Release/*.dll
	${LIBRARY_ROOT_PATH}/Release/*.exp
	${LIBRARY_ROOT_PATH}/Release/*.jnilib
)

# platform dependant libraries
foreach(PLATFORM_LIB ${PLATFORM_LIBS})
	if (PLATFORM_LIB MATCHES ".*umundocoreSwig.*")
		install(FILES ${PLATFORM_LIB} DESTINATION share/umundo/java COMPONENT librarySwig)
#		message(STATUS "PACKAGE RELEASE SWIG ${PLATFORM_LIB}")
	elseif (PLATFORM_LIB MATCHES ".*umundoserial.*")
		install(FILES ${PLATFORM_LIB} DESTINATION lib COMPONENT libraryS11N)
#		message(STATUS "PACKAGE RELEASE SERIAL ${PLATFORM_LIB}")
	elseif (PLATFORM_LIB MATCHES ".*umundocore.*")
		install(FILES ${PLATFORM_LIB} DESTINATION lib COMPONENT libraryCore)
#		message(STATUS "PACKAGE RELEASE CORE ${PLATFORM_LIB}")
	else()
		message(STATUS "PACKAGE RELEASE UNK ${PLATFORM_LIB} - not packaging")	
	endif()
endforeach()

if (WIN32)
	# we still have problems packaging jars in windows, use one from another platform
	file(GLOB_RECURSE SWIG_JARS ${PROJECT_SOURCE_DIR}/lib/*umundocoreSwigJNI.jar)
	list(REVERSE SWIG_JARS) # do not use the android jars as they are more likely out of date :(
	list(GET SWIG_JARS 0 SWIG_JAR)
	install(FILES ${SWIG_JAR} DESTINATION share/umundo COMPONENT librarySwig)
endif()

########################################
# Include documentation
########################################

file(GLOB_RECURSE HTML_DOCS ${PROJECT_SOURCE_DIR}/core/docs/html/*)
foreach(HTML_DOC ${HTML_DOCS})
	STRING(REGEX REPLACE "${PROJECT_SOURCE_DIR}/core/" "" HTML_PATH ${HTML_DOC})
	STRING(REGEX MATCH "(.*)[/\\]" HTML_PATH ${HTML_PATH})
	install(FILES ${HTML_DOC} DESTINATION share/umundo/${HTML_PATH} COMPONENT docs)
#	message(STATUS ${HTML_PATH})
endforeach()


########################################
# Support for cross compiling
########################################

# add cross-compile target support for android
file(GLOB_RECURSE ANDROID_LIBS ${PROJECT_SOURCE_DIR}/lib/android*/*)
foreach(ANDROID_LIB ${ANDROID_LIBS})
	# match android-14/armv7-a
	STRING(REGEX REPLACE "//" "/" ANDROID_LIB ${ANDROID_LIB})
	STRING(REGEX REPLACE "${PROJECT_SOURCE_DIR}/lib/" "" ANDROID_PATH ${ANDROID_LIB})
	STRING(REGEX MATCH "[^/]*/[^/]*" ANDROID_PATH ${ANDROID_PATH})
	install(FILES ${ANDROID_LIB} DESTINATION share/umundo/${ANDROID_PATH} COMPONENT libraryAndroid)
endforeach()

# add cross-compile target support for ios
if (APPLE)
	file(GLOB_RECURSE IOS_LIBS ${PROJECT_SOURCE_DIR}/lib/ios*/*.ios*)
	foreach(IOS_LIB ${IOS_LIBS})
		# match ios-5.0
		STRING(REGEX REPLACE "//" "/" IOS_LIB ${IOS_LIB})
		STRING(REGEX REPLACE "${PROJECT_SOURCE_DIR}/lib/" "" IOS_PATH ${IOS_LIB})
		STRING(REGEX MATCH "[^/]*" IOS_PATH ${IOS_PATH})
		install(FILES ${IOS_LIB} DESTINATION share/umundo/${IOS_PATH} COMPONENT libraryIOS)
	endforeach()
endif()

########################################
# Configure packagers
########################################

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

set(CPACK_PACKAGE_VERSION "0.0.2")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "2")

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
set(CPACK_RPM_PACKAGE_LICENSE "CDDL")


########################################
# Describe layout of package
########################################

set(CPACK_COMPONENTS_ALL 
	tools
#	samples
	docs
	
	librarySwig
	libraryAndroid
#	libraryIOS
	
	libraryCore
	headerCore
	 
	libraryS11N
	headerS11N
)

###
# Description of components 
#
set(CPACK_COMPONENT_TOOLS_DISPLAY_NAME "Command-line Tools")
set(CPACK_COMPONENT_TOOLS_DESCRIPTION "Command-line tools to debug and monitor a umundo network.")

# set(CPACK_COMPONENT_SAMPLES_DISPLAY_NAME "Sample Applications with IDE templates")
# set(CPACK_COMPONENT_SAMPLES_DESCRIPTION 
#   "Sample applications with source-code, illustrating the API and usage of the library.")

set(CPACK_COMPONENT_DOCS_DISPLAY_NAME "Documentation")
set(CPACK_COMPONENT_DOCS_DESCRIPTION "Auto-generated documentation.")

set(CPACK_COMPONENT_LIBRARYSWIG_DISPLAY_NAME "umundo.core JNI")
set(CPACK_COMPONENT_LIBRARYSWIG_DESCRIPTION "umundo.core library wrapped for Java per native interfaces. This will install the actual library and the JAR archive.")
set(CPACK_COMPONENT_LIBRARYANDROID_DISPLAY_NAME "Cross Compiled for Android")
set(CPACK_COMPONENT_LIBRARYANDROID_DESCRIPTION "umundo.core cross compiled for Android devices.")
#set(CPACK_COMPONENT_LIBRARYIOS_DISPLAY_NAME "Cross Compiled for iOS")
#set(CPACK_COMPONENT_LIBRARYIOS_DESCRIPTION "umundo.core cross compiled for iOS devices (universal libraries).")

set(CPACK_COMPONENT_LIBRARYCORE_DISPLAY_NAME "Library umundo.core")
set(CPACK_COMPONENT_LIBRARYCORE_DESCRIPTION "Static library libumundocore with the basic pub/sub implementation and discovery.")
set(CPACK_COMPONENT_HEADERCORE_DISPLAY_NAME "C++ Headers umundo.core")
set(CPACK_COMPONENT_HEADERCORE_DESCRIPTION "C++ header files for umundo.core")

set(CPACK_COMPONENT_LIBRARYS11N_DISPLAY_NAME "Library umundo.s11n")
set(CPACK_COMPONENT_LIBRARYS11N_DESCRIPTION "Static library libumundoserial with typed pub/sub and object serialization.")
set(CPACK_COMPONENT_HEADERS11N_DISPLAY_NAME "C++ Headers umundo.s11n")
set(CPACK_COMPONENT_HEADERS11N_DESCRIPTION "C++ header files for umundo.s11n")

###
# Interdependencies (are they transitive?)
#
set(CPACK_COMPONENT_LIBRARYCORE_DEPENDS headerCore)
set(CPACK_COMPONENT_LIBRARYS11N_DEPENDS libraryCore)
set(CPACK_COMPONENT_LIBRARYS11N_DEPENDS headerS11N)

# set(CPACK_COMPONENT_SAMPLES_DEPENDS libraryS11N)
# set(CPACK_COMPONENT_SAMPLES_DEPENDS libraryCore)

#set(CPACK_COMPONENT_LIBRARYIOS_DEPENDS headerCore)
#set(CPACK_COMPONENT_LIBRARYIOS_DEPENDS headerS11N)

###
# Grouping
#
#set(CPACK_COMPONENT_SAMPLES_GROUP "Samples")
#set(CPACK_COMPONENT_TOOLS_GROUP "Tools")

set(CPACK_COMPONENT_LIBRARYSWIG_GROUP "Development")
set(CPACK_COMPONENT_LIBRARYDEBUGSWIG_GROUP "Development")

set(CPACK_COMPONENT_LIBRARYCORE_GROUP "Development")
set(CPACK_COMPONENT_HEADERCORE_GROUP "Development")

set(CPACK_COMPONENT_LIBRARYS11N_GROUP "Development")
set(CPACK_COMPONENT_HEADERS11N_GROUP "Development")

set(CPACK_COMPONENT_LIBRARYANDROID_GROUP "Development")
#set(CPACK_COMPONENT_LIBRARYIOS_GROUP "Development")

# set(CPACK_COMPONENT_GROUP_APPLICATIONS_DESCRIPTION
#   "All of the tools you'll ever need to develop software")
