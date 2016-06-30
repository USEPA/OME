#include "SDPMapEntry.h"
#include "omeobjects/Model.h"

SDPMapEntry::SDPMapEntry()
	:m_isName(false)
	,m_canAddCols(false)
	,m_pMdl(NULL)
	,m_pSdp(NULL)
	,m_initToSDPCount(false)
{

}

/** Simple Constructor.
	@param pMdl Pointer to Model to associate with SDPMapEntry.
*/
SDPMapEntry::SDPMapEntry(Model* pMdl)
	:m_isName(false)
	,m_canAddCols(false)
	,m_pMdl(pMdl)
	,m_pSdp(NULL)
	,m_initToSDPCount(false)
{
	if (pMdl)
		m_nameOrId = pMdl->GetID();
}

/** Detailed Constructor.
	@param id unique id or human-readable name of target object.
	@param isName If true, id field is treated as name; otherwise, it is treated as unique id.
*/
SDPMapEntry::SDPMapEntry(const STLString & id, const bool & isName)
	:m_nameOrId(id)
	,m_isName(isName)
	,m_canAddCols(false)
	,m_pMdl(NULL)
	,m_pSdp(NULL)
	,m_initToSDPCount(false)
{

}

/** Add a subentry to the SDPMapEntry.
	@param entry The SDPMapSubEntry to add.
	@return false if a Model is associated but the entry could not be mapped into it; true otherwise.
*/
bool SDPMapEntry::AddSubEntry(const SDPMapSubEntry & entry)
{
	bool mapSuccessful = true;
	
	//copy to list first to avoid unecessary future copies
	m_valMaps.push_back(entry);

	if (m_pMdl)
	{
		//if parent model is present, modify entry with model index
		Evaluable* pEval = dynamic_cast<Evaluable*>(entry.GetIsName() ? m_pMdl->NestedObjectForName(entry.GetSrcNameOrId()) : m_pMdl->ObjectForKey(entry.GetSrcNameOrId()));
		if (pEval)
		{
			m_valMaps.back().SetModelComponent(pEval);
			if (m_valMaps.back().GetFlags() & SDPMapSubEntry::INIT_COMPONENT)
			{
				//register with param
				int ind=m_pMdl->AddParameter(pEval->GetID(), this);
				pEval->SetInitializer(&m_pMdl->GetParam(ind));
			}
		}
		else
			mapSuccessful = false;
	}

	if (m_pSdp)
	{
		//if SDP is present, register column
		mapSuccessful = RegisterSDPColumn(m_valMaps.back()) && mapSuccessful;
	}

	return mapSuccessful;
}

/** Associate a Model with the SDPMapEntry.
	@param pMdl Pointer to the model to associate.
	@return true if all existing entries properly map to the new model; false otherwise.
*/
bool SDPMapEntry::AssociateModel(Model* pMdl)
{
	bool allFound = true;
	if (pMdl != m_pMdl)
	{
		m_pMdl = pMdl;
		if (m_pMdl)
		{
			//initialize model index mapping for any existing entries
			for (auto itr = m_valMaps.begin(); itr != m_valMaps.end(); ++itr)
			{
				Evaluable* pEval = dynamic_cast<Evaluable*>((*itr).GetIsName() ? m_pMdl->NestedObjectForName((*itr).GetSrcNameOrId()) : m_pMdl->ObjectForKey((*itr).GetSrcNameOrId()));
				if (pEval)
				{
					(*itr).SetModelComponent(pEval);
					if ((*itr).GetFlags() & SDPMapSubEntry::INIT_COMPONENT)
					{
						//register with param
						int ind = m_pMdl->AddParameter(pEval->GetID(),this);
						pEval->SetInitializer(&m_pMdl->GetParam(ind));
					}
				}
				else
					allFound = false;
			}
		}
		else
		{
			//no model; clear all model indexes
			for (auto itr = m_valMaps.begin(); itr != m_valMaps.end(); ++itr)
				(*itr).SetModelComponent(NULL);

			/// \todo remove params from old model
		}
	}

	return allFound;
}

/** Refresh the mappings of all entries.*/
void SDPMapEntry::RefreshEntries()
{
	if (m_pMdl)
	{
		//initialize model index mapping for any existing entries
		for (auto itr = m_valMaps.begin(); itr != m_valMaps.end(); ++itr)
		{
			Evaluable* pEval = dynamic_cast<Evaluable*>((*itr).GetIsName() ? m_pMdl->NestedObjectForName((*itr).GetSrcNameOrId()) : m_pMdl->ObjectForKey((*itr).GetSrcNameOrId()));
			if (pEval)
				(*itr).SetModelComponent(pEval);
		}
	}
}

/** Associate an SDP with mapping.
	@param pSdp Pointer to SDP to associate.
	@param canAddCols if true, entries can add columns to SDP if not already present.
	@return true if association was successful; false otherwise.
*/
bool SDPMapEntry::AssociateSDP(ISpatialDataProvider* pSdp, const bool & canAddCols)
{
	m_canAddCols = canAddCols;
	bool success = true;
	if (m_pSdp != pSdp)
	{
		m_pSdp = pSdp;
		int colInd;
		if (m_pSdp)
		{
			//associate entries with their column names, or add the column to the SDP
			for (auto itr = m_valMaps.begin(); itr != m_valMaps.end(); ++itr)
				success = RegisterSDPColumn((*itr)) && success;
		}
		else
		{
			//remove all column indices
			for (auto itr = m_valMaps.begin(); itr != m_valMaps.end(); ++itr)
				(*itr).SetColInd(-1);
		}
	}
	return success;
}

