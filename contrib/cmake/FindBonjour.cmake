if(CMAKE_CROSSCOMPILING AND ANDROID)
	OPTION(DISC_BONJOUR_EMBED "Embed mDNS discovery service" ON)
else()
	OPTION(DISC_BONJOUR_EMBED "Embed mDNS discovery service" OFF) 
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
)

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
)

# handle the QUIETLY and REQUIRED arguments and set OPENAL_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Bonjour DEFAULT_MSG Bonjour_LIBRARY Bonjour_INCLUDE_DIR)
MARK_AS_ADVANCED(Bonjour_INCLUDE_DIR Bonjour_LIBRARY)
