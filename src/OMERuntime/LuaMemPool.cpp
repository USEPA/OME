#include "LuaMemPool.h"

//the +7 ensures that any value that doesn't fall on boundary will line up
// this may not always be needed, but may come in handy
#define COUNT_IDX(x)	((((PoolCell*)x)-(PoolCell*)m_pool)/OME_LUAPOOLSTEPSIZE)

#define OME_LUAPOOLSTEPSIZE	(OME_LUAPOOLSLOTSIZE/8)

LuaMemPool::LuaMemPool()
	:MemPool(), m_blockCounts(NULL), m_overflowCount(0)
{}

/** Simple constructor.
	@param slotCount The initial number of available blocks.
*/
LuaMemPool::LuaMemPool(const size_t & slotCount)
	: MemPool(slotCount), m_overflowCount(0)
{
	m_blockCounts = new size_t[slotCount]();
}

LuaMemPool::~LuaMemPool()
{
	delete[] m_blockCounts;
	for (auto itr = m_overBlocks.begin(); itr != m_overBlocks.end(); ++itr)
		operator delete((*itr).first);
}

void* LuaMemPool::NewPtr()
{
	return NewBlockPtr(1);
}
 
void LuaMemPool::ReleasePtr(void* ptr)
{
	ReleaseBlockPtr(ptr);
}

/** Attempt to resize a collection of allocated blocks in place.
	@param oldPtr Pointer to the first block of the chunk to resize.
	@param oldBlockCount The count of blocks pointed to by oldPtr.
	@param newBlockCount The desired new block count.
	@return Either oldPtr if the inplace-resize was successful, or a new set of blocks allocated with the contents of oldPtr.
*/
void* LuaMemPool::ReallocBlockPtr(void* oldPtr, const size_t & oldBlockCount, const size_t & newBlockCount)
{
	//try to allocate append to end
	if (IsInPool(oldPtr))
	{
		
		//if new size is smaller, decrement block size and return oldptr
		if (oldBlockCount >= newBlockCount)
		{
			OLP_DUMP(cout << BlockCountToBytes(newBlockCount) << endl, DBG_CYAN);
			m_blockCounts[COUNT_IDX(oldPtr)] =newBlockCount;
			return oldPtr;
		}

		//if newsize is larger, try to extend blocks
		size_t i = COUNT_IDX(oldPtr) + m_blockCounts[COUNT_IDX(oldPtr)];
		for (; i<newBlockCount && i < m_maxItems && m_blockCounts[i] == 0; ++i);

		if (i == newBlockCount)
		{
			OLP_DUMP(cout << oldPtr << endl, DBG_GREEN);
			OLP_DUMP(cout << (PoolCell*)oldPtr+newBlockCount << endl, DBG_RED);

			//in place expansion;
			m_blockCounts[COUNT_IDX(oldPtr)] = newBlockCount;
			return oldPtr;
		}
	}

	//otherwise, realloc, copy and clear
	void* outBlock=NewBlockPtr(newBlockCount);
	if (oldPtr)
	{
		memcpy(outBlock, oldPtr, BlockCountToBytes(std::min(oldBlockCount,newBlockCount)));
		ReleaseBlockPtr(oldPtr);
	}
	return outBlock;
}

void LuaMemPool::ClearPool()
{
	//MemPool::ClearPool();
	memset(m_blockCounts, 0, sizeof(m_blockCounts));
}

void LuaMemPool::ResizePool(int newSize, const bool & skipCheck)
{
	MemPool::ResizePool(newSize, skipCheck);	//will throw if check fails
	delete[] m_blockCounts;
	m_blockCounts = new size_t[newSize]();

	m_maxAllocated = 0;
	memset(m_blockCounts, 0, sizeof(m_blockCounts));
}

void LuaMemPool::RefitPool(size_t buff, const bool & skipCheck, const bool & allowShrink)
{
	if (m_maxAllocated > m_maxItems || (allowShrink && m_maxAllocated < m_maxItems))
		ResizePool(m_maxAllocated + buff, skipCheck);
}

