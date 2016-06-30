//
//  TreeViewController.h
//  OMEWizard
//
//  Created by Patrick  Wingo on 11/10/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "BaseDVController.h"
#import "FSController.h"
#import "TreeResultsDetails.h"
#import "TreeRepNode.h"
#import "SRController.h"

@class SRController;
@interface TreeViewController  : NSObject <BaseDVController,NSWindowDelegate>
{
    TreeResultsDetails* _pTreeDetails;
    TreeRepNode* _pRootNode;
    TreeRepNode* _selectedNode;
    Model* _pRootMdl;
    
    NSSortDescriptor* _nameAscSD;
    NSSortDescriptor* _nameDesSD;
    NSSortDescriptor* _cTypeSD;
    NSSortDescriptor* _mTypeSD;
    
    NSMutableArray* _sortDescriptors;
    NSMutableArray* _flatList;
    
    BOOL _activeOnly;
}
@property (weak) IBOutlet SRController* parentController;
@property (weak) IBOutlet NSWindow *mainWindow;
@property (weak) IBOutlet NSView* pPane;
@property (weak) IBOutlet NSView* dPane;
@property (weak) IBOutlet FSController* selectorController;

#pragma mark - PrefPane components
@property (weak) IBOutlet NSTextField* nameField;
@property (weak) IBOutlet NSButton* filterCB;
@property (weak) IBOutlet NSPopUpButton* sortPopup;

#pragma mark - DataPane components
@property (weak) IBOutlet NSTreeController* tvDataController;
@property (weak) IBOutlet NSOutlineView* treeOutline;
@property (weak) IBOutlet NSPanel* arrayBrowserPanel;
@property (weak) IBOutlet NSTextField* arrayBrowserInfo;
@property (weak) IBOutlet NSTableView* arrayBrowserTable;
@property (weak) IBOutlet NSPopover* modelInstancePopOver;
@property (weak) IBOutlet NSTextField* instTextField;
@property (weak) IBOutlet NSStepper* instStepper;
@property (weak) IBOutlet NSNumberFormatter* instNumFormatter;

@property (readonly) TreeRepNode* rootNode;
@property (readonly,getter=sortDescriptors,setter=setSortDescriptors:) NSMutableArray* sortDescriptors;
@property (readonly, getter=selectedNode) TreeRepNode* selectedNode;

-(TreeRepNode*)selectedNode;

-(IBAction)DoFieldSelector:(id)sender;
-(IBAction)DoNameChange:(id)sender;
-(IBAction)DoFilterChange:(id)sender;
-(IBAction)DoSortChange:(id)sender;
-(IBAction)DoInstanceChange:(id)sender;
//-(IBAction)ABUpdateSelection:(id)sender;

-(void)init:(BaseResultsDetails*)rDetails;
-(NSView*)prefPane;
-(NSView*)dataPane;

-(void)reloadData;

-(void)DoInstancePopup:(TreeRepNode*)repNode withCell:(NSTableCellView*)cell;

-(NSMutableArray*)sortDescriptors;
-(void)setSortDescriptors:(NSMutableArray *)sortDescriptors;

#pragma mark - arraybrowser delegate
- (void)windowWillClose:(NSNotification *)notification;

@end
