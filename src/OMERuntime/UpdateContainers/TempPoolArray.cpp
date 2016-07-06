#include "UpdateContainers/TempPoolArray.h"
#include "MemPool.h"
#include "OMEException.h"
namespace TPAPool
{
	ObjMemPool<TempPoolArray::TPANode> s_nodePool = ObjMemPool<TempPoolArray::TPANode>(5120);
	ObjMemPool<TempPoolArray> s_tpaPool = ObjMemPool<TempPoolArray>(1000);
};

#define MOD2(x) (x) & (TPA_NODE_LEN-1)
//#define MOD2(x) (x) % TPA_NODE_LEN

/** Detailed Constructor.
	@param dims The dimensions of the array.
	@param lvl The level of abstraction to associate with the array.
	@param fillVal Default value to initialize array slots with.
	@param pEval Pointer to associated Evaluable.
*/
TempPoolArray::TempPoolArray(const ListDims & dims, const size_t & lvl, const OME_SCALAR & fillVal, Evaluable* pEval)
	:Listable(dims), m_pRepEval(pEval), m_isRef(false), m_start(0)
{
	m_dims.Shrink(m_dims.GetSize() - lvl);
	m_length = m_dims.CalcRepLength();
	m_pFront = new(TPAPool::s_nodePool.NewCastPtr())TPANode(0);
	TPANode* pCurr = m_pFront;
	size_t nodeInd;
	for (size_t i = 0,id=0; i < m_length; ++i)
	{
		nodeInd = MOD2(i);
		if (i && !nodeInd)
		{
			pCurr->m_next = new(TPAPool::s_nodePool.NewCastPtr())TPANode(++id);
			pCurr = pCurr->m_next;
		}

		pCurr->m_vals[nodeInd] = fillVal;
	}
	m_pBack = pCurr;
}

/** Detailed Constructor.
@param length The length of the array.
@param fillVal Default value to initialize array slots with.
@param pEval Pointer to associated Evaluable.
*/
TempPoolArray::TempPoolArray(const size_t & length, const OME_SCALAR & fillVal, Evaluable* pEval)
	:Listable(length), m_pRepEval(pEval), m_isRef(false), m_start(0)
{
	size_t nodeInd;
	m_pFront = new(TPAPool::s_nodePool.NewCastPtr())TPANode(0);
	TPANode* pCurr = m_pFront;
	for (size_t i = 0,id=0; i < m_length; ++i)
	{
		nodeInd = MOD2(i);
		if (i && !nodeInd)
		{
			pCurr->m_next = new(TPAPool::s_nodePool.NewCastPtr())TPANode(++id);
			pCurr = pCurr->m_next;
		}

		pCurr->m_vals[nodeInd] = fillVal;
	}
	m_pBack = pCurr;
}

/** Copy Constructor.
	@param tpa The TempPoolArray to copy.
	@param copyVals If true, copy stored values; otehrwise just create an array with a length equivalent to tpa.
*/
TempPoolArray::TempPoolArray(const TempPoolArray & tpa, const bool & copyVals)
	: Listable(tpa.m_dims), m_pRepEval(NULL), m_isRef(false), m_start(0)
{
	m_pFront = new(TPAPool::s_nodePool.NewCastPtr())TPANode(0);

	TPANode* pLhs = m_pFront;
	TPANode* pRhs = tpa.m_pFront;

	size_t id = 0;
	while (pRhs)
	{
		if (copyVals)
			memcpy(pLhs->m_vals, pRhs->m_vals, sizeof(OME_SCALAR)*TPA_NODE_LEN);
		if (pRhs->m_next)
		{
			pLhs->m_next = new(TPAPool::s_nodePool.NewCastPtr())TPANode(++id);
			pLhs = pLhs->m_next;
		}
		pRhs = pRhs->m_next;
	}

	m_pBack = pLhs;
}

TempPoolArray::~TempPoolArray()
{
	if (!m_isRef)
		CleanupNodes(m_pFront);
}

OME_SCALAR& TempPoolArray::operator[](size_t pos)
{
	if (pos > m_length)
		throw OMEOOBException("TempPoolArray::operator[]: index is out of bounds");

	pos += m_start;

	TPANode* pCurr = m_pFront;

	///if (m_length - pos >= TPA_NODE_LEN)
	//{
	size_t lim = pos / TPA_NODE_LEN;
	for (size_t i = 0; i < lim; pCurr = pCurr->m_next, ++i);
	//}
	//else
	//	pCurr = m_pBack; //special case

	return pCurr->m_vals[pos-TPA_NODE_LEN*lim];

}

const OME_SCALAR TempPoolArray::operator[](size_t pos) const
{
	if (pos > m_length)
		throw OMEOOBException("TempPoolArray::operator[]: index is out of bounds");

	pos += m_start;

	TPANode* pCurr = m_pFront;

	///if (m_length - pos >= TPA_NODE_LEN)
	//{
		size_t lim = pos / TPA_NODE_LEN;
		for (size_t i = 0; i < lim; pCurr = pCurr->m_next,++i);
	//}
	//else
	//	pCurr = m_pBack; //special case

		return pCurr->m_vals[pos - TPA_NODE_LEN*lim];
}

bool TempPoolArray::SetValue(const size_t & index, const OME_SCALAR & value)
{
	bool ret = index + m_start< m_length;
	if (ret)
		operator[](index) = value;
	return ret;
}

void TempPoolArray::SetValues(const OME_SCALAR & val)
{
	if (m_isRef)
		throw OMEFuncOpException("TempPoolArray::SetValues","Not supported by reference type");

	TPANode* pCurr = m_pFront;
	//create fill template, then use memcpy to move values in one chunk
	OME_SCALAR vals[TPA_NODE_LEN];
	size_t i = 0;
	for (i = 0; i < TPA_NODE_LEN; ++i)
		vals[i] = val;

	for (; pCurr; pCurr = pCurr->m_next)
		memcpy(pCurr->m_vals, vals, sizeof(OME_SCALAR)*TPA_NODE_LEN);
}