/** Retrieve a pointer to a number of contiguous blocks.
	@param slotCount The number of slots to return with memory address.
	@return Pointer to the first allocated block.
*/
void* LuaMemPool::NewBlockPtr(const size_t & slotCount)
{
//	if (slotCount <= 1)
//		return NewPtr();

	void* ret = NULL;

	//run through pool until large enough block is hit
	bool go = false;
	if (GetFreeCount() >= slotCount)
	{
		size_t i = 0;
		size_t runCount;
		size_t minCount=m_maxItems+1;
		PoolCell *currPtr;

		while( i < m_maxItems)
		{
			//if (i == 0)
			//{
			currPtr = m_pool + i*(OME_LUAPOOLSTEPSIZE);
				OLP_DUMP(cout << currPtr << endl<<i<<endl, DBG_YELLOW);
				runCount = 0;
				for (; m_blockCounts[i] == 0 && i < m_maxItems; ++i, ++runCount);

				if (runCount>=slotCount && runCount<minCount)
				{
					go = true;
					ret = currPtr;
					size_t k = COUNT_IDX(currPtr);
					OLP_DUMP(cout << currPtr - m_pool << endl << k << endl , DBG_GREEN);
					m_blockCounts[COUNT_IDX(ret)] = slotCount;
					//exact match; break here
					if (runCount == slotCount)
						break;
				}

				if (i < m_maxItems)
					i += m_blockCounts[i];
			//}
		}
	}

	if (go)
	{
		//m_next = (((PoolCell*)ret) + slotCount*sizeof(PoolCell*))->next;
		OLP_DUMP(cout << ret << endl, DBG_MAGENTA);
		OLP_DUMP(cout << (PoolCell*)ret + slotCount*(OME_LUAPOOLSTEPSIZE) << endl<<OME_LUAPOOLSLOTSIZE<<endl, DBG_RED);
		for (size_t i = 0; i < slotCount; ++i)
			OLP_DUMP(cout << (PoolCell*)ret + i*(OME_LUAPOOLSTEPSIZE) << endl, DBG_WHITE);
	}
	else
	{
		OLP_DUMP(cout << BlockCountToBytes(slotCount) << endl, DBG_MAGENTA);
		ret = operator new(BlockCountToBytes(slotCount));
		
		m_overBlocks[ret] = slotCount;
		m_overflowCount += slotCount;
		
	}

	m_allocCount += slotCount;
	if (m_allocCount > m_maxAllocated)
		m_maxAllocated = m_allocCount;
	//OLP_DUMP(cout << ret << endl, DBG_GREEN);
	return ret;
}

/** Release a set of blocks back into the pool.
	@param ptr Pointer to blocks to release.
	@return true if Ptr was allocated by this pool; false otherwise. If false, no deallocation has taken place.
*/
bool LuaMemPool::ReleaseBlockPtr(void* ptr)
{
	bool ret = true;
	std::map<void*,size_t>::iterator itr;
	if (IsInPool(ptr))
	{
		//retrieve count
		size_t idx = COUNT_IDX(ptr);
		size_t tot = m_blockCounts[idx];
		

		//wire returned cells back up
		//PoolCell* currPtr;
		//PoolCell* nextPtr = (PoolCell*)&ptr;
		//for (size_t i = 0; i < tot; ++i)
		//{
		//	currPtr = nextPtr;
		//	nextPtr = &currPtr[1];
		//	currPtr->next = nextPtr;
		//}

		//currPtr->next = m_next;
		//m_next = (PoolCell*)ptr;

		//plug slots 
		//memset(&m_blockCounts[idx], 0, sizeof(size_t)*tot);
		m_blockCounts[idx] = 0;
		m_allocCount -= tot;
	}
	else 
	{
		itr = m_overBlocks.find(ptr);
		if (itr != m_overBlocks.end())
		{
			m_allocCount -= (*itr).second;
			m_overflowCount -= (*itr).second;
			operator delete(ptr);
			m_overBlocks.erase(itr);
			
		}
		else 
			ret = false;
	}

	//OLP_DUMP(cout << ptr << endl, DBG_RED);
	return ret;
}