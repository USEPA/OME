//
//  AppCppShim.m
//  OMESimRunner
//
//  Created by Patrick  Wingo on 2/8/15.
//  Copyright (c) 2015 OregonStateUniversity. All rights reserved.
//

#import "AppCppShim.h"
#import "SRController.h"

@implementation AppCppShim

-(id)initWithCtrllr:(SRController*)ctrllr
{
    _ctrllr=ctrllr;
    return self;
}

+(AppCppShim*) shimWithCtrllr:(SRController*)ctrllr
{
    return [[AppCppShim alloc] initWithCtrllr:ctrllr];
}

-(BOOL)openFile:(NSString*)path
{
    return [_ctrllr loadCtrlFileWithPath:path];
}

-(void)askForSave
{
    [_ctrllr askForSave];
}
@end
