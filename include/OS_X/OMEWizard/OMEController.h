//
//  OMEController.h
//  OMEWizard
//
//  Created by Patrick  Wingo on 10/4/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#ifndef OMEWizard_OMEController_h
#define OMEWizard_OMEController_h

@protocol OMEController
@required
-(BOOL)controlPathIsDirectory:(NSControl*)control;
@optional
-(void)updateControlsForControl:(NSControl*)control Result:(BOOL)result;
@end
#endif
