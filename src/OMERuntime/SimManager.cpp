#include <iomanip>
#include "SimManager.h"
#include "events/CentralEventDispatcher.h"
#include "events/OMEEvent.h"
#include "OMEDefines.h"
#include "DebugOutput.h"
#include "LuaOMEParser.h"
#include "CompiledMdlParser.h"
#include "Units.h"

#include "HiResIntegrationBlock.h"

#if !defined WIN32 && !defined _WIN64
#include <dlfcn.h>
#endif

/** Default / simple constructor.
	@param isSingleton If true, new instance is designated the new BaseManager singleton.
*/
SimManager::SimManager(const bool isSingleton) 
	: BaseManager(isSingleton),m_extra(NULL), m_writeInitBatch(false),m_writeEvalBatch(false),m_cachePath(STLString()),m_useFileCache(false),m_serializer(NULL),
	m_varParamFunc(DefaultInitParamFn), m_varParamExtra(NULL), m_reInitIntegrator(false), m_noRecord(false), m_incToBounds(false),m_solveType(DEFAULT)
{
	m_pIntegrator = new OMEIntegrationBlock();
	m_svDependents=EvalSet(CompareEvalObject);
	RegisterDefaultListeners();
	
	GenSDPFromLib = NULL;
	DisposeSDPFromLib = NULL;
	GetSDPLabelFromLib = NULL;
}

/** Detailed constructor.
		@param modelPath Path to the XML model file to load.
		@param start Time step at which the simulation starts.
		@param stop Time step at which the simulation stops.
		@param interval Period in between time steps.
		@param stepSize The size of each temporal step during integrations.
		@param flags Behavior modifier flags.
	*/
SimManager::SimManager(STLString modelPath,OME_SCALAR start, OME_SCALAR stop, OME_SCALAR interval,OME_SCALAR stepSize, unsigned int flags)
:BaseManager(flags & IS_SINGLETON), m_extra(NULL), m_writeInitBatch(false), m_writeEvalBatch(false), m_useFileCache(false), m_serializer(NULL),
m_varParamFunc(DefaultInitParamFn), m_varParamExtra(NULL), m_reInitIntegrator(false), m_noRecord(flags & NO_FULL_RECORD), m_incToBounds(false),m_solveType(DEFAULT)
{
	if (flags & NO_PRINT)
		DBG_DISABLE();
	m_pIntegrator = new OMEIntegrationBlock();
	m_svDependents=EvalSet(CompareEvalObject);
	RegisterDefaultListeners();
   m_pRootModel=NULL;
   oSTLSStream outStrm;
   
    GenSDPFromLib = NULL;
    DisposeSDPFromLib = NULL;
    GetSDPLabelFromLib = NULL;
    
   //open xml file
   TI2Document doc;
	bool ok = doc.LoadFile( modelPath.c_str() )==tinyxml2::XML_SUCCESS;
   
	if ( ok )     
	{
		  //initialize other args
		  m_startTime=start;
		  m_currentTime=m_startTime;
		  m_stopTime=stop;
		  m_interval=interval;
		  m_stepSize=stepSize;

		TI2Element *pXmlRoot = doc.RootElement();
		PopulateFromXMLTree(pXmlRoot,modelPath,flags,outStrm);
		
   }
	else
	{
		outStrm<<"Err Loading xml model: "<<XMLLoadable::ErrDescription(doc.ErrorID())<<": "<<doc.GetErrorStr1();
		if(doc.GetErrorStr2())
			outStrm<<", "<<doc.GetErrorStr2();
	}
		
	FindMCs(m_pRootModel, m_MCList);
		//DBG_PRINT_FLAGGED(doc.ErrorDesc(),DBG_CRIT);
	
	if(!outStrm.str().empty())
		ReportError(outStrm.str());
		
	FlushErrors(m_errors,0.0);


}

/** Detailed constructor.
@param pInterface Pointer to Compiled form of model.
@param cachePath Path to directory to contain temporary record cache files.
@param start Time step at which the simulation starts.
@param stop Time step at which the simulation stops.
@param interval Period in between time steps.
@param stepSize The size of each temporal step during integrations.
@param flags Behavior modifier flags
*/
SimManager::SimManager(CompiledInterface* pInterface, const STLString & cachePath,OME_SCALAR start, OME_SCALAR stop, OME_SCALAR interval, OME_SCALAR stepSize, unsigned int flags)
:BaseManager(flags & IS_SINGLETON), m_extra(NULL), m_writeInitBatch(false), m_writeEvalBatch(false), m_useFileCache(false), m_serializer(NULL),
m_varParamFunc(DefaultInitParamFn), m_varParamExtra(NULL), m_reInitIntegrator(false), m_noRecord(flags & NO_FULL_RECORD), m_incToBounds(false),m_solveType(DEFAULT)
{
	if (flags & NO_PRINT)
		DBG_DISABLE();
	m_pIntegrator = new OMEIntegrationBlock();
	m_svDependents = EvalSet(CompareEvalObject);
	RegisterDefaultListeners();
	m_pRootModel = NULL;
	oSTLSStream outStrm;
    
    
    GenSDPFromLib = NULL;
    DisposeSDPFromLib = NULL;
    GetSDPLabelFromLib = NULL;

		//initialize other args
		m_startTime = start;
		m_currentTime = m_startTime;
		m_stopTime = stop;
		m_interval = interval;
		m_stepSize = stepSize;


		CompiledMdlParser* pCPrsr = new CompiledMdlParser();
		if (pCPrsr->Init(pInterface, this))
		{
			//serialization and prioritization is part of the compiling process
			flags |= NO_SERIALIZING;
			m_prioritized = true;

			m_pEvalParser = pCPrsr;
			m_pRootModel = ((CompiledMdlParser*)m_pEvalParser)->GetRootModel();
			m_pRootModel->SetManager(this);
		}
		else
			outStrm << "Compiled init failed." << endl;
	//DBG_PRINT_FLAGGED(doc.ErrorDesc(),DBG_CRIT);

		if (m_pRootModel)
		{
			m_pRootModel->GetEvalParser();
			UnpackComponents();
			m_pRootModel->PrioritizeObjects(1, true);
			m_serializer = m_pEvalParser->GenerateSerializer();
			
			if (!(flags & NO_PARSING))
			{
				m_pEvalParser->DefineAll(m_pRootModel);
				m_pEvalParser->AssociateWithSerializer(m_serializer);
			}
			SetScriptDebugging(!(flags & NO_DEBUG));

			m_cachePath = cachePath;
			FindMCs(m_pRootModel, m_MCList);
		}

	if (!outStrm.str().empty())
		ReportError(outStrm.str());

	FlushErrors(m_errors, 0.0);

}

