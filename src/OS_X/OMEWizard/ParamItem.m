//
//  ParamItem.m
//  OMEWizard
//
//  Created by Patrick  Wingo on 10/10/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import "ParamItem.h"

@implementation ParamItem
@synthesize paramFilePath;
@synthesize modelPath;

-(id)init
{
    return [self initWithParamFile:@"" forModelPath:@""];
}

-(id)initWithParamFile:(NSString*)pPath forModelPath:(NSString*)mPath
{
    self=[super init];
    if(self)
    {
        modelPath=[NSString stringWithString:mPath];
        
        paramFilePath=[NSString stringWithString:pPath];
    }
    return self;
}

-(NSArray*)modelList
{
    return [paramFilePath componentsSeparatedByString:@"."];
}
@end
