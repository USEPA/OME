//
//  RunConsoleController.m
//  OME
//
//  Created by Patrick  Wingo on 6/2/16.
//  Copyright © 2016 Wingo. All rights reserved.
//

#import "RunConsoleController.h"

@implementation RunConsoleController

@synthesize textColor;
@synthesize bgColor;

-(id)init
{
    textColor=[NSColor grayColor];
    bgColor=[NSColor blackColor];
    activeTask=nil;
    
    return self;
}

-(void)RunTask:(NSTask*)theTask
{
    [consoleWindow makeKeyAndOrderFront:0];
    
    //modified from https://www.raywenderlich.com/125071/nstask-tutorial-os-x
    
    consoleTextView.string=@"";
    
    //set up piping
    NSPipe* outPipe=[NSPipe pipe];
    theTask.standardOutput=outPipe;
    theTask.standardError=outPipe;
    theTask.standardInput=outPipe;
    
    activeTask=theTask;
    [[outPipe fileHandleForReading] waitForDataInBackgroundAndNotify];
    [[NSNotificationCenter defaultCenter] addObserverForName:NSFileHandleDataAvailableNotification object:outPipe.fileHandleForReading queue:nil usingBlock:^(NSNotification* note) {
        NSData* outData=outPipe.fileHandleForReading.availableData;
        NSString* inString=[[NSString alloc] initWithData:outData encoding:NSUTF8StringEncoding];
        
        dispatch_async(dispatch_get_main_queue(), ^{
            static NSColor* activeColor=nil;
            
            NSMutableAttributedString* modStr=[[NSMutableAttributedString alloc] initWithAttributedString:consoleTextView.attributedString];
            [modStr appendAttributedString:[[NSAttributedString alloc] initWithString:@"\n"]];
            
            //
            NSScanner* valScanner=[NSScanner scannerWithString:inString];
            NSString* openCtrl=[NSString stringWithFormat:@"%C[",0x001B];
            NSCharacterSet* controlSet=[NSCharacterSet characterSetWithCharactersInString:openCtrl];
            NSCharacterSet* closeSet=[NSCharacterSet characterSetWithCharactersInString:@"m"];
            NSString* tmpStr= nil;
            while(!valScanner.atEnd)
            {
                [valScanner scanUpToString:openCtrl intoString:&tmpStr];
                
                if(tmpStr)
                {
                    if(!activeColor)
                        [modStr appendAttributedString:[[NSAttributedString alloc] initWithString:tmpStr attributes:@{NSForegroundColorAttributeName:textColor}]];
                    else
                        [modStr appendAttributedString:[[NSAttributedString alloc] initWithString:tmpStr attributes:@{NSForegroundColorAttributeName:activeColor}]];
                }
                
                if(!valScanner.atEnd)
                {
                    [valScanner scanCharactersFromSet:controlSet intoString:nil];
                    
                    //scrape color code
                    int colorCode;
                    
                    //switch to character search
                    [valScanner scanInt:&colorCode];
                    activeColor=[RunConsoleController colorForANSICode:colorCode];
                    [valScanner scanCharactersFromSet:closeSet intoString:nil];
                }
            }
            //
            
            [consoleTextView textStorage].attributedString=modStr;
            
            NSRange range=NSMakeRange(modStr.length,0);
            [consoleTextView scrollRangeToVisible:range];
        });
    }];
    
    //launch task
    [theTask launch];
    [theTask waitUntilExit];
    activeTask=nil;
}


+(NSColor*)colorForANSICode:(int)code
{
    NSColor* outColor;
    
    switch(code)
    {
        case 0:
        default:
            outColor=nil;
            break;
        case 30:
            outColor=[NSColor blackColor];
            break;
        case 31:
            outColor=[NSColor redColor];
            break;
        case 32:
            outColor=[NSColor greenColor];
            break;
        case 33:
            outColor=[NSColor yellowColor];
            break;
        case 34:
            outColor=[NSColor blueColor];
            break;
        case 35:
            outColor=[NSColor magentaColor];
            break;
        case 36:
            outColor=[NSColor cyanColor];
            break;
        case 37:
            outColor=[NSColor whiteColor];
            break;
    }
    return outColor;
}

-(void)setBgColor:(NSColor *)bgC
{
    bgColor=bgC;
    consoleTextView.backgroundColor=bgColor;
}
-(NSColor*)bgColor
{
    return bgColor;
}

#pragma mark - window delegation
- (void)windowWillClose:(NSNotification *)notification
{
    if (activeTask)
        [activeTask terminate];
}


@end