/** Fill in manager details from a TinyXml2-defined xml tree.
	@param pXmlRoot The root node of the xml tree.
	@param modelPath Path to the model file.
	@param flags SimManager behavior modifier flags.
	@param errStrm String stream to write any parsing errors to.
*/
void SimManager::PopulateFromXMLTree(TI2Element* pXmlRoot,const STLString & modelPath,unsigned int & flags,oSTLSStream & errStrm)
{
	bool useCompiled = false;
	STLString rootTag=pXmlRoot->Value();
	if(rootTag=="ome_ctrl")
	{
		TI2Element *pXmlMdlPath=pXmlRoot->FirstChildElement("ome_model");
		if(pXmlMdlPath)
		{
			TI2Document mdlDoc;
			STLString subPath = pXmlMdlPath->Attribute("filepath");
			STLString newPath = PrepPath(subPath, modelPath);
			if (mdlDoc.LoadFile(newPath.c_str()) == tinyxml2::XML_SUCCESS)
			{ 
				const char* temp=pXmlMdlPath->Attribute("compiled_name");
				STLString compiledPath;
				if(temp)
					compiledPath=temp;
				if(!(flags & NO_COMPILED) && !compiledPath.empty())
				{
					DBG_PRINTC("Using Compiled", DBG_YELLOW);
					CompiledMdlParser* pCPrsr=new CompiledMdlParser();
					if((useCompiled=pCPrsr->Init(PrepPath(compiledPath,modelPath),this)))
					{
						//serialization and prioritization is part of the compiling process
						flags |= NO_SERIALIZING;
						m_prioritized = true;

						m_pEvalParser=pCPrsr;
						m_pRootModel=((CompiledMdlParser*)m_pEvalParser)->GetRootModel();
						m_pRootModel->SetManager(this);
						if (!(flags & NO_OVERRIDES))
							ReadModelOverridesXML(pXmlMdlPath);
						//optionally override model bits
						ReadParamXML(mdlDoc.RootElement(), modelPath);
						if (!(flags & NO_OVERRIDES))
							ReadParamXML(pXmlRoot, modelPath);
					}
					else
					{
						DBG_PRINTC("Compiled not found; using Interpreted.", DBG_YELLOW);
						delete pCPrsr;
					}
				}

				if(!useCompiled)
				{
				
					if(!subPath.empty())
					{
                         newPath=PrepPath(subPath,modelPath);
                        if(mdlDoc.LoadFile(newPath.c_str())==tinyxml2::XML_SUCCESS)
						{
							TI2Element *pXmlNewRoot=mdlDoc.RootElement();
							if(pXmlNewRoot)
							{
								//load model first as usual
								ReadModelXML(pXmlNewRoot,newPath);
								if(!(flags & NO_OVERRIDES))
									ReadModelOverridesXML(pXmlMdlPath);
								//optionally override model bits
								ReadParamXML(pXmlRoot,modelPath);
							}
							else
								errStrm<<"Model element not found in file "<<newPath;
						}
						else
						{
							errStrm<<"Err Loading xml model: "<<XMLLoadable::ErrDescription(mdlDoc.ErrorID())<<": "<<mdlDoc.GetErrorStr1();
							if(mdlDoc.GetErrorStr2())
								errStrm<<", "<<mdlDoc.GetErrorStr2();
						}
					}
				}
				if(errStrm.str().empty())
				{
					if(!(flags & NO_COVERAGE))
					{
						//create a new instance struct
						TI2Element *pXMLCoverageInfo = pXmlRoot->FirstChildElement("coverage_mapping");
						ReadCoverageMappingXML(pXMLCoverageInfo);

						if(!(flags & NO_SDP))
						{
							ReadSDPXML(pXmlRoot, modelPath);
						}

					}

					//look for solver
					TI2Element *pXmlSolver=pXmlRoot->FirstChildElement("solver");
					if(pXmlSolver)
					{
						STLString method(ToLowerCase(pXmlSolver->Attribute("method")));
						std::transform(method.begin(),method.end(),method.begin(),::tolower);
						if(method=="euler")
							m_intMethod=OMEIntegrationBlock::IM_EULER;
						else if(method=="rk4")
							m_intMethod=OMEIntegrationBlock::IM_RK4;

						//select specific solver (optional)
						const OMEChar* iType = pXmlSolver->Attribute("type");

						if (iType)
						{
							STLString typeStr = STLString(iType);
							std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);
							SOLVER_TYPE sType = DEFAULT;
							if (typeStr == "hi_res")
								sType = HIRES;
							SetSolverType(sType);
						}

					}
                }
            }
            else
                errStrm<<"No filepath provided in ome_model tag in file "<<modelPath;
        }
        else
            errStrm<<"No ome_model tag in file "<<modelPath;
    }
    else
        ReadModelXML(pXmlRoot,modelPath);

    if (m_pRootModel)
    {
		if(!useCompiled)
			UnpackComponents();
        if (m_pEvalParser->RequiresRuntimePrioritization())
            m_pRootModel->PrioritizeObjects(1, true);
        m_serializer = m_pEvalParser->GenerateSerializer();
        if (!(flags & NO_PARSING))
        {
            m_pEvalParser->DefineAll(m_pRootModel);
            m_pEvalParser->AssociateWithSerializer(m_serializer);
        }

        if (!(flags & NO_DEBUG))
            ReadDBGXML(pXmlRoot, modelPath);
        if (!(flags & NO_SERIALIZING))
            UpdatePriorities();

        m_cachePath = modelPath;
    }


}

/** Default destructor. */
SimManager::~SimManager(void)
{
	if(m_pRootModel && !m_keepRecords)
		m_pRootModel->Cleanup();
	delete m_serializer;
	delete m_pIntegrator;
}

/** Copy another SimManager.
	@param sm The SimManager to copy.
*/
void SimManager::Duplicate(const SimManager & sm)
{

	BaseManager::Duplicate(sm);

	delete m_pIntegrator;
	m_pIntegrator=sm.m_pIntegrator->Clone();

	m_writeInitBatch=sm.m_writeInitBatch;
	m_writeEvalBatch=sm.m_writeEvalBatch;
	m_initBatchPath=sm.m_initBatchPath;
	m_evalBatchPath=sm.m_evalBatchPath;

	if(m_pRootModel)
	{
		SVArray stateVars;
		EvalArray evals;
		//m_pRootModel->GetStateVars(stateVars);
		//m_pRootModel->GetAllEvals(evals);
		m_svModels=sm.m_svModels;
		InitIntegrator();
	}
}

bool SimManager::InitSim()
{
	DBG_PRINTELAPSED("SM: InitSim()");

	bool ret=false;
	m_eventDispatcher->ClearEvents();
	//declared in BaseManager

	m_staticValues.UseMinRecord(m_noRecord);
	m_dynamicValues.UseMinRecord(m_noRecord);

	m_errors.ClearErrs();

	if( PrepSimulation())
	{
		m_eventDispatcher->ClearListeners();
		m_eventDispatcher->RegisterEventListener(CED_PAUSE, this);
		//m_eventDispatcher->RegisterEventListener(CED_INIT,(BaseModel*)m_pRootModel);
		//start the event dispatcher

		//InitIntegrator();
		//if the previous run was paused, or the simulation has been initialized, run simulation.
		
		m_currentTime = m_startTime;
		CentralEvent pauseEvent(this, NULL, CED_PAUSE, m_currentTime + m_interval, 0.0, 0, NULL);
		m_eventDispatcher->PostEvent(pauseEvent);
		m_eventDispatcher->RunDispatcher(m_currentTime + m_interval, m_stopTime, m_interval);

		//load batch update script from serializer
		m_pEvalParser->PushModeID("Update");
		m_pEvalParser->PushBatch(m_serializer->GetEvalStatements().c_str());
		//((LuaOMEParser*)m_pEvalParser)->EndMemCount();
		//do first increment; may want to move to runSimulation()
		//CentralEvent initialIncEvent(this,NULL, CED_INCREMENT, m_startTime, 0.0,1,NULL);
		//m_eventDispatcher->DispatchEvent(initialIncEvent);
		//m_eventDispatcher->PostEvent(initialIncEvent);
		ret=m_isRunning=true;	//let others know we are no longer in initialization
	}
	else //display/capture any errors
		m_eventDispatcher->FlushEvents();

	return ret;
}

