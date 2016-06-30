//
//  SRController.m
//  OMESimRunner
//
//  Created by Patrick  Wingo on 10/29/14.
//  Copyright (c) 2014 OregonStateUniversity. All rights reserved.
//

#import "SRController.h"
#import "ViewEntry.h"
#import "OPController.h"
#import "TreeViewController.h"
#import "TableViewController.h"

#import <string>

//C++ progress update function
void UpdateFunc(const OME_SCALAR & currTime, BaseManager* pBm,void* pObjCCtrl)
{
    [((__bridge SRController*)pObjCCtrl) performSelectorOnMainThread:@selector(updateRunViews:) withObject:[NSNumber numberWithDouble:currTime] waitUntilDone:NO];
}

@implementation SRController

@synthesize viewArray=_viewArray;
@synthesize modelURL=_path;
@synthesize viewTable;
@synthesize startTime;
@synthesize stopTime;
@synthesize stepSize;
//@synthesize interval;
@synthesize useMinRecord;
@synthesize viewMgr=_detailsMgr;
@synthesize simMgr=_pSimManager;
@synthesize hasRun=_hasRun;

-(id)init
{
    [self willChangeValueForKey:@"viewArray"];
    [self willChangeValueForKey:@"hasRun"];
    _viewArray = [NSMutableArray arrayWithCapacity:10];
    
    _currentTime=0.0;

    _hasRun=NO;
    _lastRunTime=0.0;
    _path=nil;
    _pSimManager=nullptr;
    
    
    [self didChangeValueForKey:@"viewArray"];
    [self didChangeValueForKey:@"hasRun"];
    
    return self;
}

-(void)awakeFromNib
{
    static BOOL hasLoaded=NO;
    
    if(!hasLoaded)
    {
        [self willChangeValueForKey:@"hasRun"];
        
        _currentTime=0.0;
        
        
        _hasRun=NO;
        _lastRunTime=0.0;
        
        _path=nil;
        _pSimManager=nullptr;
        hasLoaded=YES;
        
        [self updateViewSelectionButtons:NO];
        [self updateControls];
        
        _currPrefPane=nil;
        _currDataPane=nil;
        _currController=nil;
        
        _prefConstraints=[NSMutableArray arrayWithCapacity:4];
        _dataConstraints=[NSMutableArray arrayWithCapacity:4];
        
        [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"NSConstraintBasedLayoutVisualizeMutuallyExclusiveConstraints"];
        [self didChangeValueForKey:@"hasRun"];
    }
}

-(IBAction)openCtrl:(id)sender
{
    [self askForSave];
    
    NSOpenPanel* oDlg=[NSOpenPanel openPanel];
    oDlg.canChooseDirectories=NO;
    oDlg.canCreateDirectories=NO;
    oDlg.allowedFileTypes=@[@"omec"];

    [oDlg beginSheetModalForWindow:_mainWindow completionHandler:^(NSInteger result)
    {
        if(result==NSFileHandlingPanelOKButton && [self loadCtrlFileWithURL:oDlg.URL])
            [[NSDocumentController sharedDocumentController] noteNewRecentDocumentURL:oDlg.URL];
    }];
    
    
}

-(BOOL)loadCtrlFileWithPath:(NSString*)path
{
    return [self loadCtrlFileWithURL:[NSURL fileURLWithPath:path]];
}

-(BOOL)loadCtrlFileWithURL:(NSURL*)url
{
    _infoTextField.stringValue=@"Loading Model...";
    
    _path=url;
    const char* fileRep=[_path fileSystemRepresentation];
    bool ok = _xmlCtrlDoc.LoadFile(fileRep)==tinyxml2::XML_SUCCESS;
    
    if ( ok )
    {
        TI2Element *pXmlRoot = _xmlCtrlDoc.RootElement();
        _pSimManager=new SimManager(true);
        oSTLSStream errStrm;
        unsigned int flags = SimManager::IS_SINGLETON;
        _pSimManager->PopulateFromXMLTree(pXmlRoot,fileRep,flags,errStrm);
        if(!errStrm.str().empty())
        {
            // report loading errors
        }
        
        _currentTime=0.0;
        
        //prepare views
        _detailsMgr.ClearViews();
        _detailsMgr.LoadFromXMLTree(pXmlRoot,fileRep);
        _detailsMgr.ClearSaveFlags();
        
        //load views
        [self willChangeValueForKey:@"viewArray"];
        [_viewArray removeAllObjects];
        
        for(size_t i=0; i<_detailsMgr.GetViewCount();++i)
            [_viewTableController addObject:[ViewEntry viewEntryWithDetails:_detailsMgr[i]]];
        [self didChangeValueForKey:@"viewArray"];
    }
    [self updateControls];
    
    return ok;
}

