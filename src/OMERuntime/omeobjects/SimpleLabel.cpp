
#include "omeobjects/SimpleLabel.h"
#include <regex>

void SimpleLabel::GetXMLAttributeMap(XMLAttrVector & out)
{
		XmlAttr attrs[] = {
		// attr					type		   address                          isReq
		{ "text",				XML_STR,	&(this->m_text),				true  },
		{ NULL,					XML_NULL,		NULL,							false } };

		//drop null
	 int count;
	 for(count=0; attrs[count].name!=NULL; count++)
		  out.push_back(attrs[count]);


	OMEObject::GetXMLAttributeMap(out);
}

int SimpleLabel::PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
{
	return OMEObject::PopulateFromComplexElements(pCurrElem,tag,filename);
}

STLString SimpleLabel::GetXMLRepresentation(const unsigned int indent, const unsigned int inc)
{
	return GetXMLForObject(indent,inc);
}

void SimpleLabel::GetXMLAttributes(StrArray & out) const
{
	using namespace std;

	//base object attrs
	OMEObject::GetXMLAttributes(out);
	STLString qc="\"";
   if(m_text.find(qc)!=std::string::npos)
		qc="'";
   AddAttributeValue("text",qc+m_text+qc,out);
	
}

bool SimpleLabel::operator==(const SimpleLabel & rhs)
{
	return this==&rhs ||
		(
		OMEObject::operator==(rhs) &&
		m_text==rhs.m_text
		);
}