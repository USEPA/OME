//
//  RunConsoleController.h
//  OME
//
//  Created by Patrick  Wingo on 6/2/16.
//  Copyright © 2016 Wingo. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Appkit/Appkit.h>

@interface RunConsoleController : NSObject<NSWindowDelegate>
{
    IBOutlet NSWindow* consoleWindow;
    IBOutlet NSTextView* consoleTextView;
    
    NSTask* activeTask;
}
@property(readwrite) NSColor* textColor;
@property(readwrite) NSColor* bgColor;

-(void)RunTask:(NSTask*)theTask;

//window delegation
- (void)windowWillClose:(NSNotification *)notification;
@end
