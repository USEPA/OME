//
//  AppDelegate.m
//  OMESimRunner
//
//  Created by Patrick  Wingo on 10/19/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import "AppDelegate.h"
#import "AppCppShim.h"

@interface AppDelegate ()
{
    AppCppShim* _appShim;
}
@property (weak) IBOutlet NSWindow *window;
@property (weak) IBOutlet id simRunCtrllr;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    _appShim= [AppCppShim shimWithCtrllr:_simRunCtrllr];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
    [_appShim askForSave];
}

//for open recent menu
-(BOOL)application:(NSApplication *)sender openFile:(NSString *)filename
{
    [_appShim askForSave];
    return [_appShim openFile:filename];
}

@end
