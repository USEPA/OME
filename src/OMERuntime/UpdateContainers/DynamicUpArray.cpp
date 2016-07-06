#include "UpdateContainers/DynamicUpArray.h"
#include "MemPool.h"
#include "omeobjects/VarArray.h"
#include "omeobjects/Model.h"
#include "UpdateContainers/TempVCArray.h"
#include "UpdateContainers/TempPoolArray.h"

namespace DUAPool
{
	ObjMemPool<DynamicUpArray> s_pool = ObjMemPool<DynamicUpArray>(100);
};

/** Detailed constructor.
	@param pEval Pointer to represented Evaluable object.
	@param lvl The level of outscoping of list.
*/
DynamicUpArray::DynamicUpArray(Evaluable* pEval, const size_t & lvl)
:StaticUpArray(pEval,lvl)
{
	//build lookup list of only active models
	ModelInstance* pCurrInst;
	bool inActives = false;
	for (size_t i = m_start, hits = 0; hits < m_length && i < m_pRefModel->GetNumRawInstances(); ++i)
	{
		pCurrInst = m_pRefModel->GetRawInstance(i);
		if (pCurrInst->IsActive())
		{
			if (!inActives)
			{
				inActives = true;
				m_offsetTable.AppendPair(hits,i);
			}
			++hits;
		}
		else if (inActives)
			inActives = false;
	}

}

/** Detailed constructor.
@param pEval Pointer to represented VarArray object.
@param lvl The level of outscoping of list.
*/
DynamicUpArray::DynamicUpArray(VarArray* pEval, const size_t & lvl)
	:StaticUpArray(pEval, lvl)
{
	//build lookup list of only active models
	ModelInstance* pCurrInst;
	bool inActives = false;
	for (size_t i = m_start, hits = 0; hits < m_length && i < m_pRefModel->GetNumRawInstances(); ++i)
	{
		pCurrInst = m_pRefModel->GetRawInstance(i);
		if (pCurrInst->IsActive())
		{
			if (!inActives)
			{
				inActives = true;
				m_offsetTable.AppendPair(hits, i);
			}
			++hits;
		}
		else if (inActives)
			inActives = false;
	}
	if (!m_pRefModel->GetNumRawInstances())
	{
		m_dims.ClearDims();
		m_length = 0;
	}
}


OME_SCALAR& DynamicUpArray::operator[](size_t pos)
{
	IndexJumpTable::IJTPair adjust = m_offsetTable.Lookup(pos);
	return (*m_pArch)[m_start + adjust.outInd + (pos - adjust.inInd)];
}

const OME_SCALAR DynamicUpArray::operator[](size_t pos) const
{
	IndexJumpTable::IJTPair adjust = m_offsetTable.Lookup(pos);
	return (*m_pArch)[m_start + adjust.outInd + (pos - adjust.inInd)];
}

Listable& DynamicUpArray::Subset(const size_t & start, const size_t & length)
{
	DynamicUpArray* ret = new(DUAPool::s_pool.NewCastPtr())DynamicUpArray();
	ret->m_length = length;
	ret->m_dims.ClearDims();
	ret->m_dims.AppendDim(length);
	ret->m_step = 1;
	ret->m_offsetTable.AppendIndexRange(m_offsetTable, start, start + length);
	return *ret;
}

Listable& DynamicUpArray::SubsetFromStep(const size_t & ind)
{
	DynamicUpArray* ret= (DynamicUpArray*)&Subset(m_step*ind,m_step);
	ret->m_dims = ListDims(m_dims, 1);
	ret->RecalcStep();
	return *ret;

}

ListableSclrItr DynamicUpArray::SclrBegin()
{
	return ListableSclrItr(this, 0, m_length);
}

OME_SCALAR& DynamicUpArray::ValForRef(void*& ref, const size_t & pos,const bool & posChanged)
{

	if (posChanged)
	{
		IndexJumpTable::IJTPair adjust = m_offsetTable.Lookup(pos);
		if (pos < m_length)
			ref = &((*m_pArch)[m_start + adjust.outInd + (pos - adjust.inInd)]);
	}
	return *((OME_SCALAR*)ref);
}

//static funcs
/** Reset Temporary DynamicUpArray Pool.*/
void DynamicUpArray::ClearTempPool()
{
	DUAPool::s_pool.ClearPool();
}

/** Resize the DynamicUpArray Pool.*/
void DynamicUpArray::RefitTempPool()
{
	DUAPool::s_pool.RefitPool();
}

/** Allocate a new DynamicUpArray.
	@param pEval Pointer to Represented Evaluable object.
	@param lvl The degree of outscoping.
	@return Pointer to the newly allocated DynamicUpArray.
*/
DynamicUpArray* DynamicUpArray::NewDUA(Evaluable* pEval, const size_t & lvl)
{
	return new(DUAPool::s_pool.NewCastPtr())DynamicUpArray(pEval, lvl);
}

/** Release a DynamicUpArray from use.
	@param pDua Pointer to DynamicUpArray to release.
*/
void DynamicUpArray::ReleaseDUA(DynamicUpArray* pDua)
{
	DUAPool::s_pool.ReleasePtr(pDua);
}

