#include "UpdateContainers/StaticUpArray.h"
#include "omeobjects/Evaluable.h"
#include "omeobjects/VarArray.h"
#include "omeobjects/Model.h"
#include "MemPool.h"
#include "UpdateContainers/TempVCArray.h"
#include "UpdateContainers/TempPoolArray.h"

namespace SUAPool
{
	static ObjMemPool<StaticUpArray> s_pool(100);
};

/** Detailed constructor.
	@param pEval Pointer to reference Evaluable object.
	@param lvl The degree of abstraction to apply to list representation.
*/
StaticUpArray::StaticUpArray(Evaluable* pEval, const size_t & lvl)
:Listable(),m_pRefEval(pEval), m_start(0),m_pArch(pEval->GetParentModel()->GetValueArchive())
 {
	PROFILE_FUNC();
	m_dims.AppendDims(lvl,0);
	Model* pMdl = pEval->GetParentModel();
	m_pRefModel = pMdl;
	m_start = pEval->GetModelIndex();
	//skip single-instance models
	unsigned short i;
	
	for (i = 0; i < lvl && pMdl && pMdl->GetActiveInstanceCount(); pMdl = pMdl->GetParentModel())
	{
		if (pMdl->GetActiveInstanceCount()>1 || pMdl->IsDynamic() || pMdl->IsConditional())
		{
			if (!m_length)
				m_length = 1;
			m_dims[i] = pMdl->GetActiveInstanceCount();
			m_length *= m_dims[i++];
		}
	}
	
	if (i<lvl && pMdl && !pMdl->GetActiveInstanceCount())
		m_length = 0;	//if the above loop ended due to a parent model existing with no instance, kill list
	RecalcStep();
	if (pMdl)
		m_start += m_length*pMdl->GetCurrentRawInstance();
}

/** Detailed constructor.
	@param pVa Pointer to reference VarArray object.
	@param lvl The degree of abstraction to apply to list representation.
*/
StaticUpArray::StaticUpArray(VarArray* pVa, const size_t & lvl)
	:Listable(), m_pRefEval(pVa), m_pArch(pVa->GetParentModel()->GetValueArchive())
{
	PROFILE_FUNC();

	m_dims.AppendDims(lvl, 0);
	m_length = 1;
	Model* pMdl = pVa->GetParentModel();
	m_pRefModel = pMdl;
	m_start = pVa->GetModelIndex()+ m_pRefModel->GetCurrentRawInstance()*pVa->GetSize();

	unsigned short i;
	for (i = 0; i < lvl && i < pVa->GetDimCount(); i++)
	{
		m_dims[i] = pVa->GetDim(i);
		m_length *= m_dims[i];
	}

	unsigned short currInd = i;
	for (; i < lvl && pMdl && pMdl->GetActiveInstanceCount(); pMdl = pMdl->GetParentModel())
	{
		//skip single-instance models
		if (pMdl->GetNumInstances() != 1 || pMdl->IsDynamic())
		{
			m_dims[currInd] = pMdl->GetNumInstances();
			m_length *= m_dims[currInd++];
			++i;
		}
	}

	if (i<lvl && pMdl && !pMdl->GetActiveInstanceCount())
		m_length = 0;	//if the above loop ended due to a parent model existing with no instance, kill list

	RecalcStep();
	if (pMdl)
		m_start += m_length*pMdl->GetCurrentRawInstance();
}


OME_SCALAR& StaticUpArray::operator[](size_t pos)
{
	PROFILE_FUNC();
	return (*m_pArch)[pos + m_start];
}

const OME_SCALAR StaticUpArray::operator[](size_t pos) const
{
	PROFILE_FUNC();
	return (*m_pArch)[pos + m_start];
}

bool StaticUpArray::SetValue(const size_t & index, const OME_SCALAR & value)
{
	PROFILE_FUNC();
	bool ret = index < m_length;
	if (ret)
		operator[](index) = value;
	return ret;
}

void StaticUpArray::SetValues(const OME_SCALAR & val)
{
	PROFILE_FUNC();
	for (size_t i = 0; i < m_length; ++i)
		operator[](i) = val;
}
void StaticUpArray::SetValues(const OME_SCALAR* vals, const size_t count)
{
	PROFILE_FUNC();
	for (size_t i = 0; i < count; ++i)
		operator[](i) = vals[i];
}

OME_SCALAR StaticUpArray::GetValue(const size_t & i) const
{
	PROFILE_FUNC();
	return operator[](i);
}

bool StaticUpArray::GetValue(const size_t & i, OME_SCALAR & out)
{
	PROFILE_FUNC();
	bool ret = i < m_length;
	if (ret)
		out = operator[](i);
	return ret;
}

OME_SCALAR StaticUpArray::GetValue(const ListDims & iv) const
{
	PROFILE_FUNC();
	return operator[](DeriveIndex(iv, m_dims));
}

bool StaticUpArray::GetValue(const ListDims & iv, OME_SCALAR & out)
{
	PROFILE_FUNC();
	size_t ind = DeriveIndex(iv, m_dims);
	bool ret = ind < m_length;
	if (ret)
		out = operator[](DeriveIndex(iv, m_dims));
	return ret;
}

Evaluable* StaticUpArray::GetRepEval()
{
	PROFILE_FUNC();
	return m_pRefEval;
}

