//
//  ViewEntry.h
//  OMESimRunner
//
//  Created by Patrick  Wingo on 10/29/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "BaseResultsDetails.h"

@interface ViewEntry : NSObject
{
    BaseResultsDetails* pDetails;
}


@property(readonly,getter=name) NSString* name;
@property(readonly,getter=viewType) NSString* viewType;
-(id)initWithDetails:(BaseResultsDetails*)pD;
-(NSString*)name;
-(NSString*)viewType;
-(BaseResultsDetails*)rawDetails;

+(ViewEntry*)viewEntryWithDetails:(BaseResultsDetails*)pD;
@end
