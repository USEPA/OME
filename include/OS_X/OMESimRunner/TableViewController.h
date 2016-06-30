//
//  TableViewController.h
//  OMESimRunner
//
//  Created by Patrick  Wingo on 2/9/16.
//  Copyright © 2016 OregonStateUniversity. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "BaseDVController.h"
#import "TableResultsDetails.h"
#import "FSController.h"
#import "SRController.h"

@class TblNode;

@interface TableViewController : NSObject<BaseDVController>
{
    Model* _pRootMdl;
    TableResultsDetails* _pDetails;
    NSMutableArray* _nodes;
}

@property (weak) IBOutlet NSView* pPane;
@property (weak) IBOutlet NSView* dPane;
@property (weak) IBOutlet FSController* selectorController;
@property (weak) IBOutlet NSWindow* mainWindow;
@property (weak) IBOutlet SRController* parentController;

#pragma mark - PrefPane components
@property (weak) IBOutlet NSTextField* nameField;

#pragma mark - DataPane components
@property (weak) IBOutlet NSTableView* dataTable;
@property (weak) IBOutlet NSPopover* instView;
@property (weak) IBOutlet NSArrayController* dataController;
@property (readonly) NSMutableArray* nodes;

@property (readonly) NSPredicate* showPredicate;

-(IBAction)DoNameChange:(id)sender;
-(IBAction)DoFieldSelector:(id)sender;

-(void)init:(BaseResultsDetails*)rDetails;
-(NSView*)prefPane;
-(NSView*)dataPane;

-(NSPredicate*)showPredicate;
-(void)reloadData;
-(void)refreshActive;

-(void)DoInstancePopup:(TblNode*)repNode withCell:(NSTableCellView*)cell;
@end
