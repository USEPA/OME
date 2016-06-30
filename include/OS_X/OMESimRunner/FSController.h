//
//  FSController.h
//  OMESimRunner
//
//  Created by Patrick  Wingo on 12/16/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import "RepNode.h"

#import "BaseResultsDetails.h"


@interface FSController : NSObject
{
    NSMutableArray* _selectedArray;
    NSMutableArray* _availableArray;
    
    NSPredicate* _availablePred;
    NSPredicate* _selectedPred;
    
    BaseResultsDetails* _details;
}
@property (weak) IBOutlet NSWindow *selectorWindow;
@property (weak) IBOutlet NSTableView *availableTable;
@property (weak) IBOutlet NSTableView *selectedTable;
@property (weak) IBOutlet NSArrayController* availableController;
@property (weak) IBOutlet NSArrayController* selectedController;
@property (weak) IBOutlet NSTextField* availableFilter;
@property (weak) IBOutlet NSTextField* selectedFilter;
@property (weak) IBOutlet NSPopover* instPopover;

@property (readwrite,copy) NSMutableArray* selectedArray;
@property (readwrite,copy) NSMutableArray* availableArray;
@property (readwrite,assign) BaseResultsDetails* viewDetails;

@property(readonly) NSPredicate* availablePred;
@property(readonly) NSPredicate* selectedPred;


-(void)prepareWithReps:(NSMutableArray*)allNodes andViewDetails:(BaseResultsDetails*)details;

-(IBAction)onOK:(id)sender;
-(IBAction)onCancel:(id)sender;

-(IBAction)selectAll:(id)sender;
-(IBAction)selectNone:(id)sender;

-(IBAction)moveToAvailable:(id)sender;
-(IBAction)moveToSelected:(id)sender;

-(void)doubleClickContents:(NSTableView*)tv;

-(void)controlTextDidChange:(NSNotification *)note;
-(void)hideSelectedColumnWithID:(NSString*)ident;
-(void)showSelectedColumnWithID:(NSString*)ident;

-(void)DoInstancePopup:(RepNode*)repNode withCell:(NSTableCellView*)cell;
@end
