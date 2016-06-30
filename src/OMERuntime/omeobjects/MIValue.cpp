#include "omeobjects/MIValue.h"

// MIValue statics
namespace MIVStatics
{
	ValCache<OME_SCALAR> m_cache=ValCache<OME_SCALAR>();
}

/** Reset the global cache that stores the values for all MIValues.*/
void MIValue::ClearValCache() 
{ 
	MIVStatics::m_cache.ResetCache();
}

/** Append a value to the global cache.
	@param val The value to append.
	@return The index of the next available index.
*/
size_t MIValue::AppendToCache(const OME_SCALAR & val)
{
	using namespace MIVStatics;
	m_cache.Append(val);
	return m_cache.GetSize()-1;
}

/** Append a value to the global cache.
@param a A container of values to append.
@return The index of the next available index.
*/
size_t MIValue::AppendToCache(const OMESclrArray & a)
{
	using namespace MIVStatics;

	for(auto itr=a.begin(); itr!=a.end(); ++itr)
		AppendToCache(*itr);
	return m_cache.GetSize()-1;
}

/** Append a value to the global cache.
@param a A container of values to append.
@return The index of the next available index.
*/
size_t MIValue::AppendToCache(const SclrVArray & a)
{
	using namespace MIVStatics;
	size_t out=m_cache.GetSize();
	for(size_t i=0; i<a.size(); i++)
		m_cache.Append(a[i]);
	return out;
}

/** Append a value to the global cache.
@param a A pointer to an array of values to append.
@param count The number of values in the array pointed to by a.
@return The index of the next available index.
*/
size_t MIValue::AppendToCache(OME_SCALAR* a,const size_t & count)
{
	using namespace MIVStatics;
	size_t out=m_cache.GetSize();
	m_cache.Append(a,count);
	return out;
}

/** @return pointer to global MIValue ValCache.*/
ValCache<OME_SCALAR>& MIValue::GetCache()
{
	return MIVStatics::m_cache;
}


//MIValue
/** Safely retrieve multiple values.
	@param outA SclrVArray variable to store output values, if successful.
	@return true if multi-value retrieval is successful; false otherwise.
*/
bool MIValue::GetVal(SclrVArray & outA)
{ 
	PROFILE_FUNC();
	if(!IsSingle())
	{
		outA.resize(m_count);
		for(size_t i=0; i<m_count; i++)
			outA[i]=MIVStatics::m_cache[m_offset+i];
	}
	return !IsSingle(); 
}

/** Safely retrieve multiple values.
	@param outA OMESclrArray variable to store output values, if successful.
	@return true if multi-value retrieval is successful; false otherwise.
*/
bool MIValue::GetVal(OMESclrArray & outA)
{ 
	if(!IsSingle())
	{
		for(size_t i=0; i<m_count; i++)
			outA.push_back(MIVStatics::m_cache[m_offset+i]);
	}
	return !IsSingle(); 
}
