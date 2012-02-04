if(CMAKE_CROSSCOMPILING AND ANDROID)
	OPTION(DISC_BONJOUR_EMBED "Embed mDNS discovery service" ON)
	SET(CMAKE_FIND_LIBRARY_SUFFIXES .a)
	SET(BONJOUR_PREBUILT ${CONTRIB_PREBUILT}/bonjour/android/${ANDROID_NDK_ABI_EXT})
else()
	OPTION(DISC_BONJOUR_EMBED "Embed mDNS discovery service" OFF) 
	SET(BONJOUR_PREBUILT ${CONTRIB_PREBUILT}/bonjour/${CMAKE_SYSTEM_NAME_LC}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_CXX_COMPILER_ID_LC})
endif()

if (DISC_BONJOUR_EMBED)
	SET(BONJOUR_LIBNAME "mDnssdEmbed")
else()
	SET(BONJOUR_LIBNAME "dns_sd;dnssd")
endif()

FIND_PATH(Bonjour_INCLUDE_DIR dns_sd.h
  HINTS
  $ENV{BonjourDIR}
  PATH_SUFFIXES include
  PATHS
  /usr/local
  /usr
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
  ${CONTRIB_HEADERS}/bonjour # the headers we distribute
)


#set(PRE_BUILT_RE_PATH contrib/lib/${CMAKE_SYSTEM_NAME_LC}/${CMAKE_CXX_COMPILER_ID_LC})
#if(MSVC)
#	set(PRE_BUILT_RE_PATH ${PRE_BUILT_RE_PATH}/Debug)
#endif()

FIND_LIBRARY(Bonjour_LIBRARY 
  NAMES ${BONJOUR_LIBNAME}
  HINTS
  $ENV{BonjourDIR}
  PATHS
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
  ${BONJOUR_PREBUILT}
)

# handle the QUIETLY and REQUIRED arguments and set OPENAL_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Bonjour DEFAULT_MSG Bonjour_LIBRARY Bonjour_INCLUDE_DIR)
MARK_AS_ADVANCED(Bonjour_INCLUDE_DIR Bonjour_LIBRARY)
