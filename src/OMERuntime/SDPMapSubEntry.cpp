#include "SDPMapSubEntry.h"
#include "omeobjects/Evaluable.h"
#include "omeobjects/Model.h"

/** Detailed constructor.
	@param srcnori The source identifer, either common name or id.
	@param isName If true, srcnori refers to a readable name; otherwise it refers to a unique id.
	@param flgs bit-flags identifying supported associations.
*/
SDPMapSubEntry::SDPMapSubEntry(const STLString & srcnori, const bool & isName, const ENTRY_FLAGS & flgs)
	: m_srcNameOrId(srcnori)
	, m_isName(isName)
	, m_colInd(-1)
	, m_pEval(NULL)
	, m_applyTo(flgs)
	, m_offset(0)
	, m_colName(srcnori)
{}

/** Detailed constructor.
@param srcnori The source identifer, either common name or id.
@param isName If true, srcnori refers to a readable name; otherwise it refers to a unique id.
@param cName The name of the associated column in the spatial coverage. 
@param flgs bit-flags identifying supported associations.
*/
SDPMapSubEntry::SDPMapSubEntry(const STLString & srcnori, const bool & isName, const STLString & cName, const ENTRY_FLAGS & flgs)
	: m_srcNameOrId(srcnori)
	, m_isName(isName)
	, m_colInd(-1)
	, m_pEval(NULL)
	, m_applyTo(flgs)
	, m_offset(0)
	, m_colName(cName)
{}

/** Detailed constructor.
@param srcnori The source identifer, either common name or id.
@param isName If true, srcnori refers to a readable name; otherwise it refers to a unique id.
@param ii The instance-based offset of the value to reference from the associated model component.
@param cName The name of the associated column in the spatial coverage.
@param flgs bit-flags identifying supported associations.
*/
SDPMapSubEntry::SDPMapSubEntry(const STLString & srcnori, const bool & isName, const size_t & ii, const STLString & cName, const ENTRY_FLAGS & flgs)
	: m_srcNameOrId(srcnori)
	, m_isName(isName)
	, m_colInd(-1)
	, m_pEval(NULL)
	, m_applyTo(flgs)
	, m_offset(ii)
	, m_colName(cName)
{

}

SDPMapSubEntry::SDPMapSubEntry()
	:m_isName(false)
	,m_colInd(-1)
	, m_pEval(NULL)
	, m_applyTo(NONE)
	, m_offset(0)
{

}

/** Apply selected relationships, if applicable.
	@param pSDP Pointer to the SDP to use for accessing spatial coverage details.
	@param pMdl Pointer to the Model to use for component references.
	@param inst The instance of pMdl to use.
	@param toApply The relationships to attempt to apply.
	@param pErrs Optional pointer to an ErrCollection to use to catalog any significant errors in the application process.
	@return true If the apply process was sucessful or the flags are not supported, or false if something catastrophically failed.
	@throws OMEMemoryException or OMEFuncOpException on catastrophic failures if a pointer pErrs is NULL.
*/
bool SDPMapSubEntry::Apply(ISpatialDataProvider* pSDP, Model* pMdl, const size_t & inst, const ENTRY_FLAGS & toApply, ErrCollection* pErrs)
{
	//if no flags are set that correspond to the requested action, return false
	if (!(toApply & m_applyTo))
	{
		//flags do not apply, but technically no errors
		return true;
	}

	if (m_pEval == NULL || m_colInd == -1)
	{
		if (pErrs)
			pErrs->AddErr("SubEntry has not been initialized");
		else
			throw OMEMemoryException("SDPMapSubEntry::Apply", "not properly initialized.");
		return false;
	}
	if (!pSDP || !pMdl)
	{
		if (pErrs)
			pErrs->AddErr("SDP and/or Model pointers are NULL.");
		else
			throw OMEMemoryException("SDPMapSubEntry::Apply","null arguments not allowed.");
		return false;
	}

	size_t depth=FindDepth(pMdl);
	if (!depth)
	{
		if (pErrs)
			pErrs->AddErr(m_pEval->GetName() + " is not a descendent of " + pMdl->GetName());
		else
			throw OMEFuncOpException("SDPMapSubEntry::Apply",(m_pEval->GetName() + " is not a descendent of " + pMdl->GetName()).c_str());
		return false;
	}

	OME_SCALAR oldVal,val;
	if (toApply & m_applyTo & (INIT_SDP | UPDATE_SDP))
	{
		//initialize or update from model to SDP
		pMdl->GetValForRawInstIndex(inst, m_pEval->GetModelIndex()+m_offset,depth, val);

		//only update if values differ (just in case the SDP tracks such things)
		pSDP->GetData(inst, m_colInd, oldVal);
		if ((m_applyTo & INIT_SDP) || oldVal!=val)
			pSDP->SetData(inst, m_colInd, val);
	}

	if (toApply & m_applyTo & (INIT_COMPONENT | UPDATE_COMPONENT))
	{
		//initialize or update from SDP to model
		pSDP->GetData(inst, m_colInd, val);
		OME_SCALAR* pToSet;
		pMdl->GetValAddrForRawInstIndex(inst, m_pEval->GetModelIndex()+m_offset, depth, pToSet);

		//add value check here if more than just a primitive assignment takes place
		*pToSet = val;
	}

	return true;
}

