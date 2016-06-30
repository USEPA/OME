#include "omeobjects/EvalInterpTable.h"

/** Default Constructor. */
EvalInterpTable::EvalInterpTable()
:EvalTable(), m_lBound(0.0), m_uBound(0.0), m_interp(INTERP), m_bounding(CLAMP)
{
}

/** Simple Constructor.
	@param id The unique identifier for the table.
	@param data The data to use to initialize the EvalInterpTable.
*/
EvalInterpTable::EvalInterpTable(const STLString & id, const DataArray &data)
: EvalTable(id, IndVec(1), data), m_interp(INTERP), m_bounding(CLAMP)
{
	m_dims[0] = data.size();
}

/** Detailed Constructor.
	@param id The unique identifier for the table.
	@param iMode The interpolation mode to use.
	@param bMode The bounding mode to use.
	@param data The data to use to initialize the EvalInterpTable.
	@sa INTERPMODE,BOUNDSMODE.
*/
EvalInterpTable::EvalInterpTable(const STLString & id, const INTERPMODE & iMode, const BOUNDSMODE & bMode, const DataArray & data)
: EvalTable(id, IndVec(1), data), m_interp(iMode), m_bounding(bMode)
{
	m_dims[0] = data.size();
}

/** Copy Constructor.
	@param et The EvalInterpTable to copy.
*/
EvalInterpTable::EvalInterpTable(const EvalInterpTable & et)
{
	Duplicate(et);
}

/** XML Constructor.
	@param pCurrElem Pointer to The current TI2Element.
	@param tag The identifing tag.
	@param filename Name of originating file.
*/
EvalInterpTable::EvalInterpTable(TI2Element* pCurrElem, const STLString & tag, const STLString & filename)
{
	PopulateFromXML(pCurrElem, tag, filename);
}

/** Assignment operator. 
	@param rhs The EvalInterpTable to copy.
	@return a reference to the calling EvalInterpTable.
*/
EvalInterpTable& EvalInterpTable::operator=(const EvalInterpTable & rhs)
{
	Duplicate(rhs);
	return *this;
}

/** Destructor.*/
EvalInterpTable::~EvalInterpTable()
{}

/** Retrieve mapped value for a provided value, interpolating as necessary.
	@param inVal The reference value.
	@return The value mapped to inVal, incorporating any interpolation or bound operations that are assigned to the table.
*/
OME_SCALAR EvalInterpTable::GetInterpVal(OME_SCALAR inVal)
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
			ret = ExtrapInVal(inVal,isLower);
			return ret; //done here
			break;
		case WRAP:
			inVal = WrapInVal(inVal, isLower);
			break;
		}
	}

	//next, perform interpolation

	OME_SCALAR step = GetInterpStep();
	int ind = inVal/step;
	
	//grab floors, ceilings
	OME_SCALAR flrVal = ind*step;
	OME_SCALAR clVal = flrVal + step;

	switch (m_interp)
	{
	case INTERP:
		ret = m_vals[ind] + ((m_vals[ind + 1] - m_vals[ind])*((inVal - flrVal) / step));
		break;
	case ROUND:
		ret = (inVal - flrVal) <= (clVal - inVal) ? m_vals[ind] : m_vals[ind + 1];
		break;
	case FLOOR:
		ret = m_vals[ind];
		break;
	}
	return ret;
}

/** Perform a normalization operation on all stored values with a provided denominator.
	@param denom the value used to derive the value multiplier from the sum of the table bounds.
*/
void EvalInterpTable::NormalizeValsWithDenom(const OME_SCALAR & denom)
{
	OME_SCALAR modifier = (m_lBound + m_uBound) / denom;

	std::for_each(m_vals.begin(), m_vals.end(), [&modifier](OME_SCALAR & d){ d *= modifier; });
}

STLString EvalInterpTable::GetXMLRepresentation(const unsigned int indent, const unsigned int inc)
{
	return EvalTable::GetXMLRepresentation(indent, inc);
}

void EvalInterpTable::GetXMLAttributes(StrArray & out) const
{
	EvalTable::GetXMLAttributes(out);

	//save id
	AddAttributeValue("lower_bound", m_lBound, out);
	AddAttributeValue("upper_bound", m_uBound, out);
	AddAttributeValue("interp_mode", m_interp, out);
	AddAttributeValue("bound_mode", m_bounding, out);
}

void EvalInterpTable::GetXMLSubNodes(StrArray & out, const unsigned int indent, const unsigned int inc)
{
	EvalTable::GetXMLSubNodes(out, indent, inc);
}

