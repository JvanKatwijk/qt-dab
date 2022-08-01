if(NOT FDK_AAC_FOUND)

  pkg_check_modules (FDK_AAC_PKG libfdk-aac)
  find_path(FDK_AAC_INCLUDE_DIR NAMES fdk-aac/aacdecoder_lib.h
    PATHS
    ${FDK_AAC_PKG_INCLUDE_DIRS}
    /usr/include
    /usr/local/include
  )

  find_library(FDK_AAC_LIBRARIES NAMES fdk-aac
    PATHS
    ${FDK_AAC_PKG_LIBRARY_DIRS}
    /usr/lib
    /usr/lib/lib64
    /usr/local/lib
  )

  if(FDK_AAC_INCLUDE_DIR AND FDK_AAC_LIBRARIES)
    set(LIBFDK_AAC_FOUND TRUE CACHE INTERNAL "libfdk-aac found")
    message(STATUS "Found libfdk-aac: ${FDK_AAC_INCLUDE_DIR}, ${FDK_AAC_LIBRARIES}")
  else(FDK_AAC_INCLUDE_DIR AND FDK_AAC_LIBRARIES)
    set(FDK_AAC_FOUND FALSE CACHE INTERNAL "libfdk_aac found")
    message(STATUS "libfdk_aac not found.")
  endif(FDK_AAC_INCLUDE_DIR AND FDK_AAC_LIBRARIES)

  mark_as_advanced(FDK_AAC_INCLUDE_DIR FDK_AAC_LIBRARIES)

endif(NOT FDK_AAC_FOUND)
