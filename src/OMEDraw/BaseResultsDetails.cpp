#include "BaseResultsDetails.h"
#include "OMEDefines.h"

const OMEChar* BaseResultsDetails::FIELDGRP_TAG="fields";
const OMEChar* BaseResultsDetails::FIELD_TAG="field";

void BaseResultsDetails::GetXMLAttributes(StrArray & out) const
{
	XMLLoadable::AddAttributeValue("name",m_viewName,out);
}

void BaseResultsDetails::GetXMLSubNodes(StrArray & out,const unsigned int indent,const unsigned int inc)
{
	if(!m_fields.empty())
	{
		STLString baseIndent(indent,' ');
		STLString subIndent(indent+inc,' ');

		oSTLSStream outStrm;
		outStrm<<baseIndent<<'<'<<FIELDGRP_TAG<<'>'<<endl;
		for(size_t i=0; i<m_fields.size(); i++)
		{
			outStrm<<subIndent<<'<'<<FIELD_TAG<<' ';
			AddFieldXMLAttrs(outStrm,i);
			outStrm<<" />"<<endl;
		}
		outStrm<<baseIndent<<"</"<<FIELDGRP_TAG<<'>'<<endl;

		out.push_back(outStrm.str());
	}
}

void BaseResultsDetails::GetXMLAttributeMap(XMLAttrVector & out)
{
	XmlAttr attrs[]={
			// attr				type		   address				isReq 
	  { "name",		XML_STR,	&(this->m_viewName),				true }
	};

	out.push_back(attrs[0]);
}

int BaseResultsDetails::PopulateFromComplexElements(TI2Element* pCurrElem, const STLString & tag,const STLString & filename)
{
	TI2Element* pFields=pCurrElem->FirstChildElement(FIELDGRP_TAG);
	if(pFields)
	{
		TI2Element* pSub=pFields->FirstChildElement(FIELD_TAG);
		while(pSub)
		{
			GetFieldXMLAttrs(pSub);
			pSub=pSub->NextSiblingElement(FIELD_TAG);
		}
	}
	return 1;
}

/** Add a field's values to a buffer containing XML code.
	@param outStrm ostringstream object to append field details to.
	@param ind The index of the object's representation in m_fields.
*/
void BaseResultsDetails::AddFieldXMLAttrs(oSTLSStream & outStrm,const size_t & ind)
{

	outStrm<<"id='"<<XMLLoadable::XMLizeString(m_fields[ind].id)<<"' ";
	outStrm<<"color='"<<m_fields[ind].color.r<<':'<<m_fields[ind].color.g<<':'<<m_fields[ind].color.b<<"'";

}

/** Retrieve an object's representation from an xml element and append to m_fields.
	@param pElem Pointer to the element containing the field information.
*/
void BaseResultsDetails::GetFieldXMLAttrs(TI2Element* pElem)
{
	EvalRep rep;
	rep.id=pElem->Attribute("id");
	iSTLSStream instrm(pElem->Attribute("color"));
	STLString tmp;
	for (unsigned short i = 0; i < 3; ++i)
	{
		std::getline(instrm, tmp, ':');
		rep.color.rgb[i] = OME_SCLR_READSTR(tmp);
	}

	if(!rep.id.empty())
		m_fields.push_back(rep);
}

