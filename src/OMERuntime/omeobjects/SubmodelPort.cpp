#include "omeobjects/SubmodelPort.h"
#include "DebugOutput.h"
#include "omeobjects/Evaluable.h"



/** Add Source to submodel connection instances.
	@param pSrc object to add to submodel sources.
	@param useID If true, assign id from port to object.
*/
void SubmodelPort::AddSubmodelSource(Evaluable* pSrc,const bool useID)
{
	if(ObjectPresent<Evaluable*>(pSrc,m_smPSources) < 0)
	{
		m_smPSources.push_back(pSrc);
		//if (pSrc->GetOMEType() != OME_FLOW)
			pSrc->AddDownObject(this);
		if(useID)
			AddSubSourceID(pSrc->GetID());
	}
}

/** Add Target to submodel connection instances.
	@param pTrg object to add to submodel targets.
	@param useID If true, assign id from port to object.
*/
void SubmodelPort::AddSubmodelTarget(Evaluable* pTrg,const bool useID)
{
	if(ObjectPresent<Evaluable*>(pTrg,m_smPTargets) < 0)
	{
		m_smPTargets.push_back(pTrg);
		//if (pTrg->GetOMEType()!=OME_FLOW)
			pTrg->AddUpObject(this);
		if(useID)
			AddSubTargetID(pTrg->GetID());
	}
}

void SubmodelPort::SetParentModel(Model* mdl)
{
	
	if(OMEObject::m_pParentModel)
	{
		//prevent stack overflow by setting m_pParentModel to NULL ahead of time;
		Model* pParent = OMEObject::m_pParentModel;
		OMEObject::m_pParentModel = NULL;
		RemoveSubmodel(pParent);
	}

	OMEObject::SetParentModel(mdl);
}

/** Remove provided source object from submodel sources.
	@param pSrc The pointer to remove.
*/
void SubmodelPort::RemoveSubmodelSource(Evaluable* pSrc)
{
	RemoveObj(m_smPSources,pSrc);
}

/** Remove provided target object from submodel targets.
	@param pTrg The pointer to remove.
*/
void SubmodelPort::RemoveSubmodelTarget(Evaluable* pTrg)
{
	RemoveObj(m_smPTargets,pTrg);
}

void SubmodelPort::RemoveSubmodel(Model* mdl)
{
	//there may be a better way to do this
	if(mdl)
	{
		//parse all down objects
		EvalArray::iterator itr;
	  itr=m_smPTargets.begin();
		while(itr!= m_smPTargets.end())
		{
			if((*itr)->GetParentModel()==mdl)
				itr=m_smPTargets.erase(itr);
			else
				++itr;
		}

		//parse all up objects

	  itr=m_smPSources.begin();
		while(itr!= m_smPSources.end())
		{
			if((*itr)->GetParentModel()==mdl)
				itr=m_smPSources.erase(itr);
			else
				++itr;
		}
	}
}


/** Add an inbound object contained in the outer model.
	@param pSrc Pointer to the inbound object to add.
	@param useID If true, assign id from port to object.
*/
void SubmodelPort::AddOutmodelSource(Evaluable* pSrc,const bool useID)
{
	if(ObjectPresent(pSrc,m_omPSources) < 0)
	{
		m_omPSources.push_back(pSrc);
		//if (pSrc->GetOMEType() != OME_FLOW)
			pSrc->AddDownObject(this);
		if(useID)
			AddOutSourceID(pSrc->GetID());

	}
}

/** Add an outbound object contained in the outer model.
	@param pTrg Pointer to the outbound object to add.
	@param useID If true, assign id from port to object.
*/
void SubmodelPort::AddOutmodelTarget(Evaluable* pTrg,const bool useID)
{
	if(ObjectPresent(pTrg,m_omPTargets) < 0)
	{
		m_omPTargets.push_back(pTrg);
		//if (pTrg->GetOMEType()!=OME_FLOW)
			pTrg->AddUpObject(this);
		if(useID)
			AddOutTargetID(pTrg->GetID());
	}
}