-(IBAction)saveCtrl:(id)sender
{
    if(!_path)
        [self saveAsCtrl:sender];
    else
        [self writeCtrlFile:_path];
}

-(IBAction)saveAsCtrl:(id)sender
{
    NSSavePanel* sPanel=[NSSavePanel savePanel];
    sPanel.allowedFileTypes=@[@"omec"];
    sPanel.canCreateDirectories=YES;
    sPanel.canSelectHiddenExtension=YES;
    sPanel.nameFieldStringValue=@"OMEControl";
    
    if([sPanel runModal]==NSOKButton)
        [self writeCtrlFile:sPanel.URL];
}


-(IBAction)addView:(id)sender
{
    std::string name("Untitled_View");
    std::string fullName;
    if(_detailsMgr.NameIsUnique(name))
        fullName=name;
    else
    {
        int num=0;
        do
        {
            fullName=name+std::string("_")+std::to_string(num);
            ++num;
        }while(!_detailsMgr.NameIsUnique(fullName));
    }
    RV_TYPE rType;
    switch(_viewPopup.selectedTag)
    {
        case 0:
            rType=RVT_TABLE;
            break;
        case 1:
            rType=RVT_TREE;
            break;
        case 2:
            rType=RVT_MULTVAL;
            break;
    }
    [_viewTableController addObject:[ViewEntry viewEntryWithDetails:_detailsMgr.NewRD(rType, fullName)]];
}

-(IBAction)removeView:(id)sender
{
    ViewEntry* deadView=(ViewEntry*)_viewArray[_viewTableController.selectionIndex];
    _detailsMgr.RemoveRD([deadView rawDetails]);
    [_viewTableController removeObject:deadView];
    [self tableViewSelectionDidChange:nil];
}

-(IBAction)upView:(id)sender
{
    NSInteger ind=[viewTable selectedRow];
    if(ind>0)
    {
        ViewEntry* selected=_viewArray[ind];
        [_viewTableController removeObject:selected];
        [_viewTableController insertObject:selected atArrangedObjectIndex:ind-1];
        [viewTable selectRowIndexes:[NSIndexSet indexSetWithIndex:ind-1] byExtendingSelection:NO];
    }
}

-(IBAction)downView:(id)sender
{
    NSInteger ind=[viewTable selectedRow];
    if(ind<[_viewArray count]-1)
    {
        ViewEntry* selected=_viewArray[ind];
        [_viewTableController removeObject:selected];
        [_viewTableController insertObject:selected atArrangedObjectIndex:ind+1];
        [viewTable selectRowIndexes:[NSIndexSet indexSetWithIndex:ind+1] byExtendingSelection:NO];
    }
}

-(IBAction)runSimulation:(id)sender
{
    if(_pSimManager)
    {
        _pSimManager->SetManagerReportFunction(UpdateFunc,(__bridge void*)self);
        
        _pSimManager->PreloadSubmodels();
        //run model in thread seperate from the UI thread
        
        //start thread
        [NSThread detachNewThreadSelector:@selector(simThread:) toTarget:self withObject:nil];
    }
}

-(IBAction)timeSliderDrag:(id)sender
{
    //http://www.cocoabuilder.com/archive/cocoa/170471-nsslider-did-finish-sliding.html

    //intermediate update to slider.
    [_popoverContent takeDoubleValueFrom:sender];
    
    //[_popoverContent sizeToFit];
    [_tsPopover showRelativeToRect:_timeSlider.bounds ofView:sender preferredEdge:NSMinYEdge];
        //The following code ensures that the record change will occur at the end of the drag
    SEL sel=@selector(selectTimeStep:);
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:sel object:sender];
    [self performSelector:sel withObject:sender afterDelay:0.0];
}

