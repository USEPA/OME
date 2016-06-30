//
//  TreeViewController.m
//  OMEWizard
//
//  Created by Patrick  Wingo on 11/10/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import "TreeViewController.h"
#import "TreeResultsDetails.h"
#import "SRController.h"

@implementation TreeViewController

@synthesize rootNode=_pRootNode;
@synthesize sortDescriptors=_sortDescriptors;
@synthesize selectedNode=_selectedNode;

-(IBAction)DoFieldSelector:(id)sender
{
    [_selectorController prepareWithReps:_flatList andViewDetails:_pTreeDetails];
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
    
    if(_parentController.detailsMgr->NameIsUnique(newName, _pTreeDetails))
    {
        _pTreeDetails->SetViewName([_nameField.stringValue cStringUsingEncoding:NSASCIIStringEncoding]);
        [_parentController refreshViewTable];
    }
    else
    {
        STLString oldName=_pTreeDetails->GetViewName();
        
        NSAlert* alert=[[NSAlert alloc] init];
        alert.messageText=@"Name in Use";
        alert.informativeText=[NSString stringWithFormat:@"The name \"%@\" is already in use by another view.",_nameField.stringValue];
        [alert addButtonWithTitle:@"OK"];
        [alert runModal];
        _nameField.stringValue=[NSString stringWithCString:oldName.c_str() encoding:NSASCIIStringEncoding];
        
    }
    
}

-(IBAction)DoFilterChange:(id)sender
{
    _activeOnly=_filterCB.state==NSOnState;
    _pTreeDetails->SetUseSelected(_activeOnly);
    
//    [self willChangeValueForKey:@"rootNode"];
    if(_activeOnly)
        _tvDataController.childrenKeyPath=@"activeSubNodes";
    else
        _tvDataController.childrenKeyPath=@"subNodes";
    
    [_treeOutline expandItem:nil expandChildren:YES];

//    [self didChangeValueForKey:@"rootNode"];
}

-(IBAction)DoSortChange:(id)sender
{
    _pTreeDetails->SetSortMode((RN_SORTMODE)_sortPopup.selectedTag);
    
    [self willChangeValueForKey:@"sortDescriptors"];
    [_sortDescriptors removeAllObjects];
   
    switch (_sortPopup.selectedTag)
    {
        case CFIRST:
            [_sortDescriptors addObject:_cTypeSD];
            break;
        case MFIRST:
            [_sortDescriptors addObject:_mTypeSD];
            break;
        case MIXED:
            
            break;
    }
    
    [_sortDescriptors addObject:(_pTreeDetails->GetSortAsc() ? _nameAscSD : _nameDesSD)];
    
    [self didChangeValueForKey:@"sortDescriptors"];

}

-(void)DoInstancePopup:(TreeRepNode*)repNode withCell:(NSTableCellView*)cell
{
    switch(repNode.objectType)
    {
        case OME_MODEL:
            _instStepper.maxValue=repNode.valueCount;
            _instStepper.minValue=1;
            _instStepper.valueWraps=NO;
            _instStepper.integerValue=repNode.instance;
            _instNumFormatter.minimum=@1;
            _instNumFormatter.maximum=[NSNumber numberWithInteger:repNode.valueCount];
            _instTextField.integerValue=repNode.instance;
            
            [self willChangeValueForKey:@"selectedNode"];
            _selectedNode=repNode;
            [self didChangeValueForKey:@"selectedNode"];

            [_modelInstancePopOver showRelativeToRect:cell.bounds ofView:cell preferredEdge:NSMinYEdge];
            break;
        case OME_VARARRAY:
            _arrayBrowserInfo.stringValue=[NSString stringWithFormat:@"Name: %@\nDimensions: %@",repNode.name,repNode.dimensions];
            [self willChangeValueForKey:@"selectedNode"];
            _selectedNode=repNode;
            [self didChangeValueForKey:@"selectedNode"];
            _arrayBrowserPanel.title=repNode.name;
            [NSApp runModalForWindow:_arrayBrowserPanel];
            
            break;
        default:
            break;
    }
}

-(IBAction)DoInstanceChange:(id)sender
{
    //fake root node refresh
    [self willChangeValueForKey:@"rootNode"];
    switch(_selectedNode.objectType)
    {
        case OME_MODEL:
            _selectedNode.instance=((NSControl*)sender).integerValue;
            [_modelInstancePopOver close];
            break;
        default:
            break;
    }
    
    _selectedNode=nil;
    [self didChangeValueForKey:@"rootNode"];
    //[self reloadData];
}

-(NSMutableArray*)sortDescriptors
{
    return _sortDescriptors;
}

-(void)setSortDescriptors:(NSMutableArray *)sortDescriptors
{
    _pTreeDetails->SetSortAsc(!((NSSortDescriptor*)_sortDescriptors.lastObject).ascending);
     
    [self DoSortChange:self];
}

