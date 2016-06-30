//
//  RepNode.h
//  OMESimRunner
//
//  Created by Patrick  Wingo on 12/17/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#import "TreeResultsDetails.h"

@interface RepNode : NSObject
{
    OMEObject* _pObj;
    size_t _index;
    NSString* _name;
    
    BOOL _active;
    NSColor* _txtColor;
    NSMutableArray* _arrayValCache;
    
}
@property(readonly) OMEObject* object;
@property(readonly) NSString* name;
@property(readwrite,getter=isActive,setter=setIsActive:) BOOL isActive;
@property(readwrite) NSColor* color;

@property(readonly,getter=objectType) OMETYPE objectType;
@property(readonly,getter=objectTypeString) NSString* objectTypeString;
@property(readonly,getter=value) OME_SCALAR value;
@property(readonly,getter=valueCount) size_t valueCount;

@property(readonly,getter=count) size_t count;
@property(readonly,getter=isAlias) BOOL isAlias;
@property(readonly,getter=hasInstances) BOOL hasInstances;
@property(readwrite,assign,getter=instance,setter=setInstance:) size_t instance;
@property(readonly, getter=instString) NSString* instString;
@property(readwrite,assign,getter=selectedIndex,setter=setSelectedIndex:) size_t index;
@property(readonly,getter=dimString) NSString* dimensions;
@property(readonly,getter=objectID) NSString* objectID;

@property(readonly) NSString* simpleDescription;
@property(readonly) NSMutableArray* arrayValues;
@property(readonly) NSMutableArray* instanceValues;


//skip visible
//skip active
//skip treeactive

+(RepNode*)repNodeWithObject:(OMEObject*)pObj;

-(id)initWithObject:(OMEObject*)pObj;

-(NSString*)parentModelName;
-(OMETYPE)objectType;
-(OME_SCALAR)value;
-(size_t)valueCount;
-(size_t)instance;
-(void)setInstance:(size_t)inst;
-(size_t)count;
-(BOOL)isAlias;
-(BOOL)hasInstances;
-(BOOL)isActive;
-(void)setIsActive:(BOOL)act;

-(NSString*)instString;
-(void)populatePartialRep:(EvalRep*)pRep;

-(void)clearTempArrayValCache;

-(size_t)selectedIndex;
-(void)setSelectedIndex:(size_t)ind;
@end
