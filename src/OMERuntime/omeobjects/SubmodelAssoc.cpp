#include "omeobjects/SubmodelAssoc.h"
#include "omeobjects/Model.h"
#include "omeobjects/AssocPort.h"

/** Default Constructor. */
SubmodelAssoc::SubmodelAssoc()
{
}

/** Default Destructor. */
SubmodelAssoc::~SubmodelAssoc()
{
}

/** Bind submodels to SubmodelAssoc.
	@param pBase Pointer to source Model; can be NULL.
	@param pSub Pointer to target Model; can be NULL.
*/
void SubmodelAssoc::BindSubmodels(Model* pBase, Model* pSub)
{
	Detach();
	
	if(pBase)
		pBase->AddBaseMCAssoc(this);
	if(pSub)
		pSub->AddSubMCAssoc(this);

	m_pSource=pBase;
	m_pTarget=pSub;

	m_source = m_pSource->GetID();
	m_target = m_pTarget->GetID();
}

/** Bind AssocPort and Model to SubmodelAssoc.
	@param pBase Pointer to source AssocPort; can be NULL.
	@param pSub Pointer to target Model; can be NULL.
*/
void SubmodelAssoc::BindSubmodels(AssocPort* pBase, Model* pSub)
{
	Detach();

	if (pSub)
		pSub->AddSubMCAssoc(this);

	m_pSource = pBase;
	m_pTarget = pSub;

	m_source = m_pSource->GetID();
	m_target = m_pTarget->GetID();

}

/** Bind Model and AssocPort to SubmodelAssoc.
	@param pBase Pointer to source Model; can be NULL.
	@param pSub Pointer to target AssocPort; can be NULL.
*/
void SubmodelAssoc::BindSubmodels(Model* pBase, AssocPort* pSub)
{
	Detach();

	if (pBase)
		pBase->AddBaseMCAssoc(this);

	m_pSource = pBase;
	m_pTarget = pSub;

	m_source = m_pSource->GetID();
	m_target = m_pTarget->GetID();
}

/** Bind AssocPorts to SubmodelAssoc.
	@param pBase Pointer to source AssocPort; can be NULL.
	@param pSub Pointer to target AssocPort; can be NULL.
*/
void SubmodelAssoc::BindSubmodels(AssocPort* pBase, AssocPort* pSub)
{
	Detach();

	m_pSource = pBase;
	m_pTarget = pSub;

	m_source = m_pSource->GetID();
	m_target = m_pTarget->GetID();
}

/** Bind Models and/or AssocPorts to SubmodelAssoc.
	@param pBase Pointer to source AssocPort or Model; can be NULL.
	@param pSub Pointer to target AssocPort or Model; can be NULL.
*/
void SubmodelAssoc::BindSubmodels(OMEObject* pBase, OMEObject* pSub)
{
	OMETYPE baseType = pBase->GetOMEType();
	OMETYPE subType = pSub->GetOMEType();

	if (baseType == OME_MODEL && subType == OME_MODEL)
		BindSubmodels((Model*)pBase, (Model*)pSub);
	else if (baseType == OME_MODEL && subType == OME_ASSOCPORT)
		BindSubmodels((Model*)pBase, (AssocPort*)pSub);
	else if (baseType == OME_ASSOCPORT && subType == OME_MODEL)
		BindSubmodels((AssocPort*)pBase, (Model*)pSub);
	else if (baseType == OME_ASSOCPORT && subType == OME_ASSOCPORT)
		BindSubmodels((AssocPort*)pBase, (AssocPort*)pSub);
	else
		throw OMEFuncOpException("SubmodelAssoc::BindSubmodels","Base or Sub object is not of a valid type.");
}

/** Use ids to join with the appropiate Models.
	@param pRoot Pointer to the BaseModel that contains the Models.
*/
void SubmodelAssoc::JoinFromReferences(Model* pRoot)
{
	Model* pMdl;
	AssocPort* pSmp;
	OMEObject* found;
	if (!m_source.empty())
	{
		found = pRoot->ObjectForKey(m_source);
		
		if ((pMdl = dynamic_cast<Model*>(found)))
		{
			pMdl->AddBaseMCAssoc(this);
			m_pSource = pMdl;
		}
		else if ((pSmp = dynamic_cast<AssocPort*>(found)))
		{
			m_pSource = pSmp;
		}
	}
	if (!m_target.empty())
	{
		found = pRoot->ObjectForKey(m_target);
		if ((pMdl = dynamic_cast<Model*>(found)))
		{
			pMdl->AddSubMCAssoc(this);
			m_pTarget = pMdl;
		}
		else  if ((pSmp = dynamic_cast<AssocPort*>(found)))
		{
			m_pTarget = pSmp;
		}
	}
}

/** Copy a SubmodelAssoc.
	@param sma The SubmodelAssoc to copy.
*/
void SubmodelAssoc::Duplicate(const SubmodelAssoc & sma)
{
	BindSubmodels((Model*)sma.m_pSource, (Model*)sma.m_pTarget);
}

