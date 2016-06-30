//
//  FSController.m
//  OMESimRunner
//
//  Created by Patrick  Wingo on 12/16/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import "FSController.h"
#import "OMEDefines.h"
#import "RepNode.h"

@implementation FSController

@synthesize availableArray=_availableArray;
@synthesize selectedArray=_selectedArray;
@synthesize availablePred=_availablePred;
@synthesize selectedPred=_selectedPred;

-(id)init
{
    _selectedArray=[NSMutableArray arrayWithCapacity:0];
    _availableArray=[NSMutableArray arrayWithCapacity:0];
    
    _details=NULL;
    
    return self;
}

-(void)awakeFromNib
{
//    NSString* dragType=@"OMERepNode.";
//    [_availableTable setDraggingSourceOperationMask:NSDragOperationLink forLocal:NO];
//    [_availableTable setDraggingSourceOperationMask:NSDragOperationMove forLocal:YES];
//    [_availableTable registerForDraggedTypes:[NSArray arrayWithObject:dragType]];
//    
//    [_selectedTable setDraggingSourceOperationMask:NSDragOperationLink forLocal:NO];
//    [_selectedTable setDraggingSourceOperationMask:NSDragOperationMove forLocal:YES];
//    [_selectedTable registerForDraggedTypes:[NSArray arrayWithObject:dragType]];
//
}

-(void)prepareWithReps:(NSMutableArray*)allNodes andViewDetails:(BaseResultsDetails*)details
{
    EvalRepArray reps=details->GetPartialReps();
    _details=details;
    
    //finish sorting between arrays
    [_selectedController removeObjects:_selectedController.arrangedObjects];
    [_availableController removeObjects:_availableController.arrangedObjects];
    
    for(RepNode* currNode in allNodes)
    {
        STLString currID=currNode.object->GetID();
        BOOL found=NO;
        
        //determine if the current node is selected in the details
        for(auto itr=reps.begin(); itr!=reps.end() && !found;++itr)
        {
            if(currID==(*itr).id)
                found=YES;
        }
        
        if(found)
            [_selectedController addObject:currNode];
        else
            [_availableController addObject:currNode];
    }
}

-(IBAction)onOK:(id)sender
{
    //do save stuff here
    EvalRepArray toSave;
    for(RepNode* currNode in _selectedArray)
    {
        EvalRep rep;
        [currNode populatePartialRep:&rep];
        toSave.push_back(rep);
    }
    
   _details->SetPartialReps(toSave);
    [[_selectorWindow sheetParent] endSheet:_selectorWindow];
}

-(IBAction)onCancel:(id)sender
{
    [[_selectorWindow sheetParent] endSheet:_selectorWindow];
}

-(IBAction)selectAll:(id)sender
{
    [_selectedController addObjects:_availableController.arrangedObjects];
    [_availableController removeObjects:_availableController.arrangedObjects];
}

-(IBAction)selectNone:(id)sender
{
    [_availableController addObjects:_selectedController.arrangedObjects];
    [_selectedController removeObjects:_selectedController.arrangedObjects];
}

-(IBAction)moveToAvailable:(id)sender
{
    [_availableController addObjects:_selectedController.selectedObjects];
    [_selectedController removeObjects:_selectedController.selectedObjects];
}

-(IBAction)moveToSelected:(id)sender
{
    [_selectedController addObjects:_availableController.selectedObjects];
    [_availableController removeObjects:_availableController.selectedObjects];
}

-(void)doubleClickContents:(NSTableView*)tv
{
    if(tv==_selectedTable)
        [self moveToAvailable:tv];
    else
        [self moveToSelected:tv];
}

-(void)DoInstancePopup:(RepNode*)repNode withCell:(NSTableCellView*)cell
{
    [self willChangeValueForKey:@"value"];
    [self willChangeValueForKey:@"instString"];
    
    [_instPopover showRelativeToRect:cell.bounds ofView:cell preferredEdge:NSMinYEdge];
    
    [self didChangeValueForKey:@"value"];
    [self didChangeValueForKey:@"instString"];

}

#pragma mark - delegations for implementing filters
-(void)controlTextDidChange:(NSNotification *)note
{
    NSTextField* src=(NSTextField*)note.object;
    
    NSPredicate* pred=nil;
    NSString* pattern=src.stringValue;
    if(pattern.length)
    {
        pred=[NSPredicate predicateWithFormat:@"name CONTAINS[cd] %@ OR parentModelName CONTAINS[cd] %@",pattern,pattern ];
    }
    if (note.object==_availableFilter)
    {
        [self willChangeValueForKey:@"availablePred"];
        _availablePred=pred;
        [self didChangeValueForKey:@"availablePred"] ;
    }
    else
    {
        [self willChangeValueForKey:@"selectedPred"];
        _selectedPred=pred;
        [self didChangeValueForKey:@"selectedPred"];
    }
}

-(void)hideSelectedColumnWithID:(NSString*)ident
{
    NSTableColumn* trg=[_selectedTable tableColumnWithIdentifier:ident];
    if(trg)
    {
        trg.hidden=YES;
    }
}

-(void)showSelectedColumnWithID:(NSString*)ident
{
    NSTableColumn* trg=[_selectedTable tableColumnWithIdentifier:ident];
    if(trg)
    {
        trg.hidden=NO;
    }
}


@end
