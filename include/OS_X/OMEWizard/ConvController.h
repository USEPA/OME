//
//  ConvController.h
//  OMEWizard
//
//  Created by Patrick  Wingo on 10/2/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import "OMEController.h"
#import "RunConsoleController.h"

@interface ConvController : NSObject <OMEController>
{
    enum {BROWSE_SRC_TAG=0,BROWSE_TRG_TAG,BROWSE_PARAM_TAG, TAG_COUNT};

    IBOutlet NSWindow* mainWindow;
    IBOutlet NSTextField* sourceTextField;
    IBOutlet NSTextField* destTextField;
    IBOutlet NSButton* convButton;
    
    
    IBOutlet NSTableView* paramTable;
    IBOutlet NSButton* addPButton;
    IBOutlet NSButton* remPButton;
    
    IBOutlet NSArrayController* mdlListController;
    IBOutlet NSArrayController* paramController;
    
    IBOutlet NSComboBoxCell* tmpCell;
    IBOutlet NSButton* runQuietCB;
    
    //add param dialog controls
    IBOutlet NSWindow* addParamWindow;
    IBOutlet NSTextField* paramPathTextField;
    IBOutlet NSTextField* modelNameTextField;
    IBOutlet NSButton* paramOKButton;
    
    IBOutlet RunConsoleController* rcController;
    
    BOOL valids[TAG_COUNT];
    NSMutableArray* paramMappings;

    NSString* rootMdlName;
    
}
@property (copy) NSMutableArray* paramMappings;
@property (copy) NSMutableArray* modelList;

-(BOOL)controlPathIsDirectory:(NSControl*)control;
-(void)updateControlsForControl:(NSControl*)control Result:(BOOL)result;

-(IBAction)BrowseForFile:(id)sender;
-(IBAction)DoConvert:(id)sender;
-(IBAction)AddParam:(id)sender;
-(IBAction)EndParamSheet:(id)sender;

@end
