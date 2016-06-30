//
//  BaseDVController.h
//  OMEWizard
//
//  Created by Patrick  Wingo on 11/10/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Appkit/Appkit.h>

#import "BaseResultsDetails.h"

@protocol BaseDVController

@required
-(void)init:(BaseResultsDetails*)rDetails;
-(NSView*)prefPane;
-(NSView*)dataPane;

-(void)reloadData;
@optional

@end