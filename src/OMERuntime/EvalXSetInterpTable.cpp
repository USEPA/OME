#include "EvalXSetInterpTable.h"

/** Default Constructor. */
EvalXSetInterpTable::EvalXSetInterpTable()
:EvalInterpTable()
{}

/** Simple Constructor.
@param id The unique identifier for the table.
@param xData The data to use to initialize the x-axis ordinates.
@param yData The data to use to initialize the y-axis ordinates.
*/
EvalXSetInterpTable::EvalXSetInterpTable(const STLString & id, const DataArray &xData, const DataArray & yData)
: EvalInterpTable(id, yData), m_xPts(xData)
{
	if (!m_xPts.empty())
	{
		m_lBound = m_xPts.front();
		m_uBound = m_xPts.back();
	}
}

/** Detailed Constructor.
@param id The unique identifier for the table.
@param iMode The interpolation mode to use.
@param bMode The bounding mode to use.
@param xData The data to use to initialize the x-axis ordinates.
@param yData The data to use to initialize the y-axis ordinates.
@sa INTERPMODE,BOUNDSMODE.
*/
EvalXSetInterpTable::EvalXSetInterpTable(const STLString & id, const INTERPMODE & iMode, const BOUNDSMODE & bMode, const DataArray & xData, const DataArray & yData)
: EvalInterpTable(id, iMode, bMode, yData), m_xPts(xData)
{
	if (!m_xPts.empty())
	{
		m_lBound = m_xPts.front();
		m_uBound = m_xPts.back();
	}
}

/** Copy Constructor.
@param et The EvalXSetInterpTable to copy.
*/
EvalXSetInterpTable::EvalXSetInterpTable(const EvalXSetInterpTable & et)
: EvalInterpTable(et), m_xPts(et.m_xPts)
{}

/** XML Constructor.
@param pCurrElem Pointer to The current TI2Element.
@param tag The identifing tag.
@param filename Name of originating file.
*/
EvalXSetInterpTable::EvalXSetInterpTable(TI2Element* pCurrElem, const STLString & tag, const STLString & filename)
{
	PopulateFromXML(pCurrElem, tag, filename);
	if (!m_xPts.empty())
	{
		m_lBound = m_xPts.front();
		m_uBound = m_xPts.back();
	}
}

/** Assignment operator.
@param rhs The EvalXSetInterpTable to copy.
@return a reference to the calling EvalXSetInterpTable.
*/
EvalXSetInterpTable& EvalXSetInterpTable::operator=(const EvalXSetInterpTable & rhs)
{
	EvalInterpTable::operator=(rhs);
	m_xPts = rhs.m_xPts;
	return *this;
}

EvalXSetInterpTable::~EvalXSetInterpTable()
{}

OME_SCALAR EvalXSetInterpTable::GetInterpVal(OME_SCALAR inVal)
{
	//first, take care of out of bound issues.
	OME_SCALAR ret = 0.0;
	if (inVal < m_lBound || inVal > m_uBound)
	{
		bool isLower = inVal < m_lBound;
		switch (m_bounding)
		{
		case CLAMP:
			inVal = isLower ? m_lBound : m_uBound;
			break;
		case EXTRAP:
			ret = ExtrapInVal(inVal, isLower);
			return ret; //done here
			break;
		case WRAP:
			inVal = WrapInVal(inVal, isLower);
			break;
		}
	}

	//next, find two closest x pts;

	int ind;
	for (ind = 0; ind < m_xPts.size() && inVal>m_xPts[ind]; ++ind);

	//if exact match, return data val.
	if (m_xPts[ind] == inVal)
		return m_vals[ind];

	//grab floors, ceilings

	OME_SCALAR flrVal = m_xPts[ind-1];
	OME_SCALAR clVal = m_xPts[ind];

	switch (m_interp)
	{
	case INTERP:
	{
				   //(inVal / (clVal - flrVal)) - (flrVal / (clVal - flrVal));
				   OME_SCALAR fract = (inVal - flrVal) / (clVal - flrVal);
				   ret = (m_vals[ind - 1] - (m_vals[ind - 1] * fract)) + (m_vals[ind] * fract); 
				//alternate:
				   //ret=m_vals[ind-1] * (((OME_SCALAR)1.0) - fract) + (m_vals[ind] * fract);
				   // y = mx+b
				 /*  OME_SCALAR m = (m_vals[ind] - m_vals[ind - 1]) / (m_xPts[ind] - m_xPts[ind - 1]);
				   OME_SCALAR b = m_vals[ind] - m*m_xPts[ind];
				   ret = m*inVal + b;*/
	}
		break;
	case ROUND:
		ret = (inVal - flrVal) <= (clVal - inVal) ? m_vals[ind - 1] : m_vals[ind];
		break;
	case FLOOR:
		ret = m_vals[ind - 1];
		break;
	}
	return ret;
}

STLString EvalXSetInterpTable::GetXMLRepresentation(const unsigned int indent, const unsigned int inc)
{
	return EvalInterpTable::GetXMLRepresentation(indent, inc);
}
void EvalXSetInterpTable::GetXMLAttributes(StrArray & out) const
{
	EvalInterpTable::GetXMLAttributes(out);
	oSTLSStream outStrm;
	outStrm.precision(OME_WRITE_PRECISION);
	for (size_t i = 0; i<m_xPts.size(); i++)
		outStrm << ' ' << m_xPts[i];

	AddAttributeValue("xPts", outStrm.str().substr(1), out);
}

void EvalXSetInterpTable::GetXMLSubNodes(StrArray & out, const unsigned int indent, const unsigned int inc)
{}


void EvalXSetInterpTable::GetXMLAttributeMap(XMLAttrVector & out)
{}

int EvalXSetInterpTable::PopulateFromComplexElements(TI2Element* pCurrElem, const STLString & tag, const STLString & filename)
{
	int ret = EvalInterpTable::PopulateFromComplexElements(pCurrElem, tag, filename);
	if (ret && pCurrElem->Attribute("xPts"))
	{
		m_xPts.resize(m_vals.size());
		STLString inString = pCurrElem->Attribute("xPts");
		iSTLSStream inStrm(inString);

		for (size_t i = 0; i < m_vals.size() && !inStrm.eof(); i++)
			inStrm >> m_xPts[i];

		
	}
	return ret;
}