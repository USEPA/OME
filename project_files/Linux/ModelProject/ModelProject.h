//
//  ModelProject.h
//  ModelProject
//
//  Created by Patrick  Wingo on 10/19/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#pragma once

#include <stdio.h>
#include "TestModel.h"
#include "CompiledInterface.h"

extern "C" CompiledInterface*  GenModel();
extern "C" void DisposeModel(CompiledInterface* pMdlInt );