bool SimManager::PrepSimulation()
{
	bool ret=false;
	DBG_PRINTELAPSED("PrepSimulation");

	if(m_pRootModel)
	{
		//m_pEvalParser->Reset(m_pRootModel)

		m_pEvalParser->PrepTempDataHandler();

		//initialize any SDP->component relationships using params set in subentries instead
		/*for (auto itr = m_sdpEntries.begin(); itr != m_sdpEntries.end(); ++itr)
			(*itr).second.ApplyEntries(SDPMapSubEntry::INIT_COMPONENT);
*/
      //run user-param initializer func here,
      EvalArray askEvals;
      m_pRootModel->GetAllEvals(askEvals, DT_FULL | DT_ASK);
      m_varParamFunc(askEvals, m_varParamExtra);

		BreakSet breaks;
			//run initializations as batch
		//#ifdef _DEBUG
			m_serializer->PopulateBreaks(breaks,true);
			m_pEvalParser->SetBreaks(breaks);
		//#endif
			m_currentTime = m_startTime;
			m_pEvalParser->PushModeID("Init");

			m_pEvalParser->PushBatch(m_serializer->GetInitStatements().c_str());
			//((LuaOMEParser*)m_pEvalParser)->BeginMemCount();
				m_pEvalParser->BatchEvaluate();
			m_pEvalParser->PopBatch();
			m_pEvalParser->PopModeID();
			breaks.clear();

			//initialize any component->SDP relationships
			for (auto itr = m_sdpEntries.begin(); itr != m_sdpEntries.end(); ++itr)
				(*itr).second.ApplyEntries(SDPMapSubEntry::INIT_SDP);

			
		//#ifdef _DEBUG
			m_serializer->PopulateBreaks(breaks,false);
			m_pEvalParser->SetBreaks(breaks);
		//#endif

			//lock any value that has been initialized by a Param
			EvalArray evals;
			m_pRootModel->GetAllEvals(evals, DT_FULL);
			for (auto itr = evals.begin(); itr != evals.end(); ++itr)
			{
				if ((*itr)->GetInitializer() && !(*itr)->IsOMEType(OME_TSVAR))
					(*itr)->SetIsConstant(true);
			}

			m_staticValues.RefreshInds();
			m_dynamicValues.RefreshInds();

		m_startedIntegrate=false;

		ret=m_errors.GetCriticalCount()==0;
		if (ret)
		{
			m_eventDispatcher->PostEvent(OMEEvent(m_pRootModel, EV_MD_INIT, m_startTime, this, OMEEB_INIT));

			//for now, snap full state
			//m_staticValues.RecordFullState(m_startTime);
			//m_dynamicValues.RecordFullState(m_startTime);
			m_staticValues.RecordState(m_startTime);
			m_dynamicValues.RecordState(m_startTime);
			if (m_resFunc)
				m_resFunc(m_startTime, this, m_mgrRprtExtra);


			//CentralEvent recordEvent = CentralEvent(this, NULL, EV_MD_RECORD, m_startTime, 0.0, 1, NULL);
			//m_eventDispatcher->PostEvent(recordEvent);

			m_isPrepped = true;
		}
		else
			FlushErrors(m_errors, 0.0);

	}
	return ret;
}

/** Run the model simulation between the previously set start and end times. */
void SimManager::RunSimulation()
{
	bool goCode = m_eventDispatcher->GetLastRunCode() == CentralEventDispatcher::CEDR_PAUSE;
	try
	{

		if (!goCode && m_pRootModel)
		{
			InitIntegrator();
			goCode = InitSim();
		}

		if (!m_prioritized)
		{
			goCode = false;
			DBG_PRINT_FLAGGED("Objects not prioritized", DBG_ERR);
		}
		//if the previous run was paused, or the simulation has been initialized, run simulation.
		if (goCode)
		{
			//m_isRunning=true; //should be set in init sim
			m_eventDispatcher->RunDispatcher(m_startTime, m_stopTime, m_interval);
		}

	}
	catch (OMEException & e)
	{
		ReportError(STLString("Error during Model update: ") + e.what());
		FlushErrors(m_errors,GetPresentTime());
	}
}

void SimManager::RegisterModel(Model* pMdl)
{
	EvalArray allDown;
	
	if (pMdl->GetStateVarCount())
	{
		m_svModels.push_back(pMdl);
		if (m_isRunning)
		{
			//We must resize everything
			InitIntegrator();
		}
	}
}

void SimManager::MarkVariableForTracking(Evaluable* pEval)
{
	if (!m_staticValues.AddToMinTracking(pEval))
		m_dynamicValues.AddToMinTracking(pEval);
}

/** Initialize the internal OMEIntegrationBlock for a simulation run.*/
void SimManager::InitIntegrator()
{
	SclrPtrArray vals;
	Model *pMdl;
	StateVar* pSV;
	unsigned int i,sv,sc;
	
	//if init flag is set, clear it.
	m_reInitIntegrator = false;

	for (auto itr = m_svModels.begin(); itr != m_svModels.end(); ++itr)
	{
		pMdl=(*itr);

		ValueArchive& archive = *pMdl->GetValueArchive();
		for (sv = 0; sv < pMdl->GetStateVarCount(); ++sv)
		{
			pSV = pMdl->GetStateVar(sv);
			size_t slotCount = pSV->GetRequestedSlotCount();
			for (i = 0; i < pMdl->GetNumRawInstances(); ++i)
			{
				for (sc = 0; sc < slotCount;++sc)
					vals.push_back(&archive[pSV->GetModelIndex() + (i*slotCount)+sc]);
			}
		}
	}
	try 
	{
		m_pIntegrator->Init(m_intMethod,(int)vals.size(),m_stepSize);
	}
	catch(std::bad_alloc & ba)
	{
		ReportError("OMEIntegrationBlock: Out of memory allocating State Variable arrays",NULL,EC_CRITICAL,true);
	}
	//all mappings lost; must re-register all statevars.
	for(i=0; i < vals.size(); i++)
		m_pIntegrator->SetStateVar(vals[i], i);
	//UpdatePriorities();
}

