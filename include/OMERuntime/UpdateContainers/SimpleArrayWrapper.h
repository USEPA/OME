#include"Listable.h"

/** Listable wrapper for simple C arrays. */
class __EXPORT__ SimpleArrayWrapper : public Listable
{
public:
	SimpleArrayWrapper(OME_SCALAR* arr, const size_t & len, Evaluable* repEval = NULL, const bool & isPersist=true);
	SimpleArrayWrapper(OME_SCALAR* arr, const ListDims & dims, Evaluable* repEval = NULL, const bool & isPersist=true);
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

	virtual const size_t GetSize() const;
	virtual size_t GetStepSize() const;
	virtual size_t GetLevel() const;
	virtual ListDims& GetDims();
	virtual ListDims GetDims() const;

	virtual unsigned short GetDimCount() const;
	virtual size_t GetDim(const size_t & ind) const;
	
	virtual Evaluable* GetRepEval();

	virtual Listable& Subset(const size_t & start, const size_t & length);
	virtual Listable& SubsetFromStep(const size_t & ind);
	virtual Listable& RefSubset(const size_t & start, const size_t & length);
	virtual Listable& RefSubsetFromStep(const size_t & ind);
	virtual void SubCollect(Listable & out, const size_t & offset, const size_t & outStart = 0);
	
	//Listable& SubCollect();
	//Listable& SubCollect(WACollFunc collFunc, BWInitFunc initFunc);

	virtual ListableSclrItr SclrBegin();

	static void ClearTmpPool();
	static void RefitTmpPool();

	/** @return Pointer to associated primitive array.*/
	inline OME_SCALAR* GetRawArray() const { return m_pArr; }
protected:
	//for iterator access
	virtual OME_SCALAR& ValForRef(void*& ref, const size_t & pos,const bool & posChanged);

private:
	SimpleArrayWrapper();
	OME_SCALAR* m_pArr;		///< Pointer to simple array.

	Evaluable* m_pEval;		///< Pointer to associated Evaluable object, if any.

};
