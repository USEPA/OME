#pragma once

#include "OMEDefines.h"
#include "EvalTable.h"
#include "CharType.h"

/** Subclass of EvalTable that supports interpolation operations between indexed values. */
class __EXPORT__ EvalInterpTable : public EvalTable
{
public:
	/** Flags denoting the Interpolation method.*/
	enum INTERPMODE 
	{
		INTERP,	///< Perform linear interpolation between indexed values.
		ROUND,   ///< Round to the nearest indexed value.
		FLOOR	///<Round down.
	};
	
	/** Flags denoting behavior of index values that lie outside the bounds of the table.*/
	enum BOUNDSMODE 
	{ 
		CLAMP,		///< Value returned is the first valid indexed value closest to the index.
		EXTRAP,		///< Return a value that is the result of the extrapolation of the difference between the two nearest indexed values.
		WRAP		///< Use the remainder of the index divided by the number of indexed values to return a value; basically a modding operation.
	};

	EvalInterpTable();
	EvalInterpTable(const STLString & id, const DataArray &data);
	EvalInterpTable(const STLString & id, const INTERPMODE & iMode, const BOUNDSMODE & bMode, const DataArray & data);
	EvalInterpTable(const EvalInterpTable & et);
	EvalInterpTable(TI2Element* pCurrElem, const STLString & tag, const STLString & filename);

	EvalInterpTable& operator=(const EvalInterpTable & rhs);

	virtual ~EvalInterpTable();

	virtual OME_SCALAR GetInterpVal(OME_SCALAR inVal);

	OME_SCALAR operator()(const OME_SCALAR & inVal)
	{
		return GetInterpVal(inVal);
	}

	/** @return The lower index value bound. */
	inline OME_SCALAR GetLowerBound() const { return m_lBound; }
	/** Set the lower index value limit.
		@param lb The new lower bound index value.
	*/
	inline void SetLowerBound(const OME_SCALAR & lb) { m_lBound = lb; }

	/** @return The upper index value bound.*/
	inline OME_SCALAR GetUpperBound() const { return m_uBound; }
	/** Set the upper index value limit.
	@param ub The new lower bound index value.
	*/
	inline void SetUpperBound(const OME_SCALAR & ub) { m_uBound = ub; }

	/** @return the normalized steup used for walking along indices at regular intervals.*/
	inline OME_SCALAR GetInterpStep() const { return (m_uBound - m_lBound) / (m_vals.size() - 1); }

	/** Set the upper and lower bounds of the data indices.
		@param lb The lower bound.
		@param ub The upper bound.
		@sa SetLowerBound, SetUpperBound
	*/
	inline void SetBounds(const OME_SCALAR & lb, const OME_SCALAR & ub) {
		SetUpperBound(ub); SetLowerBound(lb);
	}
	
	/** @return the currently selected interpolation mode. @sa INTERPMODE.*/
	inline INTERPMODE GetInterpMode() const { return m_interp; }
	/** Set the interpolation mode.
		@param im Flag indicating the interpolation mode to use.
		@sa INTERPMODE
	*/
	inline void SetInterpMode(const INTERPMODE & im) { m_interp = im; }

	/** @return the currently selected bounding mode. @sa BOUNDSMODE.*/
	inline BOUNDSMODE GetBoundMode() const { return m_bounding; }
	/** Set the bounding mode.
	@param bm Flag indicating the bounding mode to use.
	@sa BOUNDSMODE
	*/
	inline void SetBoundMode(const BOUNDSMODE & bm) { m_bounding = bm; }

	void NormalizeValsWithDenom(const OME_SCALAR & denom);

	virtual STLString GetXMLRepresentation(const unsigned int indent = 0, const unsigned int inc = 4);
	virtual const OMEChar* GetXMLTag() const { return "interp_table_data"; }
	virtual void GetXMLAttributes(StrArray & out) const;
	virtual void GetXMLSubNodes(StrArray & out, const unsigned int indent = 0, const unsigned int inc = 4);

	static STLString GetInterpModeLabel(const unsigned int & mode, const bool & includeScope=false);
	static STLString GetBoundsModeLabel(const unsigned int & mode, const bool & includeScope=false);

protected:
	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem, const STLString & tag, const STLString & filename);

	void Duplicate(const EvalInterpTable & et);
	
	OME_SCALAR ExtrapInVal(OME_SCALAR inVal,const bool & isLower);
	OME_SCALAR WrapInVal(OME_SCALAR inVal,const bool & isLower);

	OME_SCALAR m_lBound;		///< Lower bound for mapped data indices.
	OME_SCALAR m_uBound;		///< Upper bound for mapped data indices.
	
	INTERPMODE m_interp;		///< Mode of interpolation used. @sa INTERPMODE.
	BOUNDSMODE m_bounding;		///< Method for handling data indices that lie outside of bounds. @sa BOUNDSMODE.
};