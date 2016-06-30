//
//  ISNode.h
//  OMESimRunner
//
//  Created by Patrick  Wingo on 2/9/16.
//  Copyright © 2016 OregonStateUniversity. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface ISNode : NSObject
{
    NSString* _levelName;
    unsigned int _instIndex;
}
@property(readwrite) NSString* level;
@property(readwrite) unsigned int index;

+(ISNode*)ISNodeWithIndex:(unsigned int)i;
+(ISNode*)ISNodeWithLevel:(NSString*)n andIndex:(unsigned int)i;
-(id)initWithIndex:(unsigned int)i;
-(id) initWithLevel:(NSString*)n andIndex:(unsigned int)i;

@end
