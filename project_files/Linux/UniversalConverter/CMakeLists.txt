cmake_minimum_required(VERSION 3.5)
project(UniversalConverter)

#include directories
set(INCLUDE_DIRS
  "../../../extern_support/Shiny/include"
  "../../../include"
  "../../../include/OMERuntime"
  "../../../include/OMEDraw"
  "../../../include/UniversalConverter"
  "../../../include/MainEntries"
  "../../../extern_support/lua/src"
  "../../../extern_support/tinyxml2"
  ${MY_INCLUDES}
)

#src files
set(SOURCES
    "../../../src/UniversalConverter/ConverterUtils.cpp" 
    "../../../src/MainEntries/UniversalConverter.cpp" 
    "../../../src/UniversalConverter/MDLHandlers/MDLEqnRecord.cpp" 
    "../../../src/UniversalConverter/MDLHandlers/MDLParser.cpp" 
    "../../../src/UniversalConverter/ModifierProcessor.cpp" 
    "../../../src/OMERuntime/omeobjects/NullArcLinkMap.cpp" 
    "../../../src/OMERuntime/omeobjects/NullCloud.cpp" 
    "../../../src/OMERuntime/omeobjects/NullFunc.cpp" 
    "../../../src/OMERuntime/omeobjects/NullProperties.cpp" 
    "../../../src/OMERuntime/omeobjects/NullSource.cpp" 
    "../../../src/UniversalConverter/SMLHandlers/Base64Decoder.cpp" 
    "../../../src/UniversalConverter/SMLHandlers/MIMEDecoder.cpp" 
    "../../../src/UniversalConverter/SMLHandlers/MIMEParser.cpp" 
    "../../../src/UniversalConverter/SMLHandlers/pltokens/PLTArc.cpp" 
    "../../../src/UniversalConverter/SMLHandlers/pltokens/PLTLinks.cpp" 
    "../../../src/UniversalConverter/SMLHandlers/pltokens/PLTNode.cpp" 
    "../../../src/UniversalConverter/SMLHandlers/pltokens/PLToken.cpp" 
    "../../../src/UniversalConverter/SMLHandlers/pltokens/PLTProperties.cpp" 
    "../../../src/UniversalConverter/SMLHandlers/pltokens/PLTSource.cpp" 
    "../../../src/UniversalConverter/SMLHandlers/PrologParser.cpp" 
    "../../../src/UniversalConverter/SMLHandlers/SmlParser.cpp" 
    "../../../src/UniversalConverter/XMILEHandlers/XMILENodeProcessor.cpp" 
    "../../../src/UniversalConverter/XMILEHandlers/XMILEOptionSet.cpp" 
    "../../../extern_support/tinyxml2/tinyxml2.cpp" 
 )

#compile as debug
set(CMAKE_BUILD_TYPE Debug)

#link in lua lib; it must be built before this run
set(LINK_LIBS OMERuntime OMEDraw c++ m dl ${MY_LIBS})
link_directories("${PROJECT_BINARY_DIR}")
link_directories("../../../extern_support/lua/lib")
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
  -DPASCAL=
  -DMAXPATHLEN=4096
  -DLUA_COMPAT_5_1
  -DOME_SHOW_DEBUG
  ${MY_CXX_FLAGS}
)

include_directories(${INCLUDE_DIRS})

add_executable(UniversalConverter ${SOURCES})
target_link_libraries(UniversalConverter ${LINK_LIBS})