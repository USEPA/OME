//
//  TblNode.h
//  OMESimRunner
//
//  Created by Patrick  Wingo on 2/9/16.
//  Copyright © 2016 OregonStateUniversity. All rights reserved.
//
#import <vector>
#import "RepNode.h"
#import "ValueArchive.h"


@interface TblNode : RepNode
{
    NSMutableArray* _indexes;
}
@property(readonly) NSString* instString;
@property(readwrite) NSMutableArray* indexes;
@property(readwrite) ValueArchive* valArchive;

@property(readonly) BOOL hasIndices;

+(TblNode*) TblNodeWithObject:(OMEObject*)pObj;
-(id) initNodeWithObject:(OMEObject*)pObj;

-(NSString*)instString;
-(BOOL)hasIndices;
-(OME_SCALAR)value;

-(void)updateIndex;
@end