/** Remove an inbound object contained in the outer model.
	@param pSrc Pointer to the inbound object to remove.
*/
void SubmodelPort::RemoveOutmodelSource(Evaluable* pSrc)
{
	RemoveObj(m_omPSources,pSrc);
}

/** Remove an outbound object contained in the outer model.
	@param pTrg Pointer to the outbound object to remove.
*/
void SubmodelPort::RemoveOutmodelTarget(Evaluable* pTrg)
{
	RemoveObj(m_omPTargets,pTrg);
}


/** Copies values from another SubmodelPort.
	All Values are copied except up and down object arrays.
	m_name and m_id are append with underscores to maintain uniqueness (is there a better way to handle this?).
	@param smp the SubmodelPort to be copied.
*/
void SubmodelPort::Duplicate(const SubmodelPort & smp)
{

	//dup parents
	m_pOuterModel = smp.m_pOuterModel;

	m_smSources.assign(smp.m_smSources.begin(), smp.m_smSources.end());
	m_smTargets.assign(smp.m_smTargets.begin(), smp.m_smTargets.end());

	m_omSources.assign(smp.m_omSources.begin(), smp.m_omSources.end());
	m_omTargets.assign(smp.m_omTargets.begin(), smp.m_omTargets.end());
	Evaluable::Duplicate(smp);
}

OME_SCALAR SubmodelPort::Evaluate(const OME_SCALAR time,BaseManager* pBm)
{
	m_lastTime=time;
	OME_SCALAR valSum=0.0;

	if (m_direction == PD_NONE)
		UpdateDirection();
	//nieve approach: assume models have already updated for this step, ask for value.
	//accumulate all values
	ModelInstance* pInst;
	size_t ind;
	EvalArray& sources = m_direction == PD_OUTBOUND ? m_smPSources : m_omPSources;
	Model* pSrcMdl = m_direction == PD_OUTBOUND ? m_pParentModel : m_pOuterModel;
	ValueArchive& arch = *pSrcMdl->GetValueArchive();
		
	for (auto itr = sources.begin(); itr != sources.end(); ++itr)
	{
		ind = (*itr)->GetModelIndex();
		for (size_t m = 0; m < pSrcMdl->GetNumInstances(); m++)
		{
			pInst = pSrcMdl->GetInstance(m);
			if (pInst->IsActive())
				valSum+=arch[ind + m];
		}
	}
		
	PROFILE_CODE(SetValue(valSum););

	return valSum;
}

void SubmodelPort::Reset(BaseManager* pBm)
{
	Evaluable::Reset(pBm);
}

STLString SubmodelPort::GetXMLRepresentation(const unsigned int indent, const unsigned int inc) 
{
	return Evaluable::GetXMLForObject(indent,inc);
}

void SubmodelPort::GetXMLAttributes(StrArray & out) const
{
	OMEObject::GetXMLAttributes(out);
}

void SubmodelPort::GetXMLSubNodes(StrArray & out,const unsigned int indent,const unsigned int inc)
{
	int i;
	STLString baseIndent(indent,' '),subIndent(indent+inc,' ');

	OMEObject::GetXMLSubNodes(out,indent,inc);

   oSTLSStream outStrm;
	//sub sources
	if(!m_smSources.empty())
	{
		outStrm<<baseIndent<<"<subsources>"<<std::endl;
		for(i=0; i<m_smSources.size();i++)
		   outStrm<<subIndent<<"<src name=\""<<m_smSources[i]<<"\"/>"<<std::endl;
		outStrm<<baseIndent<<"</subsources>";
		out.push_back(outStrm.str());
	  outStrm.clear();
	  outStrm.str(STLString());
	}

	//sub targets
	if(!m_smTargets.empty())
	{
		outStrm<<baseIndent<<"<subtargets>"<<std::endl;
		for(i=0; i<m_smTargets.size();i++)
			outStrm<<subIndent<<"<trg name=\""<<m_smTargets[i]<<"\"/>"<<std::endl;
		outStrm<<baseIndent<<"</subtargets>";
		out.push_back(outStrm.str());
	  outStrm.clear();
	  outStrm.str(STLString());
	}

	//outer sources
	if(!m_omSources.empty())
	{
		outStrm<<baseIndent<<"<outsources>"<<std::endl;
		for(i=0; i<m_omSources.size();i++)
		   outStrm<<subIndent<<"<src name=\""<<m_omSources[i]<<"\"/>"<<std::endl;
		outStrm<<baseIndent<<"</outsources>";
		out.push_back(outStrm.str());
	  outStrm.clear();
	  outStrm.str(STLString());
	}

	//outer targets
	if(!m_omTargets.empty())
	{
		outStrm<<baseIndent<<"<outtargets>"<<std::endl;
		for(i=0; i<m_omTargets.size();i++)
			outStrm<<subIndent<<"<trg name=\""<<m_omTargets[i]<<"\"/>"<<std::endl;
		outStrm<<baseIndent<<"</outtargets>";
		out.push_back(outStrm.str());
	  //outStrm.clear();
	}

}

