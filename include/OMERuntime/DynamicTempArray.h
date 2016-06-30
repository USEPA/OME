#pragma once
#include<deque>
#include "OMEDefines.h"

template<size_t STATSIZE>
class DynamicTempArray
{
public:
	DynamicTempArray();
	DynamicTempArray(const size_t & totSize);

	OME_SCALAR& operator[](const size_t & ind);
	OME_SCALAR operator[](const size_t & ind) const;

	size_t GetSize();
	void SetSize(const size_t & newSize);
private:

	OME_SCALAR m_array[STATSIZE];
	std::deque<OME_SCALAR> m_stretch;
	size_t m_currSize;

};

template<size_t STATSIZE>
DynamicTempArray<STATSIZE>::DynamicTempArray()
:m_currSize(STATSIZE)
{
	//m_array();
}

template<size_t STATSIZE>
DynamicTempArray<STATSIZE>::DynamicTempArray(const size_t & totSize)
:m_currSize(totSize)
{
	//m_array();
	for (size_t i = STATSIZE; i < totSize; ++i)
		m_stretch.push_back(0.0);
}

template<size_t STATSIZE>
OME_SCALAR& DynamicTempArray<STATSIZE>::operator[](const size_t & ind)
{
	if (ind<STATSIZE)
		return m_array[ind];

	return m_stretch[ind - STATSIZE];
}

template<size_t STATSIZE>
OME_SCALAR DynamicTempArray<STATSIZE>::operator[](const size_t & ind) const
{
	if (ind<STATSIZE)
		return m_array[ind];

	return m_stretch[ind - STATSIZE];
}

template<size_t STATSIZE>
size_t DynamicTempArray<STATSIZE>::GetSize()
{
	return m_currCount;
}

template<size_t STATSIZE>
void DynamicTempArray<STATSIZE>::SetSize(const size_t & newSize)
{
	if (newSize < STATSIZE)
		m_stretch.resize(0);
	else
		m_stretch.resize(newSize - STATSIZE);
	m_currCount = newSize;
}