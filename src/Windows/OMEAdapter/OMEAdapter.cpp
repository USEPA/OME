// OMEAdapter.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "VData.h"
#include "PathManager.h"
#include "OMEAdapter.h"
#include "MapLayer.h"
#include "OMEPrefDLog.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

OMEProcess::~OMEProcess( void )
   {
   POSITION pos=m_instMap.GetStartPosition();
   OMEAInst rep;
   int key;
   while(pos)
	  {
	  m_instMap.GetNextAssoc(pos,key,rep);
	  delete rep.m_pMgr;
	  delete[] rep.m_intermedArray;
	  delete rep.m_pProvider;

	  //should find a better place to clean this up.
	  //for(int i=0; i<rep.m_recs.size(); i++)
	  //   delete rep.m_recs[i].m_pDo;
	  }
   }

/** Envision Autonomous Process function. Called on load of process.
	@param pEnvContext Pointer to the active EnvContext.
	@param initStr Arguments provided by the entry in the Envision project file.
	@return true if initialization was successful; false otherwise.
*/
BOOL OMEProcess::Init( EnvContext *pEnvContext, LPCTSTR initStr )
   {
   OMEAInst dummy;
   DBG_ALT_PRINT_FUNC(DebugPrint);
   DBG_PRINT("Debug output loaded.");

   if(!m_instMap.Lookup(pEnvContext->id,dummy))
	  {
	  using namespace tinyxml2;
	  TI2Document doc;
	  CString initPath;
	  PathManager::FindPath(initStr,initPath);
	  float start=0.0;
	  float stop=0.0;
	  float interval=0.0;
	  float stepSize=1.0;

	  m_reportLevel = ALL;
	  //SubmodelSDPEntryMap sdpMappings;
	 // SubmodelSDPEntryMap feedbackMappings;

	  //StrArray SDPModelInits;
	  OMEAInst newInst;
	
		unsigned int mgrFlags=SimManager::IS_SINGLETON | SimManager::NO_DEBUG | SimManager::NO_SDP | SimManager::NO_FULL_RECORD;
		 newInst.m_pMgr= new SimManager(initPath.GetString(),start,stop+1,interval,stepSize,mgrFlags);
		 newInst.m_pMgr->SetScriptDebugging(false); //disable debugging prompt.
			newInst.m_pMgr->EnableFileCache(); //use file cache for instances
		 newInst.m_pProvider= new EnvSpatialDataProvider((MapLayer*)pEnvContext->pMapLayer);
			newInst.m_pMgr->SetSpatialDataProvider(newInst.m_pProvider);
		 
		 newInst.m_pMgr->RegisterErrListener(this);
		 newInst.m_pMgr->RegisterIncListener(this);
		 //newInst.m_pMgr->RegisterPauseListener(this);

		 //apply optional visibility rules
		 //newInst.m_pMgr->SetVisRule(vvStr);

		 //preload initial submodels for Envision
		 try
			{
			BindReadableVars(newInst);
			//newInst.m_pMgr->PreloadSubmodels();
			  //Get and register placeholders for full records.		
		   /* newInst.m_recs=newInst.m_pMgr->GetReadableRecords();
			for(int i=0; i<newInst.m_recs.size(); i++)
			   {
			   AddOutputVar(newInst.m_recs[i].m_modelName.c_str(),newInst.m_recs[i].m_pDo,"");
			   }*/
			} 
		 catch (std::bad_alloc& ba)
			{
			CString msg;
			msg.Format("OMEProcess (%i): Failure to allocate memory during initialization: %s\n",pEnvContext->id,ba.what());
			Report::ErrorMsg(msg);
			return false;
			}

		 CString indString;

	   

		 //register inputs
		 EvalRepArray inputs=newInst.m_pMgr->GetConstantObjectList();
		 //clean up if this array had been previously allocated
		 if (newInst.m_intermedArray)
			 delete[] newInst.m_intermedArray;
		 newInst.m_intermedArray=new OME_SCALAR[inputs.size()];
		 for(int i =0; i<inputs.size(); i++)
			{
			newInst.m_pMgr->GetReadableValue(inputs[i],newInst.m_intermedArray[i]);
			AddInputVar(inputs[i].name.c_str(),newInst.m_intermedArray[i],inputs[i].desc.c_str());
			}

		 //store instance struct
		 m_instMap[pEnvContext->id]=newInst;
		// }
	  //other initialization

       //load initial values
		 //m_pCurrContext = pEnvContext;
       newInst.m_pMgr->PrepSimulation();
	   //newInst.m_pMgr->AddFieldsToSDP();
   //    newInst.m_pMgr->SyncCoverage(SyncDeltas, this);
	  DBG_PRINTBIGDIV();
	  DBG_TIMESTAMP();

	  }

   ((MapLayer*)pEnvContext->pMapLayer)->ClearQueryResults();


   return TRUE;

   }

