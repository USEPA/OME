cmake_minimum_required(VERSION 3.5)
project(CSVSpatialDataProvider)

#include directories
set(INCLUDE_DIRS
  "../../../extern_support/Shiny/include"
  "../../../include"
  "../../../include/CSV-SDP"
  "../../../include/OMERuntime"
  "../../../extern_support/tinyxml2"
  "../../../extern_support/lua/src"
  ${MY_INCLUDES}
)

#src files
set(SOURCES
  "../../../src/CSV-SDP/CSVSpatialDataProvider.cpp"
  "../../../extern_support/tinyxml2/tinyxml2.cpp" 
 )

#compile as debug
set(CMAKE_BUILD_TYPE Debug)

#link in lua lib; it must be built before this run
set(LINK_LIBS lua OMERuntime m )
link_directories("${PROJECT_BINARY_DIR}" "../../../extern_support/lua/lib")
link_directories(${MY_LIB_DIRS})

#rpath settings (from https://cmake.org/Wiki/CMake_RPATH_handling under always full rpath)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/LIB" isSysDir)
if("${isSysDir}" STREQUAL "-1")
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif("${isSysDir}" STREQUAL "-1")

#preproc macros
add_definitions(
  -std=c++11
  -D__EXPORT__=
  -D__SDP_EXPORT__=
  -DPASCAL=
  -DMAXPATHLEN=4096
  -DLUA_COMPAT_5_1
  -DOME_SHOW_DEBUG
  ${MY_CXX_FLAGS}
)

include_directories(${INCLUDE_DIRS})

add_library(CSVSpatialDataProvider SHARED ${SOURCES})
target_link_libraries(CSVSpatialDataProvider PRIVATE ${LINK_LIBS})