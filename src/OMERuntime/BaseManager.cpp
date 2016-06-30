#include "BaseManager.h"
#include "events/CentralEventDispatcher.h"
#include "OMEDefines.h"
#include "DebugOutput.h"
#include "omeobjects/SimObjs.h"
//#include "LuaOMEParser.h"

#include <sstream>

namespace MgrSingleton
{
BaseManager* m_pSingleton=NULL;///< Singleton pointer in cases where there is only one Mngr.
};
BaseManager* BaseManager::GetSingleton()
{
	return MgrSingleton::m_pSingleton;
}

#define RESERVE_SIZE 16384

/** Default Constructor. */
BaseManager::BaseManager(const bool isSingleton)
: m_pRootModel(NULL), m_eventDispatcher(&CentralEventDispatcher::GetSingletion()),
m_startTime(0.0), m_stopTime(0.0), m_interval(0.0), m_stepSize(1.0),
m_intMethod(OMEIntegrationBlock::IM_EULER), m_resFunc(NULL), m_pSdp(NULL), m_timeUnits(UM_DAY), m_isRunning(false), m_pEvalParser(NULL),
m_SDPisInternal(false), m_keepRecords(false), m_prioritized(false), m_externalEvalParser(false), m_startedIntegrate(false),
m_visRule("all"), m_isPrepped(false)
{ 
	m_reserveMem= new char[RESERVE_SIZE];
	if(isSingleton)
		MgrSingleton::m_pSingleton=this;
	m_eventDispatcher->RegisterErrCollection(&m_errors);
	//RegisterDefaultListeners(); <--Doesn't do any good here; should be called in child constructor
}

/** Detailed constructor.
		@param rootModel Model to serve as root/entry point for the simulation.
		@param start Time step at which the simulation starts.
		@param stop Time step at which the simulation stops.
		@param interval Period in between time steps.
		@param stepSize The size of each step used during integration.
		@param isSingleton If true, BaseManager is registered as the current singleton object.
*/
BaseManager::BaseManager(Model* rootModel,OME_SCALAR start, OME_SCALAR stop, OME_SCALAR interval, OME_SCALAR stepSize, const bool isSingleton) 
: m_pRootModel(rootModel), m_eventDispatcher(&CentralEventDispatcher::GetSingletion()),
	m_startTime(start), m_stopTime(stop), m_interval(interval), m_stepSize(stepSize),
	m_intMethod(OMEIntegrationBlock::IM_EULER), m_resFunc(NULL),m_pSdp(NULL),m_timeUnits(UM_DAY),m_isRunning(false),m_pEvalParser(NULL),
	m_SDPisInternal(false), m_keepRecords(false), m_prioritized(false), m_externalEvalParser(false), m_startedIntegrate(false),
	m_visRule("all"), m_isPrepped(false)
{ 
	m_reserveMem= new char[RESERVE_SIZE];
	if(isSingleton)
		MgrSingleton::m_pSingleton=this;
	FindMCs(m_pRootModel, m_MCList); 
	m_eventDispatcher->RegisterErrCollection(&m_errors);
	//RegisterDefaultListeners(); <--Doesn't do any good here; should be called in child constructor
}
	
	/** Copy Constructor.
		@param bm The BaseManager to copy.
	*/
BaseManager::BaseManager(const BaseManager & bm) 
: m_eventDispatcher(&CentralEventDispatcher::GetSingletion())
{
	m_reserveMem= new char[RESERVE_SIZE];
	Duplicate(bm); 
}

/** Default destructor. */
BaseManager::~BaseManager(void)
{
	ClaimReserveMem();
	UnregisterDefaultListeners();
	if (!m_externalEvalParser)
		delete m_pEvalParser;
	delete m_pRootModel;
	if(m_SDPisInternal)
		delete m_pSdp;

	if(MgrSingleton::m_pSingleton==this)
		MgrSingleton::m_pSingleton=NULL;
}

