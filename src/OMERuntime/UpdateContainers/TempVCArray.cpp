#include "UpdateContainers/TempVCArray.h"
#include "ValCache.h"
#include "MemPool.h"
#include "OMEException.h"

namespace TVCCache
{
	ValCache<OME_SCALAR> s_cache;
	ObjMemPool<TempVCArray> s_pool = ObjMemPool<TempVCArray>(10000);
};

/** Detailed Constructor.
@param dims The dimensions of the array.
@param lvl The level of abstraction to associate with the array.
@param fillVal Default value to initialize array slots with.
@param pEval Pointer to associated Evaluable.
*/
TempVCArray::TempVCArray(const ListDims & dims, const size_t & lvl, const OME_SCALAR & fillVal, Evaluable* pEval)
	:Listable(dims), m_pRepEval(pEval)
{
	m_dims.Shrink(m_dims.GetSize() - lvl);
	m_length = m_dims.CalcRepLength();
	m_start = TVCCache::s_cache.Append(m_length, fillVal);
	m_step = m_length / m_dims.Outermost();
}

/** Detailed Constructor.
@param length The length of the array.
@param fillVal Default value to initialize array slots with.
@param pEval Pointer to associated Evaluable.
*/
TempVCArray::TempVCArray(const size_t & length, const OME_SCALAR & fillVal, Evaluable* pEval)
	:Listable(length),m_pRepEval(pEval)
{
	m_start = TVCCache::s_cache.Append(length, fillVal);
}

/** Copy Constructor.
@param tvc The TempVCArray to copy.
@param copyVals If true, copy stored values; otehrwise just create an array with a length equivalent to tpa.
*/
TempVCArray::TempVCArray(const TempVCArray & tvc,const bool & copyVals)
	: Listable( tvc.m_dims), m_pRepEval(NULL)
{
	using namespace TVCCache;
	m_start = s_cache.GetInsertIndex();
	
	if (copyVals)
	{
		for (size_t i = tvc.m_start; i < tvc.m_start + tvc.m_length; ++i)
			s_cache.Append(s_cache[i]);
	}
}

OME_SCALAR& TempVCArray::operator[](size_t pos)
{
	return TVCCache::s_cache[pos + m_start];
}

const OME_SCALAR TempVCArray::operator[](size_t pos) const
{
	return TVCCache::s_cache[pos + m_start];
}

bool TempVCArray::SetValue(const size_t & index, const OME_SCALAR & value)
{
	bool ret = index < m_length;
	if (ret)
		TVCCache::s_cache[index + m_start]=value;
	return ret;
}

void TempVCArray::SetValues(const OME_SCALAR & val)
{
	for (size_t i = 0; i < m_length; ++i)
		TVCCache::s_cache[i + m_start] = val;
}
void TempVCArray::SetValues(const OME_SCALAR* vals, const size_t count)
{
	for (size_t i = 0; i < count; ++i)
		TVCCache::s_cache[i + m_start] = vals[i];
}

OME_SCALAR TempVCArray::GetValue(const size_t & i) const
{
	return TVCCache::s_cache[i + m_start];
}

bool TempVCArray::GetValue(const size_t & i, OME_SCALAR & out)
{
	bool ret = i < m_length;
	if (ret)
		out = TVCCache::s_cache[i + m_start];
	return ret;
}

OME_SCALAR TempVCArray::GetValue(const ListDims & iv) const
{
	return TVCCache::s_cache[m_start + DeriveIndex(iv, m_dims)];
}

bool TempVCArray::GetValue(const ListDims & iv, OME_SCALAR & out)
{
	size_t ind = DeriveIndex(iv, m_dims);
	bool ret = ind < m_length;
	if (ret)
		out = TVCCache::s_cache[m_start + DeriveIndex(iv, m_dims)];
	return ret;
}

Evaluable* TempVCArray::GetRepEval()
{
	return m_pRepEval;
}

