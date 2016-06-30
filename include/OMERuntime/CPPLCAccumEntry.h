#pragma once

#include "OMEDefines.h"
#include "CompiledMdlFuncs.h"
#include <set>

class CPPLCSubEntry;
class CPPLoopCluster;

/** Record of an accumulation-focused function expression.*/
class CPPLCAccumEntry
{
public:
	
	CPPLCAccumEntry();
	CPPLCAccumEntry(const STLString & name, const STLString & fn, STLString args, const STLString & atl = STLString(),const STLString & suffix=STLString());
	CPPLCAccumEntry(const CPPLCAccumEntry & rhs);

	CPPLoopCluster* FindCommonRootUsingEntryRegistry(std::map<STLString, CPPLCSubEntry*> & registry);
	
	CPPLoopCluster* GetCommonRootCluster() const;
	const STLString& GetVarName() const;
	const STLString& GetSuffix() const;
	STLString GetVarNameWithSuffix() const;
	const STLString& GetFuncName() const;
	const STLString& GetInvocation() const;

	void AddAppliesToList(const STLString & id);
	/** @return true if the expression signature has been placed in the CPPLoopCluster tree; false otherwise.*/
	inline bool HasBeenPlaced() const { return m_hasBeenPlaced; }
	/** Designate that the entry has been placed in the associated CPPLoopCluster tree.*/
	inline void MarkAsPlaced() { m_hasBeenPlaced = true; }

private:
	STLString m_varName;			///< Name of the variable being represented.
	STLString m_funcName;			///< Name of the function being called.
	STLString m_suffix;				///< The suffix denoting list dimensions, if any.
	STLString m_invocation;			///< How the accumulation function is called.
	std::set<STLString> m_appliesToList;		///< The variables which the accumulation function is being applied to.
	CPPLoopCluster* m_pCommonRoot;	///< Pointer to the root cluster for the target list object in the current CPPLoopCluster tree.
	bool m_hasBeenPlaced;			///< If true, the accum entry has already been processed.

};