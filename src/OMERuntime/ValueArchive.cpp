#include "ValueArchive.h"
#include "omeobjects/Model.h"
#include "omeobjects/SimObjs.h"
#include "omeobjects/SubmodelPort.h"

/** Detailed Constructor.
	@param startTime The time at the beginning of the simulation.
	@param reportInterval The timestep interval used for recording values.
	@param recordDir The directory to use for storing temporary values, if necessary.
*/
ValueArchive::ValueArchive(const OME_SCALAR & startTime, const OME_SCALAR & reportInterval,const STLString & recordDir)
:m_retainRecords(false), m_records(this, startTime, reportInterval, recordDir, "Records"), m_spaceAllocated(false), SVFunc(NULL), m_SVFExtra(NULL)
{

}

/** Detailed Constructor.
	@param root Pointer to the root model to extract values from.
	@param startTime The time at the beginning of the simulation.
	@param reportInterval The timestep interval used for recording values.
	@param mode The model parsing mode.
	@param mdlOnly If true, do not allocate space for model component values.
	@param recordDir The directory to use for storing temporary values, if necessary.
*/
ValueArchive::ValueArchive(Model* root, const OME_SCALAR & startTime, const OME_SCALAR & reportInterval, const MODELPARSE & mode, const bool & mdlOnly, const STLString & recordDir)
: m_retainRecords(false), m_records(this, startTime, reportInterval, recordDir, root->GetExprName() + ModeToStr(mode)), m_spaceAllocated(false), SVFunc(NULL), m_SVFExtra(NULL)
{

	if (!mdlOnly)
	{
		AllocSpaceForModel(root, mode);
		m_spaceAllocated = true;
	}
	else
		RegisterModelTree(root,mode);
}

/** Copy constructor.
	@param archive The archive to copy.
*/
ValueArchive::ValueArchive(const ValueArchive & archive)
	:m_retainRecords(archive.m_retainRecords), m_records(archive.m_records), m_spaceAllocated(false), SVFunc(NULL), m_SVFExtra(NULL)
{
	
}

/** Alloc space for model component values for all registered models. */
void ValueArchive::AllocSpaceForRegisteredModels()
{
	EvalArray evals;
	for (auto & mPair : m_modelMap)
		mPair.second.pRefModel->GetAllEvals(evals, DT_SHALLOW);
	for (auto itr = evals.begin(); itr != evals.end(); ++itr)
		AddEvaluable(*itr, ALL);
}

/** Alloc space for model component for a model heirarchy.
	@param root Pointer to the root model to allocate value space for.
	@param mode Flag indicating the type of models to allocate space for.
*/
void ValueArchive::AllocSpaceForModel(Model* root, const MODELPARSE & mode)
{
	EvalArray evals;
	root->GetAllEvals(evals);

	for (auto itr = evals.begin(); itr != evals.end(); ++itr)
		AddEvaluable(*itr, mode);
}

ValueArchive::~ValueArchive()
{
	if (!m_retainRecords)
		m_records.Clear();
}


void ValueArchive::ClearAll()
{
	if (!m_retainRecords)
		m_records.Clear();
}


/** Expand memory space for a specific Evaluable object.
	@param pEval Pointer to the Evaluable which needs more space.
	@param numCopies The number of value slots to add.
*/
bool ValueArchive::AddSpaceForEvaluable(Evaluable* pEval, size_t numCopies)
{
	//grab node in tree
	auto trgNode = m_offsetMap.find(pEval->GetID());
	bool ret = trgNode != m_offsetMap.end();
	if (ret)
	{
		bool updateSVs = false;
		numCopies*=pEval->GetRequestedSlotCount();

		m_modelMap[pEval->GetParentModel()].instanceLength += numCopies;

		//insert before next element
		++trgNode;

		m_values.insert(trgNode!=m_offsetMap.end() ? (m_values.begin() + (*trgNode).second.m_ind) : m_values.end(), numCopies, 0.0);

		//DBG_COLOR(cout << pEval->GetName() << ": "<<pEval->m_mdlIndex<< "+" <<numCopies<< endl, DBG_RED);
		//update each following index
		for (trgNode = m_offsetMap.begin(); trgNode != m_offsetMap.end(); ++trgNode)
		{
			if ((*trgNode).second.m_pSrc!=pEval && (*trgNode).second.m_ind >= pEval->m_mdlIndex)
			{
				//DBG_COLOR(cout << (*trgNode).second.m_pSrc->GetName() << ": " << (*trgNode).second.m_ind << ", " << (*trgNode).second.m_ind + numCopies << endl, DBG_WHITE);
				(*trgNode).second.m_ind += numCopies;
				if ((*trgNode).second.m_pSrc)
					(*trgNode).second.m_pSrc->SetModelIndex((*trgNode).second.m_ind);
				if ((*trgNode).second.m_pSrc->IsOMEType(OME_STATEVAR))
					updateSVs = true;
			}
		}

		if (updateSVs && SVFunc)
			SVFunc(m_SVFExtra);
	}
	return ret;
}

