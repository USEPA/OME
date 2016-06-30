#pragma once
#include<deque>
#include<valarray>

/** Container class used to retain memory after internal reset.*/
template<class T>
class ValCache
{
public:
	//initializer signature
	
	ValCache();
	ValCache(size_t length,const T & defaultVal);
	~ValCache();

	void ResetCache();

	T& operator[](const size_t & i);
	/** @return Reference to the internal container.*/
	std::deque<T>& operator*() { return m_cache;}
	size_t Append(const T & val);
	size_t Append(const size_t & length,const T & fillVal);
	size_t Append(T* val, const size_t & s);
	size_t Append(const std::valarray<T> & vals);
	size_t Append(const std::deque<T> & vals);
	template<class LambdaFunc>
	size_t InitAppend(const size_t & length, LambdaFunc InitFunc);
	size_t GetSize() const;
	size_t GetAllocatedSize() const;

	size_t FastAppend(const size_t & length, const T & fillVal);

	/** @return index of insertion point. Synonymous of GetSize().*/
	inline size_t GetInsertIndex() const { return GetSize(); }
	void ResizeCache(const size_t & size);
private:
	size_t m_insert;			///< Current insertion point into m_cache.
	std::deque<T> m_cache;		///< Internal container, which is intended to grow as necessary when values are appended, and retain allocated space until destruction or explicit resize.
};

#pragma region Function Definitions

/** Default Constructor. */
template<class T>
ValCache<T>::ValCache()
	:m_insert(0)
{

}

/** Simple Constructor.
	@param length Initial length of internal container.
	@param defaultVal Value to assign to length count entries.
*/
template<class T>
ValCache<T>::ValCache(size_t length,const T & defaultVal)
	:m_insert(length)
{
	m_cache=std::deque<T>();
	for(size_t i=0; i<m_insert; i++)
		m_cache.push_back(defaultVal);
}

/** Destructor. */
template<class T>
ValCache<T>::~ValCache()
{}

/** Reset the ValCache insertion index.*/
template<class T>
void ValCache<T>::ResetCache()
{
	m_insert=0;
}

/** Random access operator.
	@param i The index to retrieve.
	@return Reference to object found at index i.
*/
template<class T>
T& ValCache<T>::operator[](const size_t & i)
{
	return m_cache[i];
}

/** Append a value to the internal container.
	@param val The object to insert.
	@return The index for the inserted object val.
*/
template<class T>
size_t ValCache<T>::Append(const T & val)
{
	if(m_insert<m_cache.size())
		m_cache[m_insert]=val;
	else
		m_cache.push_back(val);

	size_t ret=m_insert++;
	return ret;
}

/** Append several copies of a value to the the internal container.
@param length The number of copies of the value to append.
@param fillVal The value to use to fill the newly allocated spaces.
@return index of the first inserted value.
*/
template<class T>
size_t ValCache<T>::Append(const size_t & length, const T & fillVal)
{
	size_t i;
	for (i = 0; i<m_cache.size() - m_insert && i<length; i++)
		m_cache[m_insert + i] = fillVal;

	for (; i<length; i++)
		m_cache.push_back(fillVal);

	size_t ret = m_insert;

	m_insert += length;
	return ret;
}

/** Append a series of values to the internal container.
	@param val Pointer to an array of values to append.
	@param s The size of the array pointed to by val.
	@return index of the first inserted value.
*/
template<class T>
size_t ValCache<T>::Append(T* val, const size_t & s)
{
	size_t i;
	for(i=0; i<m_cache.size()-m_insert && i<s; i++)
		m_cache[m_insert+i]=val[i];

	for(;i<s;i++)
		m_cache.push_back(val[i]);

	size_t ret=m_insert;

	m_insert+=s;
	return ret;
}

/** Append a series of values to the internal container.
	@param vals valarray of values to append to internal container.
	@return index of the first inserted value.
*/
template<class T>
size_t ValCache<T>::Append(const std::valarray<T> & vals)
{
	size_t i;
	for(i=0; i<m_cache.size()-m_insert && i<vals.size(); i++)
		m_cache[m_insert+i]=vals[i];

	for(;i<vals.size();i++)
		m_cache.push_back(vals[i]);

	size_t ret=m_insert;

	m_insert+=vals.size();
	return ret;
}

/** Append a series of values to the internal container.
	@param vals deque of values to append to internal container.
	@return index of the first inserted value.
*/
template<class T>
size_t ValCache<T>::Append(const std::deque<T> & vals)
{
	size_t i;
	for(i=0; i<m_cache.size()-m_insert && i<vals.size(); i++)
		m_cache[m_insert+i]=vals[i];

	for(;i<vals.size();i++)
		m_cache.push_back(vals[i]);

	size_t ret=m_insert;

	m_insert+=vals.size();
	return ret;
}

/** Append new values initialized using a custom function.
	@param length The number of values to append.
	@param InitFunc Function used to initialize each newly added value.
	@return The index of the first inserted value.
*/
template<class T>
template<class LambdaFunc>
size_t ValCache<T>::InitAppend(const size_t & length, LambdaFunc InitFunc)
{
	size_t i;
	for (i = 0; i<m_cache.size() - m_insert && i<length; i++)
		m_cache[m_insert + i] = InitFunc(i);

	for (; i<length; i++)
		m_cache.push_back(InitFunc(i));

	size_t ret = m_insert;

	m_insert += length;
	return ret;
}

/** Append Space to ValCache, only applying fillVal to newly allocated slots.
	This method is faster than append since it skips initialization for previously allocated slots.
@param fillVal The value to use in the appended slots.
@param length The size of the array pointed to by val.
@return index of the first inserted value.
*/
template<class T>
size_t ValCache<T>::FastAppend(const size_t & length, const T & fillVal)
{
	size_t ret = m_insert;

	m_insert += length;
	while (m_cache.size()<m_insert)
		m_cache.push_back(fillVal);
	
	return ret;
}


/** @return The current number of objects stored within the internal container. */
template<class T>
size_t ValCache<T>::GetSize() const
{
	return m_insert;
}

/** @return The total number of slots presently allocated to the internal container. */
template<class T>
size_t ValCache<T>::GetAllocatedSize() const
{
	return m_cache.size();
}

/** Resize the allocated space for the internal container.
	@param size The number of slots to allocate.
*/
template<class T>
void ValCache<T>::ResizeCache(const size_t & size)
{
	m_insert = 0;
	size ? m_cache.resize(size) : m_cache.clear();
}

#pragma endregion