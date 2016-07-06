#include "UpdateContainers/Listable.h"
#include "UpdateContainers/TempVCArray.h"
#include "UpdateContainers/TempPoolArray.h"

#include "MemPool.h"

#pragma region Listable class stuff

//pools for internal tempList;
ObjMemPool<double> s_tmpPool(100);

/** Default constructor.
	@param isPersist If true, pull ListDim object from persistant pool. Otherwise pull ListDim from TempPool.
*/
Listable::Listable(const bool & isPersist)
	: m_dims(isPersist), m_length(0), m_step(0)
{
}

/** Default constructor.
	@param len The length of the list.
	@param isPersist If true, pull ListDim object from persistant pool. Otherwise pull ListDim from TempPool.
*/
Listable::Listable(const size_t & len, const bool & isPersist)
	: m_dims(isPersist), m_length(len)
{
	m_dims.AppendDim(len);
	RecalcStep();
}

/** Default constructor.
	@param dims The dimensions that are used to define the length and depth of the list
	@param isPersist If true, pull ListDim object from persistant pool. Otherwise pull ListDim from TempPool.
*/
Listable::Listable(const ListDims & dims, const bool & isPersist)
	: m_dims(dims, 0,isPersist), m_length(0)
{
	if (m_dims.GetSize())
	{
		m_length = 1;
		for (unsigned short i = 0; i < m_dims.GetSize(); ++i)
			m_length *= m_dims[i];
	}
	RecalcStep();
}


/** @return Length of list. */
const size_t Listable::GetSize() const
{
	return m_length;
}

/** @return The step size (ie the number elements represented by an increment of the outermost index).*/
size_t Listable::GetStepSize() const
{
	return m_step;
}

/** @return The Level of abstraction represented by the list. */
size_t Listable::GetLevel() const
{
	return m_dims.GetSize();
}

/** @return A reference to the ListDims object.*/
ListDims& Listable::GetDims()
{
	return m_dims;
}

/** @return A copy of the ListDims object.*/
ListDims Listable::GetDims() const
{
	return m_dims;
}

/** @return The number of dimensions for the list. Will be equal to GetLevel() in most cases.*/
unsigned short Listable::GetDimCount() const
{
	return m_dims.GetSize();
}

/** @return The dimension for a specific depth. */
size_t Listable::GetDim(const size_t & ind) const
{
	return m_dims[ind];
}

/** Calculate current step size for future reference.*/
void Listable::RecalcStep()
{
	m_step = m_dims.CalcStep();
}

/** Test all values against a given value.
	@param testCase Reference to the value to test against.
	@return true if any values are equal to testCase; false otherwise.
*/
bool Listable::TestAny(const OME_SCALAR & testCase) const
{
	bool ret = false;
	for (size_t i = 0; !ret && i < m_length; ++i)
		ret = testCase == operator[](i);

	return ret;
}

/** Test all values against a given value.
	@param testCase Reference to the value to test against.
	@return true if all values are equal to testCase; false otherwise.
*/
bool Listable::TestAll(const OME_SCALAR & testCase) const
{
	bool ret = true;
	for (size_t i = 0; ret && i < m_length; ++i)
		ret = testCase == operator[](i);

	return ret;
}

/** @return index of the maximum value within the list. */
size_t Listable::GetMaxInd() const
{
	OME_SCALAR currMax = std::numeric_limits<OME_SCALAR>::min();
	OME_SCALAR currVal;
	size_t ret = 0;
	for (size_t i = 0; i < m_length; ++i)
	{
		currVal = operator[](i);
		if (currVal>currMax)
		{
			currMax = currVal;
			ret = i;
		}
	}
	return ret;
}

/** @return index of the minimum value within the list. */
size_t Listable::GetMinInd() const
{
	OME_SCALAR currMin = std::numeric_limits<OME_SCALAR>::max();
	OME_SCALAR currVal;
	size_t ret = 0;
	for (size_t i = 0; i < m_length; ++i)
	{
		currVal = operator[](i);
		if (currVal<currMin)
		{
			currMin = currVal;
			ret = i;
		}
	}
	return ret;
}