/** Envision Autonomous Process function. Called At the beginning of each run.
@param pEnvContext Pointer to the active EnvContext.
@param useInitialSeed use the same random seed across all runs.
@return true if initialization was successful; false otherwise.
*/
BOOL OMEProcess::InitRun( EnvContext *pEnvContext, bool useInitialSeed )
   {

   //are there pieces that would need to be updated here?
   OMEAInst currInst;
   if(m_instMap.Lookup(pEnvContext->id,currInst))
	  {
	   OME_SCALAR stopModifier;
	   Units::ConvertVal(1.0, UM_YEAR, currInst.m_pMgr->GetTimeUnits(), stopModifier);
	  // currInst.m_pMgr->SetStartTime(pEnvContext->startYear*stopModifier);
	   currInst.m_pMgr->SetStopTime((pEnvContext->endYear - pEnvContext->startYear)*stopModifier);
	   DBG_PRINT("Start: " + std::to_string(pEnvContext->startYear)+" / "+std::to_string(currInst.m_pMgr->GetStartTime()));
	   DBG_PRINT("Stop: " + std::to_string(pEnvContext->endYear) + " / " + std::to_string(currInst.m_pMgr->GetStopTime()));
	   DBG_PRINT("Step: " + std::to_string(currInst.m_pMgr->GetStepSize()));
	   DBG_PRINT(STLString("Method: ") + (currInst.m_pMgr->GetIntMethod() == OMEIntegrationBlock::IM_EULER ? "Euler" : "RK4"));
	   DBG_PRINT(currInst.m_pMgr->GetIncToBounds() ? "Step To Boundary" : "Step to Closest");
	  if(!currInst.m_pMgr->InitSim())
		 {
		 CString msg; 
		 msg.Format( _T("OMEProcess(id=%i): Simulation initialization failed"), pEnvContext->id );
		 Report::ErrorMsg( msg );
		 }

	  //load lever values
	  EvalRepArray inputs=currInst.m_pMgr->GetConstantObjectList();
	  for(int i =0; i<inputs.size(); i++)
		 {
		 currInst.m_pMgr->SetConstantValue(inputs[i],currInst.m_intermedArray[i]);
		 }
	  }

   
   return TRUE; 
   }

/** Envision Autonomous Process function. Executes a process.
@param pContext Pointer to the active EnvContext.
@return true if run was successful; false otherwise.
*/
BOOL OMEProcess::Run( EnvContext *pContext )
   {

   OMEAInst currInst;
   if(m_instMap.Lookup(pContext->id,currInst))
	  {
	  //cache for sync
	  m_pCurrContext=pContext;

	  CentralEvent pauseEvent(this,NULL,CED_PAUSE,0.0,0.0,CE_TOP_PRIORITY-1,NULL);
	  //ensure that we are comparing apples to apples
	  OME_SCALAR convCurr;
	  Units::ConvertVal(pContext->yearOfRun,UM_YEAR,currInst.m_pMgr->GetTimeUnits(),convCurr);

	  //only run simulation if simulation time is within bounds
	  if(convCurr>=currInst.m_pMgr->GetStartTime() && convCurr<currInst.m_pMgr->GetStopTime())
		 {

       if (currInst.m_pMgr->GetIncToBounds() && m_reportLevel == ALL)
          DBG_PRINT("Bound Step to: " + std::to_string(currInst.m_pMgr->GetPresentTime()));
		 //pause after one year
		 OME_SCALAR pauseTime;
		 Units::ConvertVal(1.0,UM_YEAR,currInst.m_pMgr->GetTimeUnits(),pauseTime);
		 pauseTime+=convCurr;

#ifdef _DEBUG
		 CString mStr;
		 mStr.Format("Pause Time: %f",pauseTime);
		 DBG_PRINT(mStr.GetString());
#endif
		 pauseEvent.SetEventTime(pauseTime);

		 //update map layer if necessary
		 currInst.m_pMgr->PostEvent(pauseEvent);
		 currInst.m_pMgr->RunSimulation();

		 //apply deltas
		 currInst.m_pMgr->SyncCoverage(SyncDeltas,this);
		 }


	  }
   return TRUE;

   }

