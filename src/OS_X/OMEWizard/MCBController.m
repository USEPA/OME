//
//  MCBController.m
//  OMEWizard
//
//  Created by Patrick  Wingo on 10/2/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import "MCBController.h"

@implementation MCBController

-(IBAction)BrowseModel:(id)sender
{
    NSOpenPanel* oPanel=[NSOpenPanel openPanel];
    oPanel.canChooseDirectories=NO;
    oPanel.resolvesAliases=YES;
    oPanel.allowsMultipleSelection=NO;
    oPanel.title=@"Select a model to convert";
    oPanel.allowedFileTypes=@[@"omem",@"omec"];
    oPanel.directoryURL=[NSURL URLWithString:srcTF.stringValue];
    
    if([oPanel runModal]==NSOKButton)
    {
        srcTF.stringValue=oPanel.URL.relativePath;
        [srcTF textShouldEndEditing:[srcTF currentEditor]];
    }
}

-(IBAction)BrowseDir:(id)sender
{
    NSOpenPanel* oPanel=[NSOpenPanel openPanel];
    oPanel.canChooseDirectories=YES;
    oPanel.canChooseFiles=NO;
    oPanel.canCreateDirectories=YES;
    oPanel.resolvesAliases=YES;
    oPanel.allowsMultipleSelection=NO;
    oPanel.title=@"Select a place to save the model class";
    oPanel.directoryURL=[NSURL URLWithString:destTF.stringValue];
    
    if([oPanel runModal]==NSOKButton)
        destTF.stringValue=oPanel.URL.relativePath;

}

-(IBAction)DoConvert:(id)sender
{
    NSMutableArray* args=[NSMutableArray arrayWithCapacity:4];
    
    if(runQuietCB.state==NSOnState)
        [args addObject:@"-q"];
    if(destTF.stringValue.length>0)
        [args addObject:[NSString stringWithFormat:@"-o\"%@\"",destTF]];
    if(mNameTF.stringValue.length>0)
        [args addObject:[NSString stringWithFormat:@"-c\"%@\"",destTF]];
    
    [args addObject:srcTF.stringValue];
    NSString* mcbPath=[[NSBundle mainBundle] pathForAuxiliaryExecutable:@"ModelClassBuilder"];
    
    NSTask* ucTask=[[NSTask alloc] init];
    ucTask.launchPath=mcbPath;
    ucTask.arguments=args;
    
    //add termination handler?
    
    [rcController RunTask:ucTask];

}

-(BOOL)controlPathIsDirectory:(NSControl*)control
{
    return [control tag]==1;
}

-(void)updateControlsForControl:(NSControl*)control Result:(BOOL)result;
{
    if (control.tag==BROWSE_MDL_TAG)
        convButton.enabled=result;
        
}
@end
