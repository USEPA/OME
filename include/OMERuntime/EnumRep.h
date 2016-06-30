#pragma once
#include <cstdlib>

/** Representation of enumerated types as used by Simile */
struct EnumRep
{
	const OMEChar* name;	///< Name/identifier of value.
	OME_SCALAR value;		///< Actual value used in comparisons, substitutions, etc.
	const OMEChar* type;	///< Group that enumerated value belongs to

	//for group class only
	const OMEChar** valNames;	///< Grouping entry only; list of all enumerated names for this grouping.
	unsigned int valCount;	///< Grouping entry only; total number of values used in this group.

	/** Default/Simple Constructor.
		@param n name of value.
		@param v actual assigned value.
		@param t enumerated type.
	*/
	EnumRep(const OMEChar* n="", OME_SCALAR v=0, const OMEChar* t="")
		:name(n),value(v),type(t), valNames(NULL), valCount(0) {};

	/** Assignment operator.
		@param rhs the EnumRep to copy.
	*/
	EnumRep& operator=(const EnumRep & rhs)
	{
		name=rhs.name;
		value=rhs.value;
		type=rhs.type;

		if(rhs.valCount)
		{
			valCount=rhs.valCount;
			
			valNames = new const OMEChar*[valCount];

			for(int i=0; i< valCount; i++)
				valNames[i]=rhs.valNames[i];
		}
		return *this;
	}


	~EnumRep()
	{
		if(valNames)
			delete[] valNames;
	}
};
