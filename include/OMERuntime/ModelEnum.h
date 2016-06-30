#pragma once

#include <vector>
#include <initializer_list>

#include "OMEDefines.h"

class ModelEnum;
/** Individual enumerated value entry for a given ModelEnum.*/
struct __EXPORT__ EnumEntry
{
	unsigned int ind;		///< Index / numerical equivalent of enumerated value.
	std::string label;		///< Label representing discrete enumerated value.

	ModelEnum* pCollection;	///< Pointer to associated parent ModelEnum.

	/** Detailed Constructor.
		@param lbl Label of EnumEntry.
		@param i Index of EnumEntry.
		@param pColl Pointer to parent ModelEnum for EnumEntry.
	*/
	EnumEntry(const std::string & lbl, const unsigned int & i, ModelEnum* pColl)
		:label(lbl), ind(i), pCollection(pColl) {}
	/** Copy Constructor.
		@param ee EnumEntry to copy.
	*/
	EnumEntry(const EnumEntry & ee)
		:label(ee.label), ind(ee.ind), pCollection(ee.pCollection) {}

	/** int cast overload. */
	operator int()
	{
		return ind;
	}

	/** Scalar cast overload.*/
	operator OME_SCALAR()
	{
		return ind;
	}

	/** Sentinal value representing no entry.*/
	static const EnumEntry NO_ENTRY;

};


/** Class used to store a named set of enumerations used in expressions.
*/
class __EXPORT__ ModelEnum
{
public:
	
	ModelEnum();
	ModelEnum(const STLString & enumName);
	ModelEnum(const STLString & enumName, const initializer_list<STLString> & vals);
	ModelEnum(const ModelEnum & src);
	ModelEnum& operator=(const ModelEnum & rhs);
	
	EnumEntry* AddrForLabel(std::string lbl);
	EnumEntry* AddrForIndex(const unsigned int & ind);

	STLString ToString() const;

	short ValForLabel(const STLString & lbl) const;

	/** Set the name of the ModelEnum.
		@param name The name to assign.
	*/
	inline void SetEnumClassName(const STLString & name) { m_enumName = name; };
	/** @return The ModelEnum name. */
	inline STLString GetEnumClassName() const {	return m_enumName;};

	/** @return the total number of unique values.*/
	inline unsigned int GetEnumValueCount() const { return m_values.size(); }
	
	/** Retrieve the label of a specific enumerated value.
		@param i The 0-based index of the value to retrieve.
		@return The label of the requested enumerated value.
	*/
	inline const STLString & GetEnumValue(const unsigned int & i) const { return m_values[i].label; }
	/** Retrieve a specific EnumEntry
	@param i The 0-based index of the EnumEntry to retrieve.
	@return A reference to the requested EnumEntry.
	*/
	inline EnumEntry& GetEnumEntry(const unsigned int & i)  { return m_values[i]; }
	/** Retrieve a specific EnumEntry
	@param i The 0-based index of the EnumEntry to retrieve.
	@return A copy of the requested EnumEntry.
	*/
	inline EnumEntry GetEnumEntry(const unsigned int & i) const  { return m_values[i]; }

	unsigned int AddEnumValue(const STLString & valName);

	/** int cast.*/
	operator int()
	{
		return m_values.size();
	}

	/** Scalar cast.*/
	operator OME_SCALAR()
	{
		return m_values.size();
	}

private:
	STLString m_enumName;					///< name of the enumeration set
	std::vector<EnumEntry> m_values;		///< the unique m_values in the set;
};

typedef std::vector<ModelEnum> ModelEnumArray;
