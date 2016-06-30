#include "CPPLCSubEntry.h"
#include "CPPLoopCluster.h"
#include "omeobjects/SimObjs.h"
#include "omeobjects/Evaluable.h"

/** Evaluable-type Constructor.
	@param pParent Pointer to the parent CPPLoopCluster.
	@param pE Pointer to represented Evaluable model object.
	@param upSt The associated update statement.
*/
CPPLCSubEntry::CPPLCSubEntry(CPPLoopCluster* pParent, Evaluable* pE, const STLString & upSt)
:m_eType(EVAL), m_pEval(pE), m_count(0), m_updateStatement(upSt), m_pParentCluster(pParent), m_dimCount(1), m_label(pE->GetID())
{
	if (m_pEval && m_pEval->IsOMEType(OME_VARARRAY))
		m_count = ((VarArray*)m_pEval)->GetDims().CalcRepLength();

	m_contains.insert(pE->GetID());
}

/** General Constructor.
	@param pParent Pointer to the parent CPPLoopCluster.
	@param et The type of represented object.
	@param upSt The associated update statement.
	@param lbl The label of the entry object.
	@param num The number/count to associate with this entry.
*/
CPPLCSubEntry::CPPLCSubEntry(CPPLoopCluster* pParent, const SE_TYPE & et, const STLString & upSt, const STLString & lbl, const size_t & num)
:m_eType(et), m_updateStatement(upSt), m_count(num), m_label(lbl), m_pParentCluster(pParent), m_dimCount(1)
{

}

/** General Constructor.
	@param pParent Pointer to the parent CPPLoopCluster.
	@param et The type of represented object.
	@param upSt The associated update statement.
	@param lbl The label of the entry object.
	@param ak The ID of an associated CPPLCAccumEntry.
	@param num The number/count to associate with this entry.
*/
CPPLCSubEntry::CPPLCSubEntry(CPPLoopCluster* pParent, const SE_TYPE & et, const STLString & upSt, const STLString & lbl, const STLString & ak, const size_t & num)
: m_eType(et), m_updateStatement(upSt), m_count(num), m_label(lbl), m_accumKey(ak), m_pParentCluster(pParent), m_dimCount(1)
{

}

/** General Constructor.
	@param pParent Pointer to the parent CPPLoopCluster.
	@param et The type of represented object.
	@param num The number/count to associate with this entry.
	@param lbl The label of the entry object.
*/
CPPLCSubEntry::CPPLCSubEntry(CPPLoopCluster* pParent, const SE_TYPE & et, const size_t & num, const STLString & lbl)
: m_eType(et), m_count(num), m_label(lbl), m_dimCount(1)
{

}

/**SubCluster entry constructor.
	@param pParent Pointer to the parent CPPLoopCluster.
	@param pCS Pointer to the CPPSerializer associated with the cluster tree.
	@param useActive bool indicating if the cluster needes to check for an active model.
	@param pMdl Pointer to the associated Model object.
	@param lbl The label to apply to the new entry.
	@param iterations The count to apply to the new entry.
*/
CPPLCSubEntry::CPPLCSubEntry(CPPLoopCluster* pParent, CPPSerializer* pCS, const bool & useActive, Model* pMdl, const STLString & lbl, const size_t & iterations)
:m_eType(SUBCLST), m_pParentCluster(pParent), m_label(lbl), m_count(iterations), m_dimCount(1)
{
	m_pCluster = new CPPLoopCluster(pCS,lbl,iterations, useActive, pMdl, pParent,this);
}

/** Copy Constructor.
	@param se The CPPLCSubEntry to copy.
*/
CPPLCSubEntry::CPPLCSubEntry(const CPPLCSubEntry & se)
: m_eType(se.m_eType), m_updateStatement(se.m_updateStatement), m_label(se.m_label), 
m_count(se.m_count), m_dependents(se.m_dependents), m_accumKey(se.m_accumKey), m_dimCount(se.m_dimCount),
m_contains(se.m_contains)
{
	switch (m_eType)
	{
	case EVAL:
		m_pEval = se.m_pEval;
		break;
	case SUBCLST:
		m_pCluster = new CPPLoopCluster(*se.m_pCluster);
		break;
	}
}

/** Assignment operator.
	@param rhs The CPPLCSubEntry to copy.
	@return A reference to this.
*/
CPPLCSubEntry& CPPLCSubEntry::operator=(const CPPLCSubEntry & rhs)
{
	m_eType = rhs.m_eType;
	m_updateStatement = rhs.m_updateStatement;
	m_label = rhs.m_label;
	m_count = rhs.m_count;
	m_dependents = rhs.m_dependents;
	m_contains = rhs.m_contains;
	m_accumKey = rhs.m_accumKey;
	m_dimCount = rhs.m_dimCount;

	switch (m_eType)
	{
	case EVAL:
		m_pEval = rhs.m_pEval;
		break;
	case SUBCLST:
		m_pCluster = new CPPLoopCluster(*rhs.m_pCluster);
		break;
	}
	return *this;
}

