/*
 *  CSVSpatialDataProvider.h
 *  CSVSpatialDataProvider
 *
 *  Created by Patrick  Wingo on 12/22/14.
 *  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
 *
 */

#pragma once

#include "ISpatialDataProvider.h"
#include "SimManager.h"

extern "C" ISpatialDataProvider* GenSDP(SimManager* pSim,Model* pRoot = NULL, TI2Element* pSDPElem = NULL, const char* modelPath = "");
extern "C" void DisposeSDP(ISpatialDataProvider* pSDP);
extern "C" const char* GetSDPLabel();