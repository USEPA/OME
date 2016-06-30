//
//  TFPathDelegate.m
//  OMEWizard
//
//  Created by Patrick  Wingo on 10/4/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import "TFPathDelegate.h"

@implementation TFPathDelegate

- (BOOL)control:(NSControl *)control textShouldEndEditing:(NSText *)fieldEditor
{
    NSTextField* tf=(NSTextField*)control;
    
    NSURL* evalPath;
    
    if(![controller controlPathIsDirectory:control])
        evalPath=[NSURL fileURLWithPath:control.stringValue];
    else
        evalPath=[NSURL fileURLWithPath:[control.stringValue stringByDeletingLastPathComponent]];
    
    BOOL valid=[evalPath checkResourceIsReachableAndReturnError:nil];
    if(valid)
        tf.textColor=[NSColor blackColor];
    else
        tf.textColor=[NSColor redColor];
    
    if([(NSObject*)controller respondsToSelector:@selector(updateControlsForControl:Result:)])
        [controller updateControlsForControl:control Result:valid];
    return YES;
}

@end