/** Copy another BaseManager.
	@param bm The BaseManager to copy.
*/
void BaseManager::Duplicate(const BaseManager & bm)
{
	m_pRootModel=new Model(*bm.m_pRootModel);
	m_resFunc=bm.m_resFunc;
	m_startTime=bm.m_startTime;
	m_stopTime=bm.m_stopTime;
	m_interval=bm.m_interval;
	m_stepSize=bm.m_stepSize;
	m_currentTime=bm.m_currentTime;
	m_intMethod=bm.m_intMethod;
	m_startedIntegrate=bm.m_startedIntegrate;
	/// \todo come up with a clever way to handle copying of internal SDP of unknown type.
	m_SDPisInternal=false;
   m_pSdp=bm.m_pSdp;
   //m_simParamArray.assign(bm.m_simParamArray.begin(),bm.m_simParamArray.end());				///< Parameters that are applied to the model from the outside.
	m_isRunning=bm.m_isRunning;
	m_pEvalParser=bm.m_pEvalParser;
	m_errors=bm.m_errors;
	m_keepRecords=bm.m_keepRecords;
	m_prioritized = bm.m_prioritized;
	m_isPrepped = bm.m_isPrepped;
}

/** Sets the root model.
		@param rmdl Pointer to the model to identify as root.
*/
void BaseManager::SetRootModel(Model* rmdl) 
{ 
	m_pRootModel=rmdl; 
	m_MCList.clear();
	FindMCs(m_pRootModel, m_MCList);
}

//OMEArray sorters
/** Sorts an array of OMEObject pointers by priority.
	@param out The list to sort.
	@param useInitPriority If true, initialization priority is used; otherwise eval priority is used.
*/
void BaseManager::SortByPriority(OMEArray & out, bool useInitPriority)
{
	std::sort(out.begin(),out.end(),(useInitPriority ? OMEObject::CompareInitPriority : OMEObject::CompareEvalPriority));
}

//Evaluable sorters
/** Sorts an array of Evaluable pointers by priority.
	@param out The list to sort.
	@param useInitPriority If true, initialization priority is used; otherwise eval priority is used.
*/
void BaseManager::SortByPriority(EvalArray & out, bool useInitPriority)
{
	std::sort(out.begin(),out.end(),(useInitPriority ? OMEObject::CompareInitPriority : OMEObject::CompareEvalPriority));
}

/** Sorts a set of Evaluable pointers by priority.
	@param out The set to sort.
	@param useInitPriority If true, initialization priority is used; otherwise eval priority is used.
*/
void BaseManager::SortByPriority(EvalSet & out, bool useInitPriority)
{
	/// \todo fix sorting check.
	//sets are implicitly sorted; only resort if sorting criteria have changed
	//bool usingInit=out.key_comp()==&CompareInitObject;
	//if(usingInit!=useInitPriority)
	//{
		//the way to resort is to create a new empty set with the proper comperator and then insert the old set
		EvalSet temp=out;
		out=EvalSet(useInitPriority ? CompareInitObject : CompareEvalObject);
		out.insert(temp.begin(),temp.end());
	//}
}

void BaseManager::EventAction(const CentralEvent & ev, void* extra)
{
	//no response to events on this level
}

///* Add a parameter to be handed in to the Model at runtime.
//	@param parm The param to add.
//	@return false if there already exists a parameter by the same name; true otherwise.
//*/
//bool BaseManager::AddParameter(const Param & parm)
//{
//	bool ret=true;
//
//	//check to see if there are any name collisions. If so, fail.
//	for(int i=0; i< m_simParamArray.size() && ret; i++)
//	{
//		if(parm.GetTargetName()==m_simParamArray[i].GetTargetName())
//			ret=false;
//	}
//
//	if(ret)
//		m_simParamArray.push_back(parm);
//
//	return ret;
//}

