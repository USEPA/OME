//
//  RepNode.m
//  OMESimRunner
//
//  Created by Patrick  Wingo on 12/17/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import "RepNode.h"
#import "OMETypes.h"
#import "SimManager.h"
#import "ABNode.h"
#import "UpdateContainers/StaticUpArray.h"


@implementation RepNode

@synthesize object=_pObj;
@synthesize name=_name;
@synthesize isActive=_active;
@synthesize color=_txtColor;
@synthesize index=_index;

@synthesize objectType;
@synthesize value;
@synthesize count;
@synthesize hasInstances;
@synthesize dimensions;
@synthesize instance;
@synthesize instString;

+(RepNode*)repNodeWithObject:(OMEObject*)pObj
{
    return [[RepNode alloc] initWithObject:pObj];
}

-(id)initWithObject:(OMEObject*)pObj
{
    _pObj=pObj;
    _index=0;
    _txtColor=[NSColor colorWithRed:0.0 green:0.0 blue:0.0 alpha:1.0];
    
    _arrayValCache=nil;
    
    if(_pObj)
    {
        _name=[[NSMutableString stringWithCString:_pObj->GetName().c_str() encoding:NSASCIIStringEncoding] stringByReplacingOccurrencesOfString:@"\n" withString:@" "];
    }
    
    return self;
}

-(OMETYPE)objectType
{
    if(_pObj)
        return _pObj->GetOMEType();
    else
        return OME_NULL;
}

-(NSString*)objectTypeString
{
    if(_pObj)
        return [NSString stringWithCString:_pObj->StrForType() encoding:NSASCIIStringEncoding];
    
    return @"";
}

-(OME_SCALAR)value
{
    if (_pObj->IsOMEType(OME_EVAL) && ((Evaluable*)_pObj)->IsInitialized())
    {
                    
        if (_pObj->GetOMEType() != OME_VARARRAY)
        {
            OME_SCALAR temp = ((Evaluable*)_pObj)->GetValue();
            return temp;
        }
        else
            return (*(VarArray*)_pObj)[_index];
    }
    else
        return 0.0;
}

-(size_t)valueCount
{
    size_t ret=0;
    switch (_pObj->GetOMEType())
    {
        case OME_MODEL:
            ret=((Model*)_pObj)->GetNumInstances();
            break;
        case OME_VARARRAY:
            ret=((VarArray*)_pObj)->GetSize();
            break;
        default:
            ret=1;
            break;
    }
    return ret;
}


-(NSString*)parentModelName
{
    return _pObj->GetParentModel() ? [NSString stringWithCString:_pObj->GetParentModel()->GetName().c_str() encoding:NSASCIIStringEncoding] : @"None";
}

-(size_t)instance
{
    if(_pObj->GetOMEType()==OME_MODEL)
        return ((Model*)_pObj)->GetCurrentInstance()+1;
    else if(_pObj->GetOMEType()==OME_VARARRAY)
        return _index+1;
    
    return 0;
}

-(void)setInstance:(size_t)inst
{
    inst--;
    if(_pObj->GetOMEType()==OME_MODEL)
    {
        Model* pMdl=(Model*)_pObj;
        if(inst>=pMdl->GetNumInstances())
            inst=pMdl->GetNumInstances()-1;
        pMdl->SetCurrentInstance(inst);
    }
    else if (_pObj->GetOMEType()==OME_VARARRAY)
    {
        VarArray* pVa=(VarArray*)_pObj;
        if(inst>=pVa->GetSize())
            inst=pVa->GetSize()-1;
        _index=inst;
    }
}

-(size_t)count
{
    size_t ret=0;
    if(_pObj->GetOMEType()==OME_MODEL)
        ret=((Model*)_pObj)->GetNumInstances();
    else if (_pObj->GetOMEType()==OME_VARARRAY && ((VarArray*)_pObj)->IsInitialized())
        ret=((VarArray*)_pObj)->GetSize();
    return ret;
}


-(BOOL)isAlias
{
    return _pObj->IsOMEType(OME_ALIAS);
}