void SimManager::UpdatePriorities()
{
	SortByPriority(m_svDependents);
	DBG_PARTIAL_PRINT("Serializing Model...");
	m_serializer->SerializeEval(m_pRootModel);
	DBG_PRINT("Done");

#ifdef OME_DBG_BATCH
	DBG_PRINT(m_serializer->GetInitStatements(true));
	DBG_PRINT("");
	DBG_PRINT(m_serializer->GetEvalStatements(true));
#endif

	//assume lua serializer
	if(m_writeEvalBatch && m_evalBatchPath.empty())
		m_evalBatchPath="evalBatch.lua";
	if(m_writeInitBatch && m_initBatchPath.empty())
		m_initBatchPath="initBatch.lua";
	m_serializer->WriteSerialExpressions(m_initBatchPath,m_evalBatchPath);

	m_prioritized=true;
#ifdef OME_DBG_PRIORITIZE
	oSTLSStream dbgOut;
	DBG_SETTXCOLOR(DBG_YELLOW);
	dbgOut<<"FULL ORDER"<<endl;
	StrList ids;
	for(auto itr=m_svDependents.begin(); itr!=m_svDependents.end(); ++itr)
	{
		if(std::find(ids.begin(),ids.end(),(*itr)->m_id)==ids.end())
		{
			dbgOut<<(*itr)->GetName()<<"::"<<(*itr)->m_initPriority<<"::"<<(*itr)->m_evalPriority<<endl;
			ids.push_back((*itr)->m_id);
		}
	}
	DBG_PRINT(dbgOut.str());

	DBG_SETTXCOLOR(DBG_DEFAULT);


#endif

}

void SimManager::EventAction(const CentralEvent & ev, void* extra)
{
	STLString eType=ev.GetEventType();

	OME_SCALAR currTime = ev.GetEventTime();
	if (eType == CED_PAUSE)
	{
		//if pause is between intervals, and incremental step is enabled.
		if (m_incToBounds)
		{
			OME_SCALAR modStep = OMEFMOD(currTime - m_currentTime, m_interval);
			if (modStep > 1e-30) //step error 
			{
				eType = CED_INCREMENT; //perform inc action up to boundary.
			}
		}

	}
	if(eType==CED_INCREMENT)
	{
		//do reports
		//if(m_resFunc)
		//	m_resFunc(currTime,this);

		//if(eType==CED_INCREMENT)
		//{
			//m_eventDispatcher->DumpQueueToConsole();
			//if(currTime+m_interval<m_stopTime)
			//{
			//m_eventDispatcher->DispatchEvent(CentralEvent(this,NULL,EV_MD_RECORD,currTime,0.0,CE_TOP_PRIORITY,NULL));
				//ensure that the model is in the current timestep.

				//so here's the deal:
				//support objects should be updated first,
				//then reported on
				//then the integration should occur,
				//then submodels record their states.
				//this matches simile ordering, more or less
				//may need to update for Envision.

				if (m_reInitIntegrator)
					InitIntegrator();
				//DBG_PRINT("Curr Integration Time: " + std::to_string(currTime));
				OME_SCALAR currInterval = currTime - m_currentTime;
				m_pIntegrator->Integrate(currTime-currInterval, currTime, GetDerivatives, this,PostIntegrateStep,this);
				//UpdateSupportObjects(currTime); //updates should be called at the end of GetDerivatives
				m_currentTime = currTime;
				m_staticValues.RecordState(currTime);
				m_dynamicValues.RecordState(currTime);
				if (m_resFunc)
					m_resFunc(currTime, this, m_mgrRprtExtra);


				//CentralEvent recordEvent=CentralEvent(this,NULL,EV_MD_RECORD,currTime,0.0,CE_TOP_PRIORITY,NULL);
				//m_eventDispatcher->DispatchEvent(recordEvent);

				m_startedIntegrate=false;

			//first update done outside integrator so spawners can do their thing
				//UpdateSupportObjects(currTime);
			//}
		//}
	}
	else if(eType==CED_ERR)
	{
		ErrCollection* errMsgs=(ErrCollection*)ev.GetPayload();
		FlushErrors(*errMsgs,ev.GetEventTime());
	}
	else if(eType==CED_END)
	{
		/*OME_SCALAR repTime = ev.GetEventTime() - m_interval;
		m_pRootModel->RecordState(repTime);
		if (m_resFunc)
			m_resFunc(repTime, this, m_mgrRprtExtra);*/
		m_isRunning=false;
		//remove simManager-specific params
//		for(int i=0; i< m_simParamArray.size(); i++)
//			m_pRootModel->RemoveParameter(&m_simParamArray[i]);

	  //  CentralEvent endRecordEvent=CentralEvent(this,NULL,EV_MD_RECORD,ev.GetEventTime()-m_interval,0.0,CE_TOP_PRIORITY,NULL);
		//m_eventDispatcher->DispatchEvent(endRecordEvent);
		m_staticValues.SetRetainRecords(m_keepRecords);
		m_dynamicValues.SetRetainRecords(m_keepRecords);
	}
}

/** Default function for initializing parameter objects.
	@param evals Collection of Parameter Evaluables to process.
	@param extra Required argument for function signature; ignored.
*/
void SimManager::DefaultInitParamFn(EvalArray & evals, void* extra)
{
	Evaluable* pEval;
	Variable* pVar;
	OME_SCALAR val;
	Model* pMdl;
	size_t mInd;
	
	//for now, only support Variable types with min and max set.
	for (size_t i = 0; i < evals.size(); i++)
	{
		pEval = evals[i];
		if (pEval->IsOMEType(OME_VARIABLE) && pEval->GetOMEType()!=OME_VARARRAY && !pEval->GetInitializer() && pEval->GetInitExpr().empty())
		{
			pVar = (Variable*)pEval;
			val = 0.0;
			if (pVar->GetMinMaxLimit() == Variable::HAS_MINMAX)
				val = (pVar->GetMax() + pVar->GetMin()) / 2.0;
			pMdl = pVar->GetParentModel();
			ValueArchive& archive = *pMdl->GetValueArchive();
			mInd = pVar->GetModelIndex();
			
			if (pVar->IsAsInt())	//truncate if rep is integer
				val = (int)val;
			//distribute mean value across all instances.
			for (size_t j = 0; j < pMdl->GetNumRawInstances(); j++)
				archive[mInd+j]=val;
			if (pVar->GetOMEType() == OME_TSVAR)
				((TimeSeriesVar*)pVar)->SetFixed(val);
		}
	}
}

/** Derivative function used by Integration Blocks
	@param time the current simulation time.
	@param timestep the size of the current time step
	@param svCount the number of registered state variables
	@param deriveArray the array that will be populated by the derivatives for each statevar; should be size of svCount.
	@param extra additional user provided information; in this case, it is a pointer to the currently executing model.
*/
void SimManager::GetDerivatives( OME_SCALAR time, OME_SCALAR timestep, int svCount, OME_SCALAR *deriveArray, void* extra )
{

	SimManager* mngr=(SimManager*)extra;
	//update time (we can access the private variable since this static function is part of the class)
	
			
	int i,j;

	//update support variables
	//if(mngr->m_startedIntegrate)
	//	mngr->UpdateSupportObjects(time);
	//else
	//	mngr->m_startedIntegrate=true;

	Model* pMdl;
	ModelInstance* inst;
	size_t ind=0;
	//use indices and each instance to calculate derivatives.
	for(auto itr=mngr->m_svModels.begin(); itr!=mngr->m_svModels.end();++itr)
	{
		pMdl=(*itr);
		
		for (j = 0; j < pMdl->GetStateVarCount(); j++)
		{
			for (i = 0; i < pMdl->GetNumRawInstances(); i++)
			{
				inst = pMdl->GetRawInstance(i);
				ind = inst->CalcDerivative(pMdl->GetStateVar(j), i,deriveArray,ind);
			}
		}
	}

	// update objects post-integration
	mngr->m_startedIntegrate = true;


}

