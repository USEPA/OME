cmake_minimum_required(VERSION 3.5)
project(OME)

#set targets
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#include directories

#src files

#compile as debug
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(OMERuntime)
add_subdirectory(OMEDraw)
add_subdirectory(OMEEngine)
add_subdirectory(ModelClassBuilder)
add_subdirectory(UniversalConverter)
add_subdirectory(CSVSpatialDataProvider)
