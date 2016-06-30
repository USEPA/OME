//
//  TFPathDelegate.h
//  OMEWizard
//
//  Created by Patrick  Wingo on 10/4/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import "OMEController.h"

@interface TFPathDelegate : NSObject <NSTextFieldDelegate>
{
    IBOutlet id <OMEController> controller;
}
- (BOOL)control:(NSControl *)control textShouldEndEditing:(NSText *)fieldEditor;
@end