/** Step to take immediately after an integration step in OMEIntegrationBlock.
	@param time The present time for the integration step.
	@param timestep The span of time for the integration step.
	@param extra Pointer to additional data; in this case, a pointer to the calling SimManager object.
*/
void SimManager::PostIntegrateStep(OME_SCALAR time, OME_SCALAR timestep, void* extra)
{
	SimManager* mngr = (SimManager*)extra;
	mngr->m_currentTime = time + timestep;
	mngr->UpdateSupportObjects(time + timestep);
}

void SimManager::UpdateSupportObjects(const OME_SCALAR time)
{
	//update support variables

	//run batch
	clock_t start=clock();
	DBG_PRINTELAPSED("BEGIN_BATCH");
	//((LuaOMEParser*)m_pEvalParser)->ActivatePool();
	m_pEvalParser->BatchEvaluate();
	//((LuaOMEParser*)m_pEvalParser)->DeactivatePool();
	DBG_PRINTELAPSED("END_BATCH");
	clock_t stop=clock();
	
	//for (int i = 0; i < m_staticValues.GetSlotCount(); ++i)
	//{
	//	if ((i<865401 || i>=914696) && (isnan(m_staticValues[i]) || isinf(m_staticValues[i])))
	//	{
	//		DBG_PRINTC(std::to_string(i)+ STLString(" ")+to_string(time), DBG_RED);
	//		DBG_PRINTC(m_staticValues.DumpInds(), DBG_YELLOW);
	//	}
	//}
	//DBG_PRINTC("Total time for time step " + std::to_string(time) + " (s): " + std::to_string((stop - start) / CLOCKS_PER_SEC), DBG_YELLOW);
	//DBG_COLOR(cout<<"Total time for time step "<<time<<" (s): "<<(stop-start)/CLOCKS_PER_SEC<<endl,DBG_YELLOW);

	//DBG_PRINT(sstrm.str());
}

/** Get a list of objects that can be requested from the simManager.
	@return A list of representations of all objects that can be asked for their values externally.
*/
EvalRepArray SimManager::GetReadableObjectList(const DT_TYPE & flags)
{
   EvalRepArray out;
   if(m_pRootModel)
	  m_pRootModel->GetEvalReps(out,flags);
   return out;
}

/** Get a list of objects that are considered constant and can be used as levers.
	@return A list of representations of all objects that can be numerically modified.
*/
EvalRepArray SimManager::GetConstantObjectList()
{
   EvalRepArray out;
   if(m_pRootModel)
	  m_pRootModel->GetEvalReps(out,DT_FULL|DT_CONSTS_ONLY|DT_NO_AUTOGEN);
   return out;
}

/** Retrieve the value of an object with a given id.
   @param id String value contain the object's unique id (not name).
   @param outVal variable in which to store retrieved value.

   @return true if the value was successfully retrieved and placed in outVal; false otherwise.
*/
bool SimManager::GetReadableValue(STLString id, OME_SCALAR & outVal)
{
   bool ret=false;
   OMEObject* obj=m_pRootModel->ObjectForKey(id);

   Evaluable* evObj=dynamic_cast<Evaluable*>(obj);
   if(evObj && !evObj->GetInternal())
   {
	  outVal=evObj->GetValue();
	  ret=true;
   }

   return ret;
}

/** Retrieve the value of an object with a given id.
   @param rep EvalRep which represents the model object.
   @param outVal variable in which to store retrieved value.

   @return true if the value was successfully retrieved and placed in outVal; false otherwise.
*/
bool SimManager::GetReadableValue(const EvalRep & rep, OME_SCALAR & outVal)
{
   return GetReadableValue(rep.id,outVal);
}

/** Set the value of an object that uses constant values (non-equations).
   @param id String value contain the object's unique id (not name).
   @param inVal The new value for the target object.
   
   @return true if the assignment was successful; false otherwise.
*/
bool SimManager::SetConstantValue(STLString id, const OME_SCALAR inVal)
{
   bool ret=false;
   OMEObject* obj=m_pRootModel->ObjectForKey(id);

   Evaluable* evObj=dynamic_cast<Evaluable*>(obj);
   if(evObj && !evObj->GetInternal() && evObj->GetIsConstant())
   {
	  evObj->SetValue(inVal);
	  ret=true;
   }

   return ret;
}


/** Set the value of an object that uses constant values (non-equations).
   @param rep EvalRep which represents the model object.
   @param inVal The new value for the target object.
   
   @return true if the assignment was successful; false otherwise.
*/
bool SimManager::SetConstantValue(const EvalRep & rep, const OME_SCALAR inVal)
{
   return SetConstantValue(rep.id,inVal);
}

/** Grab address of an object's value field. This can be useful for providing live feedback.
   Note that the pointer is read-only; to set the value use SetConstantValue().

  @param id String with unique identifier of the object.

  @return a read-only pointer to the OME_SCALAR value stored in the object.

*/
const OME_SCALAR* SimManager::GetReadableRef(STLString id)
{
	OMEObject* obj=m_pRootModel->ObjectForKey(id);

   Evaluable* evObj=dynamic_cast<Evaluable*>(obj);
   if(evObj && !evObj->GetInternal())
	  return evObj->GetValueAddr();

   return NULL;
}

/** Grab address of an object's value field. This can be useful for providing live feedback.
   Note that the pointer is read-only; to set the value use SetConstantValue().

  @param rep EvalRep which represents the model object.

  @return a read-only pointer to the OME_SCALAR value stored in the object.

*/
const OME_SCALAR* SimManager::GetReadableRef(const EvalRep & rep)
{
   return GetReadableRef(rep.id);
}

void SimManager::EnableFileCache(const STLString & cachePath)
{
	m_useFileCache=true;
	if(!cachePath.empty())
		m_cachePath=cachePath;
	if(m_cachePath.empty())
	{
		ReportError("SimManager: No file cache path provided. Disabling file cache.",NULL,EC_WARNING);
		m_useFileCache=false;
	}
}

StrList SimManager::GetDefaultEventTypes() 
{
	//would rather use an initializer list and statically allocate ret, but VS2010 does not support that (its C++11, not TR1).
	StrList ret=BaseManager::GetDefaultEventTypes();
	ret.push_back(CED_INCREMENT);
	ret.push_back(CED_ERR);
	ret.push_back(CED_END);
	ret.push_back(EV_MD_REPORT);
	return ret;
}

/** Report any errors that have been reported to the manager but not yet released into the event system.
	@param errs The collection of errors to analyze and report, if necessary.
	@param time The timestamp to place on any generated error events.
*/
void SimManager::FlushErrors(ErrCollection & errs,const OME_SCALAR time)
{
	if(errs.GetCount())
	{
		const OMEChar* buff=errs.ToString();
		DBG_PRINTC(buff,DBG_RED);

		delete[] buff;
		if(errs.GetCriticalCount())
			m_eventDispatcher->PostEvent(CentralEvent(this,NULL, CED_END, time, 0.0,CE_TOP_PRIORITY,NULL));
		errs.ClearErrs();
	}
}

