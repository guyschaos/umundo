#message("Looking for iOS boost")

execute_process(
	COMMAND ln -sf ${PROJECT_SOURCE_DIR}/contrib/prebuilt/boost.framework/Headers ${PROJECT_SOURCE_DIR}/contrib/prebuilt/boost
)

FIND_PATH(Boost_INCLUDE_DIR boost/version.hpp
  HINTS $ENV{Boost_INCLUDE_DIR}
  PATHS contrib/prebuilt
)
STRING(REGEX REPLACE "(.*)/[^/]+" "\\1" Boost_INCLUDE_DIR ${Boost_INCLUDE_DIR})

FIND_LIBRARY(Boost_LIBRARY 
  NAMES boost
  HINTS $ENV{Boost_LIBRARY}
  PATHS contrib/prebuilt
)

# handle the QUIETLY and REQUIRED arguments and set OPENAL_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Boost DEFAULT_MSG Boost_LIBRARY Boost_INCLUDE_DIR)
MARK_AS_ADVANCED(Boost_INCLUDE_DIR Boost_LIBRARY)
