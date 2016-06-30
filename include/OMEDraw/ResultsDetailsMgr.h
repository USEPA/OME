#pragma once
#include "BaseResultsDetails.h"
#include "omeobjects/XMLLoadable.h"

/** Class for managing records of values used for displaying model results.v*/
class __EXPORT__ ResultsDetailsMgr : public XMLLoadable
{
public:
	ResultsDetailsMgr();
	~ResultsDetailsMgr();

	BaseResultsDetails* NewRD(const RV_TYPE & rtype,const STLString & name);
	int IndexForView(const STLString & name);
	int IndexForView(BaseResultsDetails* pRd);
	bool AddRD(BaseResultsDetails* pRd);
	void RemoveRD(BaseResultsDetails* pRd);

	/** @return the number of views stored within the ResultsDetailsMgr.*/
	inline size_t GetViewCount() const { return m_details.size();}

	BaseResultsDetails* GetDetails(size_t i);

	/** Index operator.
		@param i index of the BaseResultsDetails to retrieve.
		@return A pointer to the BaseResultsDetails object found at index i.
	*/
	BaseResultsDetails* operator[](size_t i) const { return m_details[i]; };

	/** @return Pointer to currently selected BaseResultsDetails, or NULL if none is selected.*/
	inline BaseResultsDetails* GetActiveDetails() { return m_pActiveDetails;}
	BaseResultsDetails* SetActiveDetails(size_t i);
	BaseResultsDetails* SetActiveDetails(BaseResultsDetails* pRD);
	BaseResultsDetails* SetActiveDetails(const STLString & name );
	/** Deselect the active BaseResultsDetails.*/
	inline void ClearActiveDetails() { m_pActiveDetails=NULL; }

	bool NameIsUnique(const STLString & name, BaseResultsDetails* pRD=NULL);

	bool SaveIsNeeded() const;
	void ClearSaveFlags();
	void ClearViews();
	bool LoadFromControlFile(const STLString & path);
	void LoadFromXMLTree(TI2Element* pCurrElem,const STLString & path=STLString());
	
	virtual void GetXMLAttributes(StrArray & out) const;
	virtual void GetXMLSubNodes(StrArray & out,const unsigned int indent=0,const unsigned int inc=4);
	virtual const OMEChar* GetXMLTag() const { return "results_views"; }

protected:
	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem, const STLString & tag,const STLString & filename);

private:
	std::vector<BaseResultsDetails*> m_details;		///< Structure for holding all Result Detail records.
	BaseResultsDetails* m_pActiveDetails;			///< Pointer to the currently selected details.
};