/** @return The combined product of all list values. */
OME_SCALAR Listable::GetProduct() const
{
	OME_SCALAR ret = 0.0;
	if (m_length)
	{
		ret = 1.0;

		for (size_t i = 0; i < m_length; ++i)
			ret *= operator[](i);
	}
	return ret;
}

/** @return The combined sum of all list values. */
OME_SCALAR Listable::GetSum() const
{
	OME_SCALAR ret = 0.0;

	for (size_t i = 0; i < m_length; ++i)
	{
		ret += operator[](i);
	}
	return ret;

	//OME_SCALAR ret = 0.0;
	//volatile OME_SCALAR corrected;
	//volatile OME_SCALAR tempVal;
	//volatile OME_SCALAR khErr=0.0;

	//for (size_t i = 0; i < m_length; ++i)
	//{
	//	//implement Kahan error correction, which will work for long summations
	//	
	//	//for now, use volatiles to prevent temps from being optimized out
	//	corrected = operator[](i) - khErr;
	//	tempVal = ret + corrected;
	//	khErr = (tempVal - ret) - corrected;
	//	ret = tempVal;
	//}
	//return ret;

}

/** @return The maximum value within the list. */
OME_SCALAR Listable::GetMax() const
{
	OME_SCALAR ret = std::numeric_limits<OME_SCALAR>::min();
	OME_SCALAR currVal;

	for (size_t i = 0; i < m_length; ++i)
	{
		currVal = operator[](i);
		if (currVal>ret)
			ret = currVal;
	}
	return ret;
}

/** @return The minimum value within the list. */
OME_SCALAR Listable::GetMin() const
{
	OME_SCALAR ret = std::numeric_limits<OME_SCALAR>::max();
	OME_SCALAR currVal;

	for (size_t i = 0; i < m_length; ++i)
	{
		currVal = operator[](i);
		if (currVal<ret)
			ret = currVal;
	}
	return ret;
}

