//
//  MultiValViewController.m
//  OMESimRunner
//
//  Created by Patrick  Wingo on 2/16/15.
//  Copyright (c) 2015 OregonStateUniversity. All rights reserved.
//

#import "MultiValViewController.h"
#import "RepNode.h"
#import "SRController.h"

@implementation MultiValViewController

@synthesize srcPred=_srcPred;
@synthesize repList=_repList;
@synthesize varListSortDescriptors=_varListsortDescriptors;

-(void)init:(BaseResultsDetails*)rDetails
{
    _pCurrDetails=(MultiValResultsDetails*)rDetails;
    
    _nameField.stringValue=[NSString stringWithCString:_pCurrDetails->GetViewName().c_str() encoding:NSASCIIStringEncoding];
    _filterCB.state=_pCurrDetails->GetUseSelected() ? NSOnState : NSOffState;
    _repList=[NSMutableArray arrayWithCapacity:100];
    
    _pRootMdl=_parentController.simMgr->GetRootModel();
    
    [self willChangeValueForKey:@"varListSortDescriptors"];
    _varListsortDescriptors=[NSMutableArray arrayWithObject:[NSSortDescriptor sortDescriptorWithKey:@"name" ascending:YES selector:@selector(caseInsensitiveCompare:)]];
    [self didChangeValueForKey:@"varListSortDescriptors"];
}

-(NSView*)prefPane
{
    return _basePrefView;
}

-(NSView*)dataPane
{
    return _baseDataView;
}

-(void)reloadData
{
    
    //[self setValue:[TreeViewController populateTree:_pRootMdl] forKey:@"rootNode"];
    [self willChangeValueForKey:@"repList"];
    [_repList removeAllObjects];
    EvalArray evals;
    _pRootMdl->GetAllEvals(evals,DT_FULL);
    
    for(auto itr=evals.begin(); itr!=evals.end(); ++itr)
        [_repList addObject:[RepNode repNodeWithObject:(*itr)]];
    
    [self refreshActive];
    [self didChangeValueForKey:@"repList"];
    [self updatePredicate:self];
    
    
}


-(IBAction)DoFieldSelector:(id)sender
{
    [_selectorController prepareWithReps:_repList andViewDetails:_pCurrDetails];
    [_selectorController showSelectedColumnWithID:@"selID"];
    [_selectorController hideSelectedColumnWithID:@"selInst"];
    
    [_mainWindow beginSheet:_selectorController.selectorWindow completionHandler:^(NSModalResponse returnCode)
     {
         [self refreshActive];
     }];
    
}

-(IBAction)DoNameChange:(id)sender
{
    STLString newName=[_nameField.stringValue cStringUsingEncoding:NSASCIIStringEncoding];
    
    if(_parentController.detailsMgr->NameIsUnique(newName, _pCurrDetails))
    {
        _pCurrDetails->SetViewName([_nameField.stringValue cStringUsingEncoding:NSASCIIStringEncoding]);
        [_parentController refreshViewTable];
    }
    else
    {
        STLString oldName=_pCurrDetails->GetViewName();
        
        NSAlert* alert=[[NSAlert alloc] init];
        alert.messageText=@"Name in Use";
        alert.informativeText=[NSString stringWithFormat:@"The name \"%@\" is already in use by another view.",_nameField.stringValue];
        [alert addButtonWithTitle:@"OK"];
        [alert runModal];
        _nameField.stringValue=[NSString stringWithCString:oldName.c_str() encoding:NSASCIIStringEncoding];
        
    }
        
}

-(IBAction)updatePredicate:(id)sender
{
    NSString* filterText=_filterTF.stringValue;
    NSInteger cbState=_filterCB.state;
    
    enum { NO_FILT=0,TEXT_FILT,ACT_FILT,ALL_FILT };
    
    unsigned short action=NO_FILT;
    if(filterText.length)
        action+=TEXT_FILT;
    if(cbState==NSOnState)
        action+=ACT_FILT;
    
    [self willChangeValueForKey:@"srcPred"];
    switch (action)
    {
        case NO_FILT:
            _srcPred=nil;
            break;
        case TEXT_FILT:
            _srcPred=[NSPredicate predicateWithFormat:@"name CONTAINS[cd] %@",filterText];
            break;
        case ACT_FILT:
            _srcPred=[NSPredicate predicateWithFormat:@"isActive == YES"];
            break;
        case ALL_FILT:
            _srcPred=[NSPredicate predicateWithFormat:@"name CONTAINS[cd] %@ AND isActive == YES",filterText];
            break;
        default:
            break;
    }
    [self didChangeValueForKey:@"srcPred"];
}

-(void)refreshActive
{
    EvalRepArray reps=_pCurrDetails->GetPartialReps();
    
    STLString currID;
    for(RepNode* currNode in _repList)
    {
        currNode.isActive=NO;
        currID=currNode.object->GetID();
        for(auto itr=reps.begin(); itr!=reps.end() && !currNode.isActive; ++itr)
        {
            if (currID == (*itr).id)
            {
                currNode.isActive=YES;
                currNode.color = [NSColor colorWithCalibratedRed:(*itr).color.r green:(*itr).color.g blue:(*itr).color.b alpha:1.0];
            }
        }
    }

}

#pragma mark - delegations for implementing filters
-(void)controlTextDidChange:(NSNotification *)note
{
    [self updatePredicate:note.object];

}

@end
