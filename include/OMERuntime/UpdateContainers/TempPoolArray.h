#pragma once

#include "Listable.h"

//must be power of 2
#define TPA_NODE_LEN	1024

/** Temporary list container with a memory pool backing. */
class __EXPORT__ TempPoolArray : public Listable
{
public:
	TempPoolArray(const ListDims & dims, const size_t & lvl, const OME_SCALAR & fillVal = 0, Evaluable* pEval = NULL);
	TempPoolArray(const size_t & length, const OME_SCALAR & fillVal = 0, Evaluable* pEval = NULL);
	TempPoolArray(const TempPoolArray & tvc, const bool & copyVals = true);

	virtual ~TempPoolArray();

	virtual OME_SCALAR& operator[](size_t pos);
	virtual const OME_SCALAR operator[](size_t pos) const;

	virtual bool SetValue(const size_t & index, const OME_SCALAR & value);
	virtual void SetValues(const OME_SCALAR & val);
	virtual void SetValues(const OME_SCALAR* vals, const size_t count);
	//virtual void SetValues(const VALVECTOR vals, bool resize = false);

	virtual OME_SCALAR GetValue(const size_t & i) const;
	virtual bool GetValue(const size_t & i, OME_SCALAR & out);

	virtual OME_SCALAR GetValue(const ListDims & iv) const;

	virtual bool GetValue(const ListDims & iv, OME_SCALAR & out);

	virtual Evaluable* GetRepEval();

	virtual Listable& operator+(const OME_SCALAR & rhs);
	virtual Listable& operator+(Listable & rhs);
	Listable& operator+(TempPoolArray & rhs);

	virtual Listable& operator-(const OME_SCALAR & rhs);
	virtual Listable& operator-(Listable & rhs);
	Listable& operator-(TempPoolArray & rhs);

	virtual Listable& operator*(const OME_SCALAR & rhs);
	virtual Listable& operator*(Listable & rhs);
	Listable& operator*(TempPoolArray & rhs);

	virtual Listable& operator/(const OME_SCALAR & rhs);
	virtual Listable& operator/(Listable & rhs);
	Listable& operator/(TempPoolArray & rhs);

	virtual Listable& operator==(const OME_SCALAR & rhs);
	virtual Listable& operator==(Listable & rhs);
	Listable& operator==(TempPoolArray & rhs);

	virtual Listable& operator!=(const OME_SCALAR & rhs);
	virtual Listable& operator!=(Listable & rhs);
	Listable& operator!=(TempPoolArray & rhs);
	
	virtual Listable& operator<=(const OME_SCALAR & rhs);
	virtual Listable& operator<=(Listable & rhs);
	Listable& operator<=(TempPoolArray & rhs);

	virtual Listable& operator< (const OME_SCALAR & rhs);
	virtual Listable& operator< (Listable & rhs);
	Listable& operator<(TempPoolArray & rhs);

	virtual Listable& operator>=(const OME_SCALAR & rhs);
	virtual Listable& operator>=(Listable & rhs);
	Listable& operator>=(TempPoolArray & rhs);

	virtual Listable& operator> (const OME_SCALAR & rhs);
	virtual Listable& operator> (Listable & rhs);
	Listable& operator>(TempPoolArray & rhs);

	virtual Listable& operator!();
	virtual Listable& operator-();

	virtual Listable& Negate();
	virtual Listable& Invert();
	virtual Listable& Reciprocate();

	virtual Listable& Subset(const size_t & start, const size_t & length);
	virtual Listable& SubsetFromStep(const size_t & ind);
	virtual Listable& RefSubset(const size_t & start, const size_t & length);
	virtual Listable& RefSubsetFromStep(const size_t & ind);
	virtual void SubCollect(Listable & out, const size_t & offset, const size_t & outStart = 0);

	virtual ListableSclrItr SclrBegin();

	//statics
	static TempPoolArray* NewTemp(const ListDims & dims, const size_t & lvl, const OME_SCALAR & fillVal = 0, Evaluable* repObj = NULL);
	static TempPoolArray* NewTemp(const size_t & length, const OME_SCALAR & fillVal = 0, Evaluable* repObj = NULL);
	static TempPoolArray* NewTemp(initializer_list<OME_SCALAR> initList);
	static TempPoolArray* NewTemp(initializer_list<Listable*> initList);
	static Listable* NewTemp(Listable & list); //pass through - temporary fix
	static TempPoolArray* NewTempNoFill(const ListDims & dims, const size_t & lvl, Evaluable* repObj = NULL);
	static TempPoolArray* NewTempNoFill(const size_t & length, Evaluable* repObj = NULL);
	static TempPoolArray* NewTempSubstep(Listable & parent, const size_t & offset, Evaluable* repObj = NULL);

	template<class LambdaFunc>
	static TempPoolArray* NewTempLambda(const size_t & length, LambdaFunc InitFunc, Evaluable* pEval = NULL);

	static void ReleaseTemp(TempPoolArray * tmp);
	static void ClearTempPool();
	static void ClearNodePool();
	static void RefitTempPool();
	static void RefitNodePool(const size_t & buff=0);

	/** Block node for list container.*/
	struct TPANode
	{
		OME_SCALAR m_vals[TPA_NODE_LEN]; ///< Value block.
		size_t m_id;					 ///< Id of current block.
		TPANode* m_next;				 ///< Pointer to next block.

		/** Simple constructor.
			@param id The id to assign to the block.
		*/
		TPANode(const size_t & id) :m_next(NULL),m_id(id) {}
	};
protected:

	//for iterator access
	virtual OME_SCALAR& ValForRef(void*& ref, const size_t & pos,const bool & posChanged);

private:

	TempPoolArray() :Listable(), m_pFront(NULL),m_pBack(NULL),m_pRepEval(NULL),m_isRef(false),m_start(0) {};

	TPANode* m_pFront;		///< The first node of the linked block list.
	TPANode* m_pBack;		///< The last node of the linked block list.
	Evaluable* m_pRepEval;	///< The pointer to the represented Evaluable object.
	bool m_isRef;			///< If true, TempPoolArray is not responsible for cleaning up after itself.
	size_t m_start;			///< The start of the TempPoolArray within its linked block list.

	static void CleanupNodes(TPANode* pNode);
};

/** Allocate TempPoolArray from pool, initializing each slot using a custom function.
@param length The length of the new TempPoolArray.
@param InitFunc The initialization function applied to each slot.
@param pEval Pointer to representative Evaluable object.
@return Pointer to newly allocated TempPoolArray.
	*/
template<class LambdaFunc>
TempPoolArray* TempPoolArray::NewTempLambda(const size_t & length, LambdaFunc InitFunc, Evaluable* pEval)
{
	TempPoolArray* ret = NewTempNoFill(length);
	TPANode* pCurr=ret->m_pFront;
	for (size_t i = 0; i < length; ++i)
	{
		if (i && !(i % TPA_NODE_LEN))
			pCurr = pCurr->m_next;
		pCurr->m_vals[i % TPA_NODE_LEN] = InitFunc(i);
	}

	ret->m_pRepEval = pEval;
	return ret;
}
