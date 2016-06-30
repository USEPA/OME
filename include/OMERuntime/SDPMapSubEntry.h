#pragma once

#include "OMEDefines.h"
#include "ISpatialDataProvider.h"
#include "ErrCollection.h"

typedef void(*CoverageSyncFunction)(const int, const int, const OME_SCALAR, void*);

class Evaluable;

/** Class for tracking entry identifiers for SDP.*/
class SDPMapSubEntry
{
public:
	/** Flags for supported mapping behaviors.*/
	enum ENTRY_FLAGS
	{
		NONE = 0x00,			  ///< No mapping behaviors set.
		INIT_SDP = 0x01,		  ///< Initialize a SDP field from a model component.
		INIT_COMPONENT = 0x02,	  ///< Initialize a model component from a SDP field.
		UPDATE_SDP = 0x04,		  ///< Update a SDP field from a model component.
		UPDATE_COMPONENT = 0x08,  ///< Update a model component from a SDP field.
		ALL = 0x0f				  ///< All mapping behaviors set.
	};

	SDPMapSubEntry(const STLString & srcnori, const bool & isName,const ENTRY_FLAGS & flgs=NONE);
	SDPMapSubEntry(const STLString & srcnori, const bool & isName, const STLString & cName, const ENTRY_FLAGS & flgs = NONE);
	SDPMapSubEntry(const STLString & srcnori, const bool & isName, const size_t & ii, const STLString & cName, const ENTRY_FLAGS & flgs = NONE);
	SDPMapSubEntry();

	bool Apply(ISpatialDataProvider* pSDP, Model* pMdl, const size_t & inst, const ENTRY_FLAGS & toApply, ErrCollection* pErrs = NULL);
	bool Apply(ISpatialDataProvider* pSDP, Model* pMdl, const size_t & inst, const ENTRY_FLAGS & toApply,CoverageSyncFunction syncFunc,void* pFuncExtra=NULL,ErrCollection* pErrs=NULL);
	
	/** Add a behavior flag(s) for future reference.
		@param flg The bitwise flag or flags to add to the SDPMapSubEntry.
	*/
	inline void AddFlag(const ENTRY_FLAGS & flg) { m_applyTo = (ENTRY_FLAGS)(m_applyTo | flg); };
	/** @return bitwise flags representing supported relationships. */
	inline ENTRY_FLAGS GetFlags() const { return m_applyTo; }
	/** Set the associated Model component.
		@param pEval Pointer to the associated Evaluable object to associate.
	*/
	inline void SetModelComponent(Evaluable* pEval) { m_pEval = pEval; };
	/** @return Pointer to the associated Evaluable object.*/
	inline Evaluable* GetModelComponent() { return m_pEval; };

	/** Set the SDP column index to associate with.
		@param idx The index to associate.
	*/
	inline void SetColInd(const int & idx) { m_colInd = idx; }
	/** @return true if entry is identifed by a readable name or path, or false if identified by id.*/
	inline bool GetIsName() const { return m_isName; }
	/** @return identify string of source model component.*/
	inline STLString GetSrcNameOrId() const { return m_srcNameOrId; }
	/** @return The name of the target SDP column. Used to lookup column index.*/
	inline STLString GetColName() const { return m_colName; }

private:

	STLString m_srcNameOrId;	///< String either containing name or id.
	bool m_isName;				///< If true, srcNameOrId contains a name; otherwise srcNameOrId contains an id.
	int m_colInd;				///< Cache index of SDP column, or -1 if not currently associated with column.
	STLString m_colName;		///< The name of the associated SDP column.
	Evaluable* m_pEval;			///< Pointer to the associated Evaluable object.
	ENTRY_FLAGS m_applyTo;		///< Set of bit-flags indicating direction of associative relationships.
	size_t m_offset;			///< Index-based offset for specific m_pEval instance.

	size_t FindDepth(Model* pMdl);

};

