#pragma once

#include "MemPool.h"
#include "CompiledMdlFuncs.h"
#include <set>

//#define OLP_VERBOSE 
#ifdef OLP_VERBOSE
#define OLP_DUMP(x,y) DBG_COLOR(x,y) 
#else
#define OLP_DUMP(x,y)
#endif

#define OME_LUAPOOLTYPE	OMECFuncs::BaseWrap
#define OME_LUAPOOLSLOTSIZE	(sizeof(OME_LUAPOOLTYPE)*2)
//#define OME_LUAPOOLSLOTSIZE	16

/** Block Memory Pool for use with Lua Interpreter.
*/
class LuaMemPool : MemPool < OME_LUAPOOLSLOTSIZE >
{
public:
	LuaMemPool();
	LuaMemPool(const size_t & slotCount);

	virtual ~LuaMemPool();

	virtual void* NewPtr();
	virtual void ReleasePtr(void* ptr);
	virtual void ClearPool();
	virtual void ResizePool(int newSize, const bool & skipCheck = false);
	virtual void RefitPool(size_t buff = 0, const bool & skipCheck = false, const bool & allowShrink = false);

	void* NewBlockPtr(const size_t & slotCount);
	void* ReallocBlockPtr(void* oldPtr, const size_t & oldBlockCount, const size_t & newBlockCount);
	bool ReleaseBlockPtr(void* ptr);

	/** @return The total number of overflow allocations.*/
	inline size_t GetOverflowCount() { return m_overflowCount; }

	inline bool CheckAlignment(void* ptr);
	
	static inline size_t BytesToBlockCount(const size_t & byteCount);
	static inline size_t BlockCountToBytes(const size_t & blockCount);

private:

	size_t* m_blockCounts;				   ///< An array containing the count of contiguously allocated blocks.
	size_t m_overflowCount;				   ///< The amount of overflow from the base pool (in blocks).
										   
	std::map<void*,size_t> m_overBlocks;   ///< Map that holds blocks allocated outside the pool, along with their sizes.
};

/** Check to see if an address aligns along block boundaries.
	@param ptr Address to evaluate.
	@return true if ptr lies on a block boundary; false otherwise.
*/
bool LuaMemPool::CheckAlignment(void* ptr)
{
	size_t adjusted = (char*)ptr - (char*)m_pool;
	size_t reduced = adjusted / OME_LUAPOOLSLOTSIZE;
	return !(adjusted - (reduced * OME_LUAPOOLSLOTSIZE));
}

/** Convert a number of bytes into the smaller number of blocks that can contain them.
	@param byteCount the number of bytes to evaluate.
	@return The number of blocks it would take to encapsulate the number passed in byteCount.
*/
size_t LuaMemPool::BytesToBlockCount(const size_t & byteCount)
{
	return (byteCount + (OME_LUAPOOLSLOTSIZE - 1)) / OME_LUAPOOLSLOTSIZE;
}

/** Convert a number of blocks into their equivalent in bytes.
	@param blockCount The measurement of blocks to convert into bytes.
	@return The number of bytes needed to represent the amount in blockCount.
*/
size_t LuaMemPool::BlockCountToBytes(const size_t & blockCount)
{
	return blockCount*OME_LUAPOOLSLOTSIZE;
}