void TempPoolArray::SetValues(const OME_SCALAR* vals, const size_t count)
{
	if (m_isRef)
		throw OMEFuncOpException("TempPoolArray::SetValues","Not supported by reference type");

	size_t nodeInd;
	TPANode* pCurr = m_pFront;
	for (size_t i = 0; i < count; ++i)
	{
        nodeInd = MOD2(i);
		if (i && !nodeInd)
			pCurr = pCurr->m_next;
		pCurr->m_vals[nodeInd] = vals[i];
	}
}

OME_SCALAR TempPoolArray::GetValue(const size_t & i) const
{
	return operator[](i);
}

bool TempPoolArray::GetValue(const size_t & i, OME_SCALAR & out)
{
	bool ret = m_start+i < m_length;
	if (ret)
		out = operator[](i);
	return ret;
}

OME_SCALAR TempPoolArray::GetValue(const ListDims & iv) const
{
	return operator[](DeriveIndex(iv, m_dims));
}

bool TempPoolArray::GetValue(const ListDims & iv, OME_SCALAR & out)
{
	return GetValue(DeriveIndex(iv, m_dims),out);
}

Evaluable* TempPoolArray::GetRepEval()
{
	return m_pRepEval;
}


Listable& TempPoolArray::operator+(const OME_SCALAR & rhs)
{
	using namespace TPAPool;
	TempPoolArray* ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

	TPANode* pCurr = ret->m_pFront;
	TPANode* pLhs = m_pFront;
	size_t ind;
	for (size_t i = m_start; i < m_length+m_start; ++i)
	{
		ind = MOD2(i);
		if (i && !ind)
		{
			pCurr = pCurr->m_next;
			pLhs = pLhs->m_next;
		}
		pCurr->m_vals[ind] = pLhs->m_vals[ind] + rhs;
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator+(Listable & rhs)
{
	//assume outermost dims are compatible
	using namespace TPAPool;
	TempPoolArray* ret;

	if (GetLevel() == rhs.GetLevel())
	{
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);
		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		size_t ind;
		for (size_t i = m_start; i < m_length + m_start; ++i)
		{
			ind = MOD2(i);
			if (i && !ind)
			{
				pCurr = pCurr->m_next;
				pLhs = pLhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[ind] + rhs[i];
		}
	}
	else
	{
		Listable* pLrg, *pSml;
		if (GetLevel() > rhs.GetLevel())
		{
			pLrg = this;
			pSml = &rhs;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
		}
		size_t inStep = pLrg->GetSize() / pSml->GetSize();
		
		ret = TempPoolArray::NewTempNoFill(pLrg->GetSize());
		ret->GetDims() = pLrg->GetDims();
		TPANode* pCurr = ret->m_pFront;

		for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
		{
			for (size_t flip = 0; flip < inStep; ++ind, ++flip)
			{
				(*ret)[ind] = (*pLrg)[ind] + (*pSml)[s];
				if (ind >= TPA_NODE_LEN)
				{
					ind = 0;
					pCurr = pCurr->m_next;
				}
			}
		}

	}
	ret->RecalcStep();
	return *ret;

}

/** Add two TempPoolArrays together.
@param rhs The TempPoolArray to add.
@return Reference to new TempPoolArray with results of per-value addition.
*/
Listable& TempPoolArray::operator+(TempPoolArray & rhs)
{
	TempPoolArray* ret;
	if (GetLevel() == rhs.GetLevel())
	{
		using namespace TPAPool;
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		TPANode* pRhs = rhs.m_pFront;
		size_t ind,lInd,rInd;
		for (size_t i = 0; i < m_length; ++i)
		{
			ind = MOD2(i);
			lInd = MOD2(i + m_start);
			rInd = MOD2(i + rhs.m_start);

			if (i)
			{
				if (!ind)
					pCurr = pCurr->m_next;
				if (!lInd)
					pLhs = pLhs->m_next;
				if (!rInd)
					pRhs = pRhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[lInd] + pRhs->m_vals[rInd];
		}

	}
	else
		ret = (TempPoolArray*)&operator+((Listable&)rhs);
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator-(const OME_SCALAR & rhs)
{
	using namespace TPAPool;
	TempPoolArray* ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

	TPANode* pCurr = ret->m_pFront;
	TPANode* pLhs = m_pFront;
	size_t ind;
	for (size_t i = m_start; i < m_length + m_start; ++i)
	{
		ind = MOD2(i);
		if (i && !ind)
		{
			pCurr = pCurr->m_next;
			pLhs = pLhs->m_next;
		}
		pCurr->m_vals[ind] = pLhs->m_vals[ind] - rhs;
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator-(Listable & rhs)
{
	//assume outermost dims are compatible
	using namespace TPAPool;
	TempPoolArray* ret;

	if (GetLevel() == rhs.GetLevel())
	{
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);
		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		size_t ind;
		for (size_t i = m_start; i < m_length + m_start; ++i)
		{
			ind = MOD2(i);
			if (i && !ind)
			{
				pCurr = pCurr->m_next;
				pLhs = pLhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[ind] - rhs[i];
		}
	}
	else
	{
		Listable* pLrg, *pSml;
		OME_SCALAR resSgn;
		if (GetLevel() > rhs.GetLevel())
		{
			pLrg = this;
			pSml = &rhs;
			resSgn = 1.0;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
			resSgn = -1.0;
		}
		size_t inStep = pLrg->GetSize() / pSml->GetSize();
		
		ret = TempPoolArray::NewTempNoFill(pLrg->GetSize());
		ret->GetDims() = pLrg->GetDims();
		TPANode* pCurr = ret->m_pFront;
		
		for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
		{
			for (size_t flip = 0; flip < inStep; ++ind, ++flip)
			{
				(*ret)[ind] = ((*pLrg)[ind] - (*pSml)[s])*resSgn;
				if (ind >= TPA_NODE_LEN)
				{
					ind = 0;
					pCurr = pCurr->m_next;
				}
			}
		}
	}
	ret->RecalcStep();
	return *ret;

}

/** Subtract one TempPoolArray from another.
@param rhs The TempPoolArray to subtract with.
@return Reference to new TempPoolArray with results of per-value subtraction.
*/
Listable& TempPoolArray::operator-(TempPoolArray & rhs)
{
	TempPoolArray* ret;
	if (GetLevel() == rhs.GetLevel())
	{
		using namespace TPAPool;
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		TPANode* pRhs = rhs.m_pFront;
		size_t ind, lInd, rInd;
		for (size_t i = 0; i < m_length; ++i)
		{
			ind = MOD2(i);
			lInd = MOD2(i + m_start);
			rInd = MOD2(i + rhs.m_start);

			if (i)
			{
				if (!ind)
					pCurr = pCurr->m_next;
				if (!lInd)
					pLhs = pLhs->m_next;
				if (!rInd)
					pRhs = pRhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[lInd] - pRhs->m_vals[rInd];
		}

	}
	else
		ret = (TempPoolArray*)&operator-((Listable&)rhs);
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator*(const OME_SCALAR & rhs)
{
	using namespace TPAPool;
	TempPoolArray* ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

	TPANode* pCurr = ret->m_pFront;
	TPANode* pLhs = m_pFront;
	size_t ind;
	for (size_t i = m_start; i < m_length + m_start; ++i)
	{
		ind = MOD2(i);
		if (i && !ind)
		{
			pCurr = pCurr->m_next;
			pLhs = pLhs->m_next;
		}
		pCurr->m_vals[ind] = pLhs->m_vals[ind] * rhs;
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator*(Listable & rhs)
{
	using namespace TPAPool;
	TempPoolArray* ret;

	if (GetLevel() == rhs.GetLevel())
	{
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);
		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		size_t ind;
		for (size_t i = m_start; i < m_length + m_start; ++i)
		{
			ind = MOD2(i);
			if (i && !ind)
			{
				pCurr = pCurr->m_next;
				pLhs = pLhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[ind] * rhs[i];
		}
	}
	else
	{
		Listable* pLrg, *pSml;
		if (GetLevel() > rhs.GetLevel())
		{
			pLrg = this;
			pSml = &rhs;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
		}
		size_t inStep = pLrg->GetSize() / pSml->GetSize();
		
		ret = TempPoolArray::NewTempNoFill(pLrg->GetSize());
		ret->m_dims = pLrg->GetDims();
		TPANode* pCurr = ret->m_pFront;

		for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
		{
			for (size_t flip = 0; flip < inStep; ++ind, ++flip)
			{
				(*ret)[ind] = (*pLrg)[ind] * (*pSml)[s];
				if (ind >= TPA_NODE_LEN)
				{
					ind = 0;
					pCurr = pCurr->m_next;
				}
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

/** Multiply two TempPoolArrays together.
@param rhs The TempPoolArray to multiply with.
@return Reference to new TempPoolArray with results of per-value multiplication.
*/
Listable& TempPoolArray::operator*(TempPoolArray & rhs)
{
	TempPoolArray* ret;
	if (GetLevel() == rhs.GetLevel())
	{
		using namespace TPAPool;
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		TPANode* pRhs = rhs.m_pFront;
		size_t ind, lInd, rInd;
		for (size_t i = 0; i < m_length; ++i)
		{
			ind = MOD2(i);
			lInd = MOD2(i + m_start);
			rInd = MOD2(i + rhs.m_start);

			if (i)
			{
				if (!ind)
					pCurr = pCurr->m_next;
				if (!lInd)
					pLhs = pLhs->m_next;
				if (!rInd)
					pRhs = pRhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[lInd] * pRhs->m_vals[rInd];
		}
	}
	else
		ret = (TempPoolArray*)&operator*((Listable&)rhs);
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator/(const OME_SCALAR & rhs)
{
	using namespace TPAPool;
	TempPoolArray* ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

	TPANode* pCurr = ret->m_pFront;
	TPANode* pLhs = m_pFront;
	size_t ind;
	for (size_t i = m_start; i < m_length + m_start; ++i)
	{
		ind = MOD2(i);
		if (i && !ind)
		{
			pCurr = pCurr->m_next;
			pLhs = pLhs->m_next;
		}
		pCurr->m_vals[ind] = pLhs->m_vals[ind] / rhs;
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator/(Listable & rhs)
{
	using namespace TPAPool;
	TempPoolArray* ret;

	if (GetLevel() == rhs.GetLevel())
	{
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);
		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		size_t ind;
		for (size_t i = m_start; i < m_length + m_start; ++i)
		{
			ind = MOD2(i);
			if (i && !ind)
			{
				pCurr = pCurr->m_next;
				pLhs = pLhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[ind] / rhs[i];
		}
	}
	else
	{
		Listable* pLrg, *pSml;
		bool noRecip = GetLevel() > rhs.GetLevel();
		if (noRecip)
		{
			pLrg = this;
			pSml = &rhs;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
		}
		size_t inStep = pLrg->GetSize() / pSml->GetSize();
		
		ret = TempPoolArray::NewTempNoFill(pLrg->GetSize());
		ret->GetDims() = pLrg->GetDims();
		TPANode* pCurr = ret->m_pFront;

		if (noRecip)
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
				{
					(*ret)[ind] = (*pLrg)[ind] / (*pSml)[s];
					if (ind >= TPA_NODE_LEN)
					{
						ind = 0;
						pCurr = pCurr->m_next;
					}
				}
			}
		}
		else
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
				{
					(*ret)[ind] = 1.0/((*pLrg)[ind] + (*pSml)[s]);
					if (ind >= TPA_NODE_LEN)
					{
						ind = 0;
						pCurr = pCurr->m_next;
					}
				}
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

/** Divide one TempPoolArray by another.
@param rhs The TempPoolArray to divide by.
@return Reference to new TempPoolArray with results of per-value division.
*/
Listable& TempPoolArray::operator/(TempPoolArray & rhs)
{
	TempPoolArray* ret;
	if (GetLevel() == rhs.GetLevel())
	{
		using namespace TPAPool;
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		TPANode* pRhs = rhs.m_pFront;
		size_t ind, lInd, rInd;
		for (size_t i = 0; i < m_length; ++i)
		{
			ind = MOD2(i);
			lInd = MOD2(i + m_start);
			rInd = MOD2(i + rhs.m_start);

			if (i)
			{
				if (!ind)
					pCurr = pCurr->m_next;
				if (!lInd)
					pLhs = pLhs->m_next;
				if (!rInd)
					pRhs = pRhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[lInd] / pRhs->m_vals[rInd];
		}
	}
	else
		ret = (TempPoolArray*)&operator/((Listable&)rhs);
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator==(const OME_SCALAR & rhs)
{
	using namespace TPAPool;
	TempPoolArray* ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

	TPANode* pCurr = ret->m_pFront;
	TPANode* pLhs = m_pFront;
	size_t ind;
	for (size_t i = m_start; i < m_length + m_start; ++i)
	{
		ind = MOD2(i);
		if (i && !ind)
		{
			pCurr = pCurr->m_next;
			pLhs = pLhs->m_next;
		}
		pCurr->m_vals[ind] = pLhs->m_vals[ind] == rhs;
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator==(Listable & rhs)
{
	using namespace TPAPool;
	TempPoolArray* ret;

	if (GetLevel() == rhs.GetLevel())
	{
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);
		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		size_t ind;
		for (size_t i = m_start; i < m_length + m_start; ++i) 
		{
			ind = MOD2(i);
			if (i && !ind)
			{
				pCurr = pCurr->m_next;
				pLhs = pLhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[ind] == rhs[i];
		}
	}
	else
	{
		Listable* pLrg, *pSml;
		if (GetLevel() > rhs.GetLevel())
		{
			pLrg = this;
			pSml = &rhs;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
		}
		size_t inStep = pLrg->GetSize() / pSml->GetSize();
		
		ret = TempPoolArray::NewTempNoFill(pLrg->GetSize());
		ret->GetDims() = pLrg->GetDims();
		TPANode* pCurr = ret->m_pFront;

		for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
		{
			for (size_t flip = 0; flip < inStep; ++ind, ++flip)
			{
				(*ret)[ind] = (*pLrg)[ind] == (*pSml)[s];
				if (ind >= TPA_NODE_LEN)
				{
					ind = 0;
					pCurr = pCurr->m_next;
				}
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

/** Compare two TempPoolArrays together, testing for equality.
@param rhs The TempPoolArray to test against.
@return Reference to new TempPoolArray with results of each equality comparison.
*/
Listable& TempPoolArray::operator==(TempPoolArray & rhs)
{
	TempPoolArray* ret;
	if (GetLevel() == rhs.GetLevel())
	{
		using namespace TPAPool;
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		TPANode* pRhs = rhs.m_pFront;
		size_t ind, lInd, rInd;
		for (size_t i = 0; i < m_length; ++i)
		{
			ind = MOD2(i);
			lInd = MOD2(i + m_start);
			rInd = MOD2(i + rhs.m_start);

			if (i)
			{
				if (!ind)
					pCurr = pCurr->m_next;
				if (!lInd)
					pLhs = pLhs->m_next;
				if (!rInd)
					pRhs = pRhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[lInd] == pRhs->m_vals[rInd];
		}
	}
	else
		ret = (TempPoolArray*)&operator==((Listable&)rhs);
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator!=(const OME_SCALAR & rhs)
{
	using namespace TPAPool;
	TempPoolArray* ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

	TPANode* pCurr = ret->m_pFront;
	TPANode* pLhs = m_pFront;
	size_t ind;
	for (size_t i = m_start; i < m_length + m_start; ++i)
	{
		ind = MOD2(i);
		if (i && !ind)
		{
			pCurr = pCurr->m_next;
			pLhs = pLhs->m_next;
		}
		pCurr->m_vals[ind] = pLhs->m_vals[ind] != rhs;
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator!=(Listable & rhs)
{
	using namespace TPAPool;
	TempPoolArray* ret;

	if (GetLevel() == rhs.GetLevel())
	{
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);
		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		size_t ind;
		for (size_t i = m_start; i < m_length + m_start; ++i)
		{
			ind = MOD2(i);
			if (i && !ind)
			{
				pCurr = pCurr->m_next;
				pLhs = pLhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[ind] != rhs[i];
		}
	}
	else
	{
		Listable* pLrg, *pSml;
		if (GetLevel() > rhs.GetLevel())
		{
			pLrg = this;
			pSml = &rhs;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
		}
		size_t inStep = pLrg->GetSize() / pSml->GetSize();

		ret = TempPoolArray::NewTempNoFill(pLrg->GetSize());
		ret->GetDims() = pLrg->GetDims();
		TPANode* pCurr = ret->m_pFront;

		for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
		{
			for (size_t flip = 0; flip < inStep; ++ind, ++flip)
			{
				(*ret)[ind] = (*pLrg)[ind] != (*pSml)[s];
				if (ind >= TPA_NODE_LEN)
				{
					ind = 0;
					pCurr = pCurr->m_next;
				}
			}
		}

	}
	ret->RecalcStep();
	return *ret;
}

/** Compare two TempPoolArrays together, testing for inequality.
@param rhs The TempPoolArray to test against.
@return Reference to new TempPoolArray with results of each inequality comparison.
*/
Listable& TempPoolArray::operator!=(TempPoolArray & rhs)
{
	TempPoolArray* ret;
	if (GetLevel() == rhs.GetLevel())
	{
		using namespace TPAPool;
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		TPANode* pRhs = rhs.m_pFront;

		size_t ind, lInd, rInd;
		for (size_t i = 0; i < m_length; ++i)
		{
			ind = MOD2(i);
			lInd = MOD2(i + m_start);
			rInd = MOD2(i + rhs.m_start);

			if (i)
			{
				if (!ind)
					pCurr = pCurr->m_next;
				if (!lInd)
					pLhs = pLhs->m_next;
				if (!rInd)
					pRhs = pRhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[lInd] != pRhs->m_vals[rInd];
		}
	}
	else
		ret = (TempPoolArray*)&operator!=((Listable&)rhs);
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator<=(const OME_SCALAR & rhs)
{
	using namespace TPAPool;
	TempPoolArray* ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

	TPANode* pCurr = ret->m_pFront;
	TPANode* pLhs = m_pFront;
	size_t ind;
	for (size_t i = m_start; i < m_length + m_start; ++i)
	{
		ind = MOD2(i);
		if (i && !ind)
		{
			pCurr = pCurr->m_next;
			pLhs = pLhs->m_next;
		}
		pCurr->m_vals[ind] = pLhs->m_vals[ind] <= rhs;
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator<=(Listable & rhs)
{
	using namespace TPAPool;
	TempPoolArray* ret;

	if (GetLevel() == rhs.GetLevel())
	{
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);
		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		size_t ind;
		for (size_t i = m_start; i < m_length + m_start; ++i)
		{
			ind = MOD2(i);
			if (i && !ind)
			{
				pCurr = pCurr->m_next;
				pLhs = pLhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[ind] <= rhs[i];
		}
	}
	else
	{
		Listable* pLrg, *pSml;
		bool noInvert = GetLevel() > rhs.GetLevel();
		if (noInvert)
		{
			pLrg = this;
			pSml = &rhs;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
		}
		size_t inStep = pLrg->GetSize() / pSml->GetSize();

		ret = TempPoolArray::NewTempNoFill(pLrg->GetSize());
		ret->GetDims() = pLrg->GetDims();
		TPANode* pCurr = ret->m_pFront;

		if (noInvert)
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
				{
					(*ret)[ind] = (*pLrg)[ind] <= (*pSml)[s];
					if (ind >= TPA_NODE_LEN)
					{
						ind = 0;
						pCurr = pCurr->m_next;
					}
				}
			}
		}
		else
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
				{
					(*ret)[ind] = !((*pLrg)[ind] > (*pSml)[s]);
					if (ind >= TPA_NODE_LEN)
					{
						ind = 0;
						pCurr = pCurr->m_next;
					}
				}
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

/** Compare two TempPoolArrays together, testing for less than or equal.
@param rhs The TempPoolArray to test against.
@return Reference to new TempPoolArray with results of each less than or equal comparison.
*/
Listable& TempPoolArray::operator<=(TempPoolArray & rhs)
{
	TempPoolArray* ret;
	if (GetLevel() == rhs.GetLevel())
	{
		using namespace TPAPool;
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		TPANode* pRhs = rhs.m_pFront;

		size_t ind, lInd, rInd;
		for (size_t i = 0; i < m_length; ++i)
		{
			ind = MOD2(i);
			lInd = MOD2(i + m_start);
			rInd = (i + rhs.m_start);

			if (i)
			{
				if (!ind)
					pCurr = pCurr->m_next;
				if (!lInd)
					pLhs = pLhs->m_next;
				if (!rInd)
					pRhs = pRhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[lInd] <= pRhs->m_vals[rInd];
		}
	}
	else
		ret = (TempPoolArray*)&operator<=((Listable&)rhs);
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator< (const OME_SCALAR & rhs)
{
	using namespace TPAPool;
	TempPoolArray* ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

	TPANode* pCurr = ret->m_pFront;
	TPANode* pLhs = m_pFront;
	size_t ind;
	for (size_t i = m_start; i < m_length + m_start; ++i)
	{
		ind = MOD2(i);
		if (i && !ind)
		{
			pCurr = pCurr->m_next;
			pLhs = pLhs->m_next;
		}
		pCurr->m_vals[ind] = pLhs->m_vals[ind] < rhs;
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator< (Listable & rhs)
{
	using namespace TPAPool;
	TempPoolArray* ret;

	if (GetLevel() == rhs.GetLevel())
	{
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);
		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		size_t ind;
		for (size_t i = m_start; i < m_length + m_start; ++i)
		{
			ind = MOD2(i);
			if (i && !ind)
			{
				pCurr = pCurr->m_next;
				pLhs = pLhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[ind] < rhs[i];
		}
	}
	else
	{
		Listable* pLrg, *pSml;
		bool noInvert = GetLevel() > rhs.GetLevel();
		if (noInvert)
		{
			pLrg = this;
			pSml = &rhs;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
		}
		size_t inStep = pLrg->GetSize() / pSml->GetSize();
		
		ret = TempPoolArray::NewTempNoFill(pLrg->GetSize());
		ret->GetDims() = pLrg->GetDims();
		TPANode* pCurr = ret->m_pFront;
		
		if (noInvert)
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
				{
					(*ret)[ind] = (*pLrg)[ind] < (*pSml)[s];
					if (ind >= TPA_NODE_LEN)
					{
						ind = 0;
						pCurr = pCurr->m_next;
					}
				}
			}
		}
		else
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
				{
					(*ret)[ind] = !((*pLrg)[ind] >= (*pSml)[s]);
					if (ind >= TPA_NODE_LEN)
					{
						ind = 0;
						pCurr = pCurr->m_next;
					}
				}
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

/** Compare two TempPoolArrays together, testing for less than.
@param rhs The TempPoolArray to test against.
@return Reference to new TempPoolArray with results of each less than comparison.
*/
Listable& TempPoolArray::operator<(TempPoolArray & rhs)
{
	TempPoolArray* ret;
	if (GetLevel() == rhs.GetLevel())
	{
		using namespace TPAPool;
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		TPANode* pRhs = rhs.m_pFront;
		size_t ind, lInd, rInd;
		for (size_t i = 0; i < m_length; ++i)
		{
			ind = MOD2(i);
			lInd = MOD2(i + m_start);
			rInd = MOD2(i + rhs.m_start);

			if (i)
			{
				if (!ind)
					pCurr = pCurr->m_next;
				if (!lInd)
					pLhs = pLhs->m_next;
				if (!rInd)
					pRhs = pRhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[lInd] < pRhs->m_vals[rInd];
		}
	}
	else
		ret = (TempPoolArray*)&operator<((Listable&)rhs);
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator>=(const OME_SCALAR & rhs)
{
	using namespace TPAPool;
	TempPoolArray* ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

	TPANode* pCurr = ret->m_pFront;
	TPANode* pLhs = m_pFront;
	size_t ind;
	for (size_t i = m_start; i < m_length + m_start; ++i)
	{
		ind = MOD2(i);
		if (i && !ind)
		{
			pCurr = pCurr->m_next;
			pLhs = pLhs->m_next;
		}
		pCurr->m_vals[ind] = pLhs->m_vals[ind] >= rhs;
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator>=(Listable & rhs)
{
	using namespace TPAPool;
	TempPoolArray* ret;

	if (GetLevel() == rhs.GetLevel())
	{
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);
		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		size_t ind;
		for (size_t i = m_start; i < m_length + m_start; ++i)
		{
			ind = MOD2(i);
			if (i && !ind)
			{
				pCurr = pCurr->m_next;
				pLhs = pLhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[ind] >= rhs[i];
		}
	}
	else
	{
		Listable* pLrg, *pSml;
		bool noInvert = GetLevel() > rhs.GetLevel();
		if (noInvert)
		{
			pLrg = this;
			pSml = &rhs;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
		}
		size_t inStep = pLrg->GetSize() / pSml->GetSize();
		
		ret = TempPoolArray::NewTempNoFill(pLrg->GetSize());
		ret->GetDims() = pLrg->GetDims();
		TPANode* pCurr = ret->m_pFront;
		
		if (noInvert)
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
				{
					(*ret)[ind] = (*pLrg)[ind] >= (*pSml)[s];
					if (ind >= TPA_NODE_LEN)
					{
						ind = 0;
						pCurr = pCurr->m_next;
					}
				}
			}
		}
		else
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
				{
					(*ret)[ind] = !((*pLrg)[ind] < (*pSml)[s]);
					if (ind >= TPA_NODE_LEN)
					{
						ind = 0;
						pCurr = pCurr->m_next;
					}
				}
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

/** Compare two TempPoolArrays together, testing for greater than or equal.
@param rhs The TempPoolArray to test against.
@return Reference to new TempPoolArray with results of each greater than or equal comparison.
*/
Listable& TempPoolArray::operator>=(TempPoolArray & rhs)
{
	TempPoolArray* ret;
	if (GetLevel() == rhs.GetLevel())
	{
		using namespace TPAPool;
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		TPANode* pRhs = rhs.m_pFront;
		size_t ind, lInd, rInd;
		for (size_t i = 0; i < m_length; ++i)
		{
			ind = MOD2(i);
			lInd = MOD2(i + m_start);
			rInd = MOD2(i + rhs.m_start);

			if (i)
			{
				if (!ind)
					pCurr = pCurr->m_next;
				if (!lInd)
					pLhs = pLhs->m_next;
				if (!rInd)
					pRhs = pRhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[lInd] >= pRhs->m_vals[rInd];
		}

	}
	else
		ret = (TempPoolArray*)&operator>=((Listable&)rhs);
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator>(const OME_SCALAR & rhs)
{
	using namespace TPAPool;
	TempPoolArray* ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

	TPANode* pCurr = ret->m_pFront;
	TPANode* pLhs = m_pFront;
	size_t ind;
	for (size_t i = m_start; i < m_length + m_start; ++i)
	{
		ind = MOD2(i);
		if (i && !ind)
		{
			pCurr = pCurr->m_next;
			pLhs = pLhs->m_next;
		}
		pCurr->m_vals[ind] = pLhs->m_vals[ind] > rhs;
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator> (Listable & rhs)
{
	using namespace TPAPool;
	TempPoolArray* ret;

	if (GetLevel() == rhs.GetLevel())
	{
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);
		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		size_t ind;
		for (size_t i = m_start; i < m_length + m_start; ++i)
		{
			ind = MOD2(i);
			if (i && !ind)
			{
				pCurr = pCurr->m_next;
				pLhs = pLhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[ind] > rhs[i];
		}
	}
	else
	{
		Listable* pLrg, *pSml;
		bool noInvert = GetLevel() > rhs.GetLevel();
		if (noInvert)
		{
			pLrg = this;
			pSml = &rhs;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
		}
		size_t inStep = pLrg->GetSize() / pSml->GetSize();
		ret = TempPoolArray::NewTempNoFill(pLrg->GetSize());
		ret->GetDims() = pLrg->GetDims();
		TPANode* pCurr = ret->m_pFront;

		if (noInvert)
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
				{
					(*ret)[ind] = (*pLrg)[ind] > (*pSml)[s];
					if (ind >= TPA_NODE_LEN)
					{
						ind = 0;
						pCurr = pCurr->m_next;
					}
				}
			}
		}
		else
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
				{
					(*ret)[ind] = !((*pLrg)[ind] <= (*pSml)[s]);
					if (ind >= TPA_NODE_LEN)
					{
						ind = 0;
						pCurr = pCurr->m_next;
					}
				}
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

/** Compare two TempPoolArrays together, testing for greater than.
@param rhs The TempPoolArray to test against.
@return Reference to new TempPoolArray with results of each greater than comparison.
*/
Listable& TempPoolArray::operator>(TempPoolArray & rhs)
{
	TempPoolArray* ret;
	if (GetLevel() == rhs.GetLevel())
	{
		using namespace TPAPool;
		ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

		TPANode* pCurr = ret->m_pFront;
		TPANode* pLhs = m_pFront;
		TPANode* pRhs = rhs.m_pFront;
		size_t ind, lInd, rInd;
		for (size_t i = 0; i < m_length; ++i)
		{
			ind = MOD2(i);
			lInd = MOD2(i + m_start);
			rInd = MOD2(i + rhs.m_start);

			if (i)
			{
				if (!ind)
					pCurr = pCurr->m_next;
				if (!lInd)
					pLhs = pLhs->m_next;
				if (!rInd)
					pRhs = pRhs->m_next;
			}
			pCurr->m_vals[ind] = pLhs->m_vals[lInd] > pRhs->m_vals[rInd];
		}
	}
	else
		ret = (TempPoolArray*)&operator>((Listable&)rhs);
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator!()
{
	using namespace TPAPool;
	TempPoolArray* ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

	TPANode* pCurr = ret->m_pFront;
	TPANode* pLhs = m_pFront;
	size_t ind;
	for (size_t i = m_start; i < m_length + m_start; ++i)
	{
		ind = MOD2(i);
		if (i && !ind)
		{
			pCurr = pCurr->m_next;
			pLhs = pLhs->m_next;
		}
		pCurr->m_vals[ind] = !pLhs->m_vals[ind];
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::operator-()
{
	using namespace TPAPool;
	TempPoolArray* ret = new(s_tpaPool.NewCastPtr())TempPoolArray(*this, false);

	TPANode* pCurr = ret->m_pFront;
	TPANode* pLhs = m_pFront;
	size_t ind;
	for (size_t i = m_start; i < m_length + m_start; ++i)
	{
		ind = MOD2(i);
		if (i && !ind)
		{
			pCurr = pCurr->m_next;
			pLhs = pLhs->m_next;
		}
		pCurr->m_vals[ind] = pLhs->m_vals[ind] * -1.0;
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::Invert()
{
	TPANode* pCurr = m_pFront;
	size_t ind;
	for (size_t i = m_start; i < m_length + m_start; ++i)
	{
		ind = MOD2(i);
		if (i && !ind)
			pCurr = pCurr->m_next;
		
		pCurr->m_vals[ind] = !pCurr->m_vals[ind];
	}
	return *this;
}

Listable& TempPoolArray::Negate()
{
	TPANode* pCurr = m_pFront;
	size_t ind;
	for (size_t i = m_start; i < m_length + m_start; ++i)
	{
		ind = MOD2(i);
		if (i && !ind)
			pCurr = pCurr->m_next;

		pCurr->m_vals[ind] *=-1.0;
	}
	return *this;
}
Listable& TempPoolArray::Reciprocate()
{
	TPANode* pCurr = m_pFront;
	size_t ind;
	for (size_t i = m_start; i < m_length + m_start; ++i)
	{
		ind = MOD2(i);
		if (i && !ind)
			pCurr = pCurr->m_next;

		pCurr->m_vals[ind] = 1.0/pCurr->m_vals[ind];
	}
	return *this;
}

Listable& TempPoolArray::Subset(const size_t & start, const size_t & length)
{
	TempPoolArray *ret = TempPoolArray::NewTempNoFill(length, m_pRepEval);

	size_t lim = start / TPA_NODE_LEN;
	size_t oldInd = start -TPA_NODE_LEN*lim;
	TPANode* pOld = m_pFront;
	for (size_t i = 0; i < lim; ++i, pOld = pOld->m_next);

	size_t newInd = 0;
	TPANode* pNew = ret->m_pFront;
	

	for (size_t i = 0; i < length; ++i)
	{
		pNew->m_vals[newInd++] = pOld->m_vals[oldInd++];
		if (newInd >= TPA_NODE_LEN)
		{
			newInd = 0;
			pNew = pNew->m_next;
		}
		if (oldInd >= TPA_NODE_LEN)
		{
			oldInd = 0;
			pOld = pOld->m_next;
		}
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::SubsetFromStep(const size_t & ind)
{
	return Subset(ind*m_step, m_step);
}

Listable& TempPoolArray::RefSubset(const size_t & start, const size_t & length)
{
	TempPoolArray *ret = new(TPAPool::s_tpaPool.NewCastPtr())TempPoolArray();
	size_t lim = start / TPA_NODE_LEN;
	ret->m_pRepEval = m_pRepEval;
	ret->m_start = start - TPA_NODE_LEN*lim;
	ret->m_isRef = true;
	ret->m_pFront = m_pFront;
	ret->m_dims.AppendDim(length);
	ret->m_length = length;
	
	size_t i;
	for (i = 0; i < lim; ++i)
		ret->m_pFront = ret->m_pFront->m_next;

	ret->m_pBack = ret->m_pFront;

	for (; i < (start + length) / TPA_NODE_LEN; ++i)
		ret->m_pBack = ret->m_pBack->m_next;

	ret->RecalcStep();
	return *ret;
}

Listable& TempPoolArray::RefSubsetFromStep(const size_t & ind)
{
	return RefSubset(ind*m_step, m_step);
}

void TempPoolArray::SubCollect(Listable & out, const size_t & offset, const size_t & outStart)
{
	size_t lim = (offset*m_step) / TPA_NODE_LEN;
	size_t oldInd = (offset*m_step) - TPA_NODE_LEN*lim;
	TPANode* pOld = m_pFront;
	for (size_t i = 0; i < lim; ++i, pOld = pOld->m_next);


	for (size_t i = 0; i < m_step; ++i)
	{
		out[i+outStart] = pOld->m_vals[oldInd++];
		if (oldInd >= TPA_NODE_LEN)
		{
			oldInd = 0;
			pOld = pOld->m_next;
		}
	}
}

ListableSclrItr TempPoolArray::SclrBegin()
{
	return ListableSclrItr(this, 0, m_length,m_pFront);
}

OME_SCALAR& TempPoolArray::ValForRef(void*& ref, const size_t & pos,const bool & posChanged)
{
	if (posChanged)
	{
		//pos will either be in this node or subsequent node
		
		size_t upper = (((TPANode*)ref)->m_id + 1)* TPA_NODE_LEN;
		for (; upper < pos; upper += TPA_NODE_LEN)
			ref = ((TPANode*)ref)->m_next;
	}
	return ((TPANode*)ref)->m_vals[MOD2(pos)];
}


//static funcs

/** Allocate new TempPoolArray from Pool.
	@param dims The dimensions of the new TempPoolArray.
	@param lvl The abstraction level of the new TempPoolArray.
	@param fillVal The default value to fill each slot with.
	@param repObj Pointer to representative Evaluable object.
	@return Pointer to newly allocated TempPoolArray.
*/
TempPoolArray* TempPoolArray::NewTemp(const ListDims & dims, const size_t & lvl, const OME_SCALAR & fillVal, Evaluable* repObj)
{
	TempPoolArray* ret = new(TPAPool::s_tpaPool.NewCastPtr())TempPoolArray(dims, lvl, fillVal);
	ret->m_pRepEval = repObj;
	return ret;
}

/** Allocate new TempPoolArray from Pool.
@param length The length of the new TempPoolArray.
@param fillVal The default value to fill each slot with.
@param repObj Pointer to representative Evaluable object.
@return Pointer to newly allocated TempPoolArray.
*/
TempPoolArray* TempPoolArray::NewTemp(const size_t & length, const OME_SCALAR & fillVal, Evaluable* repObj)
{
	TempPoolArray* ret = new(TPAPool::s_tpaPool.NewCastPtr())TempPoolArray(length, fillVal);
	ret->m_pRepEval = repObj;
	return ret;
}

TempPoolArray* TempPoolArray::NewTemp(initializer_list<OME_SCALAR> initList)
{
	TempPoolArray* ret = NewTempNoFill(initList.size(), NULL);
	size_t i = 0;
	for (auto itr = initList.begin(); itr != initList.end(); ++itr,++i)
		ret->SetValue(i, (*itr));

	return ret;
}

TempPoolArray* TempPoolArray::NewTemp(initializer_list<Listable*> initList)
{
	size_t count = initList.size();
	TempPoolArray* result = NewTempNoFill(count);
	ListDims addDims;

	size_t i = 0;
	for (auto itr = initList.begin(); itr != initList.end(); ++itr, ++i)
	{
		Listable* temp = (*itr);

		if (itr == initList.begin() && !addDims.GetSize())
		{
			addDims = temp->GetDims();
			ListDims newDims(addDims);
			newDims.AppendDimFront(count);
			count = newDims.CalcRepLength();
			ReleaseTemp(result);
			result = NewTempNoFill(newDims, newDims.GetSize());
		}
		if (temp->GetDims() == addDims)
		{
			//fix
			for (size_t j = 0; j < temp->GetSize(); ++j)
				result->SetValue(i++, temp->GetValue(j));
			--i;
		}
		else
			throw OMEFuncOpException("GetAsTable","Sub lists do not have matching dimensions");
	}

	return result;
}

Listable* TempPoolArray::NewTemp(Listable & list)
{
	return &list;
}

/** Allocate new TempPoolArray from Pool without initializing slots.
@param dims The dimensions of the new TempPoolArray.
@param lvl The abstraction level of the new TempPoolArray.
@param repObj Pointer to representative Evaluable object.
@return Pointer to newly allocated TempPoolArray.
*/
TempPoolArray* TempPoolArray::NewTempNoFill(const ListDims & dims, const size_t & lvl, Evaluable* repObj)
{
	TempPoolArray* ret = new(TPAPool::s_tpaPool.NewCastPtr())TempPoolArray();
	
	ret->m_dims = dims;
	ret->m_length = 0;

	ret->m_dims.Shrink(dims.GetSize() - lvl);

	ret->m_length = ret->m_dims.CalcRepLength();

	ret->RecalcStep();
	ret->m_pFront = new(TPAPool::s_nodePool.NewCastPtr())TPANode(0);
	TPANode* pCurr = ret->m_pFront;

	for (size_t i = 0,id=0; i < ret->m_length / TPA_NODE_LEN; i++)
	{
		pCurr->m_next = new(TPAPool::s_nodePool.NewCastPtr())TPANode(++id);
		pCurr = pCurr->m_next;
	}
	ret->m_pBack = pCurr;
	ret->m_pRepEval = repObj;
	return ret;
}

/** Allocate new TempPoolArray from Pool without initializing slots.
@param length The length of the new TempPoolArray.
@param repObj Pointer to representative Evaluable object.
@return Pointer to newly allocated TempPoolArray.
*/
TempPoolArray* TempPoolArray::NewTempNoFill(const size_t & length, Evaluable* repObj)
{
	TempPoolArray* ret = new(TPAPool::s_tpaPool.NewCastPtr())TempPoolArray();

	ret->m_dims.AppendDim(length);
	ret->m_length = ret->m_dims.CalcRepLength();
	ret->m_step = ret->m_dims.CalcStep();

	ret->m_pFront = new(TPAPool::s_nodePool.NewCastPtr())TPANode(0);
	TPANode* pCurr = ret->m_pFront;

	for (size_t i = 0,id=0; i < ret->m_length / TPA_NODE_LEN; i++)
	{
		pCurr->m_next = new(TPAPool::s_nodePool.NewCastPtr())TPANode(++id);
		pCurr = pCurr->m_next;
	}
	ret->m_pBack = pCurr;
	ret->m_pRepEval = repObj;
	return ret;
}

/** Allocate TempPoolArray from a substep of another TempPoolArray.
	@param parent The Listable from which the subset will originate from.
	@param offset The offset to start from in increments of the parent's stepsize.
	@param repObj Pointer to the represented Evaluable object.
*/
TempPoolArray* TempPoolArray::NewTempSubstep(Listable & parent, const size_t & offset, Evaluable* repObj)
{
	TempPoolArray* ret = new(TPAPool::s_tpaPool.NewCastPtr())TempPoolArray();

	ret->m_length = parent.GetStepSize();
	size_t retStart = offset*ret->m_length;

	for (size_t i = 0; i < parent.GetDimCount() - 1; ++i)
		ret->m_dims.AppendDim(parent.GetDim(i));

	ret->RecalcStep();

	ret->m_pFront = new(TPAPool::s_nodePool.NewCastPtr())TPANode(0);
	TPANode* pCurr = ret->m_pFront;

	size_t nodeInd;
	auto itr = parent.SclrBegin() + retStart;
	for (size_t i = 0,id=0; i < ret->m_length; ++i, ++itr)
	{
		nodeInd = MOD2(i);
		if (i && !nodeInd)
		{
			pCurr->m_next = new(TPAPool::s_nodePool.NewCastPtr())TPANode(++id);
			pCurr = pCurr->m_next;
		}
		pCurr->m_vals[nodeInd] = *itr;
	}
	ret->m_pBack = pCurr;
	ret->m_pRepEval = repObj;

	return ret;
}

/** Release TempPoolArray back into free pool.
	@param tmp Pointer to TempPoolArray to release.
*/
void TempPoolArray::ReleaseTemp(TempPoolArray * tmp)
{
	TPAPool::s_tpaPool.ReleasePtr(tmp);
}

/** Reset the temporary TempPoolArray pool. */
void TempPoolArray::ClearTempPool()
{
	TPAPool::s_tpaPool.ClearPool();
}

/** Reset pool of nodes. */
void TempPoolArray::ClearNodePool()
{
	TPAPool::s_nodePool.ClearPool();
}

/** Resize pool of TempPoolArrays to compensate for overflow.*/
void TempPoolArray::RefitTempPool()
{
	TPAPool::s_tpaPool.RefitPool();
}

/** Refit node pool to accomodate the max overflow size.
	@param buff Additional number of slots to add in addition to the max overflow size.
*/
void TempPoolArray::RefitNodePool(const size_t & buff)
{
	TPAPool::s_nodePool.RefitPool(buff);
}

/** Recursively release TempPoolArray nodes back into their pool.
	@param pNode Pointer to the current node to release.
*/
void TempPoolArray::CleanupNodes(TPANode* pNode)
{
	if (pNode)
	{
		CleanupNodes(pNode->m_next);
		TPAPool::s_nodePool.ReleasePtr(pNode);
	}
}