/** Find the number of instances associated with the base/source Model.
	@return The number of instances if a base ModelCollection is present; otherwise -1.
*/
int SubmodelAssoc::GetBaseNumInstances()
{
	int ret=-1;

	Model* pMdl = dynamic_cast<Model*>(m_pSource);
	AssocPort* pSmp;
	if(pMdl)
		ret=pMdl->GetInitNumInstances();
	else if ((pSmp = dynamic_cast<AssocPort*>(m_pSource)))
	{
		SubmodelAssoc* pAssoc = dynamic_cast<SubmodelAssoc*>(pSmp->GetInAssoc(0));
		ret = pAssoc->GetBaseNumInstances();
	}

	return ret;
}

STLString SubmodelAssoc::GetXMLRepresentation(const unsigned int indent, const unsigned int inc)
{
	return GetXMLForObject(indent,inc);
}

void SubmodelAssoc::GetXMLAttributes(StrArray & out) const
{

	OMEObject::GetXMLAttributes(out);

	if(!m_source.empty())
		out.push_back(STLString("in_object=\"")+m_source+"\"");
	if(!m_target.empty())
		out.push_back(STLString("out_object=\"")+m_target+"\"");
}

void SubmodelAssoc::GetXMLAttributeMap(XMLAttrVector & out)
{
	OMEObject::GetXMLAttributeMap(out);
	XmlAttr attrs[] = {
      // attr				type		   address                          isReq
	  { "in_object",		XML_STR,	&(this->m_source),				true },
	  { "out_object",		XML_STR,	&(this->m_target),				true },
      { NULL,				XML_NULL,	NULL,							false } };

	  //drop null
	 int count;
	 for(count=0; attrs[count].name!=NULL; count++)
		  out.push_back(attrs[count]);

}

/** Check if Model is the base.
@param pMc Pointer to the Model to test.
@return true if pMc is the base model; false otherwise.
*/
bool SubmodelAssoc::IsBase(const Model* pMc) const 
{ 
	bool ret = ((Model*)m_pSource) == pMc;
	AssocPort* pPort;
	if ((pPort = dynamic_cast<AssocPort*>(m_pSource)))
		ret = ((SubmodelAssoc*)pPort->GetInAssoc(0))->IsBase(pMc);
	return ret;
};

/** Check if Model is the sub.
@param pMc Pointer to the Model to test.
@return true if pMc is the sub model; false otherwise.
*/
bool SubmodelAssoc::IsSub(const Model* pMc) const
{
	bool ret = ((Model*)m_pTarget) == pMc;
	AssocPort* pPort;
	if ((pPort = dynamic_cast<AssocPort*>(m_pTarget)))
		ret = ((SubmodelAssoc*)pPort->GetOutAssoc(0))->IsSub(pMc);
	return ret;
}

/** Determine if a Model is linked to a SubmodelAssoc.
	@param pMc The Model to check for.
	@return true if pMc is either the source or target for the SubmodelAssoc.
*/
bool SubmodelAssoc::IsAssociated(const Model* pMc) const
{
	bool ret = pMc!=NULL;
	if (ret)
	{
		AssocPort* pPort;
		if ((pPort = dynamic_cast<AssocPort*>(m_pSource)))
			ret = ((SubmodelAssoc*)pPort->GetInAssoc(0))->WalkAssociated(pMc,false);
		else if ((pPort = dynamic_cast<AssocPort*>(m_pTarget)))
			ret = ((SubmodelAssoc*)pPort->GetOutAssoc(0))->WalkAssociated(pMc,true);

		if (!ret)
			ret= (pMc == dynamic_cast<Model*>(m_pSource) || pMc == dynamic_cast<Model*>(m_pTarget));
	}
	return ret; 
}

/** Walk along an AssocModel heirarchy, looking for a specific model.
	@param pMc Pointer to the Model to search for.
	@param fromSrc If true, walk along the direction of the SubmodelAssoc. Otherwise, walk the opposite direction.
	@return true if pMc is found during walk, false otherwise.
*/
bool SubmodelAssoc::WalkAssociated(const Model* pMc, const bool & fromSrc)
{
	bool ret = false;
	AssocPort* pPort;
	if (!fromSrc)
	{
		if ((pPort = dynamic_cast<AssocPort*>(m_pSource)))
			ret = ((SubmodelAssoc*)pPort->GetInAssoc(0))->WalkAssociated(pMc, false);
		else
			ret = pMc == dynamic_cast<Model*>(m_pSource);
	}
	else
	{
		if ((pPort = dynamic_cast<AssocPort*>(m_pTarget)))
			ret = ((SubmodelAssoc*)pPort->GetOutAssoc(0))->WalkAssociated(pMc, true);
		else
			ret = pMc == dynamic_cast<Model*>(m_pTarget);
	}

	return ret;
}