CPPLCSubEntry::~CPPLCSubEntry()
{
	switch (m_eType)
	{
	case SUBCLST:
		delete m_pCluster;
		break;
	}
}

/** Scrapes the update statement for any potential id names
	and records them.
*/
void CPPLCSubEntry::FindDependents(std::map<STLString, CPPLCSubEntry*> *pReg)
{
	//shouldn't need to clear since dependents are a set, and we aren't presently diminishing entry values
	//m_dependents.clear();
	//add self-reference
	m_dependents.insert(m_label);
	STLString statement = m_updateStatement;
	STLString varName;
	const STLRegex reg("\\b[@]?[_a-zA-Z][_a-zA-Z0-9]*(?![(])\\b");
	STLsmatch match;
	while (std::regex_search(statement, match, reg))
	{
		//remove substitution pattern
		if (match[0].str()[0] == '@')
			varName = match[0].str().substr(3);
		else
			varName = match[0].str();

		//if match was just substitution pattern, avoid adding
		//test against pReg if present
		if (!statement.empty() && (!pReg || pReg->find(statement)!=pReg->end()))
			m_dependents.insert(varName);
		
		statement = match.suffix();
	}

	if (!m_accumKey.empty())
		m_requiredAccums = m_dependents;
}

/** Calls FindDependents on itself and all subentries.
	@sa FindDependents.
*/
void CPPLCSubEntry::FindAllDependents(std::map<STLString, CPPLCSubEntry*> *pReg)
{
	
	switch (m_eType)
	{
	case SUBCLST:
		for (CPPLCSubEntry* pSe : m_pCluster->m_subEntries)
		{
			pSe->FindAllDependents();
			m_dependents.insert(pSe->m_dependents.begin(), pSe->m_dependents.end());
			m_contains.insert(pSe->m_contains.begin(), pSe->m_contains.end());
			m_requiredAccums.insert(pSe->m_requiredAccums.begin(), pSe->m_requiredAccums.end());
		}
		break;
	default:
		FindDependents();
		break;
	}
	
}

/** Test to see if one CPPLCSubEntry relies on another.
	@param se The CPPLCSubEntry to test against.
	@return true if entry relies upon se; false otherwise.
*/
bool CPPLCSubEntry::IsDependentOn(CPPLCSubEntry & se)
{
	//if cluster is marked for cleanup, exit immediately
	if (se.m_eType == SUBCLST && se.m_pCluster->IsDead())
		return false;

	bool ret = m_eType==STARTMORT || m_eType==ENDMORT || se.m_eType == STARTMORT || se.m_eType==ENDMORT;

	if (!ret)
	{
		for (auto dItr = m_dependents.begin(); !ret && dItr != m_dependents.end(); ++dItr)
		{
			for (auto cItr = se.m_contains.begin(); !ret && cItr != se.m_contains.end(); ++cItr)
			{
				ret = (*dItr) == (*cItr);
			}
		}

		/*if (!ret)
			ret = AccumulatesOn(se);*/
	}

	return ret;
}

/**
	Check to see if the the current sub-entry requires se to 
	finish its accumulation before being evaluated.
	@param se The sub-entry to check for accumulation dependency.
	@return true if accumulation of se must finish before evaluation of the current sub-entry; false otherwise.
*/
bool CPPLCSubEntry::AccumulatesOn(CPPLCSubEntry & se)
{
	//if cluster is marked for cleanup, exit immediately
	if (se.m_eType == SUBCLST && se.m_pCluster->IsDead())
		return false;

	bool ret = false;
	
	for (auto aItr = m_requiredAccums.begin(); !ret && aItr != m_requiredAccums.end(); ++aItr)
	{
		for (auto cItr = se.m_contains.begin(); !ret && cItr != se.m_contains.end(); ++cItr)
		{
			ret = (*aItr) == (*cItr);
		}
	}

	return ret;
}

/** Check to see if two entries can be shuffled or combined based on their execution groups.
	@param se The CPPLCSubEntry to compare against.
	@return true if The execution groups are compatible; false otherwise.
*/
bool CPPLCSubEntry::CompatibleExecGroups(CPPLCSubEntry & se)
{
	unsigned int lhs, rhs;
	switch (m_eType)
	{
	case SUBCLST:
		lhs = GetSubLoopCluster()->GetExecGroup();
		break;
	case EVAL:
		lhs = GetEvaluable()->GetExecGroup();
		break;
	default:
		return true;
	}

	switch (se.m_eType)
	{
	case SUBCLST:
		rhs = se.GetSubLoopCluster()->GetExecGroup();
		break;
	case EVAL:
		rhs = se.GetEvaluable()->GetExecGroup();
		break;
	default:
		return true;
	}


	return lhs == OME_NO_EXEC_GRP || rhs == OME_NO_EXEC_GRP
		|| lhs==rhs;
}

/** @return Pointer to associated CPPLoopCluster, or NULL if not associated with any CPPLoopCluster. */
CPPLoopCluster* CPPLCSubEntry::GetSubLoopCluster()
{
	return m_eType != CPPLCSubEntry::SUBCLST ? NULL : m_pCluster;
}

