#pragma once

#include "Listable.h"
#include "IndexJumpTable.h"

/** List representing a reordering of another list. */
class __EXPORT__ ShuffledList : public Listable
{

public:
	ShuffledList(Listable* src, Listable& inds, const bool & oneBasedIndex=false);

	virtual OME_SCALAR& operator[](size_t pos);
	virtual const OME_SCALAR operator[](size_t pos) const;

	virtual bool SetValue(const size_t & index, const OME_SCALAR & value);
	virtual void SetValues(const OME_SCALAR & val);

	virtual void SetValues(const OME_SCALAR* vals, const size_t count);

	virtual OME_SCALAR GetValue(const size_t & i) const;
	virtual bool GetValue(const size_t & i, OME_SCALAR & out);
	virtual OME_SCALAR GetValue(const ListDims & iv) const;
	virtual bool GetValue(const ListDims & iv, OME_SCALAR & out);

	
	virtual Evaluable* GetRepEval();

	virtual Listable& Subset(const size_t & start, const size_t & length);
	virtual Listable& SubsetFromStep(const size_t & ind);
	virtual Listable& RefSubset(const size_t & start, const size_t & length);
	virtual Listable& RefSubsetFromStep(const size_t & ind);
	virtual void SubCollect(Listable & out, const size_t & offset, const size_t & outStart=0);

	//Listable& SubCollect();
	//Listable& SubCollect(WACollFunc collFunc, BWInitFunc initFunc);

	virtual ListableSclrItr SclrBegin();

	static ShuffledList* NewTempShuffledList(Listable & refList, Listable & indList,const bool & oneBased=false);
	static void ReleaseTempShuffledList(ShuffledList* pSl);
	static void ClearShuffledListPool();
	static void RefitShuffledListPool();

protected:
	ShuffledList() :Listable(),m_pSrc(NULL){};
	//for iterator access
	virtual OME_SCALAR& ValForRef(void*& ref, const size_t & pos,const bool & posChanged);

	IndexJumpTable m_indexTable;	///< Jump table mapping into source list.
	Listable* m_pSrc;				///< Pointer to the source list.
};