#include "EvalAlias.h"

void EvalAlias::GetXMLAttributes(StrArray & out) const
{
	AddAttributeValue<STLString>("id", m_id, out);
	AddAttributeValue<STLString>("alias", m_name, out);
	AddAttributeValue<STLString>("source", m_srcID, out);

}

void EvalAlias::GetXMLAttributeMap(XMLAttrVector & out)
{
	XmlAttr attrs[] = {
		// attr				type		   address                          isReq
		{ "id", XML_STR, &(this->m_id), true },
		{ "alias", XML_STR, &(this->m_name), true },
		{ "source", XML_STR, &(this->m_srcID), true },
		{ NULL, XML_NULL, NULL, false } };

	//drop null
	int count;
	for (count = 0; attrs[count].name != NULL; count++)
		out.push_back(attrs[count]);
}

const OMEChar* EvalAlias::ToString(const OMEChar tabChar, const unsigned int indent, const unsigned int inc) const
{
	oSTLSStream buff;
	buff << "Address: " << this << std::endl;
	buff << "Type: " << LabelForType() << std::endl;
	STLString head(indent, tabChar);

	buff << head << "Alias of '" << m_pSrcEval->GetName() << "'" << endl;
	char* outStr = new char[buff.str().length() + 1];
	int i;
	for (i = 0; i < buff.str().length(); i++)
		outStr[i] = buff.str()[i];
	outStr[i] = '\0';
	return outStr;
}