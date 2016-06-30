#pragma once

#include "OMEDefines.h"
#include "omeobjects/XMLLoadable.h"
#include "omeobjects/Evaluable.h"

/** flags for type of results view. */
enum RV_TYPE { RVT_TABLE,	///< Records for table-type view.
				RVT_TREE,	///< Records for tree-type view.
				RVT_MULTVAL ///< Records for multi-instance value view.
};
struct EntryInfo;
/** Base class for Records for Results views. */
class __EXPORT__ BaseResultsDetails : public XMLLoadable
{
public:
	/** Default constructor.*/
	BaseResultsDetails() :m_saveNeeded(true) {};
	/** Destructor. */
	virtual ~BaseResultsDetails() {};

	virtual void GetXMLAttributes(StrArray & out) const;
   virtual void GetXMLSubNodes(StrArray & out,const unsigned int indent=0,const unsigned int inc=4);

   virtual const OMEChar* GetXMLTag() const=0;

   /** @return User-provided name of view. */
	inline STLString GetViewName() const { return m_viewName;} 
	/** Set user-provided name for view.
		@param name The name to associate with the view record.
	*/
	inline void SetViewName(const STLString & name) { SINAssign(name, m_viewName); }

	/** @return Type flag for ResultDetails subclasses.*/
	virtual RV_TYPE GetRVType() const=0;

	/** @return The number of partial reps in the ResultsDetails.*/
	inline size_t GetPartialRepCount() const { return m_fields.size(); }
	/** @return An array of EvalReps which represent the model objects that should be visible when this view is loaded.*/
	inline EvalRepArray GetPartialReps() const { return m_fields; }
	/** Set model objects associated with this ResultDetails record.
		@param reps An EvalRepArray containing representations of associated model objects.
	*/
	inline void SetPartialReps(EvalRepArray & reps) {m_fields=reps; m_saveNeeded=true;}

	/** @return true if changes have been made since the last save, false otherwise.*/
	inline bool GetSaveNeeded() const { return m_saveNeeded; }
	/** Clear the internal flag used for tracking whether or not a save is needed.*/
	inline void ClearSaveFlag() { m_saveNeeded=false; }

protected:
	static const OMEChar* FIELDGRP_TAG; ///< XML tag for identifying subnode containing multiple fields
	static const OMEChar* FIELD_TAG;	///< XML tag for identifying a field entry.
	STLString m_viewName;				///< User-supplied name of ResultsDetails view.

	EvalRepArray m_fields;				///< A collection of EvalRep objects which represent which objects are associated with this view.

	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem, const STLString & tag,const STLString & filename);

	virtual void AddFieldXMLAttrs(oSTLSStream & outStrm,const size_t & ind);
	virtual void GetFieldXMLAttrs(TI2Element* pElem);

	/** Specialty Set function that marks the save needed flag if necessary.
		@param inVal The variable containing the value to be assigned.
		@param assignVal The variable to hold the assigning value.
	*/
	template<class T>
	void SINAssign(const T & inVal,T & assignVal)
	{
		if (assignVal != inVal) { assignVal = inVal; m_saveNeeded = true; }
	}

private:
	bool m_saveNeeded;					///< Flag used for tracking whether or not a view record needs to be saved since previous changes.

};