/** Add scalar to all list values. 
	@param rhs The value to add to each list value.
	@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator+(const OME_SCALAR & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);

	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = operator[](i) + rhs;
	ret->RecalcStep();
	return *ret;
}

/** Add two lists together.
	@param rhs The list to add.
	@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator+(Listable & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		ret->GetDims() = m_dims;
		for (size_t i = 0; i < m_length; ++i)
			(*ret)[i] = operator[](i) + rhs[i];
	}
	else
	{
		Listable* pLrg, *pSml;
		if (m_dims.GetSize() > rhs.GetLevel())
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

		for (size_t i = 0, s = 0; i < pLrg->GetSize(); ++s)
		{
			for (size_t flip = 0; flip < inStep; ++i, ++flip)
				(*ret)[i] = (*pLrg)[i] + (*pSml)[s];
		}
	}
	ret->RecalcStep();
	return *ret;
}

/** Subtract scalar from all list values.
@param rhs The value to subtract to each list value.
@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator-(const OME_SCALAR & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);

	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = operator[](i) - rhs;
	ret->RecalcStep();
	return *ret;

}

/** Subtract two lists together.
	@param rhs The list to subtract.
	@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator-(Listable & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	ret->GetDims() = m_dims;
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		for (size_t i = 0; i < m_length; ++i)
			(*ret)[i] = operator[](i) - rhs[i];
	}
	else
	{
		Listable* pLrg, *pSml;
		OME_SCALAR resSgn;
		if (m_dims.GetSize() > rhs.GetLevel())
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

		for (size_t i = 0, s = 0; i < pLrg->GetSize(); ++s)
		{
			for (size_t flip = 0; flip < inStep; ++i, ++flip)
				(*ret)[i] = ((*pLrg)[i] - (*pSml)[s])*resSgn;
		}
	}
	ret->RecalcStep();
	return *ret;
}

/** Multiply scalar to all list values.
@param rhs The value to multiply to each list value.
@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator*(const OME_SCALAR & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);

	for (size_t i = 0; i < m_length; ++i)
		ret[i]=operator[](i) * rhs;
	ret->RecalcStep();
	return *ret;
}

/** Multiply two lists together.
@param rhs The list to multiply with.
@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator*(Listable & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	ret->GetDims() = m_dims;
	
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		ret->GetDims() = m_dims;
		for (size_t i = 0; i < m_length; ++i)
		{
			(*ret)[i] = operator[](i) * rhs[i];
		}
	}
	else
	{
		Listable* pLrg, *pSml;
		
		if (m_dims.GetSize() > rhs.GetLevel())
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

		for (size_t i = 0, s = 0; i < pLrg->GetSize(); ++s)
		{
			for (size_t flip = 0; flip < inStep; ++i, ++flip)
				(*ret)[i] = (*pLrg)[i] * (*pSml)[s];
		}
	}
	ret->RecalcStep();
	return *ret;
}

/** Divide all list values by a given scalar.
	@param rhs The value to divide each list value by.
	@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator/(const OME_SCALAR & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);

	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = operator[](i) / rhs;
	ret->RecalcStep();
	return *ret;
}

/** Divide one list by another.
@param rhs The list to divide by.
@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator/(Listable & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	ret->GetDims() = m_dims;
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		for (size_t i = 0; i < m_length; ++i)
			(*ret)[i] = operator[](i) / rhs[i];
	}
	else
	{
		Listable* pLrg, *pSml;
		
		bool noRecip = m_dims.GetSize() > rhs.GetLevel();
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
			for (size_t i = 0, s = 0; i < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++i, ++flip)
					(*ret)[i] = (*pLrg)[i] / (*pSml)[s];
			}
		}
		else
		{
			for (size_t i = 0, s = 0; i < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++i, ++flip)
					(*ret)[i] = 1.0 / ((*pLrg)[i] / (*pSml)[s]);
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

/** Compare all list values to a given scalar, testing for equality.
	@param rhs The value to compare to each list value.
	@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator==(const OME_SCALAR & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);

	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = operator[](i) == rhs;
	ret->RecalcStep();
	return *ret;
}

/** Compare two lists by value, testing for equality.
	@param rhs The list to compare against.
	@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator==(Listable & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	ret->GetDims() = m_dims;
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		for (size_t i = 0; i < m_length; ++i)
			(*ret)[i] = operator[](i) == rhs[i];
	}
	else
	{
		Listable* pLrg, *pSml;
		if (m_dims.GetSize() > rhs.GetLevel())
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
		
		for (size_t i = 0, s = 0; i < pLrg->GetSize(); ++s)
		{
			for (size_t flip = 0; flip < inStep; ++i, ++flip)
				(*ret)[i] = (*pLrg)[i] == (*pSml)[s];
		}
	}
	return *ret;
}

/** Compare all list values to a given scalar, testing for inequality.
	@param rhs The value to compare to each list value.
	@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator!=(const OME_SCALAR & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);

	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = operator[](i) != rhs;
	return *ret;
}

/** Compare two lists by value, testing for inequality.
	@param rhs The list to compare against.
	@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator!=(Listable & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	ret->GetDims() = m_dims;
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		for (size_t i = 0; i < m_length; ++i)
			(*ret)[i] = operator[](i) != rhs[i];
	}
	else
	{
		Listable* pLrg, *pSml;
		if (m_dims.GetSize() > rhs.GetLevel())
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
		
		for (size_t i = 0, s = 0; i < pLrg->GetSize(); ++s)
		{
			for (size_t flip = 0; flip < inStep; ++i, ++flip)
				(*ret)[i] = (*pLrg)[i] != (*pSml)[s];
		}
	}
	ret->RecalcStep();
	return *ret;
}

/** Compare all list values to a given scalar, testing for less than or equal.
	@param rhs The value to compare to each list value.
	@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator<=(const OME_SCALAR & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);

	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = operator[](i) <= rhs;
	return *ret;
}

/** Compare two lists by value, testing for less than or equal.
	@param rhs The list to compare against.
	@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator<=(Listable & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	ret->GetDims() = m_dims;
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		for (size_t i = 0; i < m_length; ++i)
			(*ret)[i] = operator[](i) <= rhs[i];
	}
	else
	{
		Listable* pLrg, *pSml;
		bool noInvert = m_dims.GetSize() > rhs.GetLevel();
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
			for (size_t i = 0, s = 0; i < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++i, ++flip)
					(*ret)[i] = (*pLrg)[i] <= (*pSml)[s];
			}
		}
		else
		{
			for (size_t i = 0, s = 0; i < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++i, ++flip)
					(*ret)[i] = !((*pLrg)[i] > (*pSml)[s]);
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

/** Compare all list values to a given scalar, testing for less than.
	@param rhs The value to compare to each list value.
	@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator< (const OME_SCALAR & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);

	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = operator[](i) < rhs;
	ret->RecalcStep();
	return *ret;
}

/** Compare two lists by value, testing for less than.
	@param rhs The list to compare against.
	@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator< (Listable & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	ret->GetDims() = m_dims;
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		for (size_t i = 0; i < m_length; ++i)
			(*ret)[i] = operator[](i) < rhs[i];
	}
	else
	{
		Listable* pLrg, *pSml;
		bool noInvert = m_dims.GetSize() > rhs.GetLevel();
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
			for (size_t i = 0, s = 0; i < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++i, ++flip)
					(*ret)[i] = (*pLrg)[i] < (*pSml)[s];
			}
		}
		else
		{
			for (size_t i = 0, s = 0; i < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++i, ++flip)
					(*ret)[i] = !((*pLrg)[i] >= (*pSml)[s]);
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

/** Compare all list values to a given scalar, testing for equal than or greater to.
	@param rhs The value to compare to each list value.
	@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator>=(const OME_SCALAR & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);

	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = operator[](i) >= rhs;
	ret->RecalcStep();
	return *ret;
}

/** Compare two lists by value, testing for greater than or equal.
	@param rhs The list to compare against.
	@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator>=(Listable & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	ret->GetDims() = m_dims;
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		for (size_t i = 0; i < m_length; ++i)
			(*ret)[i] = operator[](i) >= rhs[i];
	}
	else
	{
		Listable* pLrg, *pSml;
		bool noInvert = m_dims.GetSize() > rhs.GetLevel();
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
			for (size_t i = 0, s = 0; i < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++i, ++flip)
					(*ret)[i] = (*pLrg)[i] >= (*pSml)[s];
			}
		}
		else
		{
			for (size_t i = 0, s = 0; i < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++i, ++flip)
					(*ret)[i] = !((*pLrg)[i] < (*pSml)[s]);
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

/** Compare all list values to a given scalar, testing for greater than.
	@param rhs The value to compare to each list value.
	@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator>(const OME_SCALAR & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);

	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = operator[](i) > rhs;
	ret->RecalcStep();
	return *ret;
}


//Listable& Listable::operator&&(const OME_SCALAR & rhs)
//{
//
//	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);
//
//	for (size_t i = 0; i < m_length; ++i)
//		(*ret)[i] = operator[](i) && rhs;
//	ret->RecalcStep();
//	return *ret;
//}
//
//Listable& Listable::operator||(const OME_SCALAR & rhs)
//{
//
//	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);
//
//	for (size_t i = 0; i < m_length; ++i)
//		(*ret)[i] = operator[](i) || rhs;
//	ret->RecalcStep();
//	return *ret;
//}

/** Compare two lists by value, testing for greater than.
	@param rhs The list to compare against.
	@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator> (Listable & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	ret->GetDims() = m_dims;
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		ret->GetDims() = m_dims;
		for (size_t i = 0; i < m_length; ++i)
			(*ret)[i] = operator[](i) > rhs[i];
	}
	else
	{
		Listable* pLrg, *pSml;
		bool noInvert = m_dims.GetSize() > rhs.GetLevel();
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
			for (size_t i = 0, s = 0; i < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++i, ++flip)
					(*ret)[i] = (*pLrg)[i] > (*pSml)[s];
			}
		}
		else
		{
			for (size_t i = 0, s = 0; i < pLrg->GetSize(); ++s)
			{
				for (size_t flip = 0; flip < inStep; ++i, ++flip)
					(*ret)[i] = !((*pLrg)[i] <= (*pSml)[s]);
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

//Listable& Listable::operator&&(Listable & rhs)
//{
//
//	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
//	ret->GetDims() = m_dims;
//	if (m_dims.GetSize() == rhs.GetLevel())
//	{
//		ret->GetDims() = m_dims;
//		for (size_t i = 0; i < m_length; ++i)
//			(*ret)[i] = operator[](i) && rhs[i];
//	}
//	else
//	{
//		Listable* pLrg, *pSml;
//		bool noInvert = m_dims.GetSize() > rhs.GetLevel();
//		if (noInvert)
//		{
//			pLrg = this;
//			pSml = &rhs;
//		}
//		else
//		{
//			pLrg = &rhs;
//			pSml = this;
//		}
//		size_t inStep = pLrg->GetSize() / pSml->GetSize();
//		ret->GetDims() = pLrg->GetDims();
//
//		if (noInvert)
//		{
//			for (size_t i = 0, s = 0; i < pLrg->GetSize(); ++s)
//			{
//				for (size_t flip = 0; flip < inStep; ++i, ++flip)
//					(*ret)[i] = (*pLrg)[i] &&(*pSml)[s];
//			}
//		}
//		else
//		{
//			for (size_t i = 0, s = 0; i < pLrg->GetSize(); ++s)
//			{
//				for (size_t flip = 0; flip < inStep; ++i, ++flip)
//					(*ret)[i] = !((*pLrg)[i] && (*pSml)[s]);
//			}
//		}
//	}
//	ret->RecalcStep();
//	return *ret;
//}
//
//Listable& Listable::operator||(Listable & rhs)
//{
//
//	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
//	ret->GetDims() = m_dims;
//	if (m_dims.GetSize() == rhs.GetLevel())
//	{
//		ret->GetDims() = m_dims;
//		for (size_t i = 0; i < m_length; ++i)
//			(*ret)[i] = operator[](i) || rhs[i];
//	}
//	else
//	{
//		Listable* pLrg, *pSml;
//		bool noInvert = m_dims.GetSize() > rhs.GetLevel();
//		if (noInvert)
//		{
//			pLrg = this;
//			pSml = &rhs;
//		}
//		else
//		{
//			pLrg = &rhs;
//			pSml = this;
//		}
//		size_t inStep = pLrg->GetSize() / pSml->GetSize();
//		ret->GetDims() = pLrg->GetDims();
//
//		if (noInvert)
//		{
//			for (size_t i = 0, s = 0; i < pLrg->GetSize(); ++s)
//			{
//				for (size_t flip = 0; flip < inStep; ++i, ++flip)
//					(*ret)[i] = (*pLrg)[i] || (*pSml)[s];
//			}
//		}
//		else
//		{
//			for (size_t i = 0, s = 0; i < pLrg->GetSize(); ++s)
//			{
//				for (size_t flip = 0; flip < inStep; ++i, ++flip)
//					(*ret)[i] = !((*pLrg)[i] || (*pSml)[s]);
//			}
//		}
//	}
//	ret->RecalcStep();
//	return *ret;
//}

/** Create a copy of the list that contains the inversions of each list value.
	@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator!()
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);

	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = !operator[](i);
	return *ret;
}

/** Create a copy of the list that contains the negations of each list value.
	@return Reference to new temporary list containing resulting values.
*/
Listable& Listable::operator-()
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);

	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = operator[](i)*-1.0;
	ret->RecalcStep();
	return *ret;
}

