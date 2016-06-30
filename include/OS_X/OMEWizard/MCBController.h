//
//  MCBController.h
//  OMEWizard
//
//  Created by Patrick  Wingo on 10/2/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import "OMEController.h"
#import "RunConsoleController.h"

@interface MCBController : NSObject <OMEController>
{
    enum {BROWSE_MDL_TAG=1,BROWSE_DIR_TAG,TAG_COUNT};
    IBOutlet NSTextField* srcTF;
    IBOutlet NSTextField* destTF;
    IBOutlet NSTextField* mNameTF;
    
    IBOutlet NSButton* runQuietCB;
    IBOutlet NSButton* convButton;
    
    IBOutlet RunConsoleController* rcController;
}
-(IBAction)BrowseModel:(id)sender;
-(IBAction)BrowseDir:(id)sender;
-(IBAction)DoConvert:(id)sender;

-(void)updateControlsForControl:(NSControl*)control Result:(BOOL)result;
-(BOOL)controlPathIsDirectory:(NSControl*)control;
@end