void SimManager::PreloadSubmodels()
{ 
	if(m_pRootModel)
	{
		//ensure that there are no leftover sv models from a previous run
		m_svModels.clear();

		ApplyModelInits(m_pRootModel);
		BaseManager::PreloadSubmodels(m_cachePath);
		//if (!m_inits.IsEmpty())
		
		InitIntegrator();
		m_staticValues.SetSVUpdateFunc(&UpdateStateVarAddrs, this);
		m_dynamicValues.SetSVUpdateFunc(&UpdateStateVarAddrs, this);
		/// \todo add support to update integrator for spawn during run.


		//m_pRootModel->ClearValidations();
		//m_pRootModel->RunValidations();
	}
	else
	{
		OMEFuncOpException("PreloadSubmodels", "No root Model object loaded");
	}
}

/** Recursively apply listed initialization values to models and components.
	@param pRoot Pointer to the root model for initialization application.
*/
void SimManager::ApplyModelInits(Model* pRoot)
{
	STLString mdlPath;
	pRoot->GetUpPath(mdlPath);

	//make sure step size is set
	pRoot->SetStepSize(m_stepSize);

	//perform search to ensure that overlapping inits are applied in 
	//parallel to model heirarchy.
	auto fItr = m_inits.find(mdlPath);
	if (fItr != m_inits.end())
	{
		InitializerSet& inits = (*fItr).second;
		//walk initializations, looking for model entries. Apply top-down
		if (!inits.IsEmpty())
		{
			//values should be assigned to initialization containers, and not the 
			//model components themselves (that comes later).
			inits.ApplyColumnsToModel(pRoot);
			inits.ApplySinglesToModel(pRoot);
		}
	}
	
	for (size_t i = 0; i < pRoot->GetSubmodelCount(); ++i)
		ApplyModelInits(pRoot->GetSubmodel(i));
}

/** Interpret coverage mappings from XML.
	@param pCovElem Pointer to current xml element.
*/
void SimManager::ReadCoverageMappingXML(TI2Element *pCovElem)
{
	while(pCovElem!=NULL)
	{
		//
		/// \todo parsing for root level values.
		//

		TI2Element *pXMLSubCoverage=pCovElem->FirstChildElement("submodel");
		while (pXMLSubCoverage != NULL)
		{
			STLString mdlName;
			bool isName;
			const char* rawStr=NULL;
			if ((rawStr = pXMLSubCoverage->Attribute("id")))
				isName = false;
			if ((rawStr = pXMLSubCoverage->Attribute("name")))
				isName = true;
			if (rawStr)
			{

				TI2Element *pXMLVarInfo = pXMLSubCoverage->FirstChildElement("var");
				if (pXMLVarInfo)
				{
					mdlName = STLString(rawStr);
					SDPMapEntry newEntry(mdlName,isName);

					newEntry.SetInstancePerUnit(pXMLSubCoverage->BoolAttribute("instance_per_unit"));

					while (pXMLVarInfo != NULL)
					{
						SDPMapSubEntry::ENTRY_FLAGS actFlags = SDPMapSubEntry::NONE;

						//sort out init flags
						const char* sdpFlags = pXMLVarInfo->Attribute("init_dir");
						if (sdpFlags)
						{
							STLString initStr(sdpFlags);
							if (initStr == "sdp_to_component")
								actFlags = (SDPMapSubEntry::ENTRY_FLAGS)(actFlags | SDPMapSubEntry::INIT_COMPONENT);
							else if (initStr == "component_to_sdp")
								actFlags = (SDPMapSubEntry::ENTRY_FLAGS)(actFlags | SDPMapSubEntry::INIT_SDP);
						}
						
						//sort out update flags
						sdpFlags = pXMLVarInfo->Attribute("update_dir");
						if (sdpFlags)
						{
							STLString updateStr(sdpFlags);
							if (updateStr=="bidirectional" || updateStr == "sdp_to_component")
								actFlags = (SDPMapSubEntry::ENTRY_FLAGS)(actFlags | SDPMapSubEntry::UPDATE_COMPONENT);
							else if (updateStr == "bidirectional" || updateStr == "component_to_sdp")
								actFlags = (SDPMapSubEntry::ENTRY_FLAGS)(actFlags | SDPMapSubEntry::UPDATE_SDP);
						}

						TI2Element *pXMLInstMap = pXMLVarInfo->FirstChildElement("inst_map");
						
						if (!pXMLInstMap)
						{
							//single entry; read directly
							ReadMappingEntryXML(pXMLVarInfo, newEntry, actFlags);
						}
						else
						{
							//multiEntry-read sub values

							//grab common values first
                            isName=true;
							
							const char* mapTo = pXMLVarInfo->Attribute("mapto_id");
							if (mapTo)
								isName = false;
							else
								mapTo = pXMLVarInfo->Attribute("mapto_name");
								
							STLString nori(mapTo);
							do
							{
								//iterate through inst listings.
								ReadMappingEntryXML(pXMLInstMap, newEntry, nori,isName, actFlags);
								pXMLInstMap = pXMLInstMap->NextSiblingElement("inst_map");
							} while (pXMLInstMap);
						}

						pXMLVarInfo = pXMLVarInfo->NextSiblingElement("var");
					}
					m_sdpEntries[mdlName] = newEntry;
				}
				pXMLSubCoverage = pXMLSubCoverage->NextSiblingElement("submodel");
			}
			///@todo else err
		}

		pCovElem=pCovElem->NextSiblingElement("coverage_mapping");
	}
}

/** Read a specific entry for a coverage mapping.
	@param pXMLVarInfo Pointer to the current XML node to process.
	@param newEntry The SDPMapEntry to add the new SDPMapSubEntry to.
	@param actFlags Bit flags denoting associations to apply to newEntry.
	@return true if pXMLVarInfo points to a well formed node and a new SDPMapSubEntry was added to newEntry; false otherwise.
*/
bool SimManager::ReadMappingEntryXML(TI2Element *pXMLVarInfo, SDPMapEntry & newEntry, const SDPMapSubEntry::ENTRY_FLAGS & actFlags)
{
	bool isName = true;
	if (!pXMLVarInfo->Attribute("sdp_name"))
	{
		//if sdp_name attribute is missing, record issue, and move on to next var.
		ReportError("ReadCoverageMappingEntryXML: var is missing sdp_name attribute", NULL, EC_WARNING);
		//pXMLVarInfo = pXMLVarInfo->NextSiblingElement("var");
		return false;
	}

	STLString sdpName = pXMLVarInfo->Attribute("sdp_name");

	const char* mapTo = pXMLVarInfo->Attribute("mapto_id");
	if (mapTo)
		isName = false;
	else
		mapTo = pXMLVarInfo->Attribute("mapto_name");

	if (!mapTo)
		mapTo=sdpName.c_str();

	newEntry.AddSubEntry(SDPMapSubEntry(mapTo, isName, sdpName, actFlags));
    return true;
}

/** Read a specific entry for a coverage mapping.
	@param pXMLInstInfo Pointer to the current XML node to process.
	@param newEntry The SDPMapEntry to add the new SDPMapSubEntry to.
	@param nameOrId The name or id to use to identify the associated model component.
	@param isName If true, nameOrId contains a human readable name; otherwise, it contains a unique Id.
	@param actFlags Bit flags denoting associations to apply to newEntry.
	@return true if pXMLInstInfo points to a well formed node and a new SDPMapSubEntry was added to newEntry; false otherwise.
*/
bool SimManager::ReadMappingEntryXML(TI2Element *pXMLInstInfo, SDPMapEntry & newEntry, const STLString & nameOrId, const bool & isName, const SDPMapSubEntry::ENTRY_FLAGS & actFlags)
{
	if (!pXMLInstInfo->Attribute("sdp_name"))
	{
		//if sdp_name attribute is missing, record issue, and move on to next var.
		ReportError("ReadCoverageMappingEntryXML: var is missing sdp_name attribute", NULL, EC_WARNING);
		//pXMLVarInfo = pXMLVarInfo->NextSiblingElement("var");
		return false;
	}

	STLString toVal = pXMLInstInfo->Attribute("sdp_name");
	int index = 0;
	pXMLInstInfo->QueryIntAttribute("index", &index);

	newEntry.AddSubEntry(SDPMapSubEntry(nameOrId, isName, index-1, toVal, actFlags));

	return true;
}