Listable& TempVCArray::operator+(const OME_SCALAR & rhs)
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this,false);
	
	for (size_t i = m_start; i < m_start + m_length; ++i)
		s_cache.Append(s_cache[i] + rhs);

	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::operator+(Listable & rhs)
{
	//assume outermost dims are compatible
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);
	
	if (GetLevel() == rhs.GetLevel())
	{
		ret->m_dims = m_dims;
		for (size_t i = 0; i < m_length; ++i)
			s_cache.Append(s_cache[m_start+i] + rhs[i]);
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
		ret->GetDims() = pLrg->GetDims();
		
		for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
		{
			for (size_t flip = 0; flip < inStep; ++ind, ++flip)
				s_cache.Append((*pLrg)[ind] + (*pSml)[s]);
		}
	}
	ret->RecalcStep();
	return *ret;

}

Listable& TempVCArray::operator-(const OME_SCALAR & rhs)
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);

	for (size_t i = m_start; i < m_start + m_length; ++i)
		s_cache.Append(s_cache[i] - rhs);
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::operator-(Listable & rhs)
{
	//assume outermost dims are compatible
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);

	if (GetLevel() == rhs.GetLevel())
	{
		ret->m_dims = m_dims;
		for (size_t i = 0; i < m_length; ++i)
			s_cache.Append(s_cache[m_start + i] - rhs[i]);
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
		ret->GetDims() = pLrg->GetDims();
		
		for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
		{
			for (size_t flip = 0; flip < inStep; ++ind, ++flip)
				s_cache.Append(((*pLrg)[ind] - (*pSml)[s])*resSgn);
		}
	}
	ret->RecalcStep();
	return *ret;

}