/** Apply subEntries to their assoicated values and coverage columns.
	@param toApply flags indicating which associations to apply.
	@return true if all applications were successful.
*/
bool SDPMapEntry::ApplyEntries(const SDPMapSubEntry::ENTRY_FLAGS & toApply)
{
	bool success=true;
	for (auto itr = m_valMaps.begin(); itr != m_valMaps.end(); ++itr)
	{
		for (size_t i = 0; i < m_pMdl->GetNumInstances(); ++i)
		{
			/// \question check for active instance here?
			success=(*itr).Apply(m_pSdp, m_pMdl, i, toApply,&m_errs) && success;
		}
	}

	return success;
}

/** Apply subEntries to their assoicated values and coverage columns.
	@param toApply flags indicating which associations to apply.
	@param syncFunc Function to use for application.
	@param pFuncExtra Pointer to additional data to pass on to syncFunc.
	@return true if all applications were successful.
*/
bool SDPMapEntry::ApplyEntries(const SDPMapSubEntry::ENTRY_FLAGS & toApply,CoverageSyncFunction syncFunc, void* pFuncExtra)
{
	bool success=true;
	for (auto itr = m_valMaps.begin(); itr != m_valMaps.end(); ++itr)
	{
		for (size_t i = 0; i < m_pMdl->GetNumInstances(); ++i)
		{
			/// \question check for active instance here?
			success=(*itr).Apply(m_pSdp, m_pMdl, i, toApply,syncFunc,pFuncExtra,&m_errs) && success;
		}
	}

	return success;
}

/** Apply SDP relationship to the current instance of an Evaluable object, if applicable.
	@param toApply The Relationship(s) to apply.
	@param pEval Pointer to the object to apply the relationship against, if applicable.
	@return true if there was a relationship to apply, and that it was applied successfully.
*/
bool SDPMapEntry::ApplyEntryForCurrInstEval(const SDPMapSubEntry::ENTRY_FLAGS & toApply, Evaluable* pEval)
{
	bool success = false;
	for (auto itr = m_valMaps.begin(); itr != m_valMaps.end(); ++itr)
	{
		if ((*itr).GetModelComponent() == pEval)
			success = (*itr).Apply(m_pSdp, m_pMdl, m_pMdl->GetCurrentInstance(), toApply, &m_errs);
			
	}

	return success;
}

/** Register a column lookup for the SDP within a coverage.
	If the column defined in entry is not found, and the canAddCols argument is true when AssociateSDP() is called, 
	the referenced column will be added to the coverage.
	@param entry The entry to associate with a coverage column.
	@return true if the column referenced by entry is found or the equivalent column is added to the coverage; false otherwise.
	@sa AssociateSDP
*/
bool SDPMapEntry::RegisterSDPColumn(SDPMapSubEntry & entry)
{
	int colInd = m_pSdp->GetFieldCol(entry.GetColName().c_str());
	bool success = true;
	if (colInd == -1)
	{
		if (m_canAddCols)
			colInd = m_pSdp->AddFieldCol(entry.GetColName().c_str());
		else
			success = false;
	}
	entry.SetColInd(colInd);
	return success;
}

/** Return string-based identifier for the associated model.
	@param isName on return is set to true if string identifier is common name, or false if it is a unique id.
	@return The String identifier.
*/
STLString SDPMapEntry::GetNameOrId(bool & isName)
{
	isName = m_isName;
	return m_nameOrId;
}

/** @return the initial instances to assign to associated model based on the number of records with the SDP.
	If SetInstancesPerUnit was not called with the ipu argument set to true then the return value will be 0.
	@sa SetInstancesPerUnit
*/
size_t SDPMapEntry::GetIPUCount()
{
	if (m_pSdp && m_initToSDPCount)
		return m_pSdp->GetRowCount();
	else
		return 0;
}

/** If SetInstancesPerUnit was called with the ipu argument set to true, the number of initial instances for the associated model
	will be set to the total number of records reported by the SDP.
	@sa SetInstancesPerUnit
*/
void SDPMapEntry::ApplyIPUToAssociatedModel()
{
	if (m_initToSDPCount && m_pMdl && m_pSdp)
	{
		m_pMdl->SetInitNumInstances(m_pSdp->GetRowCount());
		m_pMdl->SetExpectsSpatial(true);
		m_pMdl->SetInstMode(Model::RECORD);
	}
}

/** Retrieve any reported errors, applying an optional header to the returned ErrCollection.
	@param header Optional header to add to the beginning of the returned ErrCollection.
	@return An ErrCollection containing any errors that have been captured through SDPMapEntry methods optionally prefixed
	by a supplied header.
*/
ErrCollection SDPMapEntry::GetErrors(const STLString & header)
{
	if (header.empty())
		return m_errs;
	else
	{
		ErrCollection ret(m_errs);
		ret.AddErr(header, EC_HEADER, NULL, true);
		return ret;
	}
}

