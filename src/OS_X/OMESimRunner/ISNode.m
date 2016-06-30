//
//  ISNode.m
//  OMESimRunner
//
//  Created by Patrick  Wingo on 2/9/16.
//  Copyright © 2016 OregonStateUniversity. All rights reserved.
//

#import "ISNode.h"

@implementation ISNode

@synthesize level=_levelName;
@synthesize index=_instIndex;

+(ISNode*)ISNodeWithIndex:(unsigned int)i
{
    return [[ISNode alloc] initWithIndex:i];
}

+(ISNode*)ISNodeWithLevel:(NSString*)n andIndex:(unsigned int)i
{
    return [[ISNode alloc] initWithLevel:n andIndex:i];
}

-(id)initWithIndex:(unsigned int)i
{
    _instIndex=i;
    _levelName=@"";
    return self;
}

-(id) initWithLevel:(NSString*)n andIndex:(unsigned int)i
{
    _levelName=n;
    _instIndex=i;
    return self;
}


@end