Listable& TempVCArray::operator*(const OME_SCALAR & rhs)
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);

	for (size_t i = m_start; i < m_start + m_length; ++i)
		s_cache.Append(s_cache[i] * rhs);

	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::operator*(Listable & rhs)
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);


	if (GetLevel() == rhs.GetLevel())
	{
		ret->m_dims = m_dims; 
		for (size_t i = 0; i < m_length; ++i)
			s_cache.Append(s_cache[m_start + i]*rhs[i]);
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
		ret->m_dims = pLrg->GetDims();
		
		for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
		{
			for (size_t flip = 0; flip < inStep; ++ind, ++flip)
				s_cache.Append((*pLrg)[ind] * (*pSml)[s]);
		}
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::operator/(const OME_SCALAR & rhs)
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);

	for (size_t i = m_start; i < m_start + m_length; ++i)
		s_cache.Append(s_cache[i] / rhs);
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::operator/(Listable & rhs)
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);

	if (GetLevel() == rhs.GetLevel())
	{
		ret->m_dims = m_dims; 
		for (size_t i = 0; i < m_length; ++i)
			s_cache.Append(s_cache[m_start + i] / rhs[i]);
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
		ret->GetDims() = pLrg->GetDims();
		
		if (noRecip)
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
					s_cache.Append((*pLrg)[ind] / (*pSml)[s]);
			}
		}
		else
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
					s_cache.Append(1.0/((*pLrg)[ind] / (*pSml)[s]));
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::operator==(const OME_SCALAR & rhs)
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);

	for (size_t i = m_start; i < m_start + m_length; ++i)
		s_cache.Append(s_cache[i] == rhs);

	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::operator==(Listable & rhs)
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);

	if (GetLevel() == rhs.GetLevel())
	{
		ret->m_dims = m_dims; 
		for (size_t i = 0; i < m_length; ++i)
			s_cache.Append(s_cache[m_start + i] == rhs[i]);
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
		ret->GetDims() = pLrg->GetDims();
		
		for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
		{
			for (size_t flip = 0; flip < inStep; ++ind, ++flip)
				s_cache.Append((*pLrg)[ind] == (*pSml)[s]);
		}
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::operator!=(const OME_SCALAR & rhs)
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);

	for (size_t i = m_start; i < m_start + m_length; ++i)
		s_cache.Append(s_cache[i] != rhs);
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::operator!=(Listable & rhs)
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);

	if (GetLevel() == rhs.GetLevel())
	{
		ret->m_dims = m_dims; 
		for (size_t i = 0; i < m_length; ++i)
			s_cache.Append(s_cache[m_start + i] != rhs[i]);
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
		ret->GetDims() = pLrg->GetDims();
		
		for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
		{
			for (size_t flip = 0; flip < inStep; ++ind, ++flip)
				s_cache.Append((*pLrg)[ind] != (*pSml)[s]);
		}
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::operator<=(const OME_SCALAR & rhs)
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);

	for (size_t i = m_start; i < m_start + m_length; ++i)
		s_cache.Append(s_cache[i] <= rhs);
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::operator<=(Listable & rhs)
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);

	if (GetLevel() == rhs.GetLevel())
	{
		ret->m_dims = m_dims; 
		for (size_t i = 0; i < m_length; ++i)
			s_cache.Append(s_cache[m_start + i] <= rhs[i]);
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
		ret->GetDims() = pLrg->GetDims();
		
		if (noInvert)
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
					s_cache.Append((*pLrg)[ind] <= (*pSml)[s]);
			}
		}
		else
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
					s_cache.Append(!((*pLrg)[ind] > (*pSml)[s]));
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::operator< (const OME_SCALAR & rhs)
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);

	for (size_t i = m_start; i < m_start + m_length; ++i)
		s_cache.Append(s_cache[i] < rhs);
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::operator< (Listable & rhs)
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);

	if (GetLevel() == rhs.GetLevel())
	{
		ret->m_dims = m_dims; 
		for (size_t i = 0; i < m_length; ++i)
			s_cache.Append(s_cache[m_start + i] < rhs[i]);
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
		ret->GetDims() = pLrg->GetDims();
		
		if (noInvert)
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
					s_cache.Append((*pLrg)[ind] < (*pSml)[s]);
			}
		}
		else
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
					s_cache.Append(!((*pLrg)[ind] >= (*pSml)[s]));
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::operator>=(const OME_SCALAR & rhs)
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);

	for (size_t i = m_start; i < m_start + m_length; ++i)
		s_cache.Append(s_cache[i] >= rhs);
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::operator>=(Listable & rhs)
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);

	if (GetLevel() == rhs.GetLevel())
	{
		ret->m_dims = m_dims; 
		for (size_t i = 0; i < m_length; ++i)
			s_cache.Append(s_cache[m_start + i] >= rhs[i]);
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
		ret->GetDims() = pLrg->GetDims();
		
		if (noInvert)
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
					s_cache.Append((*pLrg)[ind] >= (*pSml)[s]);
			}
		}
		else
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
					s_cache.Append(!((*pLrg)[ind] < (*pSml)[s]));
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::operator> (const OME_SCALAR & rhs)
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);

	for (size_t i = m_start; i < m_start + m_length; ++i)
		s_cache.Append(s_cache[i] > rhs);
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::operator> (Listable & rhs)
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);

	if (GetLevel() == rhs.GetLevel())
	{
		ret->m_dims = m_dims; 
		for (size_t i = 0; i < m_length; ++i)
			s_cache.Append(s_cache[m_start + i] > rhs[i]);
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
		ret->GetDims() = pLrg->GetDims();
		
		if (noInvert)
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
					s_cache.Append((*pLrg)[ind] > (*pSml)[s]);
			}
		}
		else
		{
			for (size_t ind = 0, s = 0; ind < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++ind, ++flip)
					s_cache.Append(!((*pLrg)[ind] <= (*pSml)[s]));
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::operator!()
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);

	for (size_t i = m_start; i < m_start + m_length; ++i)
		s_cache.Append(!s_cache[i]);
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::operator-()
{
	using namespace TVCCache;
	TempVCArray* ret = new(s_pool.NewCastPtr())TempVCArray(*this, false);

	for (size_t i = m_start; i < m_start + m_length; ++i)
		s_cache.Append(s_cache[i]*-1.0);
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::Invert()
{
	for (size_t i = m_start; i < m_start + m_length; ++i)
		TVCCache::s_cache[i] = !TVCCache::s_cache[i];

	return *this;
}

Listable& TempVCArray::Negate()
{
	for (size_t i = m_start; i < m_start + m_length; ++i)
		TVCCache::s_cache[i] *= -1.0;

	return *this;
}

Listable& TempVCArray::Reciprocate()
{
	for (size_t i = m_start; i < m_start + m_length; ++i)
		TVCCache::s_cache[i] = 1.0 / TVCCache::s_cache[i];

	return *this;
}

Listable& TempVCArray::Subset(const size_t & start, const size_t & length)
{
	using namespace TVCCache;
	TempVCArray *ret = s_pool.NewCastPtr();
	
	ret->m_length=length;

	ret->m_step=1;
	ret->m_dims.AppendDim(m_length);
	ret->m_start=s_cache.GetInsertIndex();

	for (size_t i = 0; i < length; ++i)
		s_cache.Append(s_cache[m_start + start + i]);
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::SubsetFromStep(const size_t & ind)
{
	using namespace TVCCache;
	TempVCArray *ret = s_pool.NewCastPtr();

	ret->m_length = m_step;
	size_t retStart = m_start + (m_step*ind);
	
	ret->m_dims = ListDims(m_dims, 1);
	ret->m_step = m_step / ret->m_dims.Outermost();

	for (size_t i = 0; i < ret->m_length; ++i)
		s_cache.Append(s_cache[m_start + retStart + i]);
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::RefSubset(const size_t & start, const size_t & length)
{
	using namespace TVCCache;
	TempVCArray *ret = new(s_pool.NewCastPtr())TempVCArray();

	ret->m_length = length;
	ret->m_start = m_start + start;
	
	ret->m_dims = ListDims(m_dims, 1);
	ret->m_step = 1;
	ret->RecalcStep();
	return *ret;
}

Listable& TempVCArray::RefSubsetFromStep(const size_t & ind)
{
	using namespace TVCCache;
	TempVCArray *ret = new(s_pool.NewCastPtr())TempVCArray();

	ret->m_length = m_step;
	ret->m_start = m_start + (m_step*ind);

	ret->m_dims = ListDims(m_dims, 1);
	ret->RecalcStep();

	return *ret;
}

void TempVCArray::SubCollect(Listable & out, const size_t & offset, const size_t & outStart)
{
	size_t start = m_start + (m_step*offset);

	for (size_t i = 0; i < m_step; ++i)
		out[i+outStart] = TVCCache::s_cache[start + i];
}

ListableSclrItr TempVCArray::SclrBegin()
{
	return ListableSclrItr(this,0, m_length);
}

OME_SCALAR& TempVCArray::ValForRef(void*& ref, const size_t & pos,const bool & posChanged)
{

	return  TVCCache::s_cache[m_start + pos];
}


//static funcs
/** Reset backing ValCache insertion index.*/
void TempVCArray::ResetGlobalCache()
{
	TVCCache::s_cache.ResetCache();
}

/** Allocate new TempVCArray from Pool.
@param dims The dimensions of the new TempVCArray.
@param lvl The abstraction level of the new TempVCArray.
@param fillVal The default value to fill each slot with.
@param repObj Pointer to representative Evaluable object.
@return Pointer to newly allocated TempVCArray.
*/
TempVCArray* TempVCArray::NewTemp(const ListDims & dims, const size_t & lvl, const OME_SCALAR & fillVal, Evaluable* repObj)
{
	TempVCArray* ret = new(TVCCache::s_pool.NewCastPtr())TempVCArray(dims, lvl, fillVal);
	ret->m_pRepEval = repObj;
	return ret;
}

/** Allocate new TempVCArray from Pool.
@param length The length of the new TempVCArray.
@param fillVal The default value to fill each slot with.
@param repObj Pointer to representative Evaluable object.
@return Pointer to newly allocated TempVCArray.
*/
TempVCArray* TempVCArray::NewTemp(const size_t & length, const OME_SCALAR & fillVal, Evaluable* repObj)
{
	TempVCArray* ret = new(TVCCache::s_pool.NewCastPtr())TempVCArray(length, fillVal);
	ret->m_pRepEval = repObj;
	return ret;
}


TempVCArray* TempVCArray::NewTemp(initializer_list<OME_SCALAR> initList)
{
	TempVCArray* ret = NewTempNoFill(initList.size(), NULL);
	size_t i = 0;
	for (auto itr = initList.begin(); itr != initList.end(); ++itr,++i)
		ret->SetValue(i, (*itr));

	return ret;
}

TempVCArray* TempVCArray::NewTemp(initializer_list<Listable*> initList)
{
	    size_t count=initList.size();
	    TempVCArray* result=NewTempNoFill(count);
	    ListDims addDims;
	    
	    size_t i=0;
	    for(auto itr=initList.begin(); itr!=initList.end();++itr,++i)
	    {
	        Listable* temp=(*itr);
	        
	        if(itr==initList.begin() && !addDims.GetSize())
	        {
	            addDims = temp->GetDims();
	            ListDims newDims(addDims);
	            newDims.AppendDimFront(count);
	            count = newDims.CalcRepLength();
				ReleaseTemp(result);
	            result = NewTempNoFill(newDims,newDims.GetSize());
	        }
	        if(temp->GetDims()==addDims)
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


Listable* TempVCArray::NewTemp(Listable & list)
{
	return &list;
}

/** Allocate new TempVCArray from Pool without initializing slots.
@param dims The dimensions of the new TempVCArray.
@param lvl The abstraction level of the new TempVCArray.
@param repObj Pointer to representative Evaluable object.
@return Pointer to newly allocated TempVCArray.
*/
TempVCArray* TempVCArray::NewTempNoFill(const ListDims & dims, const size_t & lvl, Evaluable* repObj)
{
	TempVCArray* ret = new(TVCCache::s_pool.NewCastPtr())TempVCArray();
	ret->m_dims = dims;
	ret->m_length = 0;
	
	ret->m_dims.Shrink(dims.GetSize() - lvl);

	ret->m_length = ret->m_dims.CalcRepLength();

	ret->RecalcStep();
	ret->m_start = TVCCache::s_cache.FastAppend(ret->m_length, OME_NOVAL);

	ret->m_pRepEval = repObj;
	return ret;
}

/** Allocate new TempVCArray from Pool without initializing slots.
@param length The length of the new TempVCArray.
@param repObj Pointer to representative Evaluable object.
@return Pointer to newly allocated TempVCArray.
*/
TempVCArray* TempVCArray::NewTempNoFill(const size_t & length, Evaluable* repObj)
{
	TempVCArray* ret = new(TVCCache::s_pool.NewCastPtr())TempVCArray();
	ret->m_step = 1;
	ret->m_dims.AppendDim(length);
	ret->m_length = length;
	ret->m_start = TVCCache::s_cache.FastAppend(ret->m_length, OME_NOVAL);
	ret->m_pRepEval = repObj;
	return ret;
}

/** Allocate TempVCArray from a substep of another TempVCArray.
@param parent The Listable from which the subset will originate from.
@param offset The offset to start from in increments of the parent's stepsize.
@param repObj Pointer to the represented Evaluable object.
*/
TempVCArray* TempVCArray::NewTempSubstep(Listable & parent, const size_t & offset, Evaluable* repObj)
{
	using namespace TVCCache;
	TempVCArray* ret = s_pool.NewCastPtr();

	ret->m_length = parent.GetStepSize();
	size_t retStart = offset*ret->m_length;
	
	for (size_t i = 0; i < parent.GetDimCount() - 1; ++i)
		ret->m_dims.AppendDim(parent.GetDim(i));

	ret->RecalcStep();

	ret->m_start = s_cache.GetInsertIndex();
	auto itr = parent.SclrBegin() + retStart;
	for (size_t i = 0; i < ret->m_length; ++i,++itr)
		s_cache.Append(*itr);

	ret->m_pRepEval = repObj;
	return ret;
}

/** Release TempVCArray back into free pool.
@param tmp Pointer to TempVCArray to release.
*/
void TempVCArray::ReleaseTemp(TempVCArray * tmp)
{
	TVCCache::s_pool.ReleasePtr(tmp);
}

/** Reset the temporary TempVCArray pool. */
void TempVCArray::ClearTempPool()
{
	TVCCache::s_pool.ClearPool();
}

/** Resize pool of TempVCArray to compensate for overflow.*/
void TempVCArray::RefitTempPool()
{
	TVCCache::s_pool.RefitPool();
}
