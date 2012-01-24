SET(BONJOUR_PREBUILT ${CONTRIB_PREBUILT}/bonjour/${CMAKE_SYSTEM_NAME_LC}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_CXX_COMPILER_ID_LC})

FIND_PATH(Bonjour_INCLUDE_DIR dns_sd.h
  HINTS
  $ENV{BonjourDIR}
  PATH_SUFFIXES include
  PATHS
  /usr/local
  /usr
  /usr/include/avahi-compat-libdns_sd # avahi debian
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
  NAMES dns_sd dnssd
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
