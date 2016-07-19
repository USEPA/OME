cmake_minimum_required(VERSION 3.5)
project(OMERuntime)

#include directories
set(INCLUDE_DIRS
  "../../../extern_support/Shiny/include"
  "../../../include"
  "../../../include/OMERuntime"
  "../../../extern_support/lua/src"
  "../../../extern_support/tinyxml2"
  ${MY_INCLUDES}
)

#src files
set(SOURCES
    "../../../extern_support/tinyxml2/tinyxml2.cpp" 
    "../../../src/OMERuntime/BaseEvalSerializer.cpp" 
    "../../../src/OMERuntime/BaseManager.cpp" 
    "../../../src/OMERuntime/BaseSpatialDataProvider.cpp" 
    "../../../src/OMERuntime/CompiledMdlFuncs.cpp" 
    "../../../src/OMERuntime/CompiledMdlParser.cpp" 
    "../../../src/OMERuntime/CompiledMdlWrappers.cpp" 
    "../../../src/OMERuntime/CPPLCAccumEntry.cpp" 
    "../../../src/OMERuntime/CPPLCSubEntry.cpp" 
    "../../../src/OMERuntime/CPPLoopCluster.cpp" 
    "../../../src/OMERuntime/CPPSerializer.cpp" 
    "../../../src/OMERuntime/DataSrc.cpp" 
    "../../../src/OMERuntime/DebugOutput.cpp" 
    "../../../src/OMERuntime/ErrCollection.cpp" 
    "../../../src/OMERuntime/EvalAlias.cpp" 
    "../../../src/OMERuntime/EvalXSetInterpTable.cpp" 
    "../../../src/OMERuntime/HiResIntegrationBlock.cpp" 
    "../../../src/OMERuntime/InitializerSet.cpp" 
    "../../../src/OMERuntime/LuaEvalSerializer.cpp" 
    "../../../src/OMERuntime/LuaMemPool.cpp" 
    "../../../src/OMERuntime/LuaOMEParser.cpp" 
    "../../../src/OMERuntime/ModelEnum.cpp" 
    "../../../src/OMERuntime/OMEDefines.cpp" 
    "../../../src/OMERuntime/OMEException.cpp" 
    "../../../src/OMERuntime/OMEIntegrationBlock.cpp" 
    "../../../src/OMERuntime/OMELuaDefines.cpp" 
    "../../../src/OMERuntime/OMELuaFunc.cpp" 
    "../../../src/OMERuntime/OMEParser.cpp" 
    "../../../src/OMERuntime/RecordMgr.cpp" 
    "../../../src/OMERuntime/SDPMapEntry.cpp" 
    "../../../src/OMERuntime/SDPMapSubEntry.cpp" 
    "../../../src/OMERuntime/SimManager.cpp" 
    "../../../src/OMERuntime/Summarizable.cpp" 
    "../../../src/OMERuntime/Units.cpp" 
    "../../../src/OMERuntime/ValueArchive.cpp" 
    "../../../src/OMERuntime/events/CentralEvent.cpp" 
    "../../../src/OMERuntime/events/CentralEventDispatcher.cpp" 
    "../../../src/OMERuntime/events/OMEEvent.cpp" 
    "../../../src/OMERuntime/omeobjects/AssocPort.cpp" 
    "../../../src/OMERuntime/omeobjects/Conveyor.cpp" 
    "../../../src/OMERuntime/omeobjects/EvalInterpTable.cpp" 
    "../../../src/OMERuntime/omeobjects/EvalTable.cpp" 
    "../../../src/OMERuntime/omeobjects/Evaluable.cpp" 
    "../../../src/OMERuntime/omeobjects/Flow.cpp" 
    "../../../src/OMERuntime/omeobjects/Influence.cpp" 
    "../../../src/OMERuntime/omeobjects/IterConditional.cpp" 
    "../../../src/OMERuntime/omeobjects/Link.cpp" 
    "../../../src/OMERuntime/omeobjects/MIValue.cpp" 
    "../../../src/OMERuntime/omeobjects/Model.cpp" 
    "../../../src/OMERuntime/omeobjects/ModelInstance.cpp" 
    "../../../src/OMERuntime/omeobjects/MultiPort.cpp" 
    "../../../src/OMERuntime/omeobjects/OMENull.cpp" 
    "../../../src/OMERuntime/omeobjects/OMEObject.cpp" 
    "../../../src/OMERuntime/omeobjects/Param.cpp" 
    "../../../src/OMERuntime/omeobjects/SimObjs.cpp" 
    "../../../src/OMERuntime/omeobjects/SimpleLabel.cpp" 
    "../../../src/OMERuntime/omeobjects/Spawner.cpp" 
    "../../../src/OMERuntime/omeobjects/StateVar.cpp" 
    "../../../src/OMERuntime/omeobjects/SubmodelAssoc.cpp" 
    "../../../src/OMERuntime/omeobjects/SubmodelPort.cpp" 
    "../../../src/OMERuntime/omeobjects/TimeSeriesVar.cpp" 
    "../../../src/OMERuntime/omeobjects/VarArray.cpp" 
    "../../../src/OMERuntime/omeobjects/Variable.cpp" 
    "../../../src/OMERuntime/omeobjects/XMLLoadable.cpp" 
    "../../../src/OMERuntime/UpdateContainers/DynamicUpArray.cpp" 
    "../../../src/OMERuntime/UpdateContainers/IndexJumpTable.cpp" 
    "../../../src/OMERuntime/UpdateContainers/Listable.cpp" 
    "../../../src/OMERuntime/UpdateContainers/ListDims.cpp" 
    "../../../src/OMERuntime/UpdateContainers/ShuffledList.cpp" 
    "../../../src/OMERuntime/UpdateContainers/SimpleArrayWrapper.cpp" 
    "../../../src/OMERuntime/UpdateContainers/StaticUpArray.cpp" 
    "../../../src/OMERuntime/UpdateContainers/TempPoolArray.cpp" 
    "../../../src/OMERuntime/UpdateContainers/TempVCArray.cpp" 
 )

#compile as debug
set(CMAKE_BUILD_TYPE Debug)

#link in lua lib; it must be built before this run
set(LINK_LIBS lua ${MY_LIBS})
link_directories("../../../extern_support/lua/lib")
link_directories(${MY_LIB_DIRS})

#rpath settings (from https://cmake.org/Wiki/CMake_RPATH_handling under always full rpath)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSysDir)
if("${isSysDir}" STREQUAL "-1")
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif("${isSysDir}" STREQUAL "-1")

#preproc macros
add_definitions(
  -std=c++11
  -D__EXPORT__=
  -DPASCAL=
  -DMAXPATHLEN=4096
  -DOME_SHOW_DEBUG
  ${MY_CXX_FLAGS}
)

include_directories(${INCLUDE_DIRS})

add_library(OMERuntime SHARED ${SOURCES})
target_link_libraries(OMERuntime PRIVATE ${LINK_LIBS})