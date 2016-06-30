#pragma once

#include"OMEDefines.h"
#include "OMEObject.h"
#include "Evaluable.h"
#include "ValCache.h"
#include "OMEException.h"

/** Insulated object used to safely ask for values from a model instance.
Direct Access is also available, but type/bound checking is not used for performance reasons. 

*/
struct __EXPORT__ MIQuery
{
	OMETYPE m_type;			///< Parent Variable type; Used to determine which array to access.
	Model* m_pParentModel;	///< Parent model of ModelInstance. In the case of SubmodelPorts, they query may not exist in the same model as the modelInstance.
	size_t m_ind;				///< The index to retrieve.

	/** Constructor.
		@param pEval Pointer to querying Evaluable object.
		@param ind Index to data field to target for query.
	*/
	MIQuery(const Evaluable* pEval, const size_t ind)
		:m_type(pEval->GetOMEType()),m_ind(ind),m_pParentModel(pEval->GetParentModel()) {}

	/** Constructor.
		@param pEval Pointer to querying Evaluable object.
	*/
	MIQuery(const Evaluable* pEval)
		:m_type(pEval->GetOMEType()),m_ind(pEval->GetModelIndex()),m_pParentModel(pEval->GetParentModel()) {}
};

/** Return object from query made with MIQuery. */
class __EXPORT__ MIValue
{
public:

	/** Default constructor. */
	MIValue() : m_offset(0), m_count(0) {};
	/** Simple Constructor.
		@param v Double value to store within constructed MIValue.
	*/
	MIValue(const OME_SCALAR v) : m_count(1) { m_offset= AppendToCache(v); }
	/** Simple Constructor.
	@param a An OMESclrArray of values to be stored within the constructed MIValue.
	*/
	MIValue(const OMESclrArray & a) : m_count(a.size()) { m_offset = AppendToCache(a); }
	/** Simple Constructor. 
		@param a A SclrVArray of values to be stored within the constructed MIValue.
	*/
	MIValue(const SclrVArray & a) : m_count(a.size()) { m_offset=AppendToCache(a);}
	/** Simple Constructor.
		@param a A pointer to an array of scalar values.
		@param count The number of values in the array pointed to by a.
	*/
	MIValue(OME_SCALAR* a,size_t & count) : m_count(count) { m_offset=AppendToCache(a,count);}

	/** @return true if there are no contained values; false otherwise. */
	inline bool IsEmpty() const { return !m_count; }

	/** @return true if there is only one object in the MIValue; false otherwise.*/
	inline bool IsSingle() const { return m_count==1; }
	//safe accessors
	/** Safely retrieve single value.
		@param outV OME_SCALAR variable to store output value, if successful.
		@return true if single value retrieval is successful; false otherwise.
	*/
	bool GetVal(OME_SCALAR & outV) { if(IsSingle()) outV=GetCache()[m_offset]; return IsSingle(); }
	bool GetVal(SclrVArray & outA);
	bool GetVal(OMESclrArray & outA);

	/** @return The number of values represented by this MIValue.*/
	inline size_t GetValCount() const { return m_count; }
	/** Grab a value at a specific index.
		@param ind The index to use to retrieve the value.
		@return A reference to the value found at index ind.
		@throws OMEOOBException if ind is greater than or equal to the value returned by GetValCount.
	*/
	inline OME_SCALAR& ValAtInd(const size_t & ind) 
	{
		if(ind>=m_offset+m_count)
			throw OMEOOBException("MIValue::ValAtInd: index out of bounds");
		return GetCache()[m_offset+ind];
	}

	/** Treat the MIValue as if it only contains a single value and return the first available value.
		@return A reference to the first value stored within the MIValue.
	*/
	inline OME_SCALAR& SingleVal() {return GetCache()[m_offset];}

	static void ClearValCache();

private:
	//static size_t m_cacheOffset;
	//static OMESclrArray m_array;	///< Array to store returning value(s).
	size_t m_offset;	///< The index of the first value in the global Valcache where this MIValue begins storing its values.
	size_t m_count;		///< The number of values represented by this MIValue.

	static size_t AppendToCache(const OME_SCALAR & val);
	static size_t AppendToCache(const OMESclrArray & a);
	static size_t AppendToCache(const SclrVArray & a);
	static size_t AppendToCache(OME_SCALAR* a,const size_t & count);
	static ValCache<OME_SCALAR>& GetCache();
};

typedef ARRAY_TYPE<MIQuery> MIQueryArray;

/** Wrapper class to reference a subset of MIValues in global ValCaches.*/
class MIOutArray
{
public:
	/** Constructor.
		@param pVc Pointer to the referenced ValCache.
		@param start The index of the first MIValue in the collection.
		@param count The number of contiguous MIValues represented by this object starting at the index in start.
	*/
	MIOutArray(ValCache<MIValue>* pVc,const size_t & start, const size_t & count)
		:m_pCache(pVc),m_start(start),m_count(count) {}

	/** Index operator.
		@param ind The index of the MIValue to retrieve.
		@return A reference to the MIValue at index ind.
		@throws OMEOOBException if ind is greater than or equal to the value returned by size().
	*/
	MIValue& operator[](const size_t & ind)
	{
		if(ind>=m_count)
			throw OMEOOBException("MIOutArray: index out of range");
		return (*m_pCache)[ind];
	}

	/** @return The number of MIValues referenced by this object. */
	inline size_t size() { return m_count; }
private:
	ValCache<MIValue>* m_pCache;		///< Pointer to the ValCache that contains the MIValues of interest.
	size_t m_start;						///< index of the first MIValue in m_pCache of interest.
	size_t m_count;						///< The total number of MIValues starting with the m_start in m_pCache to track.
};