void SubmodelPort::GetXMLAttributeMap(XMLAttrVector & out)
{
	OMEObject::GetXMLAttributeMap(out);
}

int SubmodelPort::PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
{
	STLString strTemp;
	XmlAttr attrs[] = {
		// attr				type		         address              isReq
			{ "name", XML_STR, &(strTemp), true },
			{ NULL, XML_NULL, NULL, false } };

	int ret = 1;
	//parentSources
	STLString srcTag = "src";
	STLString trgTag = "trg";

	TI2Element* pXmlSub = pCurrElem->FirstChildElement("subsources");
	if (pXmlSub)
	{
		pXmlSub = pXmlSub->FirstChildElement(srcTag.c_str());
		while (pXmlSub != NULL)
		{
			ret = ret && ParseAttributes(pXmlSub, attrs, tag, filename);
			if (ret)
				m_smSources.push_back(STLString(strTemp));
			pXmlSub = pXmlSub->NextSiblingElement(srcTag.c_str());
		}
	}

	pXmlSub = pCurrElem->FirstChildElement("subtargets");
	if (pXmlSub)
	{
		pXmlSub = pXmlSub->FirstChildElement(trgTag.c_str());
		while (pXmlSub != NULL)
		{
			ret = ret && ParseAttributes(pXmlSub, attrs, tag, filename);
			if (ret)
				m_smTargets.push_back(STLString(strTemp));
			pXmlSub = pXmlSub->NextSiblingElement(trgTag.c_str());
		}
	}

	pXmlSub = pCurrElem->FirstChildElement("outsources");
	if (pXmlSub)
	{
		pXmlSub = pXmlSub->FirstChildElement(srcTag.c_str());
		while (pXmlSub != NULL)
		{
			ret = ret && ParseAttributes(pXmlSub, attrs, tag, filename);
			if (ret)
				m_omSources.push_back(STLString(strTemp));
			pXmlSub = pXmlSub->NextSiblingElement(srcTag.c_str());
		}
	}

	pXmlSub = pCurrElem->FirstChildElement("outtargets");
	if (pXmlSub)
	{
		pXmlSub = pXmlSub->FirstChildElement(trgTag.c_str());
		while (pXmlSub != NULL)
		{
			ret = ret && ParseAttributes(pXmlSub, attrs, tag, filename);
			if (ret)
				m_omTargets.push_back(STLString(strTemp));
			pXmlSub = pXmlSub->NextSiblingElement(trgTag.c_str());
		}
	}
	UpdateDirection();
	return ret;
}

/** Check to see if eval resides in any submodel instances.
	@param eval The pointer to evaluate.
	@return true if eval object is connected to the SubmodelPort from a submodel instance; false otherwise.
*/
bool SubmodelPort::IsSubEval(Evaluable* eval)
{
	return ((ObjectPresent(eval,m_smPTargets) >= 0) || (ObjectPresent(eval,m_smPSources) >= 0));
}

/** Check to see if eval resides in the outer model.
	@param eval The pointer to evaluate.
	@return true if eval object is connected to the SubmodelPort from the outermodel; false otherwise.
*/
bool SubmodelPort::IsOutEval(Evaluable* eval)
{
	return ((ObjectPresent(eval,m_omPTargets) >= 0) || (ObjectPresent(eval,m_omPSources) >= 0));
}

