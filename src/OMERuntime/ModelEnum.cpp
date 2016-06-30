#include "ModelEnum.h"

 const EnumEntry EnumEntry::NO_ENTRY(std::string(), 0, NULL);

/** Default constructor
*/
ModelEnum::ModelEnum() 
{}

/** Simple Constructor.
	@param enumName The name of the new ModelEnum.
*/
ModelEnum::ModelEnum(const STLString & enumName)
:m_enumName(enumName) {}

/** Detailed Constructor.
@param enumName The name of the new ModelEnum.
@param vals Initializer list of all values for the ModelEnum.
*/
ModelEnum::ModelEnum(const STLString & enumName, const initializer_list<STLString> & vals)
	:m_enumName(enumName)
{
	size_t i = 1;
	for (auto itr = vals.begin(); itr != vals.end(); ++itr, ++i)
		m_values.emplace_back((*itr), i, this);
}

/** Copy constructor
@param src the ModelEnum to copy.
*/
ModelEnum::ModelEnum(const ModelEnum & src)
{
	this->m_enumName = src.m_enumName;
	this->m_values.assign(src.m_values.begin(), src.m_values.end());
	std::for_each(this->m_values.begin(), this->m_values.end(), [this](EnumEntry & ee){ee.pCollection = this; });
}

/** Assignment Operator.
@param rhs The ModelEnum to copy.
@return reference to this.
*/
ModelEnum& ModelEnum::operator=(const ModelEnum & rhs)
{
	this->m_enumName = rhs.m_enumName;
	this->m_values.assign(rhs.m_values.begin(), rhs.m_values.end());
	return *this;
}

/** Retrieve EnumEntry for label.
	@param lbl The label for the EnumEntry to search for.
	@return Pointer to EnumEntry with label lbl, or NULL if not found.
*/
EnumEntry* ModelEnum::AddrForLabel(std::string lbl)
{
	lbl = ScrubName(lbl);
	EnumEntry* ret = NULL;
	auto fItr = std::find_if(m_values.begin(), m_values.end(), [&lbl](EnumEntry & ee){return ScrubName(ee.label) == lbl; });
	if (fItr != m_values.end())
		ret = &(*fItr);
	return ret;
}

/** Retrieve EnumEntry for 1-based index.
@param ind The 1-based index for the EnumEntry to search for.
@return Pointer to EnumEntry with label lbl, or NULL if not found.
*/
EnumEntry* ModelEnum::AddrForIndex(const unsigned int & ind)
{
	EnumEntry* ret = NULL;
	if (ind <= m_values.size())
		ret = &m_values[ind - 1];
	return ret;
}

/** Creates a string pattern representation of the ModelEnum and its values.
	@return A string pattern representation of the ModelEnum and its values.
*/
STLString ModelEnum::ToString() const
{
	oSTLSStream outStrm;
	outStrm << m_enumName << ':';
	for (auto itr = m_values.begin(); itr != m_values.end(); ++itr)
		outStrm << (*itr).label << '|';

	return outStrm.str().substr(0, outStrm.str().length() - 1);
}

/** Retrieve value for label.
@param lbl The label for the value to search for.
@return Value for label lbl, or -1 if not found.
*/
short ModelEnum::ValForLabel(const STLString & lbl) const
{
	short ret = -1;
	if (lbl == m_enumName)
		ret = m_values.size();
	{
		for (auto itr = m_values.begin(); itr != m_values.end() && ret == -1; ++itr)
			if (lbl == (*itr).label)
				ret = (*itr).ind;
	}
	return ret;
}

/** Add a new discrete value to ModelEnum.
	@param valName The name of the new value.
	@return the numerical value of the new enumerated value.
	*/
unsigned int ModelEnum::AddEnumValue(const STLString & valName)
{
	/// @todo add check for redundant listing
	unsigned int ret = m_values.size() + 1;

	m_values.emplace_back(valName, ret, this);

	return ret;
}