/** Find all Submodel Collections.
	@param currModel The current Model to search for Models in.
	@param out The list to store pointers to the found Models.
*/
void BaseManager::FindMCs(Model* currModel,ModelList & out)
{
	if(currModel)
	{
		Model* entry;
		for(size_t i=0; i<currModel->GetSubmodelCount(); i++)
		{
			entry=currModel->GetSubmodel(i);
			out.push_back(entry);

			FindMCs(entry,out);
		}
	}
}

/** Pre-load submodel instances before the full initialization process takes place.
	This is what Envision expects.
*/
void BaseManager::PreloadSubmodels(const STLString & cacheDir)
{ 
	DBG_PRINTELAPSED("Manager: Preload models");
	if(m_pRootModel) 
	{
		//prime any submodels who take their instance counts from the SDP coverage.
		for (auto itr = m_sdpEntries.begin(); itr != m_sdpEntries.end(); ++itr)
			(*itr).second.ApplyIPUToAssociatedModel();

		m_pRootModel->ClearInitialization();
		m_pRootModel->ClearPriorities();
		if (m_pEvalParser->RequiresRuntimePrioritization())
			m_pRootModel->PrioritizeObjects(1,true);
		//m_pRootModel->SyncSMPPriorities();

		m_pRootModel->SetManager(this);
		m_pRootModel->PurgeInfluences();
		//m_pRootModel->PurgeUnusedPorts();
		
		m_pRootModel->PreloadInstances();
		m_staticValues = ValueArchive(m_pRootModel,m_startTime,m_interval, ValueArchive::STATIC,false, cacheDir);
		m_staticValues.ReassignMdlPtrs();
		m_dynamicValues = ValueArchive(m_pRootModel,m_startTime,m_interval,ValueArchive::DYNAMIC,false, cacheDir);
		m_dynamicValues.ReassignMdlPtrs();

		//refresh model indices used in spatial stuff
		// alternate approach: have params assign JIT
		for (auto itr = m_sdpEntries.begin(); itr != m_sdpEntries.end(); ++itr)
			(*itr).second.RefreshEntries();

		//m_staticValues.AllocSpaceForRegisteredModels();
		//m_dynamicValues.AllocSpaceForRegisteredModels();

		m_pRootModel->InitRun(m_startTime,this);
		
	}
}

/** Retrieve records for all managed models and submodels.
	@return An array of Record objects.
*/
//RecordRepArray BaseManager::GetReadableRecords()
//{
//	RecordRepArray ret;
//
//	if(m_pRootModel)
//		m_pRootModel->CollectReadableRecords(ret);
//	return ret;
//}

/** Synchronize model records with simplified record.
	@param records An array of records to synchronize.
*/
void BaseManager::UpdateRecords(RecordRepArray & records)
{
	for(auto itr=records.begin(); itr!=records.end(); ++itr)
		if((*itr).m_pSrcMdl) (*itr).m_pSrcMdl->GetReadableValues((*itr));
}

/** Set the current spatial Provider. Takes effect at the beginning of a new simulation.
@param pSdp Pointer to the SpatialDataProvider to associate with the model.
@param eMap Optional list of entries to extract from the SpatialDataProvider.
@param ownIt If true, pSdp will be deallocated when BaseManager is destroyed;
*/
void BaseManager::SetSpatialDataProvider(ISpatialDataProvider* pSdp,const SubmodelSDPEntryMap & eMap,const bool & ownIt)
{
	m_sdpEntries=eMap;
	SetSpatialDataProvider(pSdp,ownIt);
}