Listable& StaticUpArray::Subset(const size_t & start, const size_t & length)
{
	PROFILE_FUNC();
	StaticUpArray *ret = new(SUAPool::s_pool.NewCastPtr())StaticUpArray();

	ret->m_length = length;
	ret->m_step = 1;
	ret->m_start = m_start+start;
	ret->m_pArch = m_pArch;
	ret->m_pRefEval = m_pRefEval;
	ret->m_pRefModel = m_pRefModel;
	return *ret;
}
 
Listable& StaticUpArray::SubsetFromStep(const size_t & ind)
{
	PROFILE_FUNC();
	StaticUpArray *ret = new(SUAPool::s_pool.NewCastPtr())StaticUpArray();
	
	ret->m_length=m_step;
	ret->m_start=m_start+(m_step*ind);
	
	ret->m_dims = ListDims(m_dims, 1);
	ret->m_step=m_step/ret->m_dims.Outermost();

	ret->m_pArch=m_pArch;
	ret->m_pRefEval=m_pRefEval;
	ret->m_pRefModel = m_pRefModel;
	return *ret;
}

Listable& StaticUpArray::RefSubset(const size_t & start, const size_t & length)
{
	PROFILE_FUNC();
	return Subset(start, length);
}

Listable& StaticUpArray::RefSubsetFromStep(const size_t & ind)
{
	PROFILE_FUNC();
	return SubsetFromStep(ind);
}

void StaticUpArray::SubCollect(Listable & out, const size_t & offset, const size_t & outStart)
{
	PROFILE_FUNC();
	//size_t length = m_step;
	size_t start = m_start + (m_step*offset);
	

	for (size_t i = 0; i < m_step;++i)
		out[i+outStart] = operator[](start+i);
}

/** Offset entry point in ValueArchive by a number of step intervals.
	@param numSteps The number of steps to offset by.
*/
void StaticUpArray::OffsetByStep(const size_t & numSteps)
{
	m_start += numSteps*GetStepSize();
}

/** Offset entry point in ValueArchive by a number of individual ValueArchive slots.
	@param offset The number of slots to offset by.
*/
void StaticUpArray::OffsetByAbsolute(const size_t & offset)
{
	m_start += offset;
}

ListableSclrItr StaticUpArray::SclrBegin()
{
	PROFILE_FUNC();
	return ListableSclrItr(this, 0, m_length);
}

OME_SCALAR& StaticUpArray::ValForRef(void*& ref, const size_t & pos, const bool & posChanged)
{
	PROFILE_FUNC();
	//if (posChanged)
	//{
	//	ref = &operator[](pos);
	//}
		return operator[](pos);//*((OME_SCALAR*)ref);
}


//static funcs
/** Reset the pool containing temporary StaticUpArrays.*/
void StaticUpArray::ClearTempPool()
{
	PROFILE_FUNC();
	SUAPool::s_pool.ClearPool();
}

/** Refit the pool containing temporary StaticUpArrays.*/
void StaticUpArray::RefitTempPool()
{
	PROFILE_FUNC();
	SUAPool::s_pool.RefitPool();
}

/** Allocate new Temporary StaticUpArray.
	@param pEval Pointer to reference Evaluable object.
	@param lvl The degree of abstraction to apply to list representation.
	@return Pointer to new temporary list.
*/
StaticUpArray* StaticUpArray::NewSUA(Evaluable* pEval, const size_t & lvl)
{
	PROFILE_FUNC();
	return new(SUAPool::s_pool.NewCastPtr())StaticUpArray(pEval, lvl);
}

/** Allocate new Temporary StaticUpArray.
	@param pVa Pointer to reference VarArray object.
	@param lvl The degree of abstraction to apply to list representation.
	@return Pointer to new temporary list.
*/
StaticUpArray* StaticUpArray::NewSUA(VarArray* pVa, const size_t & lvl)
{
	PROFILE_FUNC();
	return new(SUAPool::s_pool.NewCastPtr())StaticUpArray(pVa, lvl);
}

/** Allocate new Temporary StaticUpArray that acts as sublist of VarArray.
	@param pVa Pointer to reference VarArray object.
	@param start Index to start sublist.
	@param len The length of the generated sublist.
	@return Pointer to new temporary list.
*/
StaticUpArray* StaticUpArray::NewSUA(VarArray* pVa, const size_t & start, const size_t & len)
{
	PROFILE_FUNC();
	StaticUpArray* ret= new(SUAPool::s_pool.NewCastPtr())StaticUpArray(pVa, 1);
	ret->m_start = pVa->GetModelIndex() + start;
	ret->m_dims[0] = len;
	ret->m_length = len;
	return ret;
}

/** Release a specific temporary StaticUpArray.
	@param pSua Pointer to list to release.
*/
void StaticUpArray::ReleaseSUA(StaticUpArray* pSua)
{
	PROFILE_FUNC();
	SUAPool::s_pool.ReleasePtr(pSua);
}

/** Allocate new Temporary StaticUpArray from stack instead of pools.
	@param pEval Pointer to reference Evaluable object.
	@param lvl The degree of abstraction to apply to list representation.
	@return Pointer to new temporary list residing on the stack.
*/
StaticUpArray StaticUpArray::NewSUAFromStack(Evaluable* pEval, const size_t & lvl)
{
	StaticUpArray ret;
	if (!pEval->IsOMEType(OME_VARARRAY))
		new(&ret)StaticUpArray(pEval, lvl);
	else
		new(&ret)StaticUpArray((VarArray*)pEval, lvl);

	return ret;
}