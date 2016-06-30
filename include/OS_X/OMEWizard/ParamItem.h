//
//  ParamItem.h
//  OMEWizard
//
//  Created by Patrick  Wingo on 10/10/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface ParamItem : NSObject
{
    NSString* modelPath;
    NSString* paramFilePath;
}
@property(copy) NSString* modelPath;
@property(copy) NSString* paramFilePath;

-(id)init;
-(id)initWithParamFile:(NSString*)pPath forModelPath:(NSString*)mPath;
-(NSArray*)modelList;
@end
