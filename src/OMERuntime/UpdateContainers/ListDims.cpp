#include "UpdateContainers/ListDims.h"
#include "OMEDefines.h"
#include "MemPool.h"
#include "OMEException.h"
/// @cond DOX_NO_DOC
// memory pools
namespace DimCaches
{
	ObjMemPool<ListDims::DimNode> s_tempNodePool = ObjMemPool<ListDims::DimNode>(2000);
	ObjMemPool<ListDims::DimNode> s_persistNodePool = ObjMemPool<ListDims::DimNode>(500);
};
///@endcond

/** Detailed constructor.
	@param length The total number of dimensions to include.
	@param isPersist If true, allocates nodes from persistent pool; otherwise, allocates from temporary pool.
*/
ListDims::ListDims(const size_t & length, const bool & isPersist)
	:m_length(length)
{
	m_pSrcPool = isPersist ? &DimCaches::s_persistNodePool : &DimCaches::s_tempNodePool;
	m_pSrc = new(m_pSrcPool->NewCastPtr())DimNode();
	DimNode* pNode = m_pSrc;
	for (unsigned short i = 1; i < m_length / DN_LEN; ++i)
	{
		pNode->m_next = new(m_pSrcPool->NewCastPtr())DimNode();
		pNode = pNode->m_next;
	}
	m_pTail = pNode;
}

/** Equivalent of default constructor.
	@param isPersist If true, allocates nodes from persistent pool; otherwise, allocates from temporary pool.
*/
ListDims::ListDims(const bool & isPersist)
	:m_length(0)
{
	m_pSrcPool = isPersist ? &DimCaches::s_persistNodePool : &DimCaches::s_tempNodePool;
	m_pSrc = m_pTail = new(m_pSrcPool->NewCastPtr())DimNode();
}

/** Modified copy constructor.
	@param dims The ListDims to copy.
	@param subtract The number of dimensions to subtract from the end.
	@param isPersist If true, allocates nodes from persistent pool; otherwise, allocates from temporary pool.
*/
ListDims::ListDims(const ListDims & dims, const unsigned short & subtract, const bool & isPersist)
{
	m_pSrcPool = isPersist ? &DimCaches::s_persistNodePool : &DimCaches::s_tempNodePool;
	m_pSrc = m_pTail = new(m_pSrcPool->NewCastPtr())DimNode();
	m_length = dims.m_length - subtract;
	DimNode* lNode = m_pSrc;
	DimNode* rNode = dims.m_pSrc;
	unsigned short i = 0;
	while (i < m_length)
	{
		memcpy(lNode->m_vals, rNode->m_vals, sizeof(size_t)*DN_LEN);
		i += DN_LEN;
		if (i<m_length)
		{
			if (!lNode->m_next)
				lNode->m_next = new(m_pSrcPool->NewCastPtr())DimNode();
			lNode = lNode->m_next;
			rNode = rNode->m_next;
		}

	}
}

ListDims::~ListDims()
{
	KillNodes(m_pSrc);
}

/** Recursively remove all nodes from the DimNode linked list.
	@param pNode The current node to release.
*/
void ListDims::KillNodes(DimNode* pNode)
{
	if (pNode->m_next != NULL)
	{
		KillNodes(pNode->m_next);
	}
	m_pSrcPool->ReleasePtr(pNode);
}

/** Set the number of dimensions to 0. */
void ListDims::ClearDims()
{
	m_length = 0;
}

/** Calculate the total stride to walk the outmost dimension.
	@return the stride for the outmost dimension.
*/
size_t ListDims::CalcStep() const
{
	size_t ret = 0;
	if (m_length)
	{
		DimNode* pNode = m_pSrc;
		ret = 1;
		for (unsigned short i = 0; i < m_length - 1; ++i)
		{
			if (i && !(i % DN_LEN))
			{
				if (!pNode->m_next)
					pNode->m_next = new(m_pSrcPool->NewCastPtr())DimNode();
				pNode = pNode->m_next;
			}
			ret *= pNode->m_vals[i%DN_LEN];
		}
	}

	return ret;
}

/** Calculate the total length of the represented container.
	@return The total length of the represented container.
*/
size_t ListDims::CalcRepLength() const
{
	size_t ret = 0;
	if (m_length)
		ret = CalcStep() * Outermost();
	return ret;
}

/** Append an additional Dimension to the ListDims.
	@param val The value of the new dimension.
*/
void ListDims::AppendDim(const size_t & val)
{
	//if present length is multiple of DM_LEN, then a new node will need to be appended
	unsigned short localInd = m_length % DN_LEN;
	if (m_length && !m_pTail->m_next && !localInd)
	{
		m_pTail->m_next = new(m_pSrcPool->NewCastPtr())DimNode();
		m_pTail = m_pTail->m_next;
	}
	m_pTail->m_vals[localInd] = val;
	++m_length;
}

/** Append a number of identical dims. 
	@param count The total number of dims to append.
	@param fill The value to fill each new dimension with.
*/
void ListDims::AppendDims(const size_t & count, const size_t & fill)
{
	for (unsigned short i = 0; i < count; ++i)
		AppendDim(fill);
}

/** Append another set of dimensions to this one.
	@param dims The ListDims to append.
*/
void ListDims::AppendDims(const ListDims & dims)
{
	for (unsigned short i = 0; i < dims.GetSize(); ++i)
		AppendDim(dims[i]);
}

/** Subtract a number of dimensions.
	@param count The number of dimensions to subtract.
*/
void ListDims::Shrink(const unsigned short & count)
{
	//this will leave unused spaces in valcache until reset
	m_length -= count <= m_length ? count : 0;
}

