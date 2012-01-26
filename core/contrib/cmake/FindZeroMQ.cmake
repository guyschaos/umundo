SET(ZEROMQ_PREBUILT ${CONTRIB_PREBUILT}/zeromq/${CMAKE_SYSTEM_NAME_LC}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_CXX_COMPILER_ID_LC})

if(CMAKE_CROSSCOMPILING AND IOS)
	SET(CMAKE_FIND_LIBRARY_SUFFIXES .a)
	SET(ZEROMQ_PREBUILT ${CONTRIB_PREBUILT}/zeromq/ios/${CMAKE_SYSTEM_VERSION})
endif()

if(CMAKE_CROSSCOMPILING AND ANDROID)
	SET(CMAKE_FIND_LIBRARY_SUFFIXES .a)
	SET(ZEROMQ_PREBUILT ${CONTRIB_PREBUILT}/zeromq/android/${ANDROID_NDK_ABI_EXT})
endif()

FIND_PATH(ZeroMQ_INCLUDE_DIR zmq.h
  HINTS
  $ENV{ZeroMQ_INCLUDE_DIR}
  ${CONTRIB_HEADERS}/zeromq
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
  ${ZEROMQ_PREBUILT}
  PATHS
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

FIND_LIBRARY(ZeroMQ_LIBRARY_DEBUG
  NAMES zmq libzmq_d
  HINTS
  $ENV{ZeroMQ_LIBRARY}
  ${ZEROMQ_PREBUILT}
  PATHS
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

message(${ZeroMQ_LIBRARY} - ${ZeroMQ_INCLUDE_DIR})

if(NOT ZeroMQ_LIBRARY AND IPHONE)
	message(" ")
	message("    Could not find ZeroMQ for iOS.")
	message("    run contrib/build_zeromq_iphone.sh from within the zeromq folder and make sure the following library is created:")
	message("    ${PRE_BUILT_ZeroMQ_LIBRARY}/libzmq.a")
	message(" ")
endif()

# handle the QUIETLY and REQUIRED arguments and set OPENAL_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ZeroMQ DEFAULT_MSG ZeroMQ_LIBRARY ZeroMQ_LIBRARY_DEBUG ZeroMQ_INCLUDE_DIR)
MARK_AS_ADVANCED(ZeroMQ_INCLUDE_DIR ZeroMQ_LIBRARY ZeroMQ_LIBRARY_DEBUG)
