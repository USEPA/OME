#include "UpdateContainers/IndexJumpTable.h"
#include "MemPool.h"
#include "OMEException.h"
namespace IJTPOOL
{
	ObjMemPool<IndexJumpTable::IJTNode> s_pool = ObjMemPool<IndexJumpTable::IJTNode>(1000);
}

IndexJumpTable::IndexJumpTable()
	:m_length(0), m_lastInd(0)
{
	m_head = new(IJTPOOL::s_pool.NewCastPtr())IJTNode();
	m_tail = m_head;
}

/** Recursively release linked list nodes.
	@param pNode Pointer to current node to release.
*/
void IndexJumpTable::KillNodes(IJTNode* pNode)
{
	if (pNode->next)
		KillNodes(pNode->next);
	IJTPOOL::s_pool.ReleasePtr(pNode);
}

IndexJumpTable::~IndexJumpTable()
{
	//dealloc in reverse order
	KillNodes(m_head);
}

/** Append a new node to the IndexJumpTable.
	@param orig The original/input index.
	@param adjust The adjusted/output index.
	@throws OMEOOBException if orig is not greater than each previous index.
*/
void IndexJumpTable::AppendPair(const size_t & orig, const size_t & adjust)
{
	if (m_length && m_lastInd >= orig)
		throw OMEOOBException("IndexJumpTable::AppendPair: orig argument is less than or equal to previously entered value.");

	AppendPair(IJTPair(orig, adjust));
}

/** Append a new node to the IndexJumpTable.
	@param ip IJTPair to append.
	@throws OMEOOBException if ip.inInd is not greater than each previous node.
*/
void IndexJumpTable::AppendPair(const IndexJumpTable::IJTPair & ip)
{
	if (m_length && m_lastInd >= ip.inInd)
		throw OMEOOBException("IndexJumpTable::AppendPair: orig argument is less than or equal to previously entered value.");
	
	if (!(m_length % IJT_NODE_LEN) && m_length)
	{
		m_tail->next = new(IJTPOOL::s_pool.NewCastPtr())IJTNode();
		m_tail = m_tail->next;
	}

	m_tail->inds[m_length % IJT_NODE_LEN] = ip;
	++m_length;
	m_lastInd = ip.inInd;
}

/** Retrieve node which contains an inInd value equal to orig. If a match is not found, the next
	highest node is returned.
	@param orig the inInd to search for.
	@return IJTPair that contains an inInd that is less than or equal to orig.
*/
IndexJumpTable::IJTPair IndexJumpTable::Lookup(const size_t & orig) const
{
	//an empty list is a special case
	if (m_length == 0)
		return IJTPair();

	//find target node, then perform binary search to find lowerbound;
	size_t lastInd = IJT_NODE_LEN - 1;
	
	IJTNode* pCurr = m_head;
	for (; pCurr != m_tail && orig > pCurr->inds[lastInd].inInd; pCurr = pCurr->next);

	//if we are in the last node, find the last occupied cell
	if (pCurr == m_tail)
		lastInd = (m_length-1) % IJT_NODE_LEN;

	//if upper bound is already at value, return true
	if (orig == pCurr->inds[lastInd].inInd)
		return pCurr->inds[lastInd];

	return pCurr->inds[NodeBinSearch(pCurr, 0, lastInd,orig)];
	
}

/** Append a number of IJTPairs from another IndexJumpTable.
	@param src The IndexJumpTable that contains the IJTPairs to copy.
	@param start The first IJTPair to copy from src.
	@param end The number of IJTPairs to copy.
*/
void IndexJumpTable::AppendIndexRange(const IndexJumpTable& src, const size_t & start, const size_t & end)
{
	//find starting node
	IJTNode* pCurr = m_head;
	size_t lastInd = IJT_NODE_LEN - 1;
	size_t currInd = 0;
	for (; pCurr != m_tail && start>pCurr->inds[lastInd].inInd; pCurr = pCurr->next,currInd+=IJT_NODE_LEN);

	//find start in node
	size_t subInd = NodeBinSearch(pCurr, 0, lastInd, start);
	//add first located pair as index 0;
	AppendPair(0, pCurr->inds[subInd].outInd + (start - pCurr->inds[subInd].inInd));
	++subInd;
	
	for (currInd += subInd; currInd < m_length && pCurr->inds[subInd].inInd < end; ++currInd)
	{
		IJTPair inPair = pCurr->inds[subInd++];
		inPair.inInd -= start;

		AppendPair(inPair);
		if (!(subInd % IJT_NODE_LEN))
		{
			pCurr = pCurr->next;
			subInd = 0;
		}
	}
}

/** Search within an IJTNode for an IJTPair entry using a binary search.
	@param pNode Pointer to the IJTNode to search.
	@param lowInd The lower bound index to begin search.
	@param highInd The upper bound index to begin search.
	@param trg The targer inInd to search for.
	@return The outInd of the found IJTPair.
*/
size_t IndexJumpTable::NodeBinSearch(IndexJumpTable::IJTNode *pNode, size_t lowInd, size_t highInd,const size_t & trg) const
{
	size_t midInd = highInd / 2;

	//binary search
	while (midInd != lowInd)
	{
		//target bound fond;
		if (trg == pNode->inds[midInd].inInd)
			return midInd;

		if (trg < pNode->inds[midInd].inInd)
			highInd = midInd;
		else
			lowInd = midInd;

		midInd = (highInd + lowInd) / 2;
	}

	//if we get here, return lower bound
	return midInd;
}

//statics
/** Refit pool of IJTNodes. */
void IndexJumpTable::RefitNodePool()
{
	IJTPOOL::s_pool.RefitPool();
}

/** Reset pool of IJTNodes. */
void IndexJumpTable::ClearNodePool()
{
	IJTPOOL::s_pool.ClearPool();
}