/** Prepend a dimension to the list.
	@param val The dimension to prepend.
*/
void ListDims::AppendDimFront(const size_t & val)
{
	//add space to end
	AppendDim(0);
	for (unsigned short i = m_length - 1; i > 0; --i)
		operator[](i) = operator[](i - 1);
	m_pSrc->m_vals[0] = val;
}

/** Index operator.
	@param i The index to reference.
	@return Reference to dimension at index i.
*/
size_t& ListDims::operator[](unsigned short i)
{
	if (i > m_length)
		throw OMEOOBException("ListDims::operator[]: index out of bounds");
	DimNode* pNode = m_pSrc;
	for (; i >= DN_LEN; i -= DN_LEN)
		pNode = pNode->m_next;

	return pNode->m_vals[i];
}

/** Constant Index operator.
	@param i The index to reference.
	@return Value of dimension at index i.
*/
size_t ListDims::operator[](unsigned short i) const
{
	if (i > m_length)
		throw OMEOOBException("ListDims::operator[]: index out of bounds");
	DimNode* pNode = m_pSrc;
	for (; i >= DN_LEN; i -= DN_LEN)
		pNode = pNode->m_next;

	return pNode->m_vals[i];
}

/** Assignment operator.
	@param rhs The ListDims to copy.
	@return Reference to this.
*/
ListDims& ListDims::operator=(const ListDims & rhs)
{
	//deep copy
	m_length = rhs.m_length;
	if (!m_pSrc)
		m_pSrc = new(m_pSrcPool->NewCastPtr())DimNode();
	DimNode* lNode = m_pSrc;
	DimNode* rNode = rhs.m_pSrc;
	unsigned short i = 0;
	while (i < rhs.m_length)
	{
		memcpy(lNode->m_vals, rNode->m_vals, sizeof(size_t)*DN_LEN);
		i += DN_LEN;
		if (i<rhs.m_length)
		{
			if (!lNode->m_next)
				lNode->m_next = new(m_pSrcPool->NewCastPtr())DimNode();
			lNode = lNode->m_next;
			rNode = rNode->m_next;
		}

	}

	return *this;
}

/** Assignment operator.
	@param inds The IndVec to copy.
	@return Reference to this.
*/
ListDims& ListDims::operator=(const IndVec & inds)
{
	//deep copy
	unsigned short i;

	m_length = inds.size();
	if (!m_pSrc)
		m_pSrc = new(m_pSrcPool->NewCastPtr())DimNode();
	DimNode* pNode = m_pSrc;
	for (i = 0; i < m_length; ++i)
	{
		if (i && !(i % DN_LEN))
		{
			if (!pNode->m_next)
				pNode->m_next = new(m_pSrcPool->NewCastPtr())DimNode();
			pNode = pNode->m_next;
		}
		pNode->m_vals[i%DN_LEN] = inds[i];
	}

	return *this;
}

/** Boolean equality operator.
	@param rhs The ListDims to compare against.
	@return true if this and rhs are equivalent; false otherwise. 
*/
bool ListDims::operator==(const ListDims & rhs)
{
	bool ret = false;
	if (m_length == rhs.m_length)
	{
		ret = true;
		for (unsigned short i = 0; i < m_length && ret; ++i)
			ret = operator[](i) == rhs[i];
	}
	return ret;
}

/** IndVec typecast.
*/
ListDims::operator IndVec() const
{
	IndVec ret(m_length);
	DimNode* pNode = m_pSrc;
	for (unsigned short i = 0; i < m_length; ++i)
	{
		if (i && !(i % DN_LEN))
			pNode = pNode->m_next;
		ret[i] = pNode->m_vals[i%DN_LEN];
	}
	return ret;
}

/** Create a copy of this ListDims.
	@return A copy of this ListDims.
*/
ListDims ListDims::Clone() const
{
	ListDims outDim(*this);
	return outDim;
}

/** Reverse the order of dimensions. */
void ListDims::Flip()
{
	//for now, perform flip using super inefficient way
	unsigned short lft = 0, rht = m_length - 1;
	size_t tmp;
	for (; lft < rht;++lft,--rht)
	{
		tmp = operator[](lft);
		operator[](lft) = operator[](rht);
		operator[](rht) = tmp;
	}
}

/** @return The number of dimensions. */
unsigned short ListDims::GetSize() const
{
	return m_length;
}

/** @return Reference to outermost (last) dimension. */
size_t& ListDims::Outermost()
{
	if (!m_length)
		throw OMEOOBException("ListDims::Outermost: No dimensions present (length of 0)");
	return m_pTail->m_vals[(m_length - 1) % DN_LEN];
}

/** @return Value of outermost (last) dimension. */
size_t ListDims::Outermost() const
{
	if (!m_length)
		throw OMEOOBException("ListDims::Outermost: No dimensions present (length of 0)");
	return m_pTail->m_vals[(m_length - 1) % DN_LEN];
}

/** @return Reference to innermost (first) dimension. */
size_t& ListDims::Innermost()
{
	if (!m_length)
		throw OMEOOBException("ListDims::Outermost: No dimensions present (length of 0)");
	return m_pSrc->m_vals[0];
}

/** @return Value of innermost (first) dimension. */
size_t ListDims::Innermost() const
{
	if (!m_length)
		throw OMEOOBException("ListDims::Outermost: No dimensions present (length of 0)");
	return m_pSrc->m_vals[0];
}

//statics
/** Refit DimNode memory pool for temporary ListDims.*/
void ListDims::RefitTempNodePool()
{
	DimCaches::s_tempNodePool.RefitPool();
}

/** Reset DimNode memory pool for temporary ListDims.*/
void ListDims::ClearTempNodePool()
{
	DimCaches::s_tempNodePool.ClearPool();
}