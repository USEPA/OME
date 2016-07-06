#pragma once
//#include "OMEDefines.h"
#include "DebugOutput.h"
#include<list>
#include<vector>
#include <algorithm>
#include "ValCache.h"
#include "OMEException.h"

//useful info: http://molecularmusings.wordpress.com/2012/09/17/memory-allocation-strategies-a-pool-allocator/

/** Represents an object the size of a standard 8-bit byte.*/
typedef unsigned char MPByte;
/** Used to represent the space of a single block. */
union PoolCell
{
    void* pVal;				///< The current value stored in active block.
    PoolCell* next;			///< Pointer to next available block in an inactive block.
};

#define MPOFFSET(x,y)	(PoolCell*)(((MPByte*)(x))+(y))

/** Basic Memory pool implementation. 
	@tparam bkSize The Size of an individual memory block in bytes.
*/
template<size_t bkSize>
class MemPool
{
public:
	MemPool();
	MemPool(size_t plSize);

	virtual ~MemPool();

	virtual void* NewPtr();
	virtual void ReleasePtr(void* ptr);
	virtual void ClearPool();
	virtual void ResizePool(int newSize,const bool & skipCheck=false);
	/** Resize the pool to fit the number of additional objects based on the count.
		@param buff The number of slots to include in addition to m_overshoot; can be 0.
		@param skipCheck If false, pool resizing will fail if any pool blocks are still in use. Otherwise,
		resize will occur regardless of pool block allocations.
		@param allowShrink If true, allow the pool to be resized to a smaller size than the current pool size,
		assuming the maximum allocation count never reached the total pool size. Otherwise, the pool will only
		be allowed to grow in size as needed.
		@sa GetOvershoot.
	*/
	virtual void RefitPool(size_t buff = 0, const bool & skipCheck = false, const bool & allowShrink = false) {};

	size_t GetAllocCount() const;
	size_t GetFreeCount() const;
	size_t GetAllocBytes() const;
	size_t GetFreeBytes() const;
	/** @return the size of an individual pool block in bytes.*/
	inline size_t GetPtrSize() const { return m_slotSize; };
	size_t GetMaxAllocated() const;
	size_t PoolCapacity() const;
	bool IsInPool(void* ptr) const;

	/** Utility function for producing a CLI display of the current pool availability. */
	virtual void DebugDisplay()
	{
		std::vector<bool> log(m_maxItems,true);
		PoolCell* pCurr=m_next;
		size_t i=0;
		size_t ind;
		for(; pCurr; pCurr=pCurr->next,i++)
		{
			//DBG_COLOR(cout << pCurr << endl, DBG_MAGENTA);
			ind = (pCurr - m_pool)*8 / bkSize;
			log[ind]=false;
		}
		
		for(i=0; i<log.size(); i++)
			DBG_COLOR(DBG_PARTIAL_PRINT("|"),log[i] ? DBG_RED : DBG_GREEN);
		DBG_PRINT("");
	}

	//void DebugWalk()
	//{
	//	PoolCell* pVal = m_next;
	//	std::list<PoolCell*> visited;
	//	size_t i = 0;
	//	while (pVal && i<m_maxItems)
	//	{
	//		//DBG_COLOR(cout << pVal << endl, DBG_CYAN);
	//		if (std::find(visited.begin(),visited.end(),pVal)!=visited.end())
	//		{
	//			cout << "LOOP DETECTED" << endl;
	//				break;
	//		}
	//		visited.push_back(pVal);
	//		if (pVal < m_pool || pVal >= m_last)
	//		{
	//			cout <<m_pool<<" --> "<< pVal << " <-- "<<m_last<<endl;
	//			break;
	//		}
	//		pVal = pVal->next;
	//		i++;
	//	}
	//	
	//	if (i >= m_maxItems)
	//		cout << "MaxCount exceeded." << endl;
	//}

    
protected:

	
	PoolCell* m_pool;			///< Pointer to the front of the pool block.
	PoolCell* m_next;			///< Pointer to the next available memory block.
	PoolCell* m_last;			///< Pointer to the first address beyond the end of the pool block.
	PoolCell* m_block;			///< Pointer to the start of the remaining memory during sequential block allocation.
	size_t m_maxItems;			///< The total number of items that can be held in the pool at any given time.
	size_t m_slotSize;			///< The size of an individual pool block, in bytes.
	size_t m_allocCount;		///< Total number of pool blocks currently in use.
	size_t m_maxAllocated;		///< The largest number of pool blocks that have been allocated at any given time.

};