/** Expand memory space for all components found within a specific model.
@param pMdl Pointer to the Model which needs more space.
@param numCopies The number of value slots to add.
*/
void ValueArchive::AddSpaceForModel(Model* pMdl, const size_t & numCopies)
{
	//nieve but easy approach
	for (auto itr = m_offsetMap.begin(); itr != m_offsetMap.end(); ++itr)
		if ((*itr).second.m_pSrc->GetParentModel() == pMdl)
			AddSpaceForEvaluable((*itr).second.m_pSrc, numCopies);
}

/** Add a new Evaluable to the ValueArchive if not already present.
	@param pEval Pointer to the Evaluable to insert.
	@return The model index of the inserted Evaluable object.
*/
size_t ValueArchive::InsertEvaluable(Evaluable* pEval)
{
	auto trgNode = m_offsetMap.find(pEval->GetID());
	if (trgNode == m_offsetMap.end())
	{
		AddEvaluable(pEval,ALL);
	}
	return pEval->m_mdlIndex;
}

/** Resize the number of slots occupied by a given Evaluable.
	@param pEval Pointer to the Evaluable that is being resized.
	@param oldSize The previous size allocated to pVa.
	@return true if resize was successful; false otherwise.
*/
bool ValueArchive::ResizeMultiValSlot(Evaluable* pEval, const size_t & oldSize)
{
	size_t newSize = pEval->GetRequestedSlotCount();

	size_t instCount = pEval->GetParentModel()->GetNumRawInstances();
	bool ret = oldSize < newSize && AddSpaceForEvaluable(pEval, instCount);
	if (ret && oldSize)
	{
		size_t slotCount = (newSize*instCount) - oldSize;
		
		//space old contents evenly
		size_t slotsRemaining = slotCount*(instCount-1);
		for (size_t i = pEval->GetModelIndex() + ((instCount)*oldSize) - 1, currSpot = oldSize; slotsRemaining > 0; --i, --currSpot)
		{
			if (!currSpot)
			{
				currSpot = oldSize;
				slotsRemaining -= slotCount;
			}
			//DBG_COLOR(cout << i + slotsRemaining<<endl, DBG_MAGENTA);
			m_values[i + slotsRemaining] = m_values[i];
		}
	}
	return ret;
}

/** @return the total number of value slots in use.*/
size_t ValueArchive::GetSlotCount() const
{
	return m_values.size();
}

/** Retrieve the first index assigned to an object ID.
	@param id The ID to query for. 
	@param out On success, contains the index for the first value.
	@return true if the ID was found and an index was retrieved; false otherwise.
*/
bool ValueArchive::IndexForID(const STLString & id, size_t & out) const
{
	auto fItr = m_offsetMap.find(id);

	bool ret = fItr != m_offsetMap.end();
	if (ret)
		out = (*fItr).second.m_ind;
	return ret;

}

/** Set a function to use to update StateVars.
	@param func The function to call.
	@param extra Pointer to any additional value to pass to func on invocation.
*/
void ValueArchive::SetSVUpdateFunc(SVUpdateFunc func, void* extra)
{
	SVFunc = func;
	m_SVFExtra = extra;
}

/** Refresh The model indexes used by object components to retrieve values.*/
void ValueArchive::RefreshInds()
{
	for (auto itr = m_offsetMap.begin(); itr != m_offsetMap.end(); ++itr)
	{
		(*itr).second.m_pSrc->SetModelIndex((*itr).second.m_ind);

	}
}

/** Relink all collected Models to the ValueArchive.*/
void ValueArchive::ReassignMdlPtrs()
{
	for (auto & mPair : m_modelMap)
		mPair.second.pRefModel->SetValueArchive(this);
}

/** Retrieve the first value for a specific index.
	@param i The index to query.
	@return The first value found at index i.
*/
OME_SCALAR ValueArchive::GetFirstValue(const size_t & i)
{
	OME_SCALAR out=0.0;
	m_records.SingleValForTime(i, (size_t)0, out);
	return out;
}