-(NSView*)prefPane
{
    return _pPane;
    
}

-(NSView*)dataPane
{
    return _dPane;
}

-(void)init:(BaseResultsDetails*)rDetails
{
    _pTreeDetails=(TreeResultsDetails*)rDetails;
    
    _nameField.stringValue=[NSString stringWithCString:_pTreeDetails->GetViewName().c_str() encoding:NSASCIIStringEncoding];
    _filterCB.state=_pTreeDetails->GetUseSelected() ? NSOnState : NSOffState;
    [_sortPopup selectItemAtIndex:_pTreeDetails->GetSortMode()];
    
    _activeOnly=_pTreeDetails->GetUseSelected();
    [_filterCB setState:_activeOnly ? NSOnState : NSOffState ];
    
    _mTypeSD=[NSSortDescriptor sortDescriptorWithKey:@"self" ascending:YES comparator:^NSComparisonResult(id obj1, id obj2) {
        OMETYPE lhs=((TreeRepNode*)obj1).objectType;
        OMETYPE rhs=((TreeRepNode*)obj2).objectType;
        
        if(lhs==OME_MODEL && rhs!=OME_MODEL)
            return NSOrderedAscending;
        if(lhs!=OME_MODEL && rhs==OME_MODEL)
            return NSOrderedDescending;
        return NSOrderedSame;
    }];
    
    _cTypeSD=_mTypeSD.reversedSortDescriptor;

    _nameAscSD=[NSSortDescriptor sortDescriptorWithKey:@"name" ascending:YES selector:@selector(caseInsensitiveCompare:)];
    _nameDesSD=[NSSortDescriptor sortDescriptorWithKey:@"name" ascending:NO selector:@selector(caseInsensitiveCompare:)];
    _sortDescriptors=[NSMutableArray arrayWithCapacity:2];
    
    [self DoSortChange:self];
 }

-(void)reloadData
{
    _pRootMdl=_parentController.simMgr->GetRootModel();
    
    //[self setValue:[TreeViewController populateTree:_pRootMdl] forKey:@"rootNode"];
    [self willChangeValueForKey:@"rootNode"];
    _pRootNode=[TreeViewController populateTree:_pRootMdl];
    _flatList=[NSMutableArray arrayWithCapacity:100];
    [TreeViewController flattenObjectTreeNode:_pRootNode array:_flatList];
    [self refreshActive];
    [self didChangeValueForKey:@"rootNode"];
   
    [self DoSortChange:_sortPopup];
    [self DoFilterChange:_filterCB];
    
    [_treeOutline expandItem:nil expandChildren:YES];
    //_tvDataController.content=_pRootNode;
    //[_tvDataController setContent:_pRootNode];
}

+(TreeRepNode*)populateTree:(Model*)pRoot
{
    return [TreeViewController populateTree:pRoot atLevel:0];
}

+(TreeRepNode*)populateTree:(Model*)pRoot atLevel:(int)lvl
{
    TreeRepNode* parent=[TreeRepNode treeRepNodeWithObject:pRoot andLevel:lvl];
    parent.isParent=YES;
    
    EvalArray subs;
    pRoot->GetAllEvals(subs,DT_SHALLOW|DT_ALIAS);
    lvl++;
    size_t i;
    for(i=0; i<subs.size();++i)
    {
        TreeRepNode* leaf=[TreeRepNode treeRepNodeWithObject:subs[i] andLevel:lvl];
        [parent addNode:leaf];
    }
    
    for(i=0; i< pRoot->GetSubmodelCount(); ++i)
        [parent addNode:[TreeViewController populateTree:pRoot->GetSubmodel(i) atLevel:lvl]];
    
    //sort models/leafs here
    
    return parent;
}

+(void)flattenObjectTreeNode:(TreeRepNode*)node array:(NSMutableArray*)arr
{
    for(TreeRepNode* subNode in node.subNodes)
    {
        if(subNode.objectType!=OME_MODEL)
            [arr addObject:subNode];
        else
            [self flattenObjectTreeNode:subNode array:arr];
    }
}

-(void)refreshActive
{
    EvalRepArray reps=_pTreeDetails->GetPartialReps();
    
    STLString currID;
    for(TreeRepNode* currNode in _flatList)
    {
        if(currNode.objectType!=OME_MODEL)
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
}

#pragma mark - arraybrowser delegate
- (void)windowWillClose:(NSNotification *)notification
{
    [NSApp stopModal];
    
    //set selectedNode to nil here so that the observers
    //won't complain when arrayValues are dealloc'ed.
    [self willChangeValueForKey:@"selectedNode"];
    [_selectedNode clearTempArrayValCache];
    _selectedNode=nil;
    [self didChangeValueForKey:@"selectedNode"];
    
    [self DoInstanceChange:self];
}

-(TreeRepNode*)selectedNode;
{
    return _selectedNode;
}

@end
