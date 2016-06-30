#include"CompiledMdlWrappers.h"
#include<limits>
#include<iostream>
#include <new>
#include "omeobjects/VarArray.h"

using namespace OMECFuncs;

#ifndef OME_BWPOOLSIZE
#define OME_BWPOOLSIZE 1000
#endif

///@cond DOX_NO_DOC
namespace LocalTempPools
{
	//Pool for short-lived wrappers.
	ObjMemPool<BaseWrap>	s_IWPool = ObjMemPool<BaseWrap>(OME_BWPOOLSIZE);

	//Default Memory-handling funcs.
	inline BaseWrap* DefaultBWNew(void* e) { return s_IWPool.NewCastPtr(); }
	inline void DefaultBWRelease(BaseWrap* ptr, void* e) { return s_IWPool.ReleasePtr(ptr); }
	inline void DefaultClear(void* e){ s_IWPool.ClearPool(); }
	inline void DefaultRefit(void* e){ s_IWPool.RefitPool(); }

	BWNew BWAlloc = DefaultBWNew;
	BWRelease BWDealloc = DefaultBWRelease;
	BWClearTemps BWSweep = DefaultClear;
	BWAdjustStorage BWRefit = DefaultRefit;

	void* pAllocExtra = NULL;
};
///@endcond

/** Use default temp wrapper allocator functions. */
void OMECFuncs::UseDefaultWrapAllocator()
{
	using namespace LocalTempPools;

	if (!s_IWPool.PoolCapacity())
		s_IWPool.ResizePool(OME_BWPOOLSIZE);
	BWAlloc = DefaultBWNew;
	BWDealloc = DefaultBWRelease;
	BWSweep = DefaultClear;
	BWRefit = DefaultRefit;
	pAllocExtra = NULL;
}

/** Override default temp wrapper allocator functions.
	@param newFunc Function to use for new allocation.
	@param releaseFunc Function to use for deallocation.
	@param clearFunc Function to use to wipe all current temp wrappers.
	@param adjustFunc Function used to adjust space for temp wrappers.
	@param extra Optional pointer to any extra data to pass on to Alloc functions.
*/
void OMECFuncs::OverrideWrapPoolAllocator(BWNew newFunc, BWRelease releaseFunc, BWClearTemps clearFunc, BWAdjustStorage adjustFunc, void*extra)
{
	using namespace LocalTempPools;
	if (s_IWPool.PoolCapacity())
		s_IWPool.ResizePool(0, true);

	BWAlloc = newFunc;
	BWDealloc = releaseFunc;
	BWSweep = clearFunc;
	BWRefit = adjustFunc;
	pAllocExtra = extra;
}

/** Resize the temporary pools for wrappers.
	@param sSize The current size.
	@param aSize The amount of size to add.
*/
void OMECFuncs::ResizeTempPools(const size_t & sSize, const size_t & aSize)
{
	//do nothing for now
}

/**Clear pools for current line. */
void OMECFuncs::ResetLine()
{
	PROFILE_FUNC();
	//EvalCollection::ResetCacheIndex(); 
	OMEUpdateContainers::ClearAllUCPools();
	//LocalTempPools::s_EAliasPool.ClearPool();
	if (LocalTempPools::BWSweep)
		LocalTempPools::BWSweep(LocalTempPools::pAllocExtra);
}

/** Resize temporary pools to account for overallocation. */
void OMECFuncs::AdjustTempPool()
{
	PROFILE_FUNC();
	OMEUpdateContainers::RefitAllUCPools();
	//LocalTempPools::s_EAliasPool.RefitPool();
	if (LocalTempPools::BWRefit)
		LocalTempPools::BWRefit(LocalTempPools::pAllocExtra);

	static short update = 0;
	PROFILER_UPDATE(0);
	PROFILER_OUTPUT(("/Users/pwing_000/Desktop/ShinyReports/OMER" + std::to_string(update++) + ".shiny").c_str());
}