-(IBAction)selectTimeStep:(id)sender
{
    OME_SCALAR newTime=[sender doubleValue];
    if(_currentTime!=newTime)
    {
        _currentTime=newTime;
        if(_pSimManager->GetRootModel())
        {
            _pSimManager->LoadValuesForTime(_currentTime);
            _timeSlider.doubleValue=_currentTime;
            _currTimeField.doubleValue=_currentTime;
            
            if (_currController)
                [_currController reloadData];
        }
    }
    [_tsPopover close];
}


-(IBAction)editParams:(id)sender
{
    [_prefController prepParamDetails:self];
    [_mainWindow beginSheet:_prefWindow completionHandler:^(NSModalResponse returnCode) {
        if(returnCode==NSModalResponseStop)
        {
            [self updateControls];
        }
    }];
}

-(void)updateRunViews:(NSNumber*)currTime
{
    _timeProgress.doubleValue=currTime.doubleValue;
    //[_timeProgress setNeedsDisplay:YES];
    _currTimeField.doubleValue=currTime.doubleValue;
}

-(void)updateViewSelectionButtons:(BOOL)hasSelection
{
    _upButton.enabled=hasSelection;
    _downButton.enabled=hasSelection;
    _removeButton.enabled=hasSelection;
}

-(void)askForSave
{
    if(_path && _detailsMgr.SaveIsNeeded())
    {
        //ask for save
        NSAlert* saveAlrt=[[NSAlert alloc] init];
        [saveAlrt addButtonWithTitle:@"Yes"];
        [saveAlrt addButtonWithTitle:@"No"];
        saveAlrt.messageText=@"Save Changes?";
        saveAlrt.informativeText=[NSString stringWithFormat:@"Save changes for %@?",_path.lastPathComponent];
        if([saveAlrt runModal]==NSAlertFirstButtonReturn)
            [self saveCtrl:self];
    }

}

-(BOOL)saveNeeded
{
    return _detailsMgr.SaveIsNeeded();
    
}
#pragma mark - SimManager-related properties
-(OME_SCALAR)startTime
{
    return _pSimManager ? _pSimManager->GetStartTime() : 0.0;
}

-(void)setStartTime:(OME_SCALAR)t
{
    if(_pSimManager)
        _pSimManager->SetStartTime(t);
}

-(OME_SCALAR)stopTime
{
    return _pSimManager ? _pSimManager->GetStopTime() : 0.0;
}

-(void)setStopTime:(OME_SCALAR)t
{
    if(_pSimManager)
        _pSimManager->SetStopTime(t);
}

-(OME_SCALAR)stepSize
{
    return _pSimManager ? _pSimManager->GetStepSize() : 0.0;
}

-(void)setStepSize:(OME_SCALAR)step
{
    if(_pSimManager)
        _pSimManager->SetStepSize(step);
}

//-(OME_SCALAR)interval
//{
//    
//}
//
//-(void)setInterval:(OME_SCALAR)it
//{
//    
//}

-(BOOL)useMinRecord
{
    return _pSimManager ? _pSimManager->GetMinimalRecords() : NO;
}

-(void)setUseMinRecord:(BOOL)useMin
{
    if(_pSimManager)
        _pSimManager->SetMinimalRecords(useMin);
}

-(OMEIntegrationBlock::INT_METHOD)IntMethod
{
    return _pSimManager ? _pSimManager->GetIntMethod() : OMEIntegrationBlock::IM_UNDEF;
}

-(void)setIntMethod:(OMEIntegrationBlock::INT_METHOD)im
{
    if(_pSimManager)
        _pSimManager->SetIntMethod(im);
}

-(SimManager::SOLVER_TYPE)Solver
{
    return _pSimManager ? _pSimManager->GetSolverType() : SimManager::DEFAULT;
}