/** Get pointers associated with submodel instances.
	@param out The EvalArray to populate.
*/
void SubmodelPort::GetSubObjects(EvalArray & out)
{
	out.insert(out.end(), m_smPTargets.begin(),m_smPTargets.end());
	out.insert(out.end(), m_smPSources.begin(),m_smPSources.end());
}

/** Return pointers to all objects associated with the outer model.
	@param out the EvalArray to populate.
*/
void SubmodelPort::GetOutObjects(EvalArray & out)
{
	out.insert(out.end(), m_omPTargets.begin(),m_omPTargets.end());
	out.insert(out.end(), m_omPSources.begin(),m_omPSources.end());
}

/** Retrieve pointers to all linked objects that exist in the submodel.
	@param out EvalArray to hold pointers to all linked submodel objects.
*/
void SubmodelPort::GetSubRepObjects(EvalArray & out)
{
	if(OMEObject::m_pParentModel)
	{
		for(auto itr=m_downObjects.begin(); itr!=m_downObjects.end(); ++itr)
		{
			if ((*itr)->GetParentModel() == OMEObject::m_pParentModel)
				out.push_back(*itr);
		}

		for(auto itr=m_upObjects.begin(); itr!=m_upObjects.end(); ++itr)
		{
			if ((*itr)->GetParentModel() == OMEObject::m_pParentModel)
				out.push_back(*itr);
		}
	}
}

/** Retrieve pointers to all linked objects that exist in the outermodel.
	@param out EvalArray to hold pointers to all linked outermodel objects.
*/
void SubmodelPort::GetOutRepObjects(EvalArray & out)
{
	if(m_pOuterModel)
	{
		for(auto itr=m_downObjects.begin(); itr!=m_downObjects.end(); ++itr)
		{
			if((*itr)->GetParentModel()==m_pOuterModel)
				out.push_back(*itr);
		}

		for(auto itr=m_upObjects.begin(); itr!=m_upObjects.end(); ++itr)
		{
			if((*itr)->GetParentModel()==m_pOuterModel)
				out.push_back(*itr);
		}
	}
}

/** Retrieve a pointer to an Evaluable source object regardless of whether sources exist in the submodel or outermodel.
	@param ind The index of the Evaluable to retrieve. If ind is out of bounds, NULL is returned.
	@return A pointer to Evaluable at ind, or NULL if ind is invalid.
	@sa GetAllTarget.
*/
Evaluable* SubmodelPort::GetAllSource(const int ind) const
{
	Evaluable* pEval = NULL;
	if (ind < m_smPSources.size())
		pEval = m_smPSources[ind];
	else
		pEval = m_omPSources[ind - m_smPSources.size()];

	return pEval;
}

/** Retrieve a pointer to an Evaluable target object regardless of whether targets exist in the submodel or outermodel.
	@param ind The index of the Evaluable to retrieve. If ind is out of bounds, NULL is returned.
	@return A pointer to Evaluable at ind, or NULL if ind is invalid.
	@sa GetAllSource.
*/
Evaluable* SubmodelPort::GetAllTarget(const int ind) const
{
	Evaluable* pEval = NULL;
	if (ind < m_smPTargets.size())
		pEval = m_smPTargets[ind];
	else
		pEval = m_omPTargets[ind - m_smPTargets.size()];

	return pEval;
}

/** Synchronize named objects with actual pointers in outermodel.

*/
void SubmodelPort::SyncWithParents()
{

	if(m_pOuterModel)
	{
		std::for_each(m_omSources.begin(),m_omSources.end(),[this](const STLString &key){AddOutmodelSource(dynamic_cast<Evaluable*>(m_pOuterModel->ObjectForKey(key)),false);}); 
		std::for_each(m_omTargets.begin(),m_omTargets.end(),[this](const STLString &key){AddOutmodelTarget(dynamic_cast<Evaluable*>(m_pOuterModel->ObjectForKey(key)),false);}); 
	}

	//avoid ambiguity
	Model* pParent = OMEObject::m_pParentModel;

	//find model collection
	if (pParent)
	{
		//grab objects from models for prioritization/relationship stuff
		std::for_each(m_smSources.begin(), m_smSources.end(), [this,&pParent](const STLString &key){AddSubmodelSource(dynamic_cast<Evaluable*>(pParent->ObjectForKey(key)), false); });
		std::for_each(m_smTargets.begin(), m_smTargets.end(), [this, &pParent](const STLString &key){AddSubmodelTarget(dynamic_cast<Evaluable*>(pParent->ObjectForKey(key)), false); });
	}

}

