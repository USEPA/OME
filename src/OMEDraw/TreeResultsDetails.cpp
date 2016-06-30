#include "TreeResultsDetails.h"

TreeResultsDetails::TreeResultsDetails()
:BaseResultsDetails(), m_useSelected(false), m_sortMode(CFIRST), m_sortAsc(true)
{
	
}

TreeResultsDetails::~TreeResultsDetails()
{

}

STLString TreeResultsDetails::GetXMLRepresentation(const unsigned int indent,const unsigned int inc)
{
	return BaseResultsDetails::GetXMLRepresentation(indent,inc);
}

void TreeResultsDetails::GetXMLAttributes(StrArray & out) const
{
	BaseResultsDetails::GetXMLAttributes(out);
	if(m_useSelected)
		XMLLoadable::AddAttributeValue("selected_only",1,out);
	
	XMLLoadable::AddAttributeValue("sort_dir", m_sortAsc ? 1 : 0, out);
	XMLLoadable::AddAttributeValue("sort_mode", (int)m_sortMode, out);
}

void TreeResultsDetails::GetXMLSubNodes(StrArray & out,const unsigned int indent,const unsigned int inc)
{
	BaseResultsDetails::GetXMLSubNodes(out,indent,inc);
}

void TreeResultsDetails::GetXMLAttributeMap(XMLAttrVector & out)
{
	BaseResultsDetails::GetXMLAttributeMap(out);
	XmlAttr attrs[]={
			// attr				type		   address				isReq 
	  { "selected_only",		XML_BOOL,	&(this->m_useSelected),	false },
	  { "sort_dir",				XML_BOOL,	&(this->m_sortAsc),		false },
	  { "sort_mode",			XML_INT,	&(this->m_sortMode),	false },
	  { NULL,					XML_NULL,	NULL,					false }

	};
	
	for (unsigned short i = 0; attrs[i].type != XML_NULL;++i)
		out.push_back(attrs[i]);
	
}

int TreeResultsDetails::PopulateFromComplexElements(TI2Element* pCurrElem, const STLString & tag,const STLString & filename)
{
	return BaseResultsDetails::PopulateFromComplexElements(pCurrElem,tag,filename);
}

void TreeResultsDetails::AddFieldXMLAttrs(oSTLSStream & outStrm,const size_t & ind)
{
	BaseResultsDetails::AddFieldXMLAttrs(outStrm,ind);
}

void TreeResultsDetails::GetFieldXMLAttrs(TI2Element* pElem)
{
	BaseResultsDetails::GetFieldXMLAttrs(pElem);
}