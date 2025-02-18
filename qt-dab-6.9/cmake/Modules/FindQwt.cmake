# - try to find Qwt libraries and include files
# QWT_INCLUDE_DIR where to find qwt_global.h, etc.
# QWT_LIBRARIES libraries to link against
# QWT_FOUND If false, do not try to use Qwt
# qwt_global.h holds a string with the QWT version;
#   test to make sure it's at least 5.2

find_path(QWT_INCLUDE_DIRS
	NAMES qwt_global.h
	HINTS
	${CMAKE_INSTALL_PREFIX}/include/qwt
	${CMAKE_INSTALL_PREFIX}/include/qwt-qt6
	PATHS
	/usr/local/include/qwt-qt6
	/usr/local/include/qwt
	/usr/include/qwt6
	/usr/include/qwt6-qt6
	/usr/include/qt6/qwt
	/opt/local/include/qwt
	/sw/include/qwt
	/usr/local/lib/qwt.framework/Headers
	/usr/local/lib/qwt-qt5/lib/framework/Headers
	/usr/local/qwt-6.2.0/include
)
if (APPLE)
	set(CMAKE_FIND_LIBRARY_SUFFIXES " " " .dylib" ".so" ".a ")
endif (APPLE)

find_library (QWT_LIBRARIES
	NAMES qwt6 qwt6-qt6 qwt-qt6 qwt 
	HINTS
	${CMAKE_INSTALL_PREFIX}/lib
	${CMAKE_INSTALL_PREFIX}/lib64
	PATHS
	/usr/local/lib
	/usr/lib
	/opt/local/lib
	/sw/lib
	/usr/local/lib/qwt.framework
	/usr/local/lib/qwt-qt6/lib/framework
	/usr/local/qwt-6.2.0/lib
)

set(QWT_FOUND FALSE)
if(QWT_INCLUDE_DIRS)
  file(STRINGS "${QWT_INCLUDE_DIRS}/qwt_global.h"
    QWT_STRING_VERSION REGEX "QWT_VERSION_STR")
  set(QWT_WRONG_VERSION True)
  set(QWT_VERSION "No Version")
  string(REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" QWT_VERSION ${QWT_STRING_VERSION})
  string(COMPARE LESS ${QWT_VERSION} "5.2.0" QWT_WRONG_VERSION)
  string(COMPARE GREATER ${QWT_VERSION} "6.2.0" QWT_WRONG_VERSION)

  message(STATUS "QWT Version: ${QWT_VERSION}")
  if(NOT QWT_WRONG_VERSION)
    set(QWT_FOUND TRUE)
  else(NOT QWT_WRONG_VERSION)
    message(STATUS "QWT Version must be >= 5.2 and <= 6.2.0, Found ${QWT_VERSION}")
  endif(NOT QWT_WRONG_VERSION)

endif(QWT_INCLUDE_DIRS)

if(QWT_FOUND)
  # handle the QUIETLY and REQUIRED arguments and set QWT_FOUND to TRUE if
  # all listed variables are TRUE
  include ( FindPackageHandleStandardArgs )
  find_package_handle_standard_args( Qwt DEFAULT_MSG QWT_LIBRARIES QWT_INCLUDE_DIRS )
  MARK_AS_ADVANCED(QWT_LIBRARIES QWT_INCLUDE_DIRS)
endif(QWT_FOUND)
