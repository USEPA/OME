#pragma once

#include "StaticUpArray.h"
#include "IndexJumpTable.h"

/** UpArray mapping for use with dynamic model types.*/
class DynamicUpArray : public StaticUpArray
{
public:
	DynamicUpArray(Evaluable* pEval, const size_t & lvl);
	DynamicUpArray(VarArray* pEval, const size_t & lvl);
	virtual ~DynamicUpArray() {}
	
	virtual OME_SCALAR& operator[](size_t pos);
	virtual const OME_SCALAR operator[](size_t pos) const;

	virtual Listable& Subset(const size_t & start, const size_t & length);
	virtual Listable& SubsetFromStep(const size_t & ind);

	virtual ListableSclrItr SclrBegin();

	static void ClearTempPool();
	static void RefitTempPool();
	static DynamicUpArray* NewDUA(Evaluable* pEval, const size_t & lvl);
	static void ReleaseDUA(DynamicUpArray* pDua);

	
protected:

	//for iterator access
	virtual OME_SCALAR& ValForRef(void*& ref, const size_t & pos,const bool & posChanged);

private:

	IndexJumpTable m_offsetTable;			///< Table used for for mapping list indices to active indices.
	DynamicUpArray() :StaticUpArray() {};

};