-(void)setSolver:(SimManager::SOLVER_TYPE)s
{
    if(_pSimManager)
        _pSimManager->SetSolverType(s);
}

#pragma mark - Private functions

-(BOOL)writeCtrlFile:(NSURL*)path
{
    static const STLString xmlheader="<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    BOOL ret=NO;
    
    TI2Element* pXmlRoot=_xmlCtrlDoc.RootElement();
    if(pXmlRoot)
    {
        TI2Element* pXmlViews=pXmlRoot->FirstChildElement(_detailsMgr.GetXMLTag());
        if(pXmlViews)
            pXmlRoot->DeleteChild(pXmlViews);
        
        TI2Document tempDoc;
        if(tempDoc.Parse((xmlheader+_detailsMgr.GetXMLRepresentation(0,4)).c_str())==tinyxml2::XML_SUCCESS)
        {
            [SRController deepXMLCopyToElement:(pXmlRoot->InsertEndChild(tempDoc.RootElement()->ShallowClone(&_xmlCtrlDoc)))->ToElement() document:_xmlCtrlDoc fromElement:tempDoc.RootElement()];

            
            //should check for error.
            if(_xmlCtrlDoc.SaveFile([path fileSystemRepresentation])==tinyxml2::XML_SUCCESS)
                ret=NO;
            
            _detailsMgr.ClearSaveFlags();
            _path = path;
        }
        //else err;
        
    }
    return ret;
}

-(NSString*)getModelDescription
{
    NSString* ret;
    
    if(_pSimManager)
    {
        ret= [NSString stringWithFormat:@"Start Time: %g\nStop Time: %g\nReport Interval: %g\nIntegration Step: %g\nIntegration Method: %@",self.startTime,self.stopTime,_pSimManager->GetInterval(),_pSimManager->GetStepSize(),_pSimManager->GetIntMethod()==1 ? @"Euler" : @"RK4"];
      
        if(_hasRun)
            ret=[ret stringByAppendingFormat:@"\nLast Run Duration (s): %g",_lastRunTime];
    }
    else
        ret=@"No Model Loaded";
    
    return ret;
}

-(void)updateControls
{
    _infoTextField.stringValue=[self getModelDescription];
    _startTimeField.doubleValue=self.startTime;
    _stopTimeField.doubleValue=self.stopTime;

    BOOL modelLoaded = _path ? YES : NO;
    _addButton.enabled=modelLoaded;
    _runButton.enabled=modelLoaded;
    _runMenu.enabled=modelLoaded;
    
    if(_hasRun)
    {
        _runButton.title=@"ReRun";
        if(_pSimManager->GetMinimalRecords())
        {
            _timeSlider.hidden=YES;
            _timeProgress.hidden=NO;
        }
        else
        {
            _timeSlider.hidden=NO;
            _timeProgress.hidden=YES;
        }
    }
    else
    {
        _runButton.title=@"Run";
        _timeSlider.hidden=YES;
        _timeProgress.hidden=NO;
        _currTimeField.enabled=NO;
    }
}

-(void)simThread:(id)extra
{
    NSDate* startClock=[NSDate date];
    [self performSelectorOnMainThread:@selector(configureRun:) withObject:@YES waitUntilDone:YES];
    _pSimManager->RunSimulation();
    [self performSelectorOnMainThread:@selector(configureRun:) withObject:@NO waitUntilDone:YES];
    _lastRunTime=[startClock timeIntervalSinceNow]*-1.0;
    
    [self performSelectorOnMainThread:@selector(refreshTimeSlider:) withObject:[NSNumber numberWithDouble:_pSimManager->GetStopTime()] waitUntilDone:YES];
    [self performSelectorOnMainThread:@selector(selectTimeStep:) withObject:_currTimeField waitUntilDone:NO];
}