/** @return Pointer to the enclosing CPPLoopCluster. */
CPPLoopCluster* CPPLCSubEntry::GetParentLoopCluster()
{
	return m_pParentCluster;
}

/** Assign an enclosing CPPLoopCluster.
	@param pClust Pointer to the CPPLoopCluster to assign.
*/
void CPPLCSubEntry::SetParentLoopCluster(CPPLoopCluster* pClust)
{
	m_pParentCluster = pClust;
	if (m_eType == SUBCLST)
		m_pCluster->SetParentCluster(pClust);
}

/**@return Pointer to the associated Evaluable object, or NULL if there is no associated Evaluable.*/
Evaluable* CPPLCSubEntry::GetEvaluable()
{
	return m_eType == EVAL ? m_pEval : NULL;
}

/** Set the update statement for the entry.
	@param str The update statement to assign.
*/
void CPPLCSubEntry::SetUpdateStatement(const STLString & str)
{
	m_updateStatement = str;
}

/** @return The associated update statement. */
STLString CPPLCSubEntry::GetUpdateStatement() const
{
	return m_updateStatement;
}

/** Set the associated count.
	@param c The count to associate.
*/
void CPPLCSubEntry::SetCount(const size_t & c)
{
	m_count = c;
}

/** @return The associated count value.*/
size_t CPPLCSubEntry::GetCount() const
{
	return m_count;
}

/** Set the total number of represented dimensions.
	@param c The number of dimensions to assign.
*/
void CPPLCSubEntry::SetDimCount(const unsigned short & c)
{
	m_dimCount = c;
}

/** @return The number of associated dimensions. */
unsigned short CPPLCSubEntry::GetDimCount() const
{
	return m_dimCount;
}

/** @return Pointer to internal set of IDs of dependency values.*/
std::set<STLString>* CPPLCSubEntry::GetDependsSetPtr()
{
	return &m_dependents;
}

/** Set the entry's label.
	@param str The label to assign.
*/
void CPPLCSubEntry::SetLabel(const STLString & str)
{
	m_label = str;
}

/** @return The entry's label. */
STLString CPPLCSubEntry::GetLabel() const
{
	return m_eType==EVAL ? m_pEval->GetID() : m_label;
}

/** Set the associated accumulation key.
	@param str The accumulation key to assign.
*/
void CPPLCSubEntry::SetAccumKey(const STLString & str)
{
	m_accumKey = str; 
}

/** @return The associated accumulation key. */
STLString CPPLCSubEntry::GetAccumKey() const
{
	return m_accumKey;
}

/** Determine if a subcluster-type CPPLCSubEntry contains a temporary variable or evaluable object with the provided name.
	@param name The name to search for.
	@return true if the entry contains a object with name; false otherwise or if the entry is not of the subcluster type.
*/
bool CPPLCSubEntry::ContainsVariableWithName(const STLString & name)
{
	CPPLCSubEntryPtrArray* pEntryArray=NULL;

	bool ret = false;
	switch (m_eType)
	{
	case SUBCLST:
		pEntryArray = &m_pCluster->m_subEntries;
		for (auto itr = pEntryArray->begin(); itr != pEntryArray->end() && !ret; ++itr)
		{
			switch ((*itr)->m_eType)
			{
			case EVAL:
				ret = name == (*itr)->m_pEval->GetName();
				break;
			case TMPASGN:
				ret = name == (*itr)->m_label;
				break;
			}
		}
		break;
	}

	return ret;
}

/** Assign model to mark for mortality checks. Only applies if the entry type is STARTMORT
	@param pMdl Pointer to the model to assign.
*/
void CPPLCSubEntry::SetMortModel(Model* pMdl)
{
	if (m_eType == STARTMORT || m_eType==ENDMORT)
		m_pMortMdl = pMdl;
}

/** @return A pointer to an associated model if one exists; otherwise, return NULL. */
Model* CPPLCSubEntry::GetModel() const
{
	Model* pMdl;

	switch (m_eType)
	{
	case EVAL:
		pMdl = m_pEval->GetParentModel();
		break;
	case SUBCLST:
		//return representative model (first entry)
		pMdl = m_pCluster->GetRepModel();
		break;
	case STARTMORT:
	case ENDMORT:
		pMdl = m_pMortMdl;
		break;
	default:
		pMdl = NULL;

	}
	return pMdl;
}

/** @return true if there is an association model that has conditional execution; false otherwise. */
bool CPPLCSubEntry::HasConditionalModel() const
{
	Model* pMdl = GetModel();
	if (pMdl)
		return pMdl->IsConditional();
	return false;
}

/** Add id of entry that is included within the current sub-entry.
	@param assigned ID of the entry to add.
*/
void CPPLCSubEntry::AddAssigned(const STLString & assigned)
{
	m_contains.insert(assigned);
}

/** Add id of entry that is dependent upon the current sub-entry.
	@param depends ID of the entry to add.
*/
void CPPLCSubEntry::AddDepends(const STLString & depends)
{
	m_dependents.insert(depends);
}