unsigned int SubmodelPort::Prioritize(unsigned int currPriority, const bool isInitial)
{
	unsigned int *pCompPriority = isInitial ? &(OMEObject::m_initPriority) : &(OMEObject::m_evalPriority);

	if(currPriority > *pCompPriority)
	{
		*pCompPriority=currPriority;

		unsigned int maxPriority=0;//=*pCompPriority;
		unsigned int tempPriority;

		//nieve approach: assume models have already updated for this step, ask for value.
		//accumulate all values

		for(int i=0; i< m_upObjects.size(); i++)
		{
			if (m_upObjects[i]->GetOMEType() != OME_INFLUENCE || !((Influence*)m_upObjects[i])->GetUseCurrentVal())
			{

				tempPriority=m_upObjects[i]->Prioritize(*pCompPriority+1,isInitial);

				maxPriority= maxPriority >= tempPriority ? maxPriority : tempPriority;
			}
		}

		for(int i=0; i< m_downObjects.size(); i++)
		{
			if (m_downObjects[i]->GetOMEType() == OME_INFLUENCE && ((Influence*)m_downObjects[i])->GetUseCurrentVal())
			{

				tempPriority=m_downObjects[i]->Prioritize(*pCompPriority+1,isInitial);
				maxPriority= maxPriority >= tempPriority ? maxPriority : tempPriority;
			}
		}

		if(maxPriority< *pCompPriority)
			maxPriority = *pCompPriority;
	}
	return *pCompPriority;
}

void SubmodelPort::FindNextDownObjects(EvalArray & out, bool excludeInfl,bool bypassPorts)
{
	Evaluable::FindNextDownObjects(out,excludeInfl,bypassPorts);
	//Evaluable* pTrg=dynamic_cast<Evaluable*>(m_pTarget);
	//if(pTrg && (pTrg->GetOMEType()==OME_INFLUENCE || pTrg->GetOMEType()==OME_FLOW))
	//{
	//	if(pTrg->GetOMEType()==OME_INFLUENCE && excludeInfl)
	//	{
	//		pTrg=((Influence*)pTrg)->GetPEvalTarget();
	//		if(pTrg->GetOMEType()==OME_SUBPORT)
	//			((SubmodelPort*)pTrg)->FindNextDownObjects(out,excludeInfl);

	//	}
	//	if(pTrg)
	//		out.push_back(pTrg);
	//}
}

/** Remove an Evaluable from the up/down heirarchy.
	@param pEval Pointer to Evaluable object to remove.
*/
void SubmodelPort::RemoveEval(Evaluable* pEval)
{
	//ensure that object was removed
	RemoveDownObject(pEval);
	RemoveUpObject(pEval);
	
}

/** Remove Influences from evaluation linkages. This function is
meant to work in tandem with Model::PurgeInfluences().
	
*/
void SubmodelPort::PurgeInfluences()
{
	PurgeCollInfluences(m_omSources,m_omPSources,true);
	PurgeCollInfluences(m_omTargets,m_omPTargets,false);

	//names will need to be modified as well since new instances will not be able to find non-existant influences
	PurgeCollInfluences(m_smSources,m_smPSources,true);
	PurgeCollInfluences(m_smTargets,m_smPTargets,false);

	
}

/** Comparison operator.
	@param rhs The SubmodelPort to compare values against.
	@return true If SubmodelPort values are equal; false otherwise.
*/
bool SubmodelPort::operator==(const SubmodelPort & rhs)
{
	return this==&rhs ||
		(
		Evaluable::operator==(rhs) &&
		(m_pOuterModel == rhs.m_pOuterModel)
		);
}

void SubmodelPort::RemoveUpObject  ( Evaluable *pObject )
{
	Evaluable::RemoveUpObject(pObject);
	RemoveFromDependencies(pObject);
}