/** Default constructor.*/
template<size_t bkSize>
MemPool<bkSize>::MemPool()
:m_pool(NULL), m_maxItems(0), m_next(NULL), m_last(NULL),m_block(NULL),  m_slotSize(0), m_allocCount(0), m_maxAllocated(0)
{

}

/** Simple Constructor. 
	@param size The Number of blocks of bkSize to allocate.
*/
template<size_t bkSize>
MemPool<bkSize>::MemPool(size_t size)
:m_pool(NULL), m_maxItems(0), m_next(NULL), m_last(NULL),m_block(NULL), m_slotSize(0), m_allocCount(0), m_maxAllocated(0)
{
	ResizePool((int)size);
}

/** Destructor. Frees any memory associated with the pool block.*/
template<size_t bkSize>
MemPool<bkSize>::~MemPool()
{
	free(m_pool);
	//for(auto itr=m_additionals.begin(); itr!=m_additionals.end(); ++itr)
	//	free(*itr);
}

/** Retrieve a pointer to the next available pool block.
	@return an address to a block of at least bkSize bytes, or NULL if the pool is full.
*/
template<size_t bkSize>
void* MemPool<bkSize>::NewPtr()
{
	void* ret=NULL;
	//DebugWalk();

	if (m_block < m_last)
	{
		ret = (void*)m_block;
		m_block=MPOFFSET(m_block,m_slotSize);
	}
	else if (m_next)
	{
		//if there is space in the pool, provide it
		PoolCell* newHead = m_next->next;
		ret = (void*)m_next;
		m_next = newHead;

		//DBG_PRINT("NEW");
		//DebugDisplay();

	}
	else
		return NULL;

	m_allocCount++;
	if (m_allocCount > m_maxAllocated)
		m_maxAllocated = m_allocCount;

	//DebugWalk();
	return ret;
}

/** Release a block back the the pool.
	@param ptr Address of pool block to mark as available.
*/
template<size_t bkSize>
void MemPool<bkSize>::ReleasePtr(void* ptr)
{
	//DebugWalk();

	if(ptr>=m_pool && ptr<m_last)
	{
#ifdef OME_MEMPOOLCHECK
		PoolCell* check = m_next;
		while (check)
		{
			if (check == ptr)
				cout << "OH NOES" << endl;
			check = check->next;
		}
#endif
		//ptr is in pool
		((PoolCell*)ptr)->pVal = NULL;
		((PoolCell*)ptr)->next=m_next;
		m_next=(PoolCell*)&((PoolCell*)ptr)->pVal;
		m_allocCount--;
		//DBG_PRINT("DELETE");
		
		//DebugDisplay();
	}
	else
	{
		std::cout << m_pool << "::" << ptr << "::" << m_last << std::endl;
		throw OMEMemoryException("Mempool::ReleasePtr()","MemPool is being asked to release memory it doesn't own.");
		//ptr is in additionals
		//auto fItr=std::find(m_additionals.begin(),m_additionals.end(),ptr);
		//if (fItr != m_additionals.end())
		//	m_additionals.erase(fItr);
		//free(ptr);
	}
	//DebugWalk();
}

/** Mark all pool blocks as available. */
template<size_t bkSize>
void MemPool<bkSize>::ClearPool()
{
	//set all active flags to false
	if (m_maxItems && m_slotSize)
	{
		m_next = NULL;
		m_block = m_pool;
		m_allocCount = 0;
		

		/*potential for parallelization here:
			*Each thread takes an offset from the start address
			*Takes resulting address and assigns next address.
			*last address has its next set to NULL.
		 */
		//for (size_t i = 0; i < m_maxItems; i++)
		//{
		//	m_next = m_last;
		//	
		//	m_last = MPOFFSET(m_last, m_slotSize);
		//	m_next->next = m_last;
		//	//DBG_COLOR(cout << '\t' << m_last << endl, DBG_CYAN);
		//}
		//m_next->next = NULL;
		//m_next = m_pool;
	}
	//preserve overshoot count; that isn'bkSize reset unless pool is reallocated.

	//clear any additionals
	//for(auto itr=m_additionals.begin(); itr!=m_additionals.end(); ++itr)
	//	free(*itr);
	//
	//m_additionals.clear();
	//DebugWalk();
}

