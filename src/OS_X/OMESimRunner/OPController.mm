
//
//  OPController.m
//  OMESimRunner
//
//  Created by Patrick  Wingo on 11/3/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import "OPController.h"

@implementation OPController

-(void)prepParamDetails:(SRController*)runController
{
    m_pParentController=runController;
    _startField.doubleValue=m_pParentController.startTime;
    _stopField.doubleValue=m_pParentController.stopTime;
    _stepField.doubleValue=m_pParentController.stepSize;
    //intervalField.doubleValue=m_pParentController.interval;
    _minRecordCB.state=m_pParentController.useMinRecord ? NSOnState : NSOffState;
    
    if(m_pParentController.intMethod==OMEIntegrationBlock::IM_EULER || m_pParentController.intMethod==OMEIntegrationBlock::IM_RK4)
        [_methodPop selectItemWithTag:m_pParentController.intMethod];
    else
        [_methodPop selectItemWithTag:OMEIntegrationBlock::IM_EULER];
    
    [_solverPop selectItemWithTag:m_pParentController.solver];
}

-(IBAction)apply:(id)sender
{
    m_pParentController.startTime=_startField.doubleValue;
    m_pParentController.stopTime=_stopField.doubleValue;
    m_pParentController.stepSize=_stepField.doubleValue;
    //interval
    m_pParentController.useMinRecord=_minRecordCB.state==NSOnState ? YES : NO;
    
    m_pParentController.intMethod=(OMEIntegrationBlock::INT_METHOD)_methodPop.selectedTag;
    m_pParentController.solver=(SimManager::SOLVER_TYPE)_solverPop.selectedTag;
    
    [[_prefSheet sheetParent] endSheet:_prefSheet];
}

-(IBAction)cancel:(id)sender
{
    [[_prefSheet sheetParent] endSheet:_prefSheet];
}
@end
