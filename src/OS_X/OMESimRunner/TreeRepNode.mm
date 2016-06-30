//
//  TreeRepNode.m
//  OMESimRunner
//
//  Created by Patrick  Wingo on 2/9/16.
//  Copyright © 2016 OregonStateUniversity. All rights reserved.
//

#import "TreeRepNode.h"

@implementation TreeRepNode

@synthesize level=_level;
@synthesize isParent=_isParent;

@synthesize subNodes=_subNodes;
@synthesize parentModelName;
@synthesize subNodeCount;
@synthesize isLeaf;
//@synthesize instString;

+(RepNode*)treeRepNodeWithObject:(OMEObject*)pObj
{
    return [[TreeRepNode alloc] initWithObject:pObj];
}
+(RepNode*)treeRepNodeWithObject:(OMEObject*)pObj andLevel:(const int &)lvl
{
    return [[TreeRepNode alloc] initWithObject:pObj andLevel:lvl];
}

-(id)initWithObject:(OMEObject*)pObj
{
    return [self initWithObject:pObj andLevel:0];
}

-(id)initWithObject:(OMEObject*)pObj andLevel:(const int &)lvl
{
    _level=lvl;
    _isParent=NO;
    _subNodes=[NSMutableArray arrayWithCapacity:0];
    
    return [super initWithObject:pObj];;

}

-(NSUInteger)subNodeCount
{
    return _subNodes.count;
}

-(BOOL)isLeaf
{
    return !_isParent;
}

-(void)addNode:(TreeRepNode*)pRep
{
    [_subNodes addObject:pRep];
}

-(void)removeNode:(TreeRepNode*)pRep
{
    [_subNodes removeObject:pRep];
}

-(TreeRepNode*)getNode:(NSUInteger)i
{
    return (TreeRepNode*)_subNodes[i];
}

-(NSMutableArray*)activeSubNodes
{
    NSMutableArray* activeSubs=[NSMutableArray arrayWithCapacity:0];
    
    for(TreeRepNode* currNode in _subNodes)
    {
        if(currNode.isActive)
            [activeSubs addObject:currNode];
    }
    
    return activeSubs;
}

-(BOOL)isActive
{
    if(_subNodes.count)
    {
        BOOL active=NO;
        
        for(int i=0; i<_subNodes.count && !active; ++i)
            active=((RepNode*)_subNodes[i]).isActive;
        
        return active;
    }
    else
        return [super isActive];
}
@end
