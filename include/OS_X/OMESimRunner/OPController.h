//
//  OPController.h
//  OMESimRunner
//
//  Created by Patrick  Wingo on 11/3/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SRController.h"

@interface OPController : NSObject
{
    SRController* m_pParentController;
}
@property(weak) IBOutlet NSWindow*      prefSheet;
@property(weak) IBOutlet NSTextField*   startField;
@property(weak) IBOutlet NSTextField*   stopField;
@property(weak) IBOutlet NSTextField*   stepField;
@property(weak) IBOutlet NSTextField*   intervalField;
@property(weak) IBOutlet NSButton*      minRecordCB;
@property(weak) IBOutlet NSPopUpButton* methodPop;
@property(weak) IBOutlet NSPopUpButton* solverPop;


-(void)prepParamDetails:(SRController*)runController;

-(IBAction)apply:(id)sender;
-(IBAction)cancel:(id)sender;
@end