/** Negate each list value.
	@return Reference to the calling list.
*/
Listable& Listable::Negate()
{
	for (size_t i = 0; i < m_length; ++i)
		operator[](i) = -1.0;
	return *this;
}

/** Invert each list value.
	@return Reference to the calling list.
*/
Listable& Listable::Invert()
{
	for (size_t i = 0; i < m_length; ++i)
	{
		OME_SCALAR& ref = operator[](i);
		ref = !ref;
	}
	return *this;
}

/** Convert each list value to its reciprocate.
	@return Reference to the calling list.
*/
Listable& Listable::Reciprocate()
{
	for (size_t i = 0; i < m_length; ++i)
	{
		OME_SCALAR& ref = operator[](i);
		ref = 1.0 / ref;
	}
	return *this;
}

/** Perform an aggregation operation on the outermost dimension of the list.
	@param aggFunc The aggregation operation.
	@param initVal The value to initalize each aggregation slot to.
	@return pointer to allocated list containing aggregated values.
*/
Listable* Listable::SubAggregate(AggFunc aggFunc,const size_t & initVal)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_dims,GetLevel()-1,initVal,GetRepEval());

	size_t offset;
	for (size_t i = 0,r=0; i < m_length;++r)
	{
		for (offset = 0; offset < m_step;++offset,++i)
			aggFunc(ret, this, offset, i, r);
	}
	return ret;
}

