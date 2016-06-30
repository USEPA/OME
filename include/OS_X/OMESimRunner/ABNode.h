//
//  ABNode.h
//  OMESimRunner
//
//  Created by Patrick  Wingo on 2/11/15.
//  Copyright (c) 2015 OregonStateUniversity. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "RepNode.h"

@interface ABNode : NSObject
{
    NSInteger _index;
    RepNode* _parent;
    BOOL _isSelected;
}

@property(readonly,getter=line) NSString* line;
@property(readwrite,assign,getter=selected, setter=setSelected:) NSInteger selected;
@property(readwrite,copy) NSNumber* value;
@property(readwrite,copy) NSString* coords;

-(id)initWithIndex:(NSInteger)ind value:(OME_SCALAR)val coordStr:(NSString*)coord parent:(RepNode*)pNode;
+(ABNode*)ABNodeWithIndex:(NSInteger)ind value:(OME_SCALAR)val coordStr:(NSString*)coord parent:(RepNode*)pNode;

-(NSString*)line;
-(NSInteger)selected;
-(void)setSelected:(NSInteger)state;
@end
