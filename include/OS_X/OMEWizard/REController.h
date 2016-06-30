//
//  REController.h
//  OMEWizard
//
//  Created by Patrick  Wingo on 10/2/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import "OMEController.h"
#import "RunConsoleController.h"

@interface REController : NSObject <OMEController>
{
    enum {BROWSE_RUN_TAG=0,BROWSE_RES_TAG, BROWSE_LOG_TAG,BROWSE_EXP_TAG, TAG_COUNT};
    
    IBOutlet NSTextField* srcTF;
    IBOutlet NSButton* srcButton;
    IBOutlet NSButton* resultCB;
    IBOutlet NSTextField* resultTF;
    IBOutlet NSButton* resultButton;
    IBOutlet NSButton* logCB;
    IBOutlet NSTextField* logTF;
    IBOutlet NSButton* logButton;
    
    IBOutlet NSButton* expNaRB;
    IBOutlet NSButton* expSingleRB;
    IBOutlet NSButton* expAllRB;
    
    IBOutlet NSTextField* exportTF;
    IBOutlet NSButton* expButton;
    
    IBOutlet NSButton* runQuietCB;
    IBOutlet NSButton* runButton;
    IBOutlet NSPopUpButton* integPopup;
    
    IBOutlet RunConsoleController* rcController;
    
    BOOL valids[TAG_COUNT];
}
@property BOOL saveResults;

-(IBAction)enableTextField:(id)sender;
-(IBAction)enableExportField:(id)sender;
-(IBAction)DoFileSelect:(id)sender;
-(IBAction)RunModel:(id)sender;


-(BOOL)controlPathIsDirectory:(NSControl*)control;

-(void)updateControlsForControl:(NSControl*)control Result:(BOOL)result;

@end