/** Perform a summation operation on the outermost dimension of the list.
@return pointer to allocated list containing summed values.
*/
Listable* Listable::SubSum()
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_dims, GetLevel() - 1, 0.0, GetRepEval());
	size_t lvlLen = m_length / m_step;
	//TEMP_LIST_TYPE& tmpLvl = *TEMP_LIST_TYPE::NewTempNoFill(lvlLen);
	//if (s_tmpPool.GetFreeCount() < lvlLen)
	//	s_tmpPool.ResizePool(lvlLen - s_tmpPool.GetFreeCount(),true);

	static double* tmpLvl = new double[m_length];//s_tmpPool
	static size_t currLen = m_length;

	if (currLen < m_length)
	{
		delete[] tmpLvl;
		tmpLvl = new double[m_length];
		currLen = m_length;
	}
	TEMP_LIST_TYPE& lclRet = *ret;

	size_t offset;
	//for (size_t i = 0; i < m_length;)
	//{
	//	//this would be a good place to try using SSE
	//	double tot = 0.0;
	//	for (offset = 0; offset < m_step; ++offset, ++i)
	//		(*ret)[offset] += (*this)[i];
	//}

	for (size_t r = 0; r < m_step; ++r)
	{
		//copy values into sequential list to ensure coherence;
		//if still slow, try extracting raw block from mempool and using that for coherence
		for (size_t t = 0; t < lvlLen; ++t)
			tmpLvl[t] = (*this)[r + (m_step*t)];
	}

	for (size_t r = 0,t=0; r < m_step; ++r)
	{
		//copy values into sequential list to ensure coherence;
		for (size_t s = 0; s < lvlLen; ++s,++t)
			lclRet[r] += tmpLvl[t];
	}

	//delete[] tmpLvl;

	//TEMP_LIST_TYPE::ReleaseTemp(&tmpLvl);
	return ret;
}
#pragma endregion