/** Apply selected relationships, if applicable; presently only supports INIT_SDP and UPDATE_SDP flags.
@param pSDP Pointer to the SDP to use for accessing spatial coverage details.
@param pMdl Pointer to the Model to use for component references.
@param inst The instance of pMdl to use.
@param toApply The relationships to attempt to apply.
@param pErrs Optional pointer to an ErrCollection to use to catalog any significant errors in the application process.
@param syncFunc Custom synchronization function to use when a value has changed and needs to be assigned.
@param pFuncExtra caller-defined additional data to pass to syncFunc on invocation.
@return true If the apply process was sucessful or the flags are not supported, or false if something catastrophically failed.
@throws invalid_argument on catastrophic failures if a pointer pErrs is NULL.
*/
bool SDPMapSubEntry::Apply(ISpatialDataProvider* pSDP, Model* pMdl, const size_t & inst, const ENTRY_FLAGS & toApply, CoverageSyncFunction syncFunc, void* pFuncExtra, ErrCollection* pErrs)
{
	if (!(toApply & m_applyTo))
	{
		//flags do not apply, but technically no errors
		return true;
	}

	if (m_pEval == NULL || m_colInd == -1)
	{
		if (pErrs)
			pErrs->AddErr("SubEntry has not been initialized");
		else
			throw OMEMemoryException("SDPMapSubEntry::Apply","not properly initialized.");
		return false;
	}
	if (!pSDP || !pMdl)
	{
		if (pErrs)
			pErrs->AddErr("SDP and/or Model pointers are NULL.");
		else
			throw OMEMemoryException("SDPMapSubEntry::Apply","null arguments not allowed.");
		return false;
	}

	size_t depth = FindDepth(pMdl);
	if (!depth)
	{
		if (pErrs)
			pErrs->AddErr(m_pEval->GetName() + " is not a descendent of " + pMdl->GetName());
		else
			throw OMEFuncOpException("SDPMapSubEntry::Apply",(m_pEval->GetName() + " is not a descendent of " + pMdl->GetName()).c_str());
		return false;
	}


	OME_SCALAR oldVal,val;
	if (toApply & m_applyTo & (INIT_SDP | UPDATE_SDP))
	{
		//initialize or update from model to SDP
		pMdl->GetValForRawInstIndex(inst, m_pEval->GetModelIndex()+m_offset, depth, val);

		//only call sync if vals have changed
		pSDP->GetData(inst, m_colInd, oldVal);
		if ((m_applyTo & INIT_SDP) || oldVal != val)
			syncFunc(inst, m_colInd, val,pFuncExtra);

		return true;
	}

	if (toApply & m_applyTo & (INIT_COMPONENT | UPDATE_COMPONENT))
	{
		//custom sync from sdp-to-component not presently supported
		return false;
	}

	return true;
}

/** Find depth of model component from passed in model, or fail if not in heirarchy.
	This is necessary to find correct offset for each instance of pMdl.
	@param pMdl The model to evaluate.
	@return the depth from pMdl to the associated Evaluable object, or 0 if the associated Evaluable object is not in pMdl's heirarchy.
*/
size_t SDPMapSubEntry::FindDepth(Model* pMdl)
{
	Model* pDepthModel = m_pEval->GetParentModel();
	size_t depth;
	for (depth = 1; pDepthModel && pDepthModel != pMdl; depth *= pDepthModel->GetNumInstances(), pDepthModel = pDepthModel->GetParentModel());

	if (!pDepthModel)
		return 0;
	return depth;
}