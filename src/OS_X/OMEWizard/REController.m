//
//  REController.m
//  OMEWizard
//
//  Created by Patrick  Wingo on 10/2/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import "REController.h"

@implementation REController
@synthesize saveResults;

-(IBAction)enableTextField:(id)sender
{
    BOOL isOn=[((NSButton*)sender) state]==NSOnState;
    switch([sender tag])
    {
        case BROWSE_RES_TAG:
            resultTF.enabled=isOn;
            resultButton.enabled=isOn;
            break;
        case BROWSE_LOG_TAG:
            logTF.enabled=isOn;
            logButton.enabled=isOn;
            break;
        default:
            return;
            break;
    }
    runButton.enabled=[self isRunEnabled];

}

-(IBAction)enableExportField:(id)sender
{
    BOOL isOn=[sender tag];
    exportTF.enabled=isOn;
    expButton.enabled=isOn;
    runButton.enabled=[self isRunEnabled];

}



-(IBAction)DoFileSelect:(id)sender
{
    NSTextField* tf=nil;
    switch([sender tag])
    {
        case BROWSE_RES_TAG:
            [REController DoOpenDialogForControl:resultTF withTitle:@"Select field list:" andExts:@[@"txt"]];
            tf=resultTF;
            break;
        case BROWSE_RUN_TAG:
            [REController DoOpenDialogForControl:srcTF withTitle:@"Select a model to run" andExts:@[@"omem",@"omec"]];
            tf=srcTF;
            break;
        case BROWSE_LOG_TAG:
            [REController DoSaveDialogForControl:logTF withTitle:@"Save Log File:" andExts:@[@"txt"]];
            tf=logTF;
            break;
        case BROWSE_EXP_TAG:
            [REController DoSaveDialogForControl:exportTF withTitle:@"Export Results:" andExts:@[@"csv"]];
            tf=exportTF;
            break;
            
        default:
            return;
    }
    
    
    [tf textShouldEndEditing:[tf currentEditor]];
}

+(void)DoOpenDialogForControl:(NSControl*)ctrl withTitle:(NSString*)title andExts:(NSArray*)exts
{
    NSOpenPanel* oPanel=[NSOpenPanel openPanel];
    oPanel.canChooseDirectories=NO;
    oPanel.resolvesAliases=YES;
    oPanel.allowsMultipleSelection=NO;
    
    oPanel.title=title;
    oPanel.allowedFileTypes=exts;
    oPanel.directoryURL=[NSURL URLWithString:ctrl.stringValue];
    
    if([oPanel runModal]==NSOKButton)
    {
        ctrl.stringValue=oPanel.URL.relativePath;

    }

}

+(void)DoSaveDialogForControl:(NSControl*)ctrl withTitle:(NSString*)title andExts:(NSArray*)exts
{
    NSSavePanel* sPanel=[NSSavePanel savePanel];
    sPanel.title=title;
    sPanel.canCreateDirectories=YES;
    sPanel.showsTagField=YES;
    sPanel.extensionHidden=NO;
    sPanel.canSelectHiddenExtension=YES;
    sPanel.allowedFileTypes=exts;
    
    if([sPanel runModal]==NSOKButton)
        ctrl.stringValue=sPanel.URL.relativePath;
}
-(IBAction)RunModel:(id)sender
{

    
    NSMutableArray* args=[NSMutableArray arrayWithCapacity:4];
    
    NSString* inPath=srcTF.stringValue;
    
    
    if(runQuietCB.state==NSOnState)
        [args addObject:@"-q"];
    
    if(![integPopup.selectedItem.title isEqual:@"Default"])
        [args addObject:[NSString stringWithFormat:@"-s\"%@\"",integPopup.selectedItem.title]];
    if(resultCB.state==NSOnState)
        [args addObject:[NSString stringWithFormat:@"-r\"%@\"",resultTF.stringValue]];
    if(logCB.state==NSOnState)
        [args addObject:[NSString stringWithFormat:@"-l\"%@\"",logTF.stringValue]];
    if(expSingleRB.state==NSOnState)
        [args addObject:[NSString stringWithFormat:@"-c\"%@\"",exportTF.stringValue]];
    else if(resultCB.state==NSOnState)
        [args addObject:[NSString stringWithFormat:@"-f\"%@\"",exportTF.stringValue]];
    
    [args addObject:inPath];
    
    NSString* enginePath=[[NSBundle mainBundle] pathForAuxiliaryExecutable:@"OMEEngine"];
    NSTask* ucTask=[[NSTask alloc] init];
    ucTask.launchPath=enginePath;
    ucTask.arguments=args;
    
    //add termination handler?
    
    [rcController RunTask:ucTask];

}


-(BOOL)controlPathIsDirectory:(NSControl*)control
{
    return control==logTF || control==exportTF;
}

-(BOOL)isRunEnabled
{
    return (valids[BROWSE_RUN_TAG] &&
       (resultCB.state==NSOffState || valids[BROWSE_RES_TAG]) &&
       (logCB.state==NSOffState || valids[BROWSE_LOG_TAG]) &&
            (expNaRB.state==NSOnState || valids[BROWSE_EXP_TAG]));
       
}

-(void)updateControlsForControl:(NSControl*)control Result:(BOOL)result;
{
    valids[control.tag]=result;
    
    runButton.enabled=[self isRunEnabled];
}
@end