//global scalar operators
///@cond DOX_NO_DOC
Listable& operator+(const OME_SCALAR & lhs, Listable &rhs) { return rhs + lhs; }
Listable& operator-(const OME_SCALAR & lhs, Listable &rhs) { return (rhs + lhs).Negate(); }
Listable& operator*(const OME_SCALAR & lhs, Listable &rhs) { return rhs * lhs; }
Listable& operator/(const OME_SCALAR & lhs, Listable &rhs) { return (rhs / lhs).Reciprocate(); }

Listable& operator==(const OME_SCALAR & lhs, Listable &rhs) { return  rhs == lhs; }
Listable& operator!=(const OME_SCALAR & lhs, Listable &rhs) { return  rhs != lhs; }
Listable& operator<=(const OME_SCALAR & lhs, Listable &rhs) { return (rhs <  lhs).Invert(); }
Listable& operator< (const OME_SCALAR & lhs, Listable &rhs) { return (rhs <= lhs).Invert(); }
Listable& operator>=(const OME_SCALAR & lhs, Listable &rhs) { return (rhs >  lhs).Invert(); }
Listable& operator>(const OME_SCALAR & lhs, Listable &rhs)  { return (rhs >= lhs).Invert(); }
//Listable& operator&&(const OME_SCALAR & lhs, Listable &rhs) { return rhs && lhs; }
//Listable& operator||(const OME_SCALAR & lhs, Listable &rhs) { return rhs || lhs; }
///@endcond

