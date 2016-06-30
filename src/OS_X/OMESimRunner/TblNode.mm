//
//  TblNode.m
//  OMESimRunner
//
//  Created by Patrick  Wingo on 2/9/16.
//  Copyright © 2016 OregonStateUniversity. All rights reserved.
//

#import "TblNode.h"
#import "ISNode.h"
#import "VarArray.h"
#import "Model.h"

@implementation TblNode
@synthesize indexes=_indexes;
@synthesize valArchive=_pValArch;

+(TblNode*) TblNodeWithObject:(OMEObject*)pObj
{
    return [[TblNode alloc] initNodeWithObject:pObj];
}

-(id) initNodeWithObject:(OMEObject*)pObj
{
    _indexes=[NSMutableArray arrayWithCapacity:10];
    Model* pMdl=pObj->GetParentModel();
    switch(pObj->GetOMEType())
    {
        case OME_VARARRAY:
            for(unsigned short i=0; i<((VarArray*)pObj)->GetDimCount();++i)
                [_indexes addObject:[ISNode ISNodeWithLevel:[NSString stringWithFormat:@"%s_%u",((Evaluable*)pObj)->GetExprName().c_str(),i+1 ] andIndex:1]];
            //drop through
        default:
            while(pMdl && pMdl->GetParentModel()!=nil)
            {
                [_indexes addObject:[ISNode ISNodeWithLevel:[NSString stringWithCString:pMdl->GetName().c_str() encoding:NSASCIIStringEncoding] andIndex:1]];
                pMdl=pMdl->GetParentModel();
            }
            
    }

    return [super initWithObject:pObj];
}

-(void)updateIndex
{
    size_t mdlOffset=0;
    size_t step=1;
    if(_indexes.count)
    {
        if(_pObj->IsOMEType(OME_VARARRAY))
        {
            step=((VarArray*)_pObj)->GetSize();
            mdlOffset=((VarArray*)_pObj)->GetDimCount();
            PersistListDims tmpDims(mdlOffset);
            for(unsigned short i=0; i<mdlOffset;++i)
                tmpDims[i]=((ISNode*)_indexes[i]).index;
            
            mdlOffset=Listable::DeriveIndex(tmpDims, ((VarArray*)_pObj)->GetDims());
            
        }
        _index=_pObj->GetParentModel()->GetArchiveIndex(((Evaluable*)_pObj)->GetModelIndex(),step);
    }
    //else err
}

-(NSString*)instString
{
    NSMutableString* outBuff=[NSMutableString stringWithString:@"{ "];
    for(ISNode* node in _indexes)
    {
        [outBuff appendFormat:@"%u ",node.index];
    }
    [outBuff appendString:@"}"];
    
    return outBuff;
}

-(BOOL)hasIndices
{
    return _indexes.count>0;
}

-(OME_SCALAR)value
{
    if(_pObj->GetParentModel()->GetValueArchive())
        return _pObj->GetParentModel()->GetValueArchive()->operator[](_index);
    return 0.0;
}


@end