-(BOOL)hasInstances
{
    return _pObj && (_pObj->IsOMEType(OME_MODEL) || _pObj->IsOMEType(OME_VARARRAY));
}

-(NSString*)instString
{
    if(_pObj)
    {
        if(_pObj->IsOMEType(OME_MODEL))
        {
            Model* pMdl=(Model*)_pObj;
            return [NSString stringWithFormat:@"%zu of %zu",pMdl->GetCurrentInstance()+1,pMdl->GetNumInstances()];
        }
        else if(_pObj->IsOMEType(OME_VARARRAY))
        {
            VarArray* pVa=(VarArray*)_pObj;
            return [NSString stringWithFormat:@"%zu of %zu",_index+1,pVa->GetSize()];
        }
    }
    
    return @"";
}

-(NSString*)dimString
{
    if(_pObj->GetOMEType()==OME_VARARRAY)
    {
        VarArray* pVa=(VarArray*)_pObj;
        
        PersistListDims dims(pVa->GetDims());
        
        for(unsigned short i=0; i<pVa->GetDimCount();++i)
            dims[i]=pVa->GetDim(i);
        
        return [self buildDimsString:dims withOffset:0];
    }
    
    return @"";
}

-(NSString*)objectID
{
    if(_pObj)
        return [NSString stringWithCString:_pObj->GetID().c_str() encoding:NSASCIIStringEncoding];
    return @"";
}

-(void)populatePartialRep:(EvalRep*)pRep
{
    
    pRep->id=_pObj->GetID();
    
    pRep->color.r=[_txtColor redComponent];
    pRep->color.g=[_txtColor greenComponent];
    pRep->color.b=[_txtColor blueComponent];
}


-(NSString*)simpleDescription
{
    
    NSMutableString* strBuff=[NSMutableString stringWithFormat:@"%s\n%s\n{   ",_pObj->GetName().c_str(),_pObj->LabelForType()];

    Model* pMdl;
    bool noCoord;
    if ((pMdl=_pObj->GetParentModel()))
    {
        while (pMdl->GetParentModel() != NULL)
        {
            noCoord = pMdl->GetActiveInstanceCount() <= 1 && !pMdl->IsDynamic() && !pMdl->IsConditional();
            if (noCoord)
                [strBuff appendString:@"("];
            [strBuff appendFormat:@"%s", pMdl->GetName().c_str()];
            
            if (noCoord)
                [strBuff appendString:@")"];
            [strBuff appendString:@"   "];
            
            pMdl = pMdl->GetParentModel();
        }
    }
    
    [strBuff appendString:@"}\n{   "];
    
    switch (_pObj->GetOMEType())
    {
        case OME_MODEL:
            break;
            
        case OME_SUBPORT:
            pMdl = ((SubmodelPort*)_pObj)->GetOuterModel();
            if (pMdl)
            {
                while (pMdl->GetParentModel() != NULL)
                {
                    [strBuff appendFormat:@"%zu   ",pMdl->GetActiveInstanceCount()];
                    pMdl = pMdl->GetParentModel();
                }
            }
            break;
        case OME_VARARRAY:
            for (unsigned short i = 0; i < ((VarArray*)_pObj)->GetDimCount(); ++i)
                [strBuff appendFormat:@"%zu   ", ((VarArray*)_pObj)->GetDim(i)];
            
            //drop through
        default:
            pMdl = _pObj->GetParentModel();
            
            while (pMdl != NULL)
            {
                if (pMdl->GetActiveInstanceCount()>1 || pMdl->IsDynamic() || pMdl->IsConditional())
                    [strBuff appendFormat:@"%zu   ", pMdl->GetActiveInstanceCount()];
             
                pMdl = pMdl->GetParentModel();
            }
            break;
    }
    
    [strBuff appendString:@"}"];

    return strBuff;
}

