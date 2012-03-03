if (DISC_BONJOUR_EMBED)
	SET(BONJOUR_LIBNAME "mDNSEmbedded")
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

FIND_LIBRARY(Bonjour_LIBRARY_DEBUG
  NAMES ${BONJOUR_LIBNAME}_d ${BONJOUR_LIBNAME}
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
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Bonjour DEFAULT_MSG Bonjour_LIBRARY Bonjour_LIBRARY_DEBUG Bonjour_INCLUDE_DIR)
MARK_AS_ADVANCED(Bonjour_INCLUDE_DIR Bonjour_LIBRARY)
