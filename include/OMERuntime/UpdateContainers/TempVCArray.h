#pragma once

#include "Listable.h"

/** Temporary List container with a ValCache backing. */
class __EXPORT__ TempVCArray : public Listable
{
public:
	TempVCArray(const ListDims & dims, const size_t & lvl, const OME_SCALAR & fillVal = 0,Evaluable* pEval=NULL);
	TempVCArray(const size_t & length, const OME_SCALAR & fillVal = 0, Evaluable* pEval = NULL);
	TempVCArray(const TempVCArray & tvc,const bool & copyVals=true);
	
	virtual ~TempVCArray() {}

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

	virtual Listable& operator-(const OME_SCALAR & rhs);
	virtual Listable& operator-(Listable & rhs);

	virtual Listable& operator*(const OME_SCALAR & rhs);
	virtual Listable& operator*(Listable & rhs);

	virtual Listable& operator/(const OME_SCALAR & rhs);
	virtual Listable& operator/(Listable & rhs);

	virtual Listable& operator==(const OME_SCALAR & rhs);
	virtual Listable& operator==(Listable & rhs);

	virtual Listable& operator!=(const OME_SCALAR & rhs);
	virtual Listable& operator!=(Listable & rhs);

	virtual Listable& operator<=(const OME_SCALAR & rhs);
	virtual Listable& operator<=(Listable & rhs);

	virtual Listable& operator< (const OME_SCALAR & rhs);
	virtual Listable& operator< (Listable & rhs);

	virtual Listable& operator>=(const OME_SCALAR & rhs);
	virtual Listable& operator>=(Listable & rhs);

	virtual Listable& operator> (const OME_SCALAR & rhs);
	virtual Listable& operator> (Listable & rhs);

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
	static void ResetGlobalCache();
	static TempVCArray* NewTemp(const ListDims & dims, const size_t & lvl, const OME_SCALAR & fillVal = 0,Evaluable* repObj=NULL);
	static TempVCArray* NewTemp(const size_t & length, const OME_SCALAR & fillVal = 0, Evaluable* repObj = NULL);
	static TempVCArray* NewTemp(initializer_list<OME_SCALAR> initList);
	static TempVCArray* NewTemp(initializer_list<Listable*> initList);
	static Listable* NewTemp(Listable & list); //pass through - temporary fix
	static TempVCArray* NewTempNoFill(const ListDims & dims, const size_t & lvl, Evaluable* repObj = NULL);
	static TempVCArray* NewTempNoFill(const size_t & length, Evaluable* repObj = NULL);
	static TempVCArray* NewTempSubstep(Listable & parent, const size_t & offset, Evaluable* repObj = NULL);

	template<class LambdaFunc>
	static TempVCArray* NewTempLambda(const size_t & length,LambdaFunc InitFunc,Evaluable* pEval=NULL);

	static void ReleaseTemp(TempVCArray * tmp);
	static void ClearTempPool();
	static void RefitTempPool();
protected:

	//for iterator access
	virtual OME_SCALAR& ValForRef(void*& ref, const size_t & pos,const bool & posChanged);

private:
	TempVCArray():Listable(),m_start(0) {};

	size_t m_start;					///< The starting index of the TempVCArray values in the ValCache.
	Evaluable* m_pRepEval;			///< Pointer to the represented Evaluable object.

};

/** Allocate TempVCArray from pool, initializing each slot using a custom function.
@param length The length of the new TempVCArray.
@param InitFunc The initialization function applied to each slot.
@param pEval Pointer to representative Evaluable object.
@return Pointer to newly allocated TempVCArray.
*/
template<class LambdaFunc>
TempVCArray* TempVCArray::NewTempLambda(const size_t & length, LambdaFunc InitFunc,Evaluable* pEval)
{
	TempVCArray* ret = NewTempNoFill(length);
	for (size_t i = 0; i < length; ++i)
		ret->operator[](i) = InitFunc(i);
	ret->m_pRepEval = pEval;
	return ret;
}
