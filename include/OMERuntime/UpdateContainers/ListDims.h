#pragma once
#include "OMEDefines.h"
#include "MemPool.h"

#define DN_LEN	8

/** A representation of a list-type container's dimensions.*/
class __EXPORT__ ListDims
{
public:
	ListDims(const size_t & length,const bool & isPersist=false);
	ListDims(const bool & isPersist = false);
	ListDims(const ListDims & dims, const unsigned short & subtract = 0, const bool & isPersist = false);
	~ListDims();

	void AppendDim(const size_t & val);
	void AppendDims(const size_t & count, const size_t & fill);
	void AppendDims(const ListDims & dims);
	void Shrink(const unsigned short & count);
	void AppendDimFront(const size_t & val);
	void ClearDims();
	size_t CalcStep() const;
	size_t CalcRepLength() const;

	size_t& operator[](unsigned short i);
	size_t operator[](unsigned short i) const;

	ListDims& operator=(const ListDims & rhs);
	ListDims& operator=(const IndVec & inds);

	bool operator==(const ListDims & rhs);
	operator IndVec() const;

	ListDims Clone() const;

	void Flip();
	size_t& Outermost();
	size_t Outermost() const;

	size_t& Innermost();
	size_t Innermost() const;

	unsigned short GetSize() const;

	/// @cond DOX_NO_DOC
	struct DimNode
	{
		DimNode* m_next;
		size_t m_vals[DN_LEN];

		DimNode() :m_next(NULL){}
	};
	///@endcond

	static void RefitTempNodePool();
	static void ClearTempNodePool();
private:
	void KillNodes(DimNode* node);

	unsigned short m_length;			///< The total number of dimensions
	DimNode* m_pSrc;					///< The head of the block linked list.
	DimNode* m_pTail;					///< The tail of the block linked list.

	ObjMemPool<DimNode>* m_pSrcPool;	///< Pointer to the memory pool containing the ListDims' nodes.
};

//explicit Pool types
/** ListDims which allocates its dimensions for a pool that stays allocated for the entire simulation run. */
class __EXPORT__ PersistListDims :public ListDims
{
public:
	/** Simple Constructor.
		@param length The number of dimensions to associate with PersistListDims.
	*/
	PersistListDims(const size_t & length)
		:ListDims(length, true){}
	PersistListDims()
		:ListDims(true){}

	/** Copy Constructor.
		@param dims The ListDims to copy.
		@param subtract Optional amount to subtract from dims during copy.
	*/
	PersistListDims(const ListDims & dims, const unsigned short & subtract = 0)
		:ListDims(dims,subtract,true) {}
	
};

/** ListDims which allocates its dimensions for a pool that maintains temporary allocation. */
class __EXPORT__ TempListDims :public ListDims
{
public:
	/** Simple Constructor.
		@param length The number of dimensions to associate with TempListDims.
	*/
	TempListDims(const size_t & length)
		:ListDims(length, false){}
	TempListDims()
		:ListDims(false){}
	/** Copy Constructor.
	@param dims The ListDims to copy.
	@param subtract Optional amount to subtract from dims during copy.
	*/
	TempListDims(const ListDims & dims, const unsigned short & subtract = 0)
		:ListDims(dims, subtract, false) {}
	
};