/** Reallocate a new pool block of the requested size.
	@param newSize The request size for the new pool block.
	@param skipCheck If false, pool resizing will fail if any pool blocks are still in use. Otherwise, 
	resize will occur regardless of pool block allocations.
*/
template<size_t bkSize>
void MemPool<bkSize>::ResizePool(int newSize,const bool & skipCheck)
{
	//DebugWalk();
	if(skipCheck || !m_allocCount)
	{
		free(m_pool);
		//since linked list is stored inside slots of pool, memory must always be at least the size of pointer
		m_slotSize=bkSize>sizeof(PoolCell) ? bkSize : sizeof(PoolCell);

		m_pool=(PoolCell*)calloc(newSize,m_slotSize);
		m_maxItems = newSize;
		m_last = MPOFFSET(m_pool, m_slotSize*m_maxItems);
		ClearPool();

		if (newSize != 0 && !m_pool)
			throw OMEMemoryException("MemPool","Resize failed.");
		
		// ("MemPool::ResizePool: MemoryPool allocation failed");
//		DBG_COLOR(cout<<"RESIZE TO: "<<newSize<<" ("<<sizeof(bkSize)<<" * "<<m_log.size()<<")"<<endl<<m_pool<<"->"<<m_last<<"::"<<m_last-m_pool<<endl,DBG_MAGENTA);
	}
	else throw OMEMemoryException("MemPool", "Attempt to resize MemPool while memory is still allocated");
	//DebugWalk();
}

/** @return The number of unallocated pool blocks within the pool.*/
template<size_t bkSize>
size_t MemPool<bkSize>::GetFreeCount() const
{
	return m_maxItems-m_allocCount;
}

/** @return The number of allocated pool blocks within the pool.*/
template<size_t bkSize>
size_t MemPool<bkSize>::GetAllocCount() const
{
	return m_allocCount;
}

/** @return The number of allocated bytes within the pool.*/
template<size_t bkSize>
size_t MemPool<bkSize>::GetAllocBytes() const
{
	return GetAllocCount()*sizeof(bkSize>sizeof(void*) ? bkSize : sizeof(void*));
}

/** @return The number of unallocated bytes within the pool.*/
template<size_t bkSize>
size_t MemPool<bkSize>::GetFreeBytes() const
{
	return GetFreeCount()*sizeof(bkSize>sizeof(void*) ? bkSize : sizeof(void*));
}

/** @return the largest number of pool blocks that have been allocated at any given time so far*/
template<size_t bkSize>
size_t MemPool<bkSize>::GetMaxAllocated() const
{
	return m_maxAllocated;
}

/** @return the largest number of pool blocks that can be allocated at any given time.*/
template<size_t bkSize>
size_t MemPool<bkSize>::PoolCapacity() const
{
	return m_maxItems;
}

/** Determine if an address resides in the pool block.
	@param ptr The address to query.
	@return true if ptr resides in the pool block, false otherwise.
*/
template<size_t bkSize>
bool MemPool<bkSize>::IsInPool(void* ptr) const
{
	return ptr!=NULL && m_pool <= ptr && ptr < m_last;
}

//
//
//
//

/** Wrapper struct to encapsulate. 
	@tparam T The object to take block size info from.
*/
template<class T>
struct MPMemBlock
{
	MPByte block[sizeof(T)];		///< Array used to represent the number of Bytes taken by T.
	//bool active;
	//MPMemBlock() :active(false) {}
};

/** Object-based wrapper for MemPool which uses the class size to allocate blocks.
	
	__NOTE__: Doxygen has issues with inheriting documentation from templated superclasses. For full documentation, see MemPool.
	@tparam T The type of object that is expect to be stored in this pool.
	@sa MemPool
*/
template<class T>
class ObjMemPool : public MemPool<sizeof(MPMemBlock<T>)>
{
public:
	/** Function pointer type to be used to pass in init functions for newly allocated objects of type T.*/
	typedef void(*InitFunc)(T*,void*);
	/** Aliase for memory block used as template for pool blocks.*/
	typedef MPMemBlock<T> MemBlock;

	ObjMemPool();
	ObjMemPool(size_t size);

	size_t GetOvershoot() const;

	virtual ~ObjMemPool();

	virtual void* NewPtr();
	virtual T* NewCastPtr(InitFunc IFunc=NULL,void* extra=NULL);
	virtual T* NewCastPtr(bool & isFromOverFlow, InitFunc IFunc = NULL,void* extra=NULL);
	virtual void ReleasePtr(T* ptr);
	virtual void RefitPool(size_t buff = 0, const bool & skipCheck = false,const bool & allowShrink=false);
	virtual void ResizePool(int newSize, const bool & skipCheck = false);
	virtual void ClearPool();
	bool IsInOverFlow(void* ptr) const;
	/** Reset the ValCache that holds objects that could not be fit into the pool. */
	inline void ResetOverFlowCache() { m_additionals.ResetCache(); }

protected:
	size_t m_overshoot;						///< maximum value of over allocated objects.
	ValCache< MemBlock> m_additionals;		///< ValCache used to handle any objects that overflow from MemPool allocations.
};

