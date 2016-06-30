#pragma once

#include "OMEDefines.h"
#include "ISpatialDataProvider.h"
#include "SDPMapSubEntry.h"
#include "ErrCollection.h"

/** Mapping of values between model and SpatialDataProvider */
class SDPMapEntry
{
public:
	
	SDPMapEntry();
	SDPMapEntry(Model* pMdl);
	SDPMapEntry(const STLString & id, const bool & isName);

	bool AddSubEntry(const SDPMapSubEntry & entry);
	bool AssociateModel(Model* pMdl);
	bool AssociateSDP(ISpatialDataProvider* pSdp,const bool & canAddCols);

	bool ApplyEntries(const SDPMapSubEntry::ENTRY_FLAGS & toApply);
	bool ApplyEntries(const SDPMapSubEntry::ENTRY_FLAGS & toApply, CoverageSyncFunction syncFunc, void* pFuncExtra = NULL);
	bool ApplyEntryForCurrInstEval(const SDPMapSubEntry::ENTRY_FLAGS & toApply,Evaluable* pEval);
	//bool ApplyEntryForCurrInstEval(const SDPMapSubEntry::ENTRY_FLAGS & toApply, Evaluable* pEval, CoverageSyncFunction syncFunc, void* pFuncExtra = NULL);

	
	void RefreshEntries();
	bool RegisterSDPColumn(SDPMapSubEntry & entry);

	STLString GetNameOrId(bool & isName);
	/** @return Pointer to the associated Model.*/
	inline Model* GetAssociatedModel() const { return m_pMdl; }
	
	/** @return true if the associated Model will be initialized with a number of instances
		matching the number of records reported by the associated SDP; false otherwise.
	*/
	inline bool GetInstancePerUnit() const { return m_initToSDPCount; }

	/** Set whether or not the initial number of instances for the associated model
		should correspond to the number of records reported by the assoicated SDP.
		@param ipu if true the number of initial instances should correspond with the number
		of spatial records; if false, there should be no correspondance.
	*/
	inline void SetInstancePerUnit(const bool & ipu) { m_initToSDPCount=ipu; }
	size_t GetIPUCount();
	void ApplyIPUToAssociatedModel();

	//err reports
	/** Clear any reported errors from the internal catalog.*/
	inline void ClearErrors() { m_errs.ClearErrs(); }
	/** @return true if there are any reported errors in the internal catalog; false otherwise.*/
	inline bool HasErrors() const { return m_errs.GetErrCount(); }
	/** @return true if there are any reported critical errors in the internal catalog; false otherwise.*/
	inline bool HasCritErrors() const { return m_errs.GetCriticalCount(); }
	ErrCollection GetErrors(const STLString & header = STLString());

private:
	STLString m_nameOrId;			///< Model identifier.
	bool m_isName;					///< If true, m_nameOrId is a common name; otherwise, it is a unique id.
	bool m_canAddCols;				///< If true, unmatched coverage columns can be added through the SDP interface.
	bool m_initToSDPCount;			///< If true, an associated model will have its initial number of instances set to the number of SDP records.
	Model *m_pMdl;					///< Pointer to the associated Model.
	ISpatialDataProvider* m_pSdp;	///< Pointer to the associated SDP.
	ErrCollection m_errs;			///< An internal catalog of any errors that occur during invocation of mapping-focused functions.

	std::list<SDPMapSubEntry> m_valMaps;	///< map of specific values between model and SDP 
};

typedef std::map<STLString, SDPMapEntry> SubmodelSDPEntryMap;
