#pragma once
#include "OMEDefines.h"

#define IJT_NODE_LEN	512

/** A table used to map tight indices to sparse lookup.*/
class __EXPORT__ IndexJumpTable
{
public:
	struct IJTPair;
	IndexJumpTable();
	~IndexJumpTable();
	void AppendPair(const size_t & orig, const size_t & adjust);
	void AppendPair(const IJTPair & ip);

	IJTPair Lookup(const size_t & orig) const;
	void AppendIndexRange(const IndexJumpTable& src, const size_t & start, const size_t & end);

	//structs
	/** Struct for mapping one index to another. */
	struct IJTPair
	{
		size_t inInd;		///< Index to transform.
		size_t outInd;		///< The transformed index.

		/** Detailed Constructor.
			@param i The input index.
			@param o The output index.
		*/
		IJTPair(const size_t & i, const size_t & o) : inInd(i), outInd(o){}
		IJTPair() :inInd(0), outInd(0){}
	};

	/** Node for a block-based linked list. */
	struct IJTNode
	{
		IJTNode* next;					///< Pointer to next Node in linked list.
		IJTPair inds[IJT_NODE_LEN];		///< Static block of indices.

		IJTNode() : next(NULL){}
	};

	static void RefitNodePool();
	static void ClearNodePool();
	
private:

	IJTNode* m_head;		///< Pointer to first IJTNode in linked list.
	IJTNode* m_tail;		///< Pointer to last IJTNode in linked list.
	size_t m_lastInd;		///< Last index used.

	size_t m_length;		///< Total number of indices.

	size_t NodeBinSearch(IJTNode *pNode, size_t lowInd, size_t highInd,const size_t & trg) const;
	void KillNodes(IJTNode* pNode);
};