#include "omeobjects/AssocPort.h"
#include "omeobjects/OMEObject.h"
#include "omeobjects/Model.h"

/** Comparison operator.
@param rhs The AsscocPort to compare values against.
@return true If AssocPort values are equal; false otherwise.
*/
bool AssocPort::operator==(const AssocPort & rhs)
{
	return this == &rhs ||
		(
		OMEObject::operator==(rhs) &&
		m_pOuterModel == rhs.m_pOuterModel
		);
}

/** Add an inbound SubmodelAssoc orginating from the inner submodel.
	@param pSrc Pointer to the SubmodelAssoc to add.
	@param useID If true, associate the SubmodelAssoc id with this AssocPort.
*/
void AssocPort::AddSubInAssoc(SubmodelAssoc* pSrc, const bool useID)
{
	if (ObjectPresent<SubmodelAssoc*>(pSrc, m_smInAssocs) < 0)
	{
		m_smInAssocs.push_back(pSrc);
		if (useID)
			AddSubSourceID(pSrc->GetID());

	}
}

/** Add an outbound SubmodelAssoc orginating from the inner submodel.
	@param pTrg Pointer to the SubmodelAssoc to add.
	@param useID If true, associate the SubmodelAssoc id with this AssocPort.
*/
void AssocPort::AddSubOutAssoc(SubmodelAssoc* pTrg, const bool useID)
{
	if (ObjectPresent<SubmodelAssoc*>(pTrg, m_smOutAssocs) < 0)
	{
		m_smOutAssocs.push_back(pTrg);
		if (useID)
			AddSubTargetID(pTrg->GetID());

	}
}

void AssocPort::SetParentModel(Model*mdl)
{
	if (OMEObject::m_pParentModel)
	{
		//prevent stack overflow by setting m_pParentModel to NULL ahead of time;
		Model* pParent = OMEObject::m_pParentModel;
		OMEObject::m_pParentModel = NULL;
		RemoveSubmodel(pParent);
	}

	OMEObject::SetParentModel(mdl);
}

/** Remove an inbound SubmodelAssoc orginating from the inner submodel.
	@param pSrc Pointer to the SubmodelAssoc to remove.
*/
void AssocPort::RemoveSubInAssoc(SubmodelAssoc* pSrc)
{
	RemoveAssoc(m_smInAssocs, pSrc);
}

/** Remove an outbound SubmodelAssoc orginating from the inner submodel.
	@param pTrg Pointer to the SubmodelAssoc to remove.
*/
void AssocPort::RemoveSubOutAssoc(SubmodelAssoc* pTrg)
{
	RemoveAssoc(m_smOutAssocs, pTrg);
}

void AssocPort::RemoveSubmodel(Model* mdl)
{
	if (mdl)
	{
		AssocArray::iterator itr;
		itr = m_smOutAssocs.begin();
		while (itr != m_smOutAssocs.end())
		{
			if ((*itr)->GetParentModel() == mdl)
				itr = m_smOutAssocs.erase(itr);
			else
				++itr;
		}

		//parse all up objects

		itr = m_smInAssocs.begin();
		while (itr != m_smInAssocs.end())
		{
			if ((*itr)->GetParentModel() == mdl)
				itr = m_smInAssocs.erase(itr);
			else
				++itr;
		}
		//mdl->RemoveSubPort(this);
	}
}

/** Add an inbound SubmodelAssoc orginating from the outer model.
	@param pSrc Pointer to the SubmodelAssoc to add.
	@param useID If true, associate the SubmodelAssoc id with this AssocPort.
*/
void AssocPort::AddOutInAssoc(SubmodelAssoc* pSrc, const bool useID)
{
	if (ObjectPresent<SubmodelAssoc*>(pSrc, m_omInAssocs) < 0)
	{
		m_omInAssocs.push_back(pSrc);
		if (useID)
			AddOutSourceID(pSrc->GetID());
	}
}

/** Add an outbound SubmodelAssoc orginating from the outer model.
	@param pTrg Pointer to the SubmodelAssoc to add.
	@param useID If true, associate the SubmodelAssoc id with this AssocPort.
*/
void AssocPort::AddOutOutAssoc(SubmodelAssoc* pTrg, const bool useID)
{
	if (ObjectPresent<SubmodelAssoc*>(pTrg, m_omOutAssocs) < 0)
	{
		m_omOutAssocs.push_back(pTrg);
		if (useID)
			AddSubTargetID(pTrg->GetID());

	}
}

