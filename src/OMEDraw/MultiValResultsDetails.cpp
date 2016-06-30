#include "MultiValResultsDetails.h"

MultiValResultsDetails::MultiValResultsDetails()
	:BaseResultsDetails(), m_useSelected(false)
{

}

MultiValResultsDetails::~MultiValResultsDetails()
{

}

STLString MultiValResultsDetails::GetXMLRepresentation(const unsigned int indent, const unsigned int inc)
{
	return BaseResultsDetails::GetXMLRepresentation(indent, inc);
}

void MultiValResultsDetails::GetXMLAttributes(StrArray & out) const
{
	BaseResultsDetails::GetXMLAttributes(out);
	if (m_useSelected)
		XMLLoadable::AddAttributeValue("selected_only", 1, out);
	
}

void MultiValResultsDetails::GetXMLSubNodes(StrArray & out, const unsigned int indent, const unsigned int inc)
{
	BaseResultsDetails::GetXMLSubNodes(out, indent, inc);
}

void MultiValResultsDetails::GetXMLAttributeMap(XMLAttrVector & out)
{
	BaseResultsDetails::GetXMLAttributeMap(out);
	XmlAttr attrs[] = {
		// attr				type		   address				isReq 
		{ "selected_only", XML_BOOL, &(this->m_useSelected), false },
		{ NULL, XML_NULL, NULL, false }

	};

	for (unsigned short i = 0; attrs[i].type != XML_NULL; ++i)
		out.push_back(attrs[i]);
}

int MultiValResultsDetails::PopulateFromComplexElements(TI2Element* pCurrElem, const STLString & tag, const STLString & filename)
{
	return BaseResultsDetails::PopulateFromComplexElements(pCurrElem, tag, filename);
}

void MultiValResultsDetails::AddFieldXMLAttrs(oSTLSStream & outStrm, const size_t & ind)
{
	BaseResultsDetails::AddFieldXMLAttrs(outStrm, ind);
}

void MultiValResultsDetails::GetFieldXMLAttrs(TI2Element* pElem)
{
	BaseResultsDetails::GetFieldXMLAttrs(pElem);
}