#include "CPPLCAccumEntry.h"
#include "CPPLCSubEntry.h"
#include "CPPLoopCluster.h"

CPPLCAccumEntry::CPPLCAccumEntry()
{}

/** Detailed Constructor.
	@param name The name of the variable which is calling the accum function.
	@param fn The name of the called function.
	@param args The arguments applied to the function call.
	@param atl The ID of the list which the accum function is being applied to.
	@param suffix The tail end of the function invocation containing the list details, if any.

*/
CPPLCAccumEntry::CPPLCAccumEntry(const STLString & name, const STLString & fn, STLString args, const STLString & atl,const STLString & suffix)
: m_varName(name), m_funcName(fn), m_invocation(args), m_pCommonRoot(NULL), m_hasBeenPlaced(false), m_suffix(suffix)
{
	m_appliesToList.insert(atl);
}

/** Copy Constructor.
	@param rhs The AccumEntry to copy.
*/
CPPLCAccumEntry::CPPLCAccumEntry(const CPPLCAccumEntry & rhs)
:m_varName(rhs.m_varName), m_funcName(rhs.m_funcName), m_hasBeenPlaced(rhs.m_hasBeenPlaced),
 m_appliesToList(rhs.m_appliesToList), m_pCommonRoot(rhs.m_pCommonRoot), m_suffix(rhs.m_suffix)
{}

/** Retrieve and cache a pointer to the common root cluster root.
	@param registry The Entry Registry to use for the lookup.

	@return A pointer to the parent CPPLoopCluster for the targeted list object.
*/
CPPLoopCluster* CPPLCAccumEntry::FindCommonRootUsingEntryRegistry(std::map<STLString, CPPLCSubEntry*> & registry)
{

	//for now, just use the first entry in m_appliesToList.
	m_pCommonRoot = registry[*m_appliesToList.begin()]->GetParentLoopCluster();

	return m_pCommonRoot;
}

/** @return Pointer ot the common root cluster, or NULL if FindCommonRootUsingEntryRegistry() has not been called yet.*/
CPPLoopCluster* CPPLCAccumEntry::GetCommonRootCluster() const
{
	return m_pCommonRoot;
}

/** @return The target Variable name. */
const STLString& CPPLCAccumEntry::GetVarName() const
{
	return m_varName;
}

/** @return The suffix used by this invocation. */
const STLString& CPPLCAccumEntry::GetSuffix() const
{
	return m_suffix;
}

/** @return The name and suffix concatenated into a single string. */
STLString CPPLCAccumEntry::GetVarNameWithSuffix() const
{
	return m_varName+m_suffix;
}

/** @return The name of the accumulation function. */
const STLString& CPPLCAccumEntry::GetFuncName() const
{
	return m_funcName;
}

/** @return The invocation of the function in the originating expression. */
const STLString& CPPLCAccumEntry::GetInvocation() const
{
	return m_invocation;
}

/** 
	Add an id to the set of variables that are effected by this accumulation function.
	@param id The ID to add the list of affected variables.
*/
void CPPLCAccumEntry::AddAppliesToList(const STLString & id)
{
	m_appliesToList.insert(id);
}
