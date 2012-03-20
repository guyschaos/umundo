# - Find UMundo
# This module checks if UMundo is installed and determines where the
# include files and libraries are. This code sets the following
# variables:
#
# UMUNDO_INCLUDE_DIR             = The full path to the umundo headers
# UMUNDO_LIBRARIES               = All umundo libraries for release build
# UMUNDO_LIBRARIES_DEBUG         = All umundo libraries for debug build
# UMUNDO_CORE_LIBRARY            = The umundo.core library for release builds
# UMUNDO_CORE_LIBRARY_DEBUG      = The umundo.core library for debug builds
# UMUNDO_S11N_LIBRARY            = The umundo.s11n library for release builds
# UMUNDO_S11N_LIBRARY_DEBUG      = The umundo.s11n library for debug builds
# UMUNDO_RPC_LIBRARY             = The umundo.rpc library for release builds
# UMUNDO_RPC_LIBRARY_DEBUG       = The umundo.rpc library for debug builds
#

FIND_PATH(UMUNDO_INCLUDE_DIR umundo/core.h
  PATH_SUFFIXES include PATHS 
		/usr/local 
		/opt/local
		C:\\Program\ Files\ \(x86\)\\uMundo
	ENV UMUNDO_INCLUDE_DIR
)

FIND_LIBRARY(UMUNDO_CORE_LIBRARY 
  NAMES umundocore PATHS 
		/usr/local 
		/opt/local
		C:\\Program\ Files\ \(x86\)\\uMundo
	ENV UMUNDO_LIB_DIR
)

FIND_LIBRARY(UMUNDO_CORE_LIBRARY_DEBUG
  NAMES umundocore_d PATHS 
		/usr/local 
		/opt/local
		C:\\Program\ Files\ \(x86\)\\uMundo
	ENV UMUNDO_LIB_DIR
)

FIND_LIBRARY(UMUNDO_S11N_LIBRARY 
  NAMES umundoserial PATHS
		/usr/local 
		/opt/local
		C:\\Program\ Files\ \(x86\)\\uMundo
	ENV UMUNDO_LIB_DIR
)

FIND_LIBRARY(UMUNDO_S11N_LIBRARY_DEBUG
  NAMES umundoserial_d PATHS
		/usr/local 
		/opt/local
		C:\\Program\ Files\ \(x86\)\\uMundo
	ENV UMUNDO_LIB_DIR
)

FIND_LIBRARY(UMUNDO_RPC_LIBRARY 
  NAMES umundorpc PATHS
		/usr/local 
		/opt/local
		C:\\Program\ Files\ \(x86\)\\uMundo
	ENV UMUNDO_LIB_DIR
)

FIND_LIBRARY(UMUNDO_RPC_LIBRARY_DEBUG
  NAMES umundorpc_d PATHS
		/usr/local 
		/opt/local
		C:\\Program\ Files\ \(x86\)\\uMundo
	ENV UMUNDO_LIB_DIR
)

SET(UMUNDO_LIBRARIES optimized ${UMUNDO_CORE_LIBRARY})
LIST(APPEND UMUNDO_LIBRARIES optimized ${UMUNDO_S11N_LIBRARY})
LIST(APPEND UMUNDO_LIBRARIES optimized ${UMUNDO_RPC_LIBRARY})

SET(UMUNDO_LIBRARIES_DEBUG debug ${UMUNDO_CORE_LIBRARY_DEBUG})
LIST(APPEND UMUNDO_LIBRARIES_DEBUG debug ${UMUNDO_S11N_LIBRARY_DEBUG})
LIST(APPEND UMUNDO_LIBRARIES_DEBUG debug ${UMUNDO_RPC_LIBRARY_DEBUG})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(UMUNDO DEFAULT_MSG UMUNDO_CORE_LIBRARY)
MARK_AS_ADVANCED(UMUNDO_INCLUDE_DIR UMUNDO_CORE_LIBRARY UMUNDO_S11N_LIBRARY UMUNDO_RPC_LIBRARY UMUNDO_CORE_LIBRARY_DEBUG UMUNDO_S11N_LIBRARY_DEBUG UMUNDO_RPC_LIBRARY_DEBUG UMUNDO_INCLUDED_DIR)