/** Retrieve the last value for a specific index.
	@param i The index to query.
	@return The last value found at index i.
*/
OME_SCALAR ValueArchive::GetPreviousValue(const size_t & i)
{
	OME_SCALAR out=0.0;
	m_records.SingleLastValForTime(i, out);
	return out;
}

/** Retrieve the value for a specific time and index.
@param i The index to query.
@param time The timestep to retrieve a value from.
@return The value for time found at index i.
*/
OME_SCALAR ValueArchive::GetValueForTime(const size_t & i, const OME_SCALAR & time)
{
	OME_SCALAR out;
	m_records.SingleValForTime(i, time, out);
	return out;
}

/** Set all internal values to those recorded for a specific time.
	@param time The time to load values from.
*/
void ValueArchive::LoadRecordsForTime(const OME_SCALAR & time)
{
	if (!m_values.empty())
		m_records.SyncArchiveWithCache(time,this);
}


/** Cache values for a specific time.
	@param time The time to load values from.
*/
void ValueArchive::CacheRecordForTime(const OME_SCALAR & time)
{
	m_records.CacheRecord(time);
}

/** Add an Evaluable object to the ValueArchive.
	@param pEval Pointer to the Evaluable to add.
	@param mode The model parsing mode used for tracking.
*/
void ValueArchive::AddEvaluable(Evaluable* pEval, const MODELPARSE & mode)
{

	Model* pMdl = pEval->GetOMEType() != OME_SUBPORT ? pEval->GetParentModel() : ((SubmodelPort*)pEval)->GetOuterModel();
	if (mode == ALL || (mode == STATIC && !pMdl->IsDynamic()) || (mode == DYNAMIC && pMdl->IsDynamic()))
	{
		//this will create a new summary if necessary
		ModelSummary& currSummary = m_modelMap[pMdl];
		
		currSummary.pRefModel = pMdl;
		pMdl->SetValueArchive(this);
		size_t insertSize;
		switch (pEval->GetOMEType())
		{

		case OME_INFLUENCE:
			//just in case
			break;
		case OME_SUBPORT:
			if (!((SubmodelPort*)pEval)->LinkedToFlow() || pMdl == ((SubmodelPort*)pEval)->GetParentModel())
				break;
			//otherwise, drop through
		default:
			
			pEval->SetModelIndex(m_values.size());
			m_offsetMap[pEval->GetID()] = MapRefNode(pEval, pEval->m_mdlIndex);
			if (currSummary.srcKey.empty())
				currSummary.srcKey = pEval->GetID();
			insertSize = pMdl->GetNumRawInstances()*pEval->GetRequestedSlotCount();
			currSummary.instanceLength += pEval->GetRequestedSlotCount();
			//insert placeholder. this will increase array size by one. for now, ignore extra space
			if (!insertSize)
			{
				insertSize = 1;
				currSummary.instanceLength += 1;
			}

			m_values.insert(m_values.end(), insertSize, 0.0);
			//DBG_COLOR(cout << pEval->GetName() << "::" << pEval->m_mdlIndex << endl, DBG_YELLOW);

		}
	}
}

/** Register all Models that match the provided model parsing mode.
	@param root Pointer to the root model for registration.
	@param mode The mode indicating which models should be registered.
*/
void ValueArchive::RegisterModelTree(Model* root, const MODELPARSE & mode)
{
	if (mode == ALL || (mode == STATIC && !root->IsDynamic()) || (mode == DYNAMIC && root->IsDynamic()))
	{
		m_modelMap[root].pRefModel = root;
		
		root->SetValueArchive(this);
		for (size_t i = 0; i < root->GetSubmodelCount(); ++i)
			RegisterModelTree(root->GetSubmodel(i),mode);
	}
}

/** Debug Print function
	@return A string listing each variable and its ValueCache index.
*/
STLString ValueArchive::DumpInds()
{
	struct sortOp {

		bool operator() (MapRefNode* lhs, MapRefNode* rhs){ return lhs->m_ind < rhs->m_ind; }
	} sorter;
	oSTLSStream buff;
	std::list<MapRefNode*> sorted;
	for (auto itr = m_offsetMap.begin(); itr != m_offsetMap.end(); ++itr)
		sorted.push_back(&(*itr).second);
	sorted.sort(sorter);

	for (auto itr = sorted.begin(); itr != sorted.end();++itr)
		buff << (*itr)->m_pSrc->GetName() << ": " << (*itr)->m_ind << endl;

	buff << endl << "Total Slots: " << m_values.size() << endl;
	return buff.str();
}