void SubmodelPort::RemoveDownObject( Evaluable *pObject )
{
	Evaluable::RemoveDownObject(pObject);
	RemoveFromDependencies(pObject);
}

/** Remove any and all references to pEval from the SubmodelPort.
	@param pEval Pointer to an Evaluable object to remove from internal references.
*/
void SubmodelPort::RemoveFromDependencies(Evaluable *pEval)
{
	RemoveSubmodelSource(pEval);
	RemoveSubSourceID(pEval->GetID());
	RemoveSubmodelTarget(pEval);
	RemoveSubTargetID(pEval->GetID());

	RemoveOutmodelSource(pEval);
	RemoveOutSourceID(pEval->GetID());
	RemoveOutmodelTarget(pEval);
	RemoveOutTargetID(pEval->GetID());
}

OME_SCALAR SubmodelPort::GetValue() const
{
	return m_pOuterModel->GetCurrentInstanceVal(m_mdlIndex);
}

const OME_SCALAR* SubmodelPort::GetValueAddr()
{
	return m_pOuterModel->GetCurrentInstanceValAddr(m_mdlIndex);
}

void SubmodelPort::SetValue(const OME_SCALAR val)
{
	m_pOuterModel->GetCurrentInstanceVal(m_mdlIndex)=val;
}

void SubmodelPort::Detach()
{
	if (m_pOuterModel)
		m_pOuterModel->RemoveSubPort(this);
	Evaluable::Detach();
}

/** Test to see if SubmodelPort is linking a Flow hierarchy.
	@return true if SubmodelPort links Flows, false otherwise.
*/
bool SubmodelPort::LinkedToFlow() const
{
	size_t i;
	for (i = 0; i < m_smPSources.size(); ++i)
		if (m_smPSources[i]->GetOMEType() == OME_FLOW)
			return true;

	for (i = 0; i < m_smPTargets.size(); ++i)
		if (m_smPTargets[i]->GetOMEType() == OME_FLOW)
			return true;

	for (i = 0; i < m_omPSources.size(); ++i)
		if (m_omPSources[i]->GetOMEType() == OME_FLOW)
			return true;

	for (i = 0; i < m_omPTargets.size(); ++i)
		if (m_omPTargets[i]->GetOMEType() == OME_FLOW)
			return true;

	return false;
}

void SubmodelPort::AddSubSourceID(STLString src)
{
	AddID(m_smSources, src);
}

void SubmodelPort::AddSubTargetID(STLString trg)
{
	AddID(m_smTargets, trg);
}

void SubmodelPort::RemoveSubSourceID(STLString src)
{
	RemoveID(m_smSources, src);
}

void SubmodelPort::RemoveSubTargetID(STLString trg)
{
	RemoveID(m_smTargets, trg);
}


void SubmodelPort::AddOutSourceID(STLString src)
{
	AddID(m_omSources, src);
}


void SubmodelPort::AddOutTargetID(STLString trg)
{
	AddID(m_omTargets, trg);
}

void SubmodelPort::RemoveOutSourceID(STLString src)
{
	RemoveID(m_omSources, src);
}

void SubmodelPort::RemoveOutTargetID(STLString trg)
{
	RemoveID(m_omTargets, trg);
}

void SubmodelPort::GetSubIDs(StrArray & out)
{
	out.insert(out.end(), m_smSources.begin(), m_smSources.end());
	out.insert(out.end(), m_smTargets.begin(), m_smTargets.end());
}

void SubmodelPort::GetOutIDs(StrArray & out)
{
	out.insert(out.end(), m_omSources.begin(), m_omSources.end());
	out.insert(out.end(), m_omTargets.begin(), m_omTargets.end());
}

PORT_DIR SubmodelPort::UpdateDirection()
{
	m_direction = PD_NONE;
	if ((!m_smTargets.empty() || !m_omSources.empty()) && m_omTargets.empty() && m_smSources.empty())
		m_direction = PD_INBOUND;
	else if ((!m_smSources.empty() || !m_omTargets.empty()) && m_omSources.empty() && m_smTargets.empty())
		m_direction = PD_OUTBOUND;

	return m_direction;
}
