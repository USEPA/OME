//
//  ModelProject.cpp
//  ModelProject
//
//  Created by Patrick  Wingo on 10/19/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#include "ModelProject.h"

// ModelProject.cpp : Defines the initialization routines for the DLL.
//

#include "ModelProject.h"
#include "TestModel.h"

extern "C" CompiledInterface* GenModel()
{
    return new OME_MDL_CLASS();
}

extern "C" void DisposeModel(CompiledInterface* pMdlInt )
{
    delete pMdlInt;
}

#undef OME_MDL_CLASS
#undef OME_MDL_NAME