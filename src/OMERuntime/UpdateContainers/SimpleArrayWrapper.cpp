#include "UpdateContainers/SimpleArrayWrapper.h"
#include "UpdateContainers/TempVCArray.h"
#include "UpdateContainers/TempPoolArray.h"
#include "MemPool.h"

///@cond DOX_NO_DOC
namespace SubPool
{
	ObjMemPool<SimpleArrayWrapper> s_pool = ObjMemPool<SimpleArrayWrapper>(100);
}
///@endcond

/** 1D array constructor.
	@param arr Pointer to source primitive array.
	@param len The length of the primitive array.
	@param repEval Pointer to reference Evaluable object, if any.
	@param isPersist If true, SimpleArray is treated as a persistant object.
*/
SimpleArrayWrapper::SimpleArrayWrapper(OME_SCALAR* arr, const size_t & len, Evaluable* repEval,const bool & isPersist)
:Listable(len,isPersist),m_pArr(arr), m_pEval(repEval)
{}

/** Multidimensional array constructor.
	@param arr Pointer to source primitive array.
	@param dims The Dimensions used to interpret the primitive array.
	@param repEval Pointer to reference Evaluable object, if any.
	@param isPersist If true, SimpleArray is treated as a persistant object.
*/
SimpleArrayWrapper::SimpleArrayWrapper(OME_SCALAR* arr, const ListDims & dims, Evaluable* repEval, const bool & isPersist)
:Listable(), m_pArr(arr), m_pEval(repEval)
{
	PROFILE_FUNC();
	unsigned short dSize = m_dims.GetSize();
	if (dSize)
	{
		m_length = 1;
		for (size_t i = 0; i < dSize; ++i)
			m_length *= m_dims[i];
		
		m_step = m_length / m_dims.Outermost();
	}
}

OME_SCALAR& SimpleArrayWrapper::operator[](size_t pos)
{
	return m_pArr[pos];
}

const OME_SCALAR SimpleArrayWrapper::operator[](size_t pos) const
{
	return m_pArr[pos];
}

bool SimpleArrayWrapper::SetValue(const size_t & index, const OME_SCALAR & value)
{
	PROFILE_FUNC();
	bool ret = index<m_length;
	if (ret)
		m_pArr[index] = value;
	return ret;
}
void SimpleArrayWrapper::SetValues(const OME_SCALAR & val)
{
	PROFILE_FUNC();
	for (size_t i = 0; i < m_length; ++i)
		m_pArr[i] = val;
}

void SimpleArrayWrapper::SetValues(const OME_SCALAR* vals, const size_t count)
{
	PROFILE_FUNC();
	for (size_t i = 0; i < m_length; ++i)
		m_pArr[i] = vals[i];
}

OME_SCALAR SimpleArrayWrapper::GetValue(const size_t & i) const
{
	return m_pArr[i];
}

bool SimpleArrayWrapper::GetValue(const size_t & i, OME_SCALAR & out)
{
	bool ret = i<m_length;
	if (ret)
		out=m_pArr[i];
	return ret;
}

OME_SCALAR SimpleArrayWrapper::GetValue(const ListDims & iv) const
{
	return m_pArr[DeriveIndex(iv, m_dims) ];
}

bool SimpleArrayWrapper::GetValue(const ListDims & iv, OME_SCALAR & out)
{
	size_t ind = DeriveIndex(iv, m_dims);
	bool ret = ind<m_length;
	if (ret)
		out = m_pArr[ind];
	return ret;
}


const size_t SimpleArrayWrapper::GetSize() const
{
	return m_length;
}

size_t SimpleArrayWrapper::GetStepSize() const
{
	return m_step;
}

size_t SimpleArrayWrapper::GetLevel() const
{
	return m_dims.GetSize();
}

ListDims& SimpleArrayWrapper::GetDims()
{
	return m_dims;
}

ListDims SimpleArrayWrapper::GetDims() const
{
	return m_dims;
}

unsigned short SimpleArrayWrapper::GetDimCount() const
{
	return m_dims.GetSize();
}

size_t SimpleArrayWrapper::GetDim(const size_t & ind) const
{
	PROFILE_FUNC();
	return m_dims[ind];
}

Evaluable* SimpleArrayWrapper::GetRepEval()
{
	PROFILE_FUNC();
	return m_pEval;
}

Listable& SimpleArrayWrapper::Subset(const size_t & start, const size_t & length)
{
	PROFILE_FUNC();
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(length);
	for (size_t i = 0; i < length; ++i)
		(*ret)[i] = m_pArr[i + start];

	return *ret;
}
Listable& SimpleArrayWrapper::SubsetFromStep(const size_t & ind)
{
	PROFILE_FUNC();
	return *TEMP_LIST_TYPE::NewTempSubstep(*this, ind);
}

Listable& SimpleArrayWrapper::RefSubset(const size_t & start, const size_t & length)
{
	PROFILE_FUNC();
	return *(new(SubPool::s_pool.NewCastPtr())SimpleArrayWrapper(&m_pArr[start], length,m_pEval,false));
}

Listable& SimpleArrayWrapper::RefSubsetFromStep(const size_t & ind)
{
	PROFILE_FUNC();
	SimpleArrayWrapper* ret=new(SubPool::s_pool.NewCastPtr())SimpleArrayWrapper(&m_pArr[ind], m_step, m_pEval,false);
	
	ret->m_dims = ListDims(m_dims, 1);
	ret->m_step = ret->m_length / ret->m_dims.Outermost();
	return *ret;
}

void SimpleArrayWrapper::SubCollect(Listable & out, const size_t & offset, const size_t & outStart)
{
	PROFILE_FUNC();
	for (size_t i = 0; i < m_step; ++i)
		out[i+outStart] = m_pArr[i + (m_step*offset)];
}

ListableSclrItr SimpleArrayWrapper::SclrBegin()
{
	PROFILE_FUNC();
	return ListableSclrItr(this, 0, m_length);
}

OME_SCALAR& SimpleArrayWrapper::ValForRef(void*& ref, const size_t & pos,const bool & posChanged)
{
	PROFILE_FUNC();
	return m_pArr[pos];
}

/** Clear SimpleArrayWrapper temporary pool*/
void SimpleArrayWrapper::ClearTmpPool()
{
	PROFILE_FUNC();
	SubPool::s_pool.ClearPool();
}

/** Refit SimpleArrayWrapper temporary pool*/
void SimpleArrayWrapper::RefitTmpPool()
{
	PROFILE_FUNC();
	SubPool::s_pool.RefitPool();
}