/** Interpret parameter file from xml.
	@param pXml Pointer to current xml element.
	@param mPath Filepath to model definition.
*/
void SimManager::ReadParamXML(TI2Element *pXml,const STLString & mPath)
{
	TI2Element *pXmlParamFile= pXml->FirstChildElement("param_file");
	while(pXmlParamFile)
	{
		DBG_PRINT("Loading Parameters from \""+mPath+'"');
		STLString relParamPath=pXmlParamFile->Attribute("filepath");
		STLString targName = m_pRootModel->GetExprName();
		if (pXmlParamFile->Attribute("target_model_path"))
			targName = pXmlParamFile->Attribute("target_model_path");
		m_inits[targName]=InitializerSet(PrepPath(relParamPath,mPath));

		pXmlParamFile=pXmlParamFile->NextSiblingElement("param_file");
	}
}

/** Interpret model definiton from xml.
	@param pXml Pointer to current xml element.
	@param mPath Filepath to model definition.
*/
void SimManager::ReadModelXML(TI2Element *pXml,const STLString & mPath)
{
	DBG_PRINT("Loading from Model File: \""+mPath+'"');
	m_pRootModel=new Model;
	  
		//if we are loading a configuration file instead of a model file, create mngr and duplicate
		
		//load model From xml
	  
		TI2Element *pXmlModel = pXml->FirstChildElement( "model" );
		m_pRootModel->PopulateFromXML(pXmlModel,STLString("model"),mPath);
		//set simmanager here for preload stuff
		m_pRootModel->SetManager(this);
		//init evalParser here for ReadSDPXML
		m_pEvalParser=new EVAL_PARSER(m_pRootModel);
		//check for param file;
		ReadParamXML(pXml,mPath);
		ReadSDPXML(pXml,mPath);
}

/** Construct spatial data provider from xml definition.
	@param pXml Pointer to current xml element.
	@param mPath Filepath to model definition.
*/
void SimManager::ReadSDPXML(TI2Element *pXml,const STLString & mPath)
{

	//optional spatial provider stuff
	TI2Element *pXmlSPFile= pXml->FirstChildElement("spatial_provider");
	if(pXmlSPFile)
	{
		DBG_PRINT("Loading coverage details:");
		DBG_PUSHLEVEL();
	
		//load spatial provider stuff;
		ISpatialDataProvider* pExternalSDP=NULL;
		STLString providerLib=STLString();
		if (pXmlSPFile->Attribute("library_path"))
			providerLib = pXmlSPFile->Attribute("library_path");

		if (!providerLib.empty() && LoadSDPLib(providerLib))
		{
			DBG_PRINT(STLString("Using ") + GetSDPLabelFromLib() + " spatial coverage");

			pExternalSDP = GenSDPFromLib(this, m_pRootModel, pXmlSPFile, mPath.c_str());
			SetSpatialDataProvider(pExternalSDP);

			//notify manager that it is in charge of the provider
			m_SDPisInternal = true;

			if (pExternalSDP)
				m_pEvalParser->LoadSDPFuncs(pExternalSDP);
			else
				ReportError("SDP allocation failed.");
		}
		else
			ReportError("Target SDP Failed to load");
		DBG_POPLEVEL();
	}
}

/** Parse information from xml for debugging info.
	@param pXml Pointer to current pXml element.
	@param mPath Absolute path to model file. Used for relative path references.
*/
void SimManager::ReadDBGXML(TI2Element *pXml,const STLString & mPath)
{

	//optional spatial provider stuff
	TI2Element *pXmlDBGFile= pXml->FirstChildElement("debug");
	if(pXmlDBGFile)
	{
		TI2Element* pBPEntry;
		int bpLine;
		bool bpEnabled;
		const OMEChar* bpPath;
		XmlAttr bPAttrs[] = {
			// attr				type		   address                          isReq  checkCol
			{ "line", XML_INT, &bpLine, true },
			{ "enabled", XML_BOOL, &bpEnabled, false },
			{ NULL, XML_NULL, NULL, false } };
		//breakpoints
		TI2Element* subXml=pXmlDBGFile->FirstChildElement("init_break");
		if (subXml)
		{
			if((bpPath = subXml->Attribute("path")))
				m_serializer->SetInitBreakPath(PrepPath(STLString(bpPath), mPath));
			pBPEntry = subXml->FirstChildElement("bp");
			while (pBPEntry)
			{
				XMLLoadable::ParseAttributes(pBPEntry, bPAttrs, "bp",mPath);
				m_serializer->SetBreak(bpLine, bpEnabled, true);

				pBPEntry=pBPEntry->NextSiblingElement("bp");
			}
		}

		subXml=pXmlDBGFile->FirstChildElement("eval_break");
		if (subXml)
		{
			if ((bpPath = subXml->Attribute("path")))
				m_serializer->SetEvalBreakPath(PrepPath(STLString(bpPath), mPath));
			pBPEntry = subXml->FirstChildElement("bp");
			while (pBPEntry)
			{
				XMLLoadable::ParseAttributes(pBPEntry, bPAttrs, "bp", mPath);
				m_serializer->SetBreak(bpLine, bpEnabled, false);

				pBPEntry = pBPEntry->NextSiblingElement("bp");
			}
		}
		//batch dumps
		subXml=pXmlDBGFile->FirstChildElement("dump_init_batch");
		const OMEChar* tempPath;
		if(subXml)
		{
			m_writeInitBatch=true;
			tempPath=subXml->Attribute("path");
			m_initBatchPath=PrepPath(tempPath ? tempPath : "initBatch.lua",mPath);
		}
		subXml=pXmlDBGFile->FirstChildElement("dump_eval_batch");
		if(subXml)
		{
			m_writeEvalBatch=true;
			tempPath=subXml->Attribute("path");
			m_evalBatchPath=PrepPath(tempPath ? tempPath : "evalBatch.lua",mPath);
		}
	}
}

/** Read in model overrides from the xml from a control file.
	@param pXml Pointer to the root xml element.
*/
void SimManager::ReadModelOverridesXML(TI2Element *pXml)
{
	XMLVALQUERY(pXml, "start_time", &m_startTime);
	XMLVALQUERY(pXml, "stop_time", &m_stopTime);

	if (XMLVALQUERY(pXml,"interval", &m_interval) == tinyxml2::XML_NO_ATTRIBUTE )
		XMLVALQUERY(pXml, "report_interval", &m_interval);
	if (XMLVALQUERY(pXml, "step_size", &m_stepSize) == tinyxml2::XML_NO_ATTRIBUTE)
		m_stepSize=m_interval;
	OME_SCALAR relSS;
	if (XMLVALQUERY(pXml, "relative_step_size", &relSS) != tinyxml2::XML_NO_ATTRIBUTE)
	{
		if(relSS>1.0 || relSS<0.0)
			relSS=1.0;
		m_stepSize*=relSS;
	}

	const OMEChar* unitStr = pXml->Attribute("time_units");
	if (unitStr)
	{
		Units uPrsr=Units(STLString(unitStr));
		UNIT uSpec;
		uPrsr.Parse(uSpec);
		m_timeUnits = uSpec.measure;
	}
}

