
function(print)
  foreach (var ${ARGN})
    message("${var} = ${${var}}")
  endforeach ()
endfunction()


function(print_env)
  foreach (var ${ARGN})
    message("${var} = $ENV{${var}}")
  endforeach ()
endfunction()


function(print_all_vars argVarStart)
  get_cmake_property(_variableNames VARIABLES)
  foreach (_variableName ${_variableNames})
    string(REGEX MATCH "^${argVarStart}" _isMatched ${_variableName})
    if (_isMatched)
      message("*** ${_variableName}=${${_variableName}}")
    endif ()
  endforeach ()
endfunction()


function(get_git_commit_hash)
  if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    execute_process(
            COMMAND git rev-parse --short HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE ARG_GIT_COMMIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(GIT_COMMIT_HASH ${ARG_GIT_COMMIT_HASH} PARENT_SCOPE)
  endif ()
endfunction()


macro(search_for_library preFixName moduleName)
  pkg_check_modules(${preFixName} ${moduleName})
  if (${preFixName}_FOUND)
    link_directories(${${preFixName}_LIBRARY_DIRS})
  else ()
    find_package(${moduleName})
    if (NOT ${preFixName}_FOUND)
      message(FATAL_ERROR "Please install ${preFixName}")
    endif ()
  endif ()
  list(APPEND extraLibs ${${preFixName}_LIBRARIES})
  #list(APPEND extraLibs ${${preFixName}_LIBRARY})
  include_directories(${${preFixName}_INCLUDE_DIRS})
  #print_all_vars(${preFixName})
endmacro()


macro(try_find_vcpkg_toolset_path)
  if (DEFINED ENV{PKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    # or at cmake prompt: -DCMAKE_TOOLCHAIN_FILE=c:\vcpkg\scripts\buildsystems\vcpkg.cmake
    set(CMAKE_TOOLCHAIN_FILE "$ENV{PKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    message("Automatically set toolchain file to ${CMAKE_TOOLCHAIN_FILE}")
  endif ()
endmacro()