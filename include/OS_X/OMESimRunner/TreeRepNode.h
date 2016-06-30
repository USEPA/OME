//
//  TreeRepNode.h
//  OMESimRunner
//
//  Created by Patrick  Wingo on 2/9/16.
//  Copyright © 2016 OregonStateUniversity. All rights reserved.
//

#import "RepNode.h"

@interface TreeRepNode : RepNode
{
    int _level;
    BOOL _isParent;
    
    NSMutableArray* _subNodes;

}

@property(readwrite,assign) int level;
@property(readwrite,assign) BOOL isParent;
@property(readonly) NSMutableArray* subNodes;
@property(readonly,getter=activeSubNodes) NSMutableArray* activeSubNodes;
@property(readwrite,assign,getter=expanded,setter=setExpanded:) BOOL expanded;

@property(readonly,getter=parentModelName) NSString* parentModelName;

@property(readonly,getter=subNodeCount) NSUInteger subNodeCount;
@property(readonly,getter=isLeaf) BOOL isLeaf;

+(TreeRepNode*)treeRepNodeWithObject:(OMEObject*)pObj;
+(TreeRepNode*)treeRepNodeWithObject:(OMEObject*)pObj andLevel:(const int &)lvl;

-(id)initWithObject:(OMEObject*)pObj;
-(id)initWithObject:(OMEObject*)pObj andLevel:(const int &)lvl;

-(BOOL)expanded;
-(void)setExpanded:(BOOL)exp;
-(NSUInteger)subNodeCount;
-(BOOL)isLeaf;
-(void)addNode:(TreeRepNode*)pRep;
-(void)removeNode:(TreeRepNode*)pRep;
-(TreeRepNode*)getNode:(NSUInteger)i;
-(NSMutableArray*)activeSubNodes;

-(BOOL)isActive;

@end