// SDPDLL.cpp : Defines the exported functions for the DLL application.
//

#include "SDPDylib.h"
#include "CSVSpatialDataProvider.h"
#include "LuaOMEParser.h"

extern "C" ISpatialDataProvider* GenSDP(SimManager* pSim,Model* pRoot, TI2Element* pSDPElem, const char* modelPath)
{
    //return a new instance of the target model
    //pick and choose arguments for specified
    
    //either return SpatialDataProvider on success, or NULL on failure.
}

extern "C" void DisposeSDP(ISpatialDataProvider* pSDP)
{
    //cleanup sdp passed out from the dll.
    //default handler
    delete pSDP;
}

extern "C" const char* GetSDPLabel()
{
    //label identifier for SDP module
}