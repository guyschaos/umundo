FIND_PATH(Avahi_INCLUDE_DIR avahi-client/client.h
  HINTS
  $ENV{AvahiDIR}
  PATH_SUFFIXES include
  PATHS
  /usr/local
  /usr
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
)


#set(PRE_BUILT_RE_PATH contrib/lib/${CMAKE_SYSTEM_NAME_LC}/${CMAKE_CXX_COMPILER_ID_LC})
#if(MSVC)
#	set(PRE_BUILT_RE_PATH ${PRE_BUILT_RE_PATH}/Debug)
#endif()

FIND_LIBRARY(Avahi_LIBRARY 
  NAMES avahi-client
  HINTS
  $ENV{AvahiDIR}
  PATHS
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
#  ${PRE_BUILT_RE_PATH}
)

# handle the QUIETLY and REQUIRED arguments and set OPENAL_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Avahi DEFAULT_MSG Avahi_LIBRARY Avahi_INCLUDE_DIR)
MARK_AS_ADVANCED(Avahi_INCLUDE_DIR Avahi_LIBRARY)
