#include "TableResultsDetails.h"

TableResultsDetails::TableResultsDetails()
	:BaseResultsDetails(), m_showSigChange(false) 
{}

TableResultsDetails::~TableResultsDetails()
{

}

STLString TableResultsDetails::GetXMLRepresentation(const unsigned int indent,const unsigned int inc)
{
	return BaseResultsDetails::GetXMLRepresentation(indent,inc);
}

void TableResultsDetails::GetXMLAttributes(StrArray & out) const
{
	BaseResultsDetails::GetXMLAttributes(out);
	XMLLoadable::AddAttributeValue<int>("show_change",m_showSigChange,out);

}

void TableResultsDetails::GetXMLSubNodes(StrArray & out,const unsigned int indent,const unsigned int inc)
{
	BaseResultsDetails::GetXMLSubNodes(out,indent,inc);
}

void TableResultsDetails::GetXMLAttributeMap(XMLAttrVector & out)
{
	BaseResultsDetails::GetXMLAttributeMap(out);

	XmlAttr attrs[]= {
		  // attr				type		   address									isReq 
	  { "show_change",		XML_INT,	&(this->m_showSigChange),				false }
	};

	out.push_back(attrs[0]);
}

int TableResultsDetails::PopulateFromComplexElements(TI2Element* pCurrElem, const STLString & tag,const STLString & filename)
{
	return BaseResultsDetails::PopulateFromComplexElements(pCurrElem,tag,filename);
}

void TableResultsDetails::AddFieldXMLAttrs(oSTLSStream & outStrm,const size_t & ind)
{
	BaseResultsDetails::AddFieldXMLAttrs(outStrm,ind);
	auto fItr=m_changeThresholds.find(m_fields[ind].id);
	if(fItr!=m_changeThresholds.end())
		outStrm<<" threshold='"<<(*fItr).second<<"'";

	if (!m_fields[ind].selected.empty())
	{
		outStrm << " coords='";
		for (auto itr = m_fields[ind].selected.begin(); itr != m_fields[ind].selected.end(); ++itr)
			outStrm << ' ' << (*itr);
		outStrm << "'";
	}
}

void TableResultsDetails::GetFieldXMLAttrs(TI2Element* pElem)
{
	EvalRep rep;
	rep.id=pElem->Attribute("id");
	iSTLSStream instrm(pElem->Attribute("color"));
	instrm>>rep.color.r>>rep.color.g>>rep.color.b;

	if (pElem->Attribute("coords"))
	{
		//reuse input stream
		instrm.clear();
		instrm.str(pElem->Attribute("coords"));

		size_t currVal = 0;
		while (instrm >> currVal)
			rep.selected.push_back(currVal);
	}

	if(!rep.id.empty())
	{
		m_fields.push_back(rep);
		OME_SCALAR thresh;
		if (XMLVALQUERY(pElem,"threshold", &thresh) == tinyxml2::XML_NO_ERROR)
			m_changeThresholds[rep.id]=thresh;
	}
}

/** Set Coordinate for a given rep. 
	@param repInd The index of the field rep to modify.
	@param coordInd The index of the coordinate
	@param val The new coordinate.
*/
void TableResultsDetails::SetCoordForRep(int repInd, int coordInd, size_t val)
{
	if (repInd < m_fields.size() && coordInd < m_fields[repInd].selected.size())
		SINAssign(val, m_fields[repInd].selected[coordInd]);
}