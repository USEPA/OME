#pragma once
#include "BaseResultsDetails.h"

/** @defgroup TRDSortTypes Tree Results Details Sort Flags
@{
*/
/** @enum RN_SORTMODE
	Flags used to determine how to sort Tree components.
*/
enum RN_SORTMODE {	CFIRST=0,	///< Model components are listed first.
					MFIRST,		///< Models are listed first.
					MIXED		///< Models and Model compenents are intermixed.
};
/** 
@}
*/

/** Class for storing data describing how to display a tree view.*/
class __EXPORT__ TreeResultsDetails : public BaseResultsDetails
{
public:
	TreeResultsDetails();
	virtual ~TreeResultsDetails();

	virtual STLString GetXMLRepresentation(const unsigned int indent=0,const unsigned int inc=4);
	virtual void GetXMLAttributes(StrArray & out) const;
   virtual void GetXMLSubNodes(StrArray & out,const unsigned int indent=0,const unsigned int inc=4);

	virtual const OMEChar* GetXMLTag() const { return "tree_view";};
	virtual RV_TYPE GetRVType() const { return RVT_TREE; }

	/** @return true if only showing selected components; false otherwise.*/
	inline bool GetUseSelected() const { return m_useSelected;}
	/** Indicate whether or not only selected components should be shown.
		@param s If true, only selected components are shown; otherwise, all values will be shown.
	*/
	inline void SetUseSelected(const bool & s) { SINAssign(s, m_useSelected); }

	/** @return The currently selected sort mode. @sa RN_SORTMODE. */
	inline RN_SORTMODE GetSortMode() const { return m_sortMode; }
	/** Set the method to use to sort Models and model components.
		@param sm The method to use for sorting tree objects.
		@sa RN_SORTMODE.
	*/
	inline void SetSortMode(const RN_SORTMODE & sm) { SINAssign(sm, m_sortMode); }

	/** @return true if listings are to be sorted in ascending order, and false if they are to be sorted in descending order.*/
	inline bool GetSortAsc() const { return m_sortAsc; }
	/** Set the sorting direction.
		@param sd If true, components are sorted in ascending alpha order; otherwise, components are sorted in descending alpha order.
	*/
	inline void SetSortAsc(const bool & sd) { SINAssign(sd, m_sortAsc); }
protected:

	bool m_useSelected;			///< If true, only show selected variables.
	bool m_sortAsc;				///< Sorting direction. Sorts in ascending order if true; otherwise sorts in descending order.
	RN_SORTMODE m_sortMode;		///< Flag indicating how to sort Model objects relative to other components. @sa RN_SORTMODE

	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem, const STLString & tag,const STLString & filename);

	virtual void AddFieldXMLAttrs(oSTLSStream & outStrm,const size_t & ind);
	virtual void GetFieldXMLAttrs(TI2Element* pElem);
};