/** Default Constructor. */
template<class T>
ObjMemPool<T>::ObjMemPool()
:MemPool<sizeof(MemBlock)>()
{

}

/** Simple Constructor. 
	@param size The number of T-sized objects to allocate.
*/
template<class T>
ObjMemPool<T>::ObjMemPool(size_t size)
:MemPool<sizeof(MemBlock)>(size), m_overshoot(0)
{}

/** Destructor. */
template<class T>
ObjMemPool<T>::~ObjMemPool()
{}

/** Retrieve Address to pool block that has been cast to type T.
	@param IFunc Optional pointer to function to be used to initialize the pool block.
	@param extra Optional pointer to user-defined data to pass into IFunc.
	@return A Pointer to an object of type T on success, Null otherwise.
*/
template<class T>
T* ObjMemPool<T>::NewCastPtr(InitFunc IFunc,void* extra)
{
	T* ret=(T*)NewPtr();
	if (IFunc)
		IFunc(ret,extra);
	return ret;
}

/** Retrieve Address to pool block that has been cast to type T.
@param isFromOverFlow Variable to hold flag which indicates if the memory was allocated from the overflow space.
@param IFunc Optional pointer to function to be used to initialize the pool block.
@param extra Optional pointer to user-defined data to pass into IFunc.
@return A Pointer to an object of type T on success, Null otherwise.
*/
template<class T>
T* ObjMemPool<T>::NewCastPtr(bool & isFromOverFlow, InitFunc IFunc,void* extra)
{
	isFromOverFlow = !this->m_next;
	return NewCastPtr(IFunc,extra);
}

template<class T>
void* ObjMemPool<T>::NewPtr()
{
	if (this->m_next || this->m_block<this->m_last)
		return (void*)MemPool<sizeof(MemBlock)>::NewPtr();
	else
	{
		if (m_additionals.GetSize()>m_overshoot)
			m_overshoot=m_additionals.GetSize();
		size_t last = m_additionals.Append(MemBlock());

		return (void*)&m_additionals[last];
	}
}

/** Release pointer of type T and call its destructor.
	@param ptr Pointer to destruct and release.
*/
template<class T>
void ObjMemPool<T>::ReleasePtr(T* ptr)
{
	ptr->~T();
	if (this->IsInPool(ptr))
		MemPool<sizeof(MemBlock)>::ReleasePtr((void*)ptr);

	//else pointer probably belongs to valCache so ignore
}

template<class T>
void ObjMemPool<T>::ClearPool()
{
	MemPool<sizeof(MemBlock)>::ClearPool();
	m_additionals.ResetCache();
}

/** @return the number of objects allocated beyond the capacity of the MemPool. */
template<class T>
size_t ObjMemPool<T>::GetOvershoot() const
{
	return m_overshoot;
}

template<class T>
void ObjMemPool<T>::RefitPool(size_t buff, const bool & skipCheck,const bool & allowShrink)
{
	size_t extra = buff + m_overshoot;
	if (extra)
	{
		this->ResizePool(int(this->m_maxItems + extra), skipCheck);
		m_additionals.ResizeCache(0);
	}
	else if (allowShrink && this->m_maxAllocated<this->m_maxItems)
	{
		this->ResizePool(int(this->m_maxAllocated + buff), skipCheck);
		m_additionals.ResizeCache(0);
	}
	else if (skipCheck && this->m_allocCount)
		this->ClearPool();

}

template<class T>
void ObjMemPool<T>::ResizePool(int buff, const bool & skipCheck)
{
	if (skipCheck || !m_additionals.GetSize())
	{
		MemPool<sizeof(MemBlock)>::ResizePool(buff, skipCheck);
		m_overshoot = 0;
		m_additionals.ResetCache();
	}
}

/** Determine if a memory address in this pool belongs to the overflow ValCache.
	@param ptr The address to test.
	@return true if the address in ptr is in the overflow ValCache; false otherwise.
*/
template<class T>
bool ObjMemPool<T>::IsInOverFlow(void* ptr) const
{
	return std::any_of(m_additionals.begin(), m_additionals.end(), [&ptr](void* p){return p == ptr; });
}