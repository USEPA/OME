//
//  TableViewController.m
//  OMESimRunner
//
//  Created by Patrick  Wingo on 2/9/16.
//  Copyright © 2016 OregonStateUniversity. All rights reserved.
//

#import "TableViewController.h"
#import "TblNode.h"
#import "ISNode.h"

@implementation TableViewController

@synthesize nodes=_nodes;

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
    _nodes=[NSMutableArray arrayWithCapacity:100];
    _pDetails=(TableResultsDetails*)rDetails;
    _nameField.stringValue=[NSString stringWithCString:_pDetails->GetViewName().c_str() encoding:NSASCIIStringEncoding];
    [self reloadData];
}

-(NSPredicate*)showPredicate
{
    return [NSPredicate predicateWithFormat:@"isActive == YES"];
}

-(void)reloadData
{
    _pRootMdl=_parentController.simMgr->GetRootModel();
    
    [self willChangeValueForKey:@"nodes"];
    
    [_nodes removeAllObjects];
    EvalArray evals;
    _pRootMdl->GetAllEvals(evals,DT_FULL);
    
    for(auto itr=evals.begin(); itr!=evals.end(); ++itr)
    {
        [_nodes addObject:[TblNode TblNodeWithObject:(*itr)]];
       // [[_nodes lastObject] updateIndex];
    }
    
    
    [self refreshActive];
    [self didChangeValueForKey:@"nodes"];
}

-(void)refreshActive
{
    //calculate lookup index
    EvalRepArray partReps=_pDetails->GetPartialReps();
    STLString currID;
    for(TblNode* node in _nodes)
    {
        node.isActive=NO;
        currID=node.object->GetID();
        for(EvalRep & rep :partReps)
        {
            if (currID == rep.id)
            {
                [node.indexes removeAllObjects];
                node.isActive=YES;
                //set color
                node.color = [NSColor colorWithCalibratedRed:rep.color.r green:rep.color.g blue:rep.color.b alpha:1.0];
                
                //build coordinate list
                Model* pMdl=node.object->GetParentModel();
                node.valArchive=pMdl->GetValueArchive();
                unsigned int ind=0;
                switch(node.object->GetOMEType())
                {
                    case OME_VARARRAY:
                        for(unsigned short i=0; i<((VarArray*)node.object)->GetDimCount();++i)
                            [node.indexes addObject:[ISNode ISNodeWithLevel:[NSString stringWithFormat:@"%s_%u",((Evaluable*)node.object)->GetExprName().c_str(),i+1 ] andIndex:rep.selected[ind++]+1]];
                        //drop through
                    default:
                        while(pMdl && pMdl->GetParentModel()!=nil)
                        {
                            [node.indexes addObject:[ISNode ISNodeWithLevel:[NSString stringWithCString:pMdl->GetName().c_str() encoding:NSASCIIStringEncoding] andIndex:rep.selected[ind++]+1]];
                            pMdl=pMdl->GetParentModel();
                        }
                            
                }
                [node updateIndex];
            }
        }

    }
}


-(IBAction)DoFieldSelector:(id)sender
{
    [_selectorController prepareWithReps:_nodes andViewDetails:_pDetails];
    [_selectorController showSelectedColumnWithID:@"selInst"];
    [_selectorController hideSelectedColumnWithID:@"selID"];
    [_mainWindow beginSheet:_selectorController.selectorWindow completionHandler:^(NSModalResponse returnCode)
     {
         [self refreshActive];
     }];
    
}

-(IBAction)DoNameChange:(id)sender
{
    STLString newName=[_nameField.stringValue cStringUsingEncoding:NSASCIIStringEncoding];
    
    if(_parentController.detailsMgr->NameIsUnique(newName, _pDetails))
    {
        _pDetails->SetViewName([_nameField.stringValue cStringUsingEncoding:NSASCIIStringEncoding]);
        [_parentController refreshViewTable];
    }
    else
    {
        STLString oldName=_pDetails->GetViewName();
        
        NSAlert* alert=[[NSAlert alloc] init];
        alert.messageText=@"Name in Use";
        alert.informativeText=[NSString stringWithFormat:@"The name \"%@\" is already in use by another view.",_nameField.stringValue];
        [alert addButtonWithTitle:@"OK"];
        [alert runModal];
        _nameField.stringValue=[NSString stringWithCString:oldName.c_str() encoding:NSASCIIStringEncoding];
        
    }
    
}

-(void)DoInstancePopup:(TblNode*)repNode withCell:(NSTableCellView*)cell
{
    [self willChangeValueForKey:@"value"];
    [self willChangeValueForKey:@"instString"];
    
    [_instView showRelativeToRect:cell.bounds ofView:cell preferredEdge:NSMinYEdge];
    
    [self didChangeValueForKey:@"value"];
    [self didChangeValueForKey:@"instString"];

}

@end