/** @return a pointer to the stack keeping track of call depth; for use with makeArray.*/
std::vector<std::pair<size_t, size_t> >* OMECFuncs::GetPositionStack()
{
	PROFILE_FUNC();
	//position, offset
	static std::vector<std::pair<size_t, size_t> > positionStack;
	return &positionStack;
}

/** Convenience function for Dumping performance profile if Shiny Profiler is enabled.
	@param file Path to location to dump file. 
*/
void OMECFuncs::DumpProfile(const char* file)
{
	PROFILER_UPDATE(0);
	PROFILER_OUTPUT(file);

	//DBG_PRINTC(std::to_string(Caches::s_waNodePool.m_dbgCount), DBG_GREEN);
	//Caches::s_waNodePool.m_dbgCount = 0;
}

/** Retrieve a new temporary list.
	@param len The length of the new temporary list.
	@param pEval Pointer to the represented Evaluable object.
	@return Pointer to new temp list.
*/
Listable* OMECFuncs::NewTempList(const size_t & len, Evaluable* pEval)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTempNoFill(len, pEval);
	return ret;
}

/** Retrieve a new temporary list.
	@param len The length of the new temporary list.
	@param fill The default value to fill the new list with.
	@param pEval Pointer to the represented Evaluable object.
	@return Pointer to new temp list.
*/
Listable* OMECFuncs::NewTempList(const size_t & len, const size_t & fill, Evaluable* pEval)
{
	return TEMP_LIST_TYPE::NewTemp(len, fill, pEval);
}

/** Retrieve a new temporary list.
	@param dims The dimensions used to define the new temporary list.
	@param lvl The depth level to associate with a temporary list.
	@param pEval Pointer to the represented Evaluable object.
	@return Pointer to new temp list.
*/
Listable* OMECFuncs::NewTempList(const ListDims & dims, const size_t & lvl, Evaluable* pEval)
{
	return TEMP_LIST_TYPE::NewTempNoFill(dims, lvl, pEval);
}

/** Create a temporary copy of a list.
	@param lstl The list to copy.	
	@return Pointer to new temp list.
*/
Listable* OMECFuncs::NewTempCopyList(Listable & lstl)
{
	Listable* ret = TEMP_LIST_TYPE::NewTempNoFill(lstl.GetSize(), lstl.GetRepEval());
	for (size_t i = 0; i < lstl.GetSize(); ++i)
		(*ret)[i] = lstl[i];

	return ret;
}

/** Create a temporary copy of a list.
	@param lst The list to copy.
	@param inds List of specific indices to copy.
	@return Pointer to new temp list.
*/
Listable* OMECFuncs::NewTempCopyList(Listable & lst, Listable & inds)
{
	return ShuffledList::NewTempShuffledList(lst, inds, true);
}

/** Create a temporary alias for a new object.
	@param pEval Pointer to the Evaluable object to alias.
	@param pRep Pointer to value to associate with alias.
	@param index Index of aliased value.
	@return Pointer to a newly allocated temporary wrapper.
*/
AliasWrap* OMECFuncs::NewAlias(Evaluable* pEval, OME_SCALAR* pRep, const size_t & index)
{
	//return new(LocalTempPools::s_EAliasPool.NewCastPtr())AliasWrap(pEval,pRep,index);
	return new(LocalTempPools::BWAlloc(LocalTempPools::pAllocExtra))AliasWrap(pEval, pRep, index);
}

/** Retrieve a temporary array that covers multiple instance values for a provided object.
	@param pEval Pointer to the object to collect values from.
	@param lvl The degree of up-scoping to use to retrieve values.
	@return Pointer to temp list containing upscoped values.
*/
StaticUpArray* OMECFuncs::NewUpArray(Evaluable* pEval, const size_t & lvl)
{
	StaticUpArray* ret;
	if (!pEval->GetParentModel()->IsDynamic())
		ret = StaticUpArray::NewSUA(pEval, lvl);
	else
		ret = DynamicUpArray::NewDUA(pEval, lvl);
	return ret;
}

