#pragma once
#include "BaseResultsDetails.h"

/** Container for storing preferences for a Multivalue Browser view.*/
class __EXPORT__ MultiValResultsDetails : public BaseResultsDetails
{
public:
	MultiValResultsDetails();
	virtual ~MultiValResultsDetails();

	virtual STLString GetXMLRepresentation(const unsigned int indent = 0, const unsigned int inc = 4);
	virtual void GetXMLAttributes(StrArray & out) const;
	virtual void GetXMLSubNodes(StrArray & out, const unsigned int indent = 0, const unsigned int inc = 4);

	virtual const OMEChar* GetXMLTag() const { return "multival_view"; };
	virtual RV_TYPE GetRVType() const { return RVT_MULTVAL; }

	/** @return true if only showing selected components; false otherwise.*/
	inline bool GetUseSelected() const { return m_useSelected; }
	/** Indicate whether or not only selected components should be shown.
	@param s If true, only selected components are shown; otherwise, all values will be shown.
	*/
	inline void SetUseSelected(const bool & s) { SINAssign(s, m_useSelected); }


protected:

	bool m_useSelected;		///< Flag to filter out non-selected fields.

	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem, const STLString & tag, const STLString & filename);

	virtual void AddFieldXMLAttrs(oSTLSStream & outStrm, const size_t & ind);
	virtual void GetFieldXMLAttrs(TI2Element* pElem);
};