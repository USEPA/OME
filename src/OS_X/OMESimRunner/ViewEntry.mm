//
//  ViewEntry.m
//  OMESimRunner
//
//  Created by Patrick  Wingo on 10/29/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import "ViewEntry.h"

@implementation ViewEntry
@synthesize name;
@synthesize viewType;

-(id)initWithDetails:(BaseResultsDetails*)pD
{
    pDetails=pD;
    return self;
}

-(NSString*)name
{
    return [NSString stringWithCString:(pDetails->GetViewName()).c_str() encoding:NSASCIIStringEncoding];
}

-(NSString*)viewType
{
    NSString* ret;
    
    switch(pDetails->GetRVType())
    {
        case RVT_TABLE:
            ret=@"Table";
            break;
        case RVT_TREE:
            ret=@"Tree";
            break;
        case RVT_MULTVAL:
            ret=@"MultiValue";
            break;
    }
    return ret;
}

-(BaseResultsDetails*)rawDetails
{
    return pDetails;
}

+(ViewEntry*)viewEntryWithDetails:(BaseResultsDetails*)pD
{
    return [[ViewEntry alloc] initWithDetails:pD];
}
@end
