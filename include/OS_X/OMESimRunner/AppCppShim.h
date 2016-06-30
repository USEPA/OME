//
//  AppCppShim.h
//  OMESimRunner
//
//  Created by Patrick  Wingo on 2/8/15.
//  Copyright (c) 2015 OregonStateUniversity. All rights reserved.
//

#import <Foundation/Foundation.h>

@class SRController;
@interface AppCppShim : NSObject
{
    SRController* _ctrllr;
}

-(id)initWithCtrllr:(SRController*)ctrllr;
+(AppCppShim*) shimWithCtrllr:(SRController*)ctrllr;

-(BOOL)openFile:(NSString*)path;
-(void)askForSave;
@end