#pragma region Iterator stuff

/** Default Constructor.*/
ListableSclrItr::ListableSclrItr()
	:m_pSrc(NULL), m_pRef(NULL), m_pos(0), m_endPos(0), m_posChanged(true)
{}

/** Copy Constructor. 
	@param i The Iterator to copy.
*/
ListableSclrItr::ListableSclrItr(const ListableSclrItr & i) 
	: m_pSrc(i.m_pSrc), m_pRef(i.m_pRef), m_pos(i.m_pos), m_endPos(i.m_endPos),m_posChanged(i.m_posChanged)
{}

/** Detailed Constructor.
	@param pSrc The Listable to be referenced by this iterator.
	@param initPos The start index of the iterator.
	@param len The length of the target list.
	@param initRef List specific data for use with ValForRef().
*/
ListableSclrItr::ListableSclrItr(Listable* pSrc, const size_t & initPos, const size_t & len, void* initRef)
	: m_pSrc(pSrc), m_pRef(initRef), m_pos(initPos), m_endPos(len), m_posChanged(true)
{}

/** Assignment operator.
	@param itr The iterator to copy.
	@return Reference to calling iterator.
*/
ListableSclrItr& ListableSclrItr::operator=(const ListableSclrItr & itr)
{
	m_pSrc = itr.m_pSrc;
	m_pRef = itr.m_pRef;
	m_pos = itr.m_pos;
	m_endPos = itr.m_endPos;
	m_posChanged = itr.m_posChanged;

	return *this;
}

/** Pre-increment operator.
	@return reference to calling iterator after increment. 
*/
ListableSclrItr& ListableSclrItr::operator++() 
{
	++m_pos;
	
	m_posChanged = true;
	return *this; 
}

/** Post-increment operator
	@param dummy not used.
	@return Copy of iterator before increment
*/
ListableSclrItr ListableSclrItr::operator++(int dummy)
{
	ListableSclrItr ret(*this); 
	++m_pos;

	m_posChanged = true;
	return ret; 
}

/** Addition assignment operator.
	@param offset The value to increment the iterator by.
	@return reference to the calling iterator after it has been incremented.
*/
ListableSclrItr& ListableSclrItr::operator+=(size_t offset)
{
	m_pos+=offset;

	m_posChanged = true;
	return *this; 
}

/** Create a copy of an iterator incremented by a specific value.
	@param offset The value to increment the iterator by.
	@return A copy of the iterator incremented by offset.
*/
ListableSclrItr ListableSclrItr::operator+(const size_t & offset)
{
	ListableSclrItr ret(*this); 
	ret += offset;
	return ret; 
}

/** Determine if two iterators point to the same position within the same object. 
	@param rhs The right hand side of the iterator comparison.
	@return true if iterators are equivalent.
*/
bool ListableSclrItr::operator==(const ListableSclrItr & rhs)
{
	return m_pSrc == rhs.m_pSrc && (m_pRef == rhs.m_pRef || m_pos == rhs.m_pos); 
}

/** Determine if two iterators point to different positions and/or to values within different objects.
@param rhs The right hand side of the iterator comparison.
@return true if iterators are not equivalent.
*/
bool ListableSclrItr::operator!=(const ListableSclrItr & rhs) 
{
	return m_pSrc != rhs.m_pSrc || (m_pRef != rhs.m_pRef && m_pos != rhs.m_pos); 
}

/** Dereference operator.
	@return Reference to value being pointed to by the iterator. 
	Note that IsValid() should be called in order to determine if the returned value is valid.
*/
OME_SCALAR& ListableSclrItr::operator*()
{
	OME_SCALAR& ret= m_pSrc->ValForRef(m_pRef, m_pos,m_posChanged);
	m_posChanged = false;
	return ret;
};

/** Determine if the iterator points to a valid position.
	@return true if the calling iterator points to a valid index; false otherwise.
*/
bool ListableSclrItr::IsValid() const 
{
	return m_pSrc && m_pos < m_endPos; 
}

#pragma endregion