-(NSMutableArray*)arrayValues
{
    
    if(!_arrayValCache && _pObj->GetOMEType()==OME_VARARRAY)
    {
        
        VarArray* pVa=(VarArray*)_pObj;
        
        _arrayValCache=[NSMutableArray arrayWithCapacity:pVa->GetSize()];
        
        PersistListDims currDims(pVa->GetDimCount());
        
        for(size_t i=0; i<pVa->GetSize(); ++i)
        {
            pVa->GetDimsForRawIndex(i, currDims);
            [_arrayValCache addObject:[ABNode ABNodeWithIndex:i value:pVa->GetValue(i) coordStr:[self buildDimsString:currDims withOffset:1] parent:self]];
        }
        
        ((ABNode*)_arrayValCache[_index]).selected=NSOnState;
    }
    
    return _arrayValCache;
}

-(NSMutableArray*)instanceValues
{
    //reuse array value cache
    if(!_arrayValCache)
    {
        size_t lvl = 0;
        Model* pMdl;
        
        //find depth level
        switch (_pObj->GetOMEType())
        {
            case OME_MODEL:
                break;
                
            case OME_SUBPORT:
                pMdl = ((SubmodelPort*)_pObj)->GetOuterModel();
                if (pMdl)
                {
                    while (pMdl->GetParentModel() != NULL)
                    {
                        pMdl = pMdl->GetParentModel();
                        ++lvl;
                    }
                }
                break;
            case OME_VARARRAY:
                for (unsigned short i = 0; i < ((VarArray*)_pObj)->GetDimCount(); ++i)
                    ++lvl;
                //drop through
            default:
                pMdl = _pObj->GetParentModel();
                
                while (pMdl != NULL)
                {
                    if (pMdl->GetActiveInstanceCount()>1 || pMdl->IsDynamic() || pMdl->IsConditional())
                        ++lvl;
                    pMdl = pMdl->GetParentModel();
                }
                if (lvl == 0 && _pObj->GetParentModel()->GetNumInstances())
                    lvl = 1;
                break;
        }

        
        _arrayValCache=[NSMutableArray arrayWithCapacity:1];
        //construct and assign list values
        if (_pObj->IsOMEType(OME_EVAL))
        {
            if (!_pObj->IsOMEType(OME_VARARRAY) && lvl == 1 && ((Evaluable*)_pObj)->GetParentModel()->GetActiveInstanceCount() == 1)
                [_arrayValCache addObject:@{@"value":[NSNumber numberWithDouble:((Evaluable*)_pObj)->GetValue()],@"coords":@"{ 1 }"} ];
            else
            {
                /// TODO: figure out why the below conditional does not work!
                //OMETYPE oType = rp->ObjectType;
                //StaticUpArray upList(oType != OME_VARARRAY ? (Evaluable*)rp->Object : (VarArray*)rp->Object, lvl);
                StaticUpArray upList = StaticUpArray::NewSUAFromStack((Evaluable*)_pObj, lvl);
                PersistListDims currCoord(upList.GetDimCount());
                
                for (size_t i = 0; i < upList.GetSize(); ++i)
                {
                    upList.DeriveDims(i, currCoord);
                    [_arrayValCache addObject:@{@"value":[NSNumber numberWithDouble:upList[i]],@"coords":[self buildDimsString:currCoord withOffset:1]} ];
               }
            }
        }
    }
    return _arrayValCache;
}

-(NSString*)buildDimsString:(PersistListDims&)dims withOffset:(unsigned int)offset
{
    NSMutableString* ret= [NSMutableString stringWithString:@"{ "];
       for(unsigned short i=0; i<dims.GetSize();++i)
        [ret appendFormat:@"%lu ",dims[i]+offset];
    
    [ret appendString:@"}"];
    return ret;

}

-(BOOL)isActive
{
    return _active;
}

-(void)setIsActive:(BOOL)act
{
    _active=act;
}

-(void)clearTempArrayValCache
{
    _arrayValCache=nil;
}

-(size_t)selectedIndex
{
    return _index;
}

-(void)setSelectedIndex:(size_t)ind
{
    if(_arrayValCache && ind!=_index)
    {
        [self willChangeValueForKey:@"instString"];
        [self willChangeValueForKey:@"value"];
        ((ABNode*)_arrayValCache[_index]).selected=NSOffState;
        _index=ind;
        [self didChangeValueForKey:@"instString"];
        [self didChangeValueForKey:@"value"];

    }
}
@end
