#pragma once
#include "BaseResultsDetails.h"

/** Container for storing preferences for a Table Data view.*/
class __EXPORT__ TableResultsDetails : public BaseResultsDetails
{
public:
	TableResultsDetails();
	virtual ~TableResultsDetails();

	virtual STLString GetXMLRepresentation(const unsigned int indent=0,const unsigned int inc=4);
	virtual void GetXMLAttributes(StrArray & out) const;
   virtual void GetXMLSubNodes(StrArray & out,const unsigned int indent=0,const unsigned int inc=4);

	virtual const OMEChar* GetXMLTag() const { return "table_view";};


	virtual RV_TYPE GetRVType() const { return RVT_TABLE; }

	void SetCoordForRep(int repInd, int coordInd, size_t val);
protected:
	bool m_showSigChange;								///< If true, values above values mapped in changeThresholds should be somehow indicated. @todo move to Base
	std::map<STLString,OME_SCALAR> m_changeThresholds;	///< Map of thresholds of change above which are considered significant for a given named variable.
	
	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem, const STLString & tag,const STLString & filename);

	virtual void AddFieldXMLAttrs(oSTLSStream & outStrm,const size_t & ind);
	virtual void GetFieldXMLAttrs(TI2Element* pElem);

};