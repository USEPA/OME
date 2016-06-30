
#pragma once

#include "omeobjects/EvalInterpTable.h"

/** Variant of EvalInterpTable where x-axis distribution is explicitly provided. */
class __EXPORT__ EvalXSetInterpTable : public EvalInterpTable
{
public:
	EvalXSetInterpTable();
	EvalXSetInterpTable(const STLString & id, const DataArray &xData, const DataArray & yData);
	EvalXSetInterpTable(const STLString & id, const INTERPMODE & iMode, const BOUNDSMODE & bMode, const DataArray & xData, const DataArray & yData);
	EvalXSetInterpTable(const EvalXSetInterpTable & et);
	EvalXSetInterpTable(TI2Element* pCurrElem, const STLString & tag, const STLString & filename);

	EvalXSetInterpTable& operator=(const EvalXSetInterpTable & rhs);

	virtual ~EvalXSetInterpTable();

	virtual OME_SCALAR GetInterpVal(OME_SCALAR inVal);

	virtual STLString GetXMLRepresentation(const unsigned int indent = 0, const unsigned int inc = 4);
	virtual const OMEChar* GetXMLTag() const { return "interp_xs_table_data"; }
	virtual void GetXMLAttributes(StrArray & out) const;
	virtual void GetXMLSubNodes(StrArray & out, const unsigned int indent = 0, const unsigned int inc = 4);

	/** @return DataArray containing the x-axis ordinates of anchor points.*/
	DataArray GetXPts() const { return m_xPts; }

protected:
	DataArray m_xPts;		///< The x coordinates to use.

	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem, const STLString & tag, const STLString & filename);
};