/** Get base model instance associated with target model instance.
	@param bm Pointer to the target model instance.
	@return Pointer to the appropriate source model instance.
*/
ModelInstance* SubmodelAssoc::GetBaseModelInstance(Model* bm) const
{
	ModelInstance* ret=NULL;

	/// \todo update for new instance stuff
	//check for valid instance identifier
	//if(bm->m_instance>=0)
	//{
	//	int index= bm->GetAssocIndex(this);

	//	//find parent ModelCollection
	//	ModelInstance* pParentMdl=bm->GetSpawningParent();
	//	if(pParentMdl && pParentMdl==m_pSubMdl)
	//		ret=m_pBaseMdl->GetInstance(index);
	//}
	return ret;
}

/** @return Pointer to the Assoc's source Model.*/
Model* SubmodelAssoc::GetBaseModel()
{ 
	Model* ret = dynamic_cast<Model*>(m_pSource);
	AssocPort *pPort;
	if ((pPort = dynamic_cast<AssocPort*>(m_pSource)))
		ret = ((SubmodelAssoc*)pPort->GetInAssoc(0))->GetBaseModel();
	return ret;
}

/** @return Pointer to the Assoc's target Model.*/
Model* SubmodelAssoc::GetSubModel()
{ 
	Model* ret = dynamic_cast<Model*>(m_pTarget);
	AssocPort *pPort;
	if ((pPort = dynamic_cast<AssocPort*>(m_pTarget)))
		ret = ((SubmodelAssoc*)pPort->GetOutAssoc(0))->GetSubModel();
	return ret;
}

/** Update cache of EvalCollections referenced by the SubmodelAssoc. */
void SubmodelAssoc::RefreshCollectionCache()
{
	/// \todo update for new instance handling
	//m_subECMap.clear();

	//if(m_pSubMdl)
	//{
	//	ModelArray instances=m_pSubMdl->GetInstancesForAssoc(this);

	//	EvalArray toReg;
	//	m_pSubMdl->GetVarsToRegister(toReg, false);
	//	int fieldCount=toReg.size();
	//	int currAssocInd;
	//	STLString currName;
	//	
	//	if(!instances.empty())
	//		{
	//		//#pragma omp parallel for shared(m_instances)
	//	
	//		for(int i=0, currInd=0; i<fieldCount; i++)
	//		{
	//		
	//			if(toReg[i]->GetOMEType()!=OME_SUBPORT)
	//			{
	//				//set a reserve size here?
	//				currName=toReg[i]->GetName();
	//				m_subECMap[currName]=EvalCollections();

	//				EvalArray valInstances;

	//				//#pragma omp parallel for
	//				int deadOff=0;
	//				int adjustedOff=0;
	//				currAssocInd=instances[0]->GetAssocIndex(this);
	//				for(int j=0; j < instances.size() && adjustedOff<instances.size(); j++)
	//				{
	//					if(currAssocInd!=instances[j]->GetAssocIndex(this))
	//					{
	//						m_subECMap[currName].push_back(EvalCollection(currName,valInstances,m_pBaseMdl));
	//						valInstances.clear();
	//						currAssocInd=instances[j]->GetAssocIndex(this);
	//					}
	//					//grab value from instances starting from offset
	//					while((j+deadOff) < instances.size() && instances[j+deadOff]==NULL )
	//						deadOff++;
	//					adjustedOff=j+deadOff;
	//					if(adjustedOff<instances.size())
	//						valInstances.push_back((Evaluable*)instances[j+deadOff]->ObjectForKey(toReg[i]->m_id));
	//				}
	//		
	//				m_subECMap[currName].push_back(EvalCollection(currName,valInstances,m_pBaseMdl));
	//			}
	//			currInd++;
	//		}
	//	}
	//}
}

/** Disconnect SubmodelAssoc from source and target ModelCollections. */
void SubmodelAssoc::Detach()
{
	Model* pMdl;
	if((pMdl=dynamic_cast<Model*>(m_pSource)))
		pMdl->RemoveBaseMCAssoc(this);
	if((pMdl=dynamic_cast<Model*>(m_pTarget)))
		pMdl->RemoveSubMCAssoc(this);
}

void SubmodelAssoc::SetPSource(OMEObject* src)
{
	if (!dynamic_cast<Model*>(src) && !dynamic_cast<AssocPort*>(src))
		throw OMEFuncOpException("SubmodelAssoc", (STLString("cannot link with type ") + src->StrForType()).c_str());
	Link::SetPSource(src);
}

void SubmodelAssoc::SetPTarget(OMEObject* trg)
{
	if (!dynamic_cast<Model*>(trg) && !dynamic_cast<AssocPort*>(trg))
		throw OMEFuncOpException("SubmodelAssoc",(STLString("cannot link with type ") + trg->StrForType()).c_str());
	Link::SetPSource(trg);
}