/** Remove an inbound SubmodelAssoc orginating from the outer model.
	@param pSrc Pointer to the SubmodelAssoc to remove.
*/
void AssocPort::RemoveOutInAssoc(SubmodelAssoc* pSrc)
{
	RemoveAssoc(m_omInAssocs, pSrc);
}

/** Remove an outbound SubmodelAssoc orginating from the outer model.
	@param pTrg Pointer to the SubmodelAssoc to remove.
*/
void AssocPort::RemoveOutOutAssoc(SubmodelAssoc* pTrg)
{
	RemoveAssoc(m_omOutAssocs, pTrg);
}

/** Retrieve a pointer to an inbound SubmodelAssoc.
	@param ind The index of the SubmodelAssoc to retrieve. Inner model SubmodelAssocs are listed before outer mode SubmodelAssocs.
	@return Pointer to SubmodelAssoc at ind, or NULL if ind is an invalid index.
*/
SubmodelAssoc* AssocPort::GetInAssoc(const int ind) const
{
	SubmodelAssoc* pAssoc = NULL;
	if (ind < m_smInAssocs.size())
		pAssoc = m_smInAssocs[ind];
	else
		pAssoc = m_omInAssocs[ind - m_smInAssocs.size()];

	return pAssoc;
}

/** Retrieve a pointer to an outbound SubmodelAssoc.
	@param ind The index of the SubmodelAssoc to retrieve. Inner model SubmodelAssocs are listed before outer mode SubmodelAssocs.
	@return Pointer to SubmodelAssoc at ind, or NULL if ind is an invalid index.
*/
SubmodelAssoc* AssocPort::GetOutAssoc(const int ind) const
{
	SubmodelAssoc* pAssoc = NULL;
	if (ind < m_smOutAssocs.size())
		pAssoc = m_smOutAssocs[ind];
	else
		pAssoc = m_omOutAssocs[ind - m_smOutAssocs.size()];

	return pAssoc;
}


STLString AssocPort::GetXMLRepresentation(const unsigned int indent, const unsigned int inc)
{
	return OMEObject::GetXMLForObject(indent, inc);
}

void AssocPort::GetXMLAttributes(StrArray & out) const
{
	OMEObject::GetXMLAttributes(out);
}

void AssocPort::GetXMLSubNodes(StrArray & out, const unsigned int indent, const unsigned int inc)
{
	int i;
	STLString baseIndent(indent, ' '), subIndent(indent + inc, ' ');

	OMEObject::GetXMLSubNodes(out, indent, inc);

	oSTLSStream outStrm;
	//sub sources
	if (!m_smInAssocs.empty())
	{
		outStrm << baseIndent << "<subsources>" << std::endl;
		for (i = 0; i<m_smInAssocs.size(); i++)
			outStrm << subIndent << "<src name=\"" << m_smInAssocs[i] << "\"/>" << std::endl;
		outStrm << baseIndent << "</subsources>";
		out.push_back(outStrm.str());
		outStrm.clear();
		outStrm.str(STLString());
	}

	//sub targets
	if (!m_smOutAssocs.empty())
	{
		outStrm << baseIndent << "<subtargets>" << std::endl;
		for (i = 0; i<m_smOutAssocs.size(); i++)
			outStrm << subIndent << "<trg name=\"" << m_smOutAssocs[i] << "\"/>" << std::endl;
		outStrm << baseIndent << "</subtargets>";
		out.push_back(outStrm.str());
		outStrm.clear();
		outStrm.str(STLString());
	}

	//outer sources
	if (!m_omInAssocs.empty())
	{
		outStrm << baseIndent << "<outsources>" << std::endl;
		for (i = 0; i<m_omInAssocs.size(); i++)
			outStrm << subIndent << "<src name=\"" << m_omInAssocs[i] << "\"/>" << std::endl;
		outStrm << baseIndent << "</outsources>";
		out.push_back(outStrm.str());
		outStrm.clear();
		outStrm.str(STLString());
	}

	//outer targets
	if (!m_omOutAssocs.empty())
	{
		outStrm << baseIndent << "<outtargets>" << std::endl;
		for (i = 0; i<m_omOutAssocs.size(); i++)
			outStrm << subIndent << "<trg name=\"" << m_omOutAssocs[i] << "\"/>" << std::endl;
		outStrm << baseIndent << "</outtargets>";
		out.push_back(outStrm.str());
		//outStrm.clear();
	}

}