/** Envision Autonomous Process function. Called At the end of each run.
@param pContext Pointer to the active EnvContext.
@return true if processing was successful; false otherwise.
*/
BOOL OMEProcess::EndRun ( EnvContext *pContext )
   {
   //Synchronize records with internal model histories.
   OMEAInst currInst;
   if(m_instMap.Lookup(pContext->id,currInst))
	  {
	  currInst.m_pMgr->UpdateRecords(currInst.m_recs);
	  currInst.m_pMgr->SyncCoverage(SyncDeltas,this);
	  }
   /// \todo add cleanup stuff for models.

   return TRUE;
   }

/** Provide bindings from readable model components from OME.
	@param inst The OME instance to reference.
*/
void OMEProcess::BindReadableVars(OMEAInst & inst)
   {
   /// \todo rewrite for better clearer summation
   inst.m_pMgr->PreloadSubmodels();
   //Get and register placeholders for full records.		
   //inst.m_recs=inst.m_pMgr->GetReadableRecords();
   //for(size_t i=0; i<newInst.m_recs.size(); i++)
   //   {
   //   //for now, just add simplevars
   //   AddOutputVar(newInst.m_recs[i].m_modelName.c_str(),newInst.m_recs[i].m_pDo,"");
   //   }
   }

/** Synchronize changes in values between OME and Envision.
	@param row The row of the value to synchronize.
	@param col The column of the value to synchronize.
	@param val The value to apply.
	@param extra Pointer to additional information passed in when function is registered with SimManager.
*/
void OMEProcess::SyncDeltas(const int row, const int col, const OME_SCALAR val,void* extra)
   {
   OMEProcess* pProcess=(OMEProcess*)extra;

   pProcess->AddDelta(pProcess->GetLastContext(),row,col,(float)val);
   }

/** Interpret OME messages and print to Envision's output.
	@param message The message to print.
	@param noEnd if true, add a newline character to the end of the message.
	@param type The type of message.
*/
void OMEProcess::DebugPrint(const TCHAR* message, bool noEnd,const OMEDBG_TYPE type)
   {
   CString msg=CString("OME: ")+message;
   if(noEnd)
	  msg.Append("\n");
   switch(type)
	  {
   case DBG_INFO:
	  Report::LogMsg( msg );
	  break;
   case DBG_WARN:
	  Report::WarningMsg(msg);
	  break;
   case DBG_ERR:
	  Report::ErrorMsg(msg);
	  break;
   case DBG_CRIT:
	  Report::FatalMsg(msg);
	  break;
	  };
   }

void OMEProcess::EventAction(const CentralEvent & ev, void* extra)
   {
   if(ev.GetEventType()==CED_ERR)
	  {
	  ErrCollection* errs=(ErrCollection*)ev.GetPayload();
	  if(errs)
		 {
		 OMEDBG_TYPE type;
		 CString msg;
		 EnvContext* pContext=(EnvContext*)extra;

		 bool goLevel = true;

		 for(ErrItr itr=errs->GetStart(); itr!=errs->GetEnd(); ++itr)
			{
			switch((*itr).m_errType)
			   {
			case EC_HEADER:
			case EC_GROUP:
				goLevel = m_reportLevel <= ALL;
			   type=DBG_INFO;
			   break;
			case EC_WARNING:
				goLevel = m_reportLevel <= WARN;
			   type=DBG_WARN;
			   break;
			case EC_CRITICAL:
			   type=DBG_ERR;
			   break;
			   };

			if (goLevel)
			   {
			   msg = CString();
			   if (pContext)
			   	msg.AppendFormat("EnvContext %i: ", pContext->id);
			   if ((*itr).m_pErrObj)
			   	msg.AppendFormat("%s: ", (*itr).m_pErrObj->GetName().c_str());
			   msg.Append((*itr).m_errMsg.c_str());
			   
			   DebugPrint(msg, false, type);
			   }
			}
		 }
	  }
   else if (m_reportLevel == ALL)
      {
      if (ev.GetEventType() == CED_INCREMENT)
         {
         DBG_PRINT("Increment at: " + std::to_string(ev.GetEventTime()));
         }
      }
   }
	
/** Envision Autonomous Process function. Called on request for preference dialog.
@param pContext Pointer to the active EnvContext.
@param hWnd The parent window for the spawned dialog.
@return true if invocation was successful; false otherwise.
*/
BOOL OMEProcess::Setup(EnvContext *pContext, HWND hWnd)
   {
   OMEAInst currInst;
   if (m_instMap.Lookup(pContext->id, currInst))
      {
      OMEPrefDLog(currInst.m_pMgr,this).DoModal();
      }
   return TRUE;
   }