//
//  MultiValViewController.h
//  OMESimRunner
//
//  Created by Patrick  Wingo on 2/16/15.
//  Copyright (c) 2015 OregonStateUniversity. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "BaseDVController.h"
#import "MultiValResultsDetails.h"
#import "FSController.h"

@class SRController;
@interface MultiValViewController : NSObject <BaseDVController>
{
    MultiValResultsDetails* _pCurrDetails;
    Model* _pRootMdl;
    
    NSMutableArray* _repList;
    NSPredicate* _srcPred;
    NSMutableArray* _varListsortDescriptors;
}

@property (weak) IBOutlet SRController *parentController;
@property (weak) IBOutlet NSView *baseDataView;
@property (weak) IBOutlet NSView *basePrefView;
@property (weak) IBOutlet FSController* selectorController;
@property (weak) IBOutlet NSWindow* mainWindow;

#pragma mark - pref pane components
@property (weak) IBOutlet NSTextField* nameField;
@property (weak) IBOutlet NSButton* filterCB;

#pragma mark - data pane components
@property (weak) IBOutlet NSTextField *descriptionLabel;

@property (weak) IBOutlet NSTextField *filterTF;
@property (weak) IBOutlet NSTableView *variableTable;
@property (weak) IBOutlet NSTableView *valueTable;

@property (readonly) NSPredicate* srcPred;
@property (readonly) NSMutableArray* repList;
@property (readwrite) NSMutableArray* varListSortDescriptors;

-(void)init:(BaseResultsDetails*)rDetails;
-(NSView*)prefPane;
-(NSView*)dataPane;

-(void)reloadData;

-(IBAction)DoFieldSelector:(id)sender;
-(IBAction)DoNameChange:(id)sender;

-(IBAction)updatePredicate:(id)sender;

-(void)refreshActive;

#pragma mark - delegations for implementing filters
-(void)controlTextDidChange:(NSNotification *)note;

@end