/** Check to see if a SubmodelAssoc lies within the inner submodel.
	@param sa Pointer to SubmodelAssoc to query for.
	@return true if sa resides in the inner submodel; false otherwise.
*/
bool AssocPort::IsSubAssoc(SubmodelAssoc* sa)
{
	return ((ObjectPresent(sa, m_smOutAssocs) >= 0) || (ObjectPresent(sa, m_smInAssocs) >= 0));
}

/** Check to see if a SubmodelAssoc lies within the outer model.
	@param sa Pointer to SubmodelAssoc to query for.
	@return true if sa resides in the outer model; false otherwise.
*/
bool AssocPort::IsOutAssoc(SubmodelAssoc* sa)
{
	return ((ObjectPresent(sa, m_omOutAssocs) >= 0) || (ObjectPresent(sa, m_omInAssocs) >= 0));
}

void AssocPort::SyncWithParents()
{
	if (m_pOuterModel)
	{
		std::for_each(m_omSources.begin(), m_omSources.end(), [this](const STLString &key){AddOutInAssoc(dynamic_cast<SubmodelAssoc*>(m_pOuterModel->ObjectForKey(key)), false); });
		std::for_each(m_omTargets.begin(), m_omTargets.end(), [this](const STLString &key){AddOutOutAssoc(dynamic_cast<SubmodelAssoc*>(m_pOuterModel->ObjectForKey(key)), false); });
	}

	//avoid ambiguity
	Model* pParent = OMEObject::m_pParentModel;

	//find model collection
	if (pParent)
	{
		//grab objects from models for prioritization/relationship stuff
		std::for_each(m_smSources.begin(), m_smSources.end(), [this, &pParent](const STLString &key){AddSubInAssoc(dynamic_cast<SubmodelAssoc*>(pParent->ObjectForKey(key)), false); });
		std::for_each(m_smTargets.begin(), m_smTargets.end(), [this, &pParent](const STLString &key){AddSubOutAssoc(dynamic_cast<SubmodelAssoc*>(pParent->ObjectForKey(key)), false); });
	}

}

/** Copy another AssocPort
	@param ap the AssocPort to copy.
*/
void AssocPort::Duplicate(const AssocPort & ap)
{
	//duplicate parents
	m_pOuterModel = ap.m_pOuterModel;

	m_smSources.assign(ap.m_smSources.begin(), ap.m_smSources.end());
	m_smTargets.assign(ap.m_smTargets.begin(), ap.m_smTargets.end());

	m_omSources.assign(ap.m_omSources.begin(), ap.m_omSources.end());
	m_omTargets.assign(ap.m_omTargets.begin(), ap.m_omTargets.end());

	OMEObject::Duplicate(ap);
}

void AssocPort::GetXMLAttributeMap(XMLAttrVector & out)
{
	OMEObject::GetXMLAttributeMap(out);
}

int AssocPort::PopulateFromComplexElements(TI2Element* pCurrElem, const STLString & tag, const STLString & filename)
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


void AssocPort::AddSubSourceID(STLString src)
{
	AddID(m_smSources, src);
}

void AssocPort::AddSubTargetID(STLString trg)
{
	AddID(m_smTargets, trg);
}

void AssocPort::RemoveSubSourceID(STLString src)
{
	RemoveID(m_smSources, src);
}

void AssocPort::RemoveSubTargetID(STLString trg)
{
	RemoveID(m_smTargets, trg);
}

void AssocPort::AddOutSourceID(STLString src)
{
	AddID(m_omSources, src);
}

void AssocPort::AddOutTargetID(STLString trg)
{
	AddID(m_omTargets, trg);
}

void AssocPort::RemoveOutSourceID(STLString src)
{
	RemoveID(m_omSources, src);
}

void AssocPort::RemoveOutTargetID(STLString trg)
{
	RemoveID(m_omTargets, trg);
}

void AssocPort::GetSubIDs(StrArray & out)
{
	out.insert(out.end(), m_smSources.begin(), m_smSources.end());
	out.insert(out.end(), m_smTargets.begin(), m_smTargets.end());
}

void AssocPort::GetOutIDs(StrArray & out)
{
	out.insert(out.end(), m_omSources.begin(), m_omSources.end());
	out.insert(out.end(), m_omTargets.begin(), m_omTargets.end());
}

PORT_DIR AssocPort::UpdateDirection()
{
	m_direction = PD_NONE;
	if ((!m_smTargets.empty() || !m_omSources.empty()) && m_omTargets.empty() && m_smSources.empty())
		m_direction = PD_INBOUND;
	else if ((!m_smSources.empty() || !m_omTargets.empty()) && m_omSources.empty() && m_smTargets.empty())
		m_direction = PD_OUTBOUND;

	return m_direction;
}