/** Set the current spatial Provider. Takes effect at the beginning of a new simulation.
@param pSdp Pointer to the SpatialDataProvider to associate with the model.
@param ownIt If true, pSdp will be deallocated when BaseManager is destroyed;
*/
void BaseManager::SetSpatialDataProvider(ISpatialDataProvider* pSdp, const bool & ownIt)
{
	if (m_pSdp && m_SDPisInternal)
		DisposeSDP(m_pSdp);
	m_SDPisInternal = ownIt;

	m_pSdp = pSdp;
	Model* pMdl;
	if (m_pRootModel)
	{
		for (auto itr = m_sdpEntries.begin(); itr != m_sdpEntries.end(); ++itr)
		{
			bool isName;
			STLString nori = (*itr).second.GetNameOrId(isName);
			if (isName)
				pMdl = m_pRootModel->SubmodelForName(nori, true);
			else
				pMdl = dynamic_cast<Model*>(m_pRootModel->ObjectForKey(nori));
			
			if (pMdl)
			{
				(*itr).second.AssociateModel(pMdl);
				(*itr).second.AssociateSDP(pSdp,true);
			}
		}

		//register spatial funcs
		InitSDP();
	}
}


/** Synchronize coverage values with flagged model variables. 
	@param syncFunc Optional function to be called for each coverage entry.
	@param pFuncExtra Optional pointer to be passed to syncFunc.
*/
void BaseManager::SyncCoverage(CoverageSyncFunction syncFunc, void* pFuncExtra)
{
	for (auto itr = m_sdpEntries.begin(); itr != m_sdpEntries.end(); ++itr)
	{
		//first, sink any components->to->SDPs;
		if (syncFunc)
			(*itr).second.ApplyEntries(SDPMapSubEntry::UPDATE_SDP, syncFunc, pFuncExtra);
		else
			(*itr).second.ApplyEntries(SDPMapSubEntry::UPDATE_SDP);

		(*itr).second.ApplyEntries(SDPMapSubEntry::UPDATE_COMPONENT);

		if ((*itr).second.HasErrors())
		{
			STLString errHeader;
			if ((*itr).second.GetAssociatedModel())
				errHeader = "SDPMapEntry for " + (*itr).second.GetAssociatedModel()->GetName() + " has the following errors:";
			else
				errHeader = "An SDPMapEntry has reported the following errors:";

			//m_errors should be registered with the central event dispatcher
			m_errors += ((*itr).second.GetErrors(errHeader));

			m_eventDispatcher->BroadcastAnyErrors();
		}
	}
}

/** Initialize the local SpatialDataProvider.
*/
void BaseManager::InitSDP()
{
	m_pEvalParser->LoadSDPFuncs(m_pSdp);
}

/** Register BaseManager or subclass for all events returned by GetDefaultEventTypes().
*/
void BaseManager::RegisterDefaultListeners()
{
	StrList dls=GetDefaultEventTypes();
	std::for_each(dls.begin(),dls.end(),[&](STLString & str){m_eventDispatcher->RegisterEventListener(str,this,NULL,true);});
}

/** Unregister BaseManager or subclass for all events returned by GetDefaultEventTypes().
*/
void BaseManager::UnregisterDefaultListeners()
{
	StrList dls=GetDefaultEventTypes();
	std::for_each(dls.begin(),dls.end(),[&](STLString & str){m_eventDispatcher->RemoveListener(str,this);});

}

/** Deallocate the associated ISpatialDataProvider. */
void BaseManager::DisposeSDP(ISpatialDataProvider* pSDP)
{
	delete pSDP;
}

/** Unpack any Model components who have expressions that require expansion.*/
void BaseManager::UnpackComponents()
{
	m_pRootModel->UnpackModelComponentExpressions();
	m_pRootModel->RefreshObjectMaps(true);
}

/** Compare two objects based on their eval priority. 
	@param lhs The left hand object of the comparison.
	@param rhs The right hand object of the comparison.
	@return true If the lhs eval priority is less than the rhs eval priority; if they are equal, then pointer values are compared, and true is returned if lhs<rhs.
*/
bool BaseManager::CompareEvalObject(const OMEObject* lhs, const OMEObject* rhs)
{
	//if compare priorities; if equal, compare addresses
	return lhs->GetEvalPriority() == rhs->GetEvalPriority() ? lhs->GetID()<rhs->GetID() : OMEObject::CompareEvalPriority(lhs, rhs);
}

