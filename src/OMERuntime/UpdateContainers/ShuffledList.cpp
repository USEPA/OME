#include "UpdateContainers/ShuffledList.h"
#include "UpdateContainers/TempVCArray.h"
#include"UpdateContainers/TempPoolArray.h"
#include "MemPool.h"

///@cond DOX_NO_DOC
namespace ShuffledPool
{
	ObjMemPool<ShuffledList> s_pool = ObjMemPool<ShuffledList>(100);
};
///@endcond

/** Detailed constructor.
	@param src Pointer to the source list.
	@param inds The list of indices to reorder src by.
	@param oneBasedIndex If true indexing starts at 1; otherwise, it starts at 0.
*/
ShuffledList::ShuffledList(Listable* src, Listable& inds,const bool & oneBasedIndex)
	:Listable(), m_pSrc(src)
{

	size_t adjustment = oneBasedIndex ? 1 : 0;
	m_dims = src->GetDims();
	m_dims.Outermost() = inds.GetSize();

	size_t srcStep = src->GetStepSize();
	m_length = srcStep*inds.GetSize();

	for (size_t i = 0; i < inds.GetSize(); ++i)
	{
		m_indexTable.AppendPair((i*srcStep), (inds[i] - adjustment)*srcStep);
	}
	RecalcStep();
}

OME_SCALAR& ShuffledList::operator[](size_t pos)
{
	IndexJumpTable::IJTPair indPr = m_indexTable.Lookup(pos);
	return (*m_pSrc)[indPr.outInd];
}

const OME_SCALAR ShuffledList::operator[](size_t pos) const
{
	IndexJumpTable::IJTPair indPr = m_indexTable.Lookup(pos);
	return (*m_pSrc)[indPr.outInd];
}

bool ShuffledList::SetValue(const size_t & index, const OME_SCALAR & value)
{
	bool ret = index < m_length;
	if (ret)
	{
		IndexJumpTable::IJTPair indPr = m_indexTable.Lookup(index);
		m_pSrc->SetValue(indPr.outInd, value);
	}
	return ret;
}

void ShuffledList::SetValues(const OME_SCALAR & val)
{
	for (size_t i = 0; i < m_length; ++i)
		SetValue(i, val);
}

void ShuffledList::SetValues(const OME_SCALAR* vals, const size_t count)
{
	for (size_t i = 0; i < count; ++i)
		SetValue(i, vals[i]);
}

OME_SCALAR ShuffledList::GetValue(const size_t & i) const
{
	return operator[](i);
}

bool ShuffledList::GetValue(const size_t & i, OME_SCALAR & out)
{
	bool ret = i < m_length;
	if (ret)
		out = operator[](i);
	return ret;
}

OME_SCALAR ShuffledList::GetValue(const ListDims & iv) const
{
	return operator[](DeriveIndex(iv,m_dims));
}

bool ShuffledList::GetValue(const ListDims & iv, OME_SCALAR & out)
{
	size_t ind = DeriveIndex(iv, m_dims);
	bool ret = ind < m_length;
	if (ret)
		out = operator[](ind);
	return ret;
}


Evaluable* ShuffledList::GetRepEval()
{
	return m_pSrc->GetRepEval();
}

Listable& ShuffledList::Subset(const size_t & start, const size_t & length)
{
	return *TEMP_LIST_TYPE::NewTempLambda(length, [this, &start](const size_t & i){ return this->GetValue(start + i); }, m_pSrc->GetRepEval());
}

Listable& ShuffledList::SubsetFromStep(const size_t & ind)
{
	size_t offset = ind*m_pSrc->GetStepSize();
	return *TEMP_LIST_TYPE::NewTempLambda(m_pSrc->GetStepSize(), [this, &offset](const size_t & i){ return this->GetValue(offset + i); }, m_pSrc->GetRepEval());
}

Listable& ShuffledList::RefSubset(const size_t & start, const size_t & length)
{
	ShuffledList* ret = new(ShuffledPool::s_pool.NewCastPtr())ShuffledList();
	ret->m_pSrc = m_pSrc;
	ret->m_length = length;
	ret->m_dims.AppendDim(length);

	size_t lBound = start*m_pSrc->GetStepSize();
	size_t uBound = (start+length)*m_pSrc->GetStepSize();
	ret->m_indexTable.AppendIndexRange(m_indexTable,lBound,uBound);
	
	ret->RecalcStep();
	return *ret;
}

Listable& ShuffledList::RefSubsetFromStep(const size_t & ind)
{
	ShuffledList* ret = (ShuffledList*)&RefSubset(ind*m_pSrc->GetStepSize(), m_pSrc->GetStepSize());
	ret->m_dims=ListDims(m_dims, 1);
	ret->RecalcStep();
	return *ret;
}

void ShuffledList::SubCollect(Listable & out, const size_t & offset, const size_t & outStart)
{
	size_t stepSize = GetStepSize();
	for (size_t i = 0; i < stepSize; ++i)
	{
		out[i + outStart] = operator[]((i + offset)*stepSize);
	}
}

//Listable& SubCollect();
//Listable& SubCollect(WACollFunc collFunc, BWInitFunc initFunc);

ListableSclrItr ShuffledList::SclrBegin()
{
	return ListableSclrItr(this, 0, m_length);
}

OME_SCALAR& ShuffledList::ValForRef(void*& ref, const size_t & pos,const bool & posChanged)
{
	if (posChanged)
		ref = &operator[](pos);
	return *((OME_SCALAR*)ref);
}

//statics
/** Retrieve a newly allocated ShuffledList from a temporary pool.
	@param refList Reference to the source list.
	@param indList The list of indices to reorder src by.
	@param oneBased If true indexing starts at 1; otherwise, it starts at 0.
	@return Pointer to newly allocated ShuffledList.
*/
ShuffledList* ShuffledList::NewTempShuffledList(Listable & refList, Listable & indList,const bool & oneBased)
{
	return new(ShuffledPool::s_pool.NewCastPtr())ShuffledList(&refList, indList,oneBased);
}

/** Dealloc a temporary ShuffledList.
	@param pSl Pointer to the ShuffledList to release.
*/
void ShuffledList::ReleaseTempShuffledList(ShuffledList* pSl)
{
	ShuffledPool::s_pool.ReleasePtr(pSl);
}

/** Clear the temporary pool of ShuffledLists. */
void ShuffledList::ClearShuffledListPool()
{
	ShuffledPool::s_pool.ClearPool();
}

/** Refit temporary pool of ShuffledLists. */
void ShuffledList::RefitShuffledListPool()
{
	ShuffledPool::s_pool.RefitPool();
}