/** Retrieve a temporary array that covers multiple instance values for a provided object.
@param pVa Pointer to the VarArray to collect values from.
@param lvl The degree of up-scoping to use to retrieve values.
@return Pointer to temp list containing upscoped values.
*/
StaticUpArray* OMECFuncs::NewUpArray(VarArray* pVa, const size_t & lvl)
{
	StaticUpArray* ret;
	if (!pVa->GetParentModel()->IsDynamic())
		ret = StaticUpArray::NewSUA(pVa, lvl);
	else
		ret = DynamicUpArray::NewDUA(pVa, lvl);
	return ret;
}

/** Release a temporary list.
	@param il Pointer to Listable to dealloc.
*/
void OMECFuncs::DeletePoolPtr(Listable* il)
{
	TEMP_LIST_TYPE::ReleaseTemp((TEMP_LIST_TYPE*)il);
}

//void OMECFuncs::DeletePoolPtr(AliasWrap* ea)
//{
//	LocalTempPools::s_EAliasPool.ReleasePtr(ea);
//}

/** Release a temporary list.
	@param sua Pointer to StaticUpArray to dealloc.
*/
void OMECFuncs::DeletePoolPtr(StaticUpArray* sua)
{
	StaticUpArray::ReleaseSUA(sua);
}

/** Release a temporary list.
	@param dua Pointer to DynamicUpArray to dealloc.
*/

void OMECFuncs::DeletePoolPtr(DynamicUpArray* dua)
{
	DynamicUpArray::ReleaseDUA(dua);
}

/** Release a temporary list.
	@param sl Pointer to ShuffledList to dealloc.
*/

void OMECFuncs::DeletePoolPtr(ShuffledList* sl)
{
	ShuffledList::ReleaseTempShuffledList(sl);
}

/** Release a temporary list.
	@param ifw Pointer to Wrapper to dealloc.
*/

void OMECFuncs::DeletePoolPtr(BaseWrap* ifw)
{
	LocalTempPools::BWDealloc(ifw, LocalTempPools::pAllocExtra);
}

/** Create a new wrapper for the supplied value.
	@param scalar The Scalar to encapsulate.
	@return Reference to newly allocated wrapper.
*/
BaseWrap& OMECFuncs::Wrap(const OME_SCALAR & scalar)
{
	return *new(LocalTempPools::BWAlloc(LocalTempPools::pAllocExtra))SclrWrap(scalar);
}

/** Create a new wrapper for the supplied value.
	@param pEval Pointer to the Evaluable to encapsulate.
	@return Reference to newly allocated wrapper.
*/
BaseWrap& OMECFuncs::Wrap(Evaluable* pEval)
{
	return *new(LocalTempPools::BWAlloc(LocalTempPools::pAllocExtra))EvalWrap(pEval);
}

/** Create a new wrapper for the supplied value.
	@param pVa Pointer to the VarArray to encapsulate.
	@return Reference to newly allocated wrapper.
*/
BaseWrap& OMECFuncs::Wrap(VarArray* pVa)
{
	return *new(LocalTempPools::BWAlloc(LocalTempPools::pAllocExtra))ListWrap(*pVa);
}

/** Create a new wrapper for the supplied value.
	@param list The List to encapsulate.
	@return Reference to newly allocated wrapper.
*/

BaseWrap& OMECFuncs::Wrap(Listable& list)
{
	return *new(LocalTempPools::BWAlloc(LocalTempPools::pAllocExtra))ListWrap(list);
}

/** Create a new wrapper for the supplied value.
	@param pEe Pointer to the EnumEntry to encapsulate.
	@return Reference to newly allocated wrapper.
*/
BaseWrap& OMECFuncs::Wrap(EnumEntry* pEe)
{
	return *new(LocalTempPools::BWAlloc(LocalTempPools::pAllocExtra))EnumWrap(pEe);
}