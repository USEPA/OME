//
//  ABNode.m
//  OMESimRunner
//
//  Created by Patrick  Wingo on 2/11/15.
//  Copyright (c) 2015 OregonStateUniversity. All rights reserved.
//

#import "ABNode.h"

@implementation ABNode

@synthesize line;
@synthesize selected;
@synthesize value=_value;
@synthesize coords=_coords;

-(id)initWithIndex:(NSInteger)ind value:(OME_SCALAR)val coordStr:(NSString*)coord parent:(RepNode*)pNode
{
    _index=ind;
    _parent=pNode;
    _value=[NSNumber numberWithDouble:val];
    _coords=coord;
    return self;
}

+(ABNode*)ABNodeWithIndex:(NSInteger)ind value:(OME_SCALAR)val coordStr:(NSString*)coord parent:(RepNode*)pNode
{
    return [[ABNode alloc] initWithIndex:ind value:val coordStr:coord parent:pNode];
}

-(NSString*)line
{
    return [NSString stringWithFormat:@"%zu",_index+1];
}

-(NSInteger)selected
{
    return _isSelected ? NSOnState : NSOffState;
}

-(void)setSelected:(NSInteger)state
{
    if(state==NSOnState && _parent.index!=_index)
    {
        _parent.index=_index;
    }
    _isSelected=(state==NSOnState ? YES : NO);
}

@end