/** Retrieve a list of id-value pairs.
	@param out On return, a list of id-value pairs.
*/
void ValueArchive::GetIDValPairs(ValueArchive::IDValList & out) const
{
	if (!m_offsetMap.empty())
	{
		std::set<MapRefNode> orderSet;
		for (auto entry : m_offsetMap)
		{
			orderSet.insert(entry.second);
		}

		auto nextItr = orderSet.begin();
	
		STLString currID = (*nextItr).m_pSrc->GetID();
		STLString currName = (*nextItr).m_pSrc->GetExprName();
		bool autogen = (*nextItr).m_pSrc->GetAutoGenerated();
		size_t inst = 0;

		size_t skip = !(*nextItr).m_pSrc->IsOMEType(OME_CONVEYOR) ? 1 : ((Conveyor*)(*nextItr).m_pSrc)->GetRequestedSlotCount();
		size_t skipCount = 1;
		++nextItr;
		for (size_t i = 0; i < m_values.size(); ++i)
		{
			if (nextItr != orderSet.end() && (*nextItr).m_ind == i)
			{
				currID = (*nextItr).m_pSrc->GetID();
				currName = (*nextItr).m_pSrc->GetExprName();
				autogen = (*nextItr).m_pSrc->GetAutoGenerated();
				inst = 0;
				skipCount = 1;
				skip = !(*nextItr).m_pSrc->IsOMEType(OME_CONVEYOR) ? 1 : ((Conveyor*)(*nextItr).m_pSrc)->GetRequestedSlotCount();
				++nextItr;
			}

			if (skip == skipCount)
			{
				skipCount = 1;
				out.emplace_back(currID, currName, inst, m_values[i],autogen);
				++inst;
			}
			else
				++skipCount;
		}
	}
}

/** @return A map of all values across all timesteps. */
RecordMgr::FullValueMap ValueArchive::GetAllRecordedValues()
{
	return m_records.GetAllRecordedValues();
}

/** Retrieve a map of all values across all timesteps. 
	@param fvm The FullValueMap to populate.
*/
void ValueArchive::GetAllRecordedValues(RecordMgr::FullValueMap & fvm)
{
	m_records.GetAllRecordedValues(fvm);
}

/** @return an array of all recorded times. */
OMESclrArray ValueArchive::GetAllRecordedTimes()
{
	return m_records.GetRecordTimes();
}

/** Set the active flag for a specific instance of a given model.
	Marking an instance as inactive sets all its internal values to 0.

	@param pMdl Pointer to the Model to update.
	@param inst Index of instance to modify.
	@param active boolean value for new state: true for active, false for inactive.
*/
void ValueArchive::SetModelInstanceActive(Model* pMdl, const size_t & inst, const bool & active)
{
	///\todo Ensure that all submodel instances are recursively disabled
	auto fItr = m_modelMap.find(pMdl);
	if (fItr != m_modelMap.end() && active==false)
	{
		ModelSummary & currSum = (*fItr).second;
		size_t startInd = m_offsetMap[currSum.srcKey].m_ind + (inst * currSum.instanceLength);
		
		//wipe values
		for (size_t i = 0; i < currSum.instanceLength; ++i)
			m_values[i+startInd] = 0.0;
	}
}

/** Register an Evaluable object to be included in minimum record tracking.
	@param pSrc Pointer to Evaluable to register.
	@return true if The ValueArchive tracks pSrc; false otherwise.
*/
bool ValueArchive::AddToMinTracking(Evaluable* pSrc)
{
	bool ret = false;
	for (auto itr = m_offsetMap.begin(); !ret && itr != m_offsetMap.end(); ++itr)
	{
		ret = (*itr).second.m_pSrc == pSrc;
		if (ret)
			m_records.AddToMinTrackList(pSrc);
	}
	return ret;
}

//structs stuff
/** Comparator for use with Model set.
	@param lhs Pointer to model on left hand side of comparison.
	@param rhs Pointer to model on right hand side of comparison.
	@return if id of rhs is less than id of lhs; false otherwise.
*/
bool ValueArchive::MSetCompare::operator()(Model* lhs, Model* rhs)
{ 
	return rhs->GetID() < lhs->GetID(); 
}