/** Export all current values to a CSV file.
	@param path The path to the export file.
	@return true if values are successfully exported; false otherwise.
*/
bool SimManager::ExportLoadedValuesToCSV(const STLString & path)
{
	ofstream outFile(path);
	outFile.precision(OME_WRITE_PRECISION);
	bool ret = outFile.good();
	if (ret)
	{
		//grab present values from archive.
		ValueArchive::IDValList entries;
		m_staticValues.GetIDValPairs(entries);
		m_dynamicValues.GetIDValPairs(entries);
		outFile << "Id,Name,Instance,Value" << endl;

		for (auto & idVal : entries)
		{
			if (!idVal.isAutoGen)
				outFile << idVal.id << ',' << idVal.name<<','<<idVal.ind<<','<<idVal.val << endl;
		}
	}
	return ret;
}

/** Export all values across all timesteps to a CSV file.
@param path The path to the export file.
@return true if values are successfully exported; false otherwise.
*/
bool SimManager::ExportAllValuesToCSV(const STLString & path)
{
	bool ret = !m_noRecord;
	if (ret)
	{
		ofstream outFile(path);
		
		ret = outFile.good();
		if (ret)
		{
			//get record details
			ValueArchive::IDValList entries;
			m_staticValues.GetIDValPairs(entries);
			m_dynamicValues.GetIDValPairs(entries);

			OMESclrArray keys = m_staticValues.GetAllRecordedTimes();
			RecordMgr::FullValueMap allVals=m_staticValues.GetAllRecordedValues();
			m_dynamicValues.GetAllRecordedValues(allVals);

			//build header
			outFile << "Id,Name,Instance";
			for (OME_SCALAR & d : keys)
			{
				outFile << ',' << m_startTime+d;
			}
			outFile << endl;
			outFile.precision(OME_WRITE_PRECISION);

			//run through each variable instance and record all values
			size_t mdlInd;
			for (auto & idVal : entries)
			{

				if (!idVal.isAutoGen)
				{
					if (!m_staticValues.IndexForID(idVal.id, mdlInd))
					{
						m_dynamicValues.IndexForID(idVal.id, mdlInd);
						mdlInd += m_staticValues.GetSlotCount();
					}
					mdlInd += idVal.ind;
					outFile << idVal.id << ',' << idVal.name << ',' << idVal.ind;
					for (OME_SCALAR & k : keys)
						outFile << ',' << allVals[k][mdlInd];
					outFile << endl;
				}
				
			}
			outFile << endl;
		}
	}

	return ret;
}

/** Add a user-supplied parameter to a specific model component.
	@param p The Param object to add.
	@return true if The target object for Param was found; false otherwise.
*/
bool SimManager::AddUserParameter(const Param & p)
{
	//assume name contains fully qualified model path
	Evaluable* pObj = dynamic_cast<Evaluable*>(m_pRootModel->NestedObjectForName(p.GetTargetName()));
	if (!pObj)
		return false;

	int ind=pObj->GetParentModel()->AddParameter(p);
	pObj->SetInitializer(&pObj->GetParentModel()->GetParam(ind));
	return true;
}

/** Set the current solver type. Memory associated with newly selected solver is handed over by
	previous solver.
	@param t The type of solver to select for use.
	*/
void SimManager::SetSolverType(const SimManager::SOLVER_TYPE & t)
{
	if (t != m_solveType)
	{
		m_solveType = t;
		OMEIntegrationBlock* pTmp;
		OMEIntegrationBlock* pSingleTmp;
		switch (m_solveType)
		{
		case DEFAULT:
			pTmp = new OMEIntegrationBlock();
			pSingleTmp = new OMEIntegrationBlock();
			break;
		case HIRES:
			pTmp = new HiResIntegrationBlock();
			pSingleTmp = new HiResIntegrationBlock();
			break;
		default:
			return;	//err?
		}

		m_pIntegrator->TransferTo(*pTmp);
		delete m_pIntegrator;
		m_pIntegrator = pTmp;

		
	}
}

//SDP dll loader stuff
/** Attempts to load a dynamic library containing factory functions to generate a new SDP.
	@param libPath Path to the dynamic library, minus the file extension.
	@return true if Dynamic Library is successfully loaded; false otherwise.
*/
bool SimManager::LoadSDPLib(const STLString & libPath)
{
#if defined WIN32 || defined _WIN64
	//assume args is path to dynamic lib containing compiled library.
	CString dllName(libPath.c_str());
	dllName += ".dll";
	HINSTANCE hDLL = AfxLoadLibrary(dllName.AllocSysString());
	if (hDLL == NULL)
	{
		DBG_PRINTC(std::to_string(GetLastError()), DBG_RED);
		return false;
	}
	//bind function calls
	GenSDPFromLib = NULL;
	DisposeSDPFromLib = NULL;
	GetSDPLabelFromLib = NULL;

	GenSDPFromLib = (GenSDPFunc)GetProcAddress(hDLL, "GenSDP");
	DisposeSDPFromLib = (DisposeSDPFunc)GetProcAddress(hDLL, "DisposeSDP");
	GetSDPLabelFromLib = (GetSDPLabelFunc)GetProcAddress(hDLL, "GetSDPLabel");

	if (!GenSDPFromLib || !DisposeSDPFromLib || !GetSDPLabelFromLib)
		return false;

#else
    //other environments here
    //unix/linux/osx
    char* errStr = NULL;
    m_dlHandle = dlopen((libPath + ".dylib").c_str(), RTLD_LAZY);
    if ((errStr = dlerror()))
    {
        DBG_PRINTC(errStr, DBG_RED);
        dlclose(m_dlHandle);
        return false;
    }
    GenSDPFromLib = (GenSDPFunc)dlsym(m_dlHandle, "GenSDP");
    if ((errStr = dlerror()))
    {
        DBG_PRINTC(errStr, DBG_RED);
        dlclose(m_dlHandle);
        return false;
    }
    DisposeSDPFromLib = (DisposeSDPFunc)dlsym(m_dlHandle, "DisposeSDP");
    if ((errStr = dlerror()))
    {
        DBG_PRINTC(errStr, DBG_RED);
        dlclose(m_dlHandle);
        return false;
    }

    GetSDPLabelFromLib = (GetSDPLabelFunc)dlsym(m_dlHandle, "GetSDPLabel");
    if ((errStr = dlerror()))
    {
        DBG_PRINTC(errStr, DBG_RED);
        dlclose(m_dlHandle);
        return false;
    }

    //TODO: ensure factories are exported correctly

#endif

return true;
}

void SimManager::DisposeSDP(ISpatialDataProvider* pSDP)
{
	if (DisposeSDPFromLib)
		DisposeSDPFromLib(pSDP);
	else
		BaseManager::DisposeSDP(pSDP);
}