/** Compare two objects based on their init priority. 
	@param lhs The left hand object of the comparison.
	@param rhs The right hand object of the comparison.
	@return true If the lhs init priority is less than the rhs init priority; if they are equal, then pointer values are compared, and true is returned if lhs<rhs.
*/
bool BaseManager::CompareInitObject(const OMEObject* lhs, const OMEObject* rhs)
{
	//if compare priorities; if equal, compare addresses
	return lhs->GetInitPriority()==rhs->GetInitPriority() ? lhs->GetID()<rhs->GetID() : OMEObject::CompareInitPriority(lhs,rhs);
}

OMEObject* BaseManager::GenModelObject(const OMETYPE & typ)
{
	OMEObject* ret = NULL;
	switch (typ)
	{
	case OME_STATEVAR:
		ret = new StateVar();
		break;
	case OME_VARIABLE:
		ret = new Variable();
		break;
	case OME_VARARRAY:
		ret = new VarArray();
		break;
	case OME_TSVAR:
		ret= new TimeSeriesVar();
		break;
	case OME_FLOW:
		ret = new Flow();
		break;
	case OME_INFLUENCE:
		ret = new Influence();
		break;
	case OME_MODEL:
		ret = new Model();
		break;
	case OME_SUBPORT:
		ret = new SubmodelPort();
		break;
	case OME_ASSOCPORT:
		ret = new AssocPort();
	case OME_SPAWNER:
		ret = new Spawner();
		break;
	case OME_LABEL:
		ret = new SimpleLabel();
		break;
	case OME_ITERCOND:
		ret = new IterConditional();
		break;
	case OME_SUBASSOC:
		ret = new SubmodelAssoc();
	case OME_ALIAS:
		ret = new EvalAlias();
		break;
	case OME_CONVEYOR:
		ret = new Conveyor();
		break;
	};
	return ret;
}

/** Retrieves the identifier of the current execution mode.
	@return STLString containing identifier for the current execution mode.
*/
STLString BaseManager::GetExecMode()
{
	STLString ret;
	if (m_pEvalParser)
		ret = m_pEvalParser->GetModeID();
	return ret;
}

/** Populate ValueArchives with data from a specified timestep.
	@param time The timestep to attempt to load.
*/
void BaseManager::LoadValuesForTime(const OME_SCALAR & time)
{
	m_staticValues.LoadRecordsForTime(time);
	m_dynamicValues.LoadRecordsForTime(time);
}

/**Pass an event to the internal dispatcher for dispatch
@param ce The event to dispatch.
*/
void BaseManager::DispatchEvent(CentralEvent & ce) { m_eventDispatcher->DispatchEvent(ce); }

/** Forward event to the Manager's CentralEventDispatcher.
@param ce The CentralEvent to post to the dispatch queue.
*/
void BaseManager::PostEvent(CentralEvent & ce) { m_eventDispatcher->PostEvent(ce); }
/** Registers an Eventable object to receive error events.
@param listener The Eventable object that is to listen for error events.
*/
void BaseManager::RegisterErrListener(Eventable* listener)
{
	m_eventDispatcher->RegisterEventListener(CED_ERR, listener, listener->RefreshExtraOnCall() ? NULL : listener->GetEventExtra());
}

/** Registers an Eventable object to receive increment events.
@param listener The Eventable object that is to listen for increment events.
*/
void BaseManager::RegisterIncListener(Eventable* listener)
{
	m_eventDispatcher->RegisterEventListener(CED_INCREMENT, listener, listener->RefreshExtraOnCall() ? NULL : listener->GetEventExtra());
}

/** Registers an Eventable object to receive pause events.
@param listener The Eventable object that is to listen for pause events.
*/
void BaseManager::RegisterPauseListener(Eventable* listener)
{
	m_eventDispatcher->RegisterEventListener(CED_PAUSE, listener, listener->RefreshExtraOnCall() ? NULL : listener->GetEventExtra());
}