FIND_PATH(ZeroMQ_INCLUDE_DIR zmq.h
  HINTS
  $ENV{ZeroMQ_INCLUDE_DIR}
  PATH_SUFFIXES include
  PATHS
  /usr/local
  /usr
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
)

FIND_LIBRARY(ZeroMQ_LIBRARY 
  NAMES zmq libzmq
  HINTS
  $ENV{ZeroMQ_LIBRARY}
  PATH_SUFFIXES lib
  PATHS
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

FIND_LIBRARY(ZeroMQ_LIBRARY_DEBUG
  NAMES libzmq_d zmq_d zmq
  HINTS
  $ENV{ZeroMQ_LIBRARY}
  PATH_SUFFIXES lib
  ${ZEROMQ_PREBUILT}
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
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ZeroMQ DEFAULT_MSG ZeroMQ_LIBRARY ZeroMQ_LIBRARY_DEBUG ZeroMQ_INCLUDE_DIR)
MARK_AS_ADVANCED(ZeroMQ_INCLUDE_DIR ZeroMQ_LIBRARY ZeroMQ_LIBRARY_DEBUG)