void EvalInterpTable::GetXMLAttributeMap(XMLAttrVector & out)
{
	EvalTable::GetXMLAttributeMap(out);
	XmlAttr attrs[] = {
		// attr					type		   address                          isReq 
		{ "lower_bound",		XML_SCALAR,		&(this->m_lBound),				true },
		{ "upper_bound",		XML_SCALAR,		&(this->m_uBound),				true },
		{ "interp_mode",		XML_INT,		&(this->m_interp),				true },
		{ "bound_mode",			XML_INT,		&(this->m_bounding),			true },
		{ NULL, XML_NULL, NULL, false } };

	//drop null
	int count;
	for (count = 0; attrs[count].name != NULL; count++)
		out.push_back(attrs[count]);
}

int EvalInterpTable::PopulateFromComplexElements(TI2Element* pCurrElem, const STLString & tag, const STLString & filename)
{
	return EvalTable::PopulateFromComplexElements(pCurrElem, tag, filename);
}

/** Copy values from another EvalInterpTable.
	@param et The EvalInterpTable to copy.
*/
void EvalInterpTable::Duplicate(const EvalInterpTable & et)
{
	EvalTable::Duplicate(et);
	m_lBound = et.m_lBound;
	m_uBound = et.m_uBound;
	m_interp = et.m_interp;
	m_bounding = et.m_bounding;
}

/** Extrapolate a data value for an index value that lies out of bounds.
	@param inVal The out of bounds index value.
	@param isLower If true, inVal lies beyond the lower bound; otherwise it lies beyond the upper bound.
	@return The extrapolated value.
*/
OME_SCALAR EvalInterpTable::ExtrapInVal(OME_SCALAR inVal, const bool & isLower)
{
	//y=mx+b
	OME_SCALAR x1, x2,y1,y2;
	OME_SCALAR m, b;

	OME_SCALAR inc = (m_vals.size() - 1) / (m_uBound - m_lBound);
	if (isLower)
	{
		x1 = m_lBound;
		x2 = x1 + inc;
		y1 = m_vals[0];
		y2 = m_vals[1];
	}
	else
	{
		x2 = m_uBound;
		x1 = x2 - inc;
		y2 = m_vals[m_vals.size() - 1];
		y1 = m_vals[m_vals.size() - 2];
	}

	m = (y2 - y1) / (x2 - x1);
	b = y1 - m*x1;

	return m*inVal + b;
}

/** Wrap an out of bound data value to the other side of the table until it falls in bounds. This
	is equivalent to modding the index value by the appropriate bound.
@param inVal The out of bounds index value.
@param isLower If true, inVal lies beyond the lower bound; otherwise it lies beyond the upper bound.
@return The data value found by the wrapping operation.
*/
OME_SCALAR EvalInterpTable::WrapInVal(OME_SCALAR inVal, const bool & isLower)
{
	//shift to zero
	OME_SCALAR lim = m_uBound - m_lBound;
	if (isLower)
		lim *= -1.0;
	while (inVal > m_uBound || inVal < m_lBound)
		inVal -= lim;

	return inVal;
}

/** Utility function for generating console-friendly labels for INTERPMODE flags.
	@param mode The mode to interpret.
	@param includeScope If true, the return value is prepended with "EvalInterpTable::"
	@return A string reprentation of the passed in mode.
	@sa INTERPMODE
*/
STLString EvalInterpTable::GetInterpModeLabel(const unsigned int & mode,const bool & includeScope)
{
	STLString ret=includeScope ? STLString("EvalInterpTable::") : STLString();
	switch ((INTERPMODE)mode)
	{
	case INTERP:
		ret += "INTERP";
		break;
	case ROUND:
		ret += "ROUND";
		break;
	case FLOOR:
		ret += "FLOOR";
		break;
	default:
		ret = STLString();
	}
	return ret;
}

/** Utility function for generating console-friendly labels for BOUNDSMODE flags.
	@param mode The mode to interpret.
	@param includeScope If true, the return value is prepended with "EvalInterpTable::"
	@return A string reprentation of the passed in mode.
	@sa BOUNDSMODE
*/
STLString EvalInterpTable::GetBoundsModeLabel(const unsigned int & mode, const bool & includeScope)
{
	STLString ret = includeScope ? STLString("EvalInterpTable::") : STLString();
	switch ((BOUNDSMODE)mode)
	{
	case CLAMP:
		ret += "CLAMP";
		break;
	case EXTRAP:
		ret += "EXTRAP";
		break;
	case WRAP:
		ret += "WRAP";
		break;
	default:
		ret = STLString();
	}
	return ret;
}