-(void)configureRun:(NSNumber*)runVal
{
    BOOL isRunning=runVal.boolValue;
    _runButton.enabled=!isRunning;
    _timeSlider.hidden=isRunning;
    _timeProgress.hidden=!isRunning;
    _currTimeField.enabled=!isRunning;
    
    if(isRunning)
    {
        _timeProgress.minValue=self.startTime;
        _timeProgress.maxValue=self.stopTime;
        _timeProgress.doubleValue=self.startTime;
        
    }
    else
    {
        _timeSlider.minValue=self.startTime;
        _timeSlider.maxValue=self.stopTime;
        _timeSlider.doubleValue=self.startTime;
        [self willChangeValueForKey:@"hasRun"];
        _hasRun=YES;
        [self didChangeValueForKey:@"hasRun"];
    }
}

+(void)deepXMLCopyToElement:(TI2Element*)pCopyTo document:(TI2Document&)toDoc fromElement:(TI2Element*)pCopyFrom
{
    TI2Element* pCurrElem=pCopyFrom->FirstChildElement();
    while(pCurrElem)
    {
        [SRController deepXMLCopyToElement:(pCopyTo->InsertEndChild(pCurrElem->ShallowClone(&toDoc)))->ToElement() document:toDoc fromElement:pCurrElem];
        pCurrElem=pCurrElem->NextSiblingElement();
    }
}

+(void)insertView:(NSView*)inView intoView:(NSView*)parentView updatingConstraints:(NSMutableArray*)constraints
{
    if(constraints.count)
        [parentView removeConstraints:constraints];
    [inView setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    [parentView addSubview:inView];

    [constraints removeAllObjects];
    
    [constraints addObjectsFromArray:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|[inView]|"
                                                        options:0 metrics:nil
                                                        views:NSDictionaryOfVariableBindings(inView)]];
    [constraints addObjectsFromArray:[NSLayoutConstraint constraintsWithVisualFormat:@"V:|[inView]|"
                                                                             options:0 metrics:nil
                                                                               views:NSDictionaryOfVariableBindings(inView)]];
    
    [parentView addConstraints:constraints];
    [parentView layoutSubtreeIfNeeded];
}

-(void)refreshViewTable
{
    [viewTable reloadData];
}

#pragma mark - Table delegation
- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
    [self updateViewSelectionButtons:[viewTable numberOfSelectedRows]];
    //add view updating stuff here.
    
    if(_currPrefPane)
    {
        //[currPrefPane retain];
        [_currPrefPane removeFromSuperview];
    }
    if(_currDataPane)
    {
        //[currDataPane retain];
        [_currDataPane removeFromSuperview];
    }
    
    if(viewTable.selectedRow>=0)
    {
        _detailsMgr.SetActiveDetails(viewTable.selectedRow);
        switch(_detailsMgr.GetActiveDetails()->GetRVType())
        {
            case RVT_TREE:
                _currPrefPane=[_tvController prefPane];
                _currDataPane=[_tvController dataPane];
                _currController=_tvController;
            break;
            case RVT_MULTVAL:
                _currPrefPane=[_mvController prefPane];
                _currDataPane=[_mvController dataPane];
                _currController=_mvController;
                break;
            case RVT_TABLE:
                _currPrefPane=[_tblController prefPane];
                _currDataPane=[_tblController dataPane];
                _currController=_tblController;
                break;
            default:
                _currPrefPane=nil;
                _currDataPane=nil;
                _currController=nil;
        }
    }
    else
    {
        _currPrefPane=nil;
        _currDataPane=nil;
        _currController=nil;
    }

    if(_currPrefPane)
        [SRController insertView:_currPrefPane intoView:_visPrefsView updatingConstraints:_prefConstraints];
    
    if(_currDataPane)
        [SRController insertView:_currDataPane intoView:_visDispView updatingConstraints:_dataConstraints];
    
    if(_currController)
    {
        [_currController init:_detailsMgr.GetActiveDetails()];
        [_currController reloadData];

    }
}

-(void)refreshTimeSlider:(NSNumber*)currStep
{
    //update slider info
    _timeSlider.numberOfTickMarks=(_pSimManager->GetStopTime()-_pSimManager->GetStartTime())/_pSimManager->GetInterval()+1;
    _timeSlider.doubleValue=currStep.doubleValue;
}

-(ResultsDetailsMgr*)detailsMgr
{
    return &_detailsMgr;
}
@end
