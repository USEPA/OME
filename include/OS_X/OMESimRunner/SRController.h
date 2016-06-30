//
//  SRController.h
//  OMESimRunner
//
//  Created by Patrick  Wingo on 10/29/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#import "ResultsDetailsMgr.h"
#import "SimManager.h"


#import "MultiValViewController.h"
#import "BaseDVController.h"

@class OPController;
@class TreeViewController;
@class TableViewController;
@interface SRController : NSObject <NSTableViewDelegate>
{

    
    NSMutableArray* _viewArray;
    ResultsDetailsMgr _detailsMgr;
    
    SimManager* _pSimManager;

    OME_SCALAR _currentTime;
    
    BOOL _hasRun;
    double _lastRunTime;
    
    TI2Document _xmlCtrlDoc;
    NSURL* _path;
    
    NSMutableArray* _prefConstraints;
    NSMutableArray* _dataConstraints;
    
    NSView* _currPrefPane;
    NSView* _currDataPane;
    id <BaseDVController> _currController;
    
   }
@property(weak) IBOutlet NSWindow* mainWindow;
@property(weak) IBOutlet NSWindow* prefWindow;
@property(weak) IBOutlet NSMenuItem* runMenu;

@property(weak) IBOutlet NSTextField* infoTextField;

@property(weak) IBOutlet NSPopUpButton* viewPopup;
@property(weak) IBOutlet NSButton* addButton;
@property(weak) IBOutlet NSButton* removeButton;
@property(weak) IBOutlet NSButton* upButton;
@property(weak) IBOutlet NSButton* downButton;
@property(weak) IBOutlet NSTableView* viewTable;

@property(weak) IBOutlet NSView* visPrefsView;
@property(weak) IBOutlet NSView* visDispView;

@property(weak) IBOutlet NSButton* runButton;
@property(weak) IBOutlet NSTextField* startTimeField;
@property(weak) IBOutlet NSSlider* timeSlider;

@property(weak) IBOutlet NSProgressIndicator* timeProgress;
@property(weak) IBOutlet NSTextField* currTimeField;
@property(weak) IBOutlet NSTextField* stopTimeField;
@property(weak) IBOutlet NSPopover* tsPopover;
@property(weak) IBOutlet NSTextField* popoverContent;
@property(weak) IBOutlet NSArrayController* viewTableController;
@property(weak) IBOutlet OPController* prefController;

//view controllers
@property(weak) IBOutlet TreeViewController* tvController;
@property (weak) IBOutlet MultiValViewController *mvController;
@property(weak) IBOutlet TableViewController* tblController;

@property(readwrite,retain) NSMutableArray* viewArray;
@property (copy) NSURL* modelURL;
@property(readwrite,getter=startTime,setter=setStartTime:) OME_SCALAR startTime;
@property(readwrite,getter=stopTime,setter=setStopTime:) OME_SCALAR stopTime;
@property(readwrite,getter=stepSize,setter=setStepSize:) OME_SCALAR stepSize;
//@property(readwrite,getter=interval,setter=setInterval:) OME_SCALAR interval;
@property(readwrite,getter=useMinRecord,setter=setUseMinRecord:) BOOL useMinRecord;
@property(readwrite,getter=IntMethod,setter=setIntMethod:) OMEIntegrationBlock::INT_METHOD intMethod;
@property(readwrite,getter=Solver,setter=setSolver:) SimManager::SOLVER_TYPE solver;
@property(readonly) ResultsDetailsMgr& viewMgr;
@property(readonly) SimManager* simMgr;
@property(readonly) BOOL hasRun;
@property(readonly,getter=detailsMgr) ResultsDetailsMgr* detailsMgr;
@property(readonly,getter=saveNeeded) BOOL saveNeeded;

-(IBAction)openCtrl:(id)sender;
-(IBAction)saveCtrl:(id)sender;
-(IBAction)saveAsCtrl:(id)sender;
-(IBAction)editParams:(id)sender;

//-(IBAction)quit:(id)sender;

-(IBAction)addView:(id)sender;
-(IBAction)removeView:(id)sender;
-(IBAction)upView:(id)sender;
-(IBAction)downView:(id)sender;

-(IBAction)runSimulation:(id)sender;
-(IBAction)timeSliderDrag:(id)sender;
-(IBAction)selectTimeStep:(id)sender;


-(void)updateRunViews:(NSNumber*)currTime;
-(void)updateViewSelectionButtons:(BOOL)hasSelection;
-(void)askForSave;

-(BOOL)loadCtrlFileWithPath:(NSString*)path;

-(void)refreshViewTable;
//TableDelegate stuff
- (void)tableViewSelectionDidChange:(NSNotification *)aNotification;

-(ResultsDetailsMgr*)detailsMgr;
@end
