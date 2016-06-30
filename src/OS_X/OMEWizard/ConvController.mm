//
//  ConvController.m
//  OMEWizard
//
//  Created by Patrick  Wingo on 10/2/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import "ConvController.h"
#import "ParamItem.h"

#include "tinyxml2.h"

@implementation ConvController
@synthesize paramMappings;

-(void)awakeFromNib
{
    static BOOL hasRun=NO;
    if(!hasRun)
    {
        for(int i=0; i<TAG_COUNT;++i)
            valids[i]=NO;
        
        paramMappings=[NSMutableArray arrayWithCapacity:10];

        rootMdlName=nil;
        hasRun=YES;
    }
}

#pragma mark - Protocol methods

-(BOOL)controlPathIsDirectory:(NSControl*)control
{
    return control.tag==BROWSE_TRG_TAG;
}

-(void)updateControlsForControl:(NSControl*)control Result:(BOOL)result
{
    NSInteger tg=control.tag;
    
    valids[tg]=result;
    
    switch (tg) {
        case BROWSE_SRC_TAG:
        case BROWSE_TRG_TAG:
            [self ConvertEndabled:valids[BROWSE_SRC_TAG] && valids[BROWSE_TRG_TAG] && sourceTextField.stringValue.length && destTextField.stringValue.length];
            break;
        case BROWSE_PARAM_TAG:
            paramOKButton.enabled=result;
            break;
        default:
            break;
    }
}

#pragma mark - Convert section

-(IBAction)BrowseForFile:(id)sender
{

    switch ([sender tag]) {
        case BROWSE_SRC_TAG:
            [ConvController DoOpenDialogForControl:sourceTextField withTitle:@"Please select a model to Convert" andExts:@[@"sml",@"pl",@"mdl",@"xml"]];
            [sourceTextField textShouldEndEditing:[sourceTextField currentEditor]];
            break;
        case BROWSE_TRG_TAG:
            [ConvController DoSaveDialogForControl:destTextField];
            [destTextField textShouldEndEditing:[destTextField currentEditor]];
            break;
        case BROWSE_PARAM_TAG:
            [ConvController DoOpenDialogForControl:paramPathTextField withTitle:@"Please select a paramFile" andExts:@[@"csv",@"spf"]];
            [paramPathTextField textShouldEndEditing:[paramPathTextField currentEditor]];
            
            break;
        default:
            //for parameter stuff
            break;
    }
    
}

+(void)DoOpenDialogForControl:(NSControl*)ctrl withTitle:(NSString*)title andExts:(NSArray*)exts
{
    NSOpenPanel* oPanel=[NSOpenPanel openPanel];
    oPanel.canChooseDirectories=NO;
    oPanel.resolvesAliases=YES;
    oPanel.allowsMultipleSelection=NO;
    //oPanel.title=@"Convert Model";
    oPanel.title=title;
    oPanel.allowedFileTypes=exts;
    oPanel.directoryURL=[NSURL URLWithString:ctrl.stringValue];
    
    if([oPanel runModal]==NSOKButton)
        ctrl.stringValue=oPanel.URL.relativePath;

}

+(void)DoSaveDialogForControl:(NSControl*)ctrl
{
    NSSavePanel* sPanel=[NSSavePanel savePanel];
    sPanel.title=@"Please choose a destination:";
    sPanel.canCreateDirectories=YES;
    sPanel.showsTagField=YES;
    sPanel.extensionHidden=NO;
    sPanel.canSelectHiddenExtension=YES;
    sPanel.allowedFileTypes=@[@"omem"];
    
    if([sPanel runModal]==NSOKButton)
        ctrl.stringValue=sPanel.URL.relativePath;
}

-(void)ConvertEndabled:(BOOL)isEnabled
{
    [convButton setEnabled:isEnabled];
}

-(IBAction)DoConvert:(id)sender
{
    NSMutableArray* args=[NSMutableArray arrayWithCapacity:4];
    
    NSString* inPath=[sourceTextField stringValue];
    NSString* outPath=[destTextField stringValue];


    [args addObject:inPath];
    [args addObject:outPath];
    
    if(runQuietCB.state==NSOnState)
        [args addObject:@"-q"];
    
    for (ParamItem* pItem in paramMappings)
    {
        if(pItem.modelPath.length>0)
            [args addObject:[NSString stringWithFormat:@" -p\\\"%@\"::%@\\\"",pItem.modelPath,pItem.paramFilePath]];
        else
            [args addObject:[NSString stringWithFormat:@" -p\\\"%@\\\"",pItem.paramFilePath]];
    }
    NSString* convPath=[[NSBundle mainBundle] pathForAuxiliaryExecutable:@"UniversalConverter"];

    NSTask* ucTask=[[NSTask alloc] init];
    ucTask.launchPath=convPath;
    ucTask.arguments=args;
    
    //add termination handler?
    
    [rcController RunTask:ucTask];
}

#pragma mark - Parameter section
-(IBAction)AddParam:(id)sender
{
    //clear widgets
    paramOKButton.enabled=NO;
    paramPathTextField.stringValue=@"";
    modelNameTextField.stringValue=@"";
    
    [mainWindow beginSheet:addParamWindow completionHandler:^(NSModalResponse returnCode)
     {
        if(returnCode==NSModalResponseOK)
        {
            //add to param
            [paramController addObject:[[ParamItem alloc] initWithParamFile:paramPathTextField.stringValue forModelPath:modelNameTextField.stringValue]];
        }
     }];
}

-(IBAction)EndParamSheet:(id)sender
{
    [mainWindow endSheet:addParamWindow returnCode:[sender tag]];
}



@end
