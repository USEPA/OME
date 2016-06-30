#pragma once
#include "OMEDefines.h"
#include "ValCache.h"
#include "ListDims.h"

#include "OMEException.h"

class Evaluable;
class ListableSclrItr;


#ifndef TEMP_LIST_TYPE
#define TEMP_LIST_TYPE	TempVCArray
#define TEMP_LIST_TYPE_STR	"TempVCArray"
//#define TEMP_LIST_TYPE	TempPoolArray
#endif

/** Interface class for objects that can be indexed and iterated through as lists/arrays */
class __EXPORT__ Listable
{
	friend class ListableSclrItr;
	//friend class ListableListItr;
public:
	/** Function used for aggregating the contents of a list into a single value.*/
	typedef void(*AggFunc)(Listable* pDest, Listable* pSrc, const size_t & dInd, const size_t & sInd, const size_t & rInd);

	/** Index operator.
		@param pos The index to access.
		@return reference to value at pos.
	*/
	virtual OME_SCALAR& operator[](size_t pos)=0;
	/** Const Index operator.
		@param pos The index to access.
		@return copy of value at pos.
	*/
	virtual const OME_SCALAR operator[](size_t pos) const=0;
	/** Safe value assignment.
		@param index The index to assign the value to.
		@param value The value to attempt to assign.
		@return true if index is valid and value is assigned at index; false otherwise.
	*/
	virtual bool SetValue(const size_t & index, const OME_SCALAR & value)=0;
	/** Assign a value to each spot in a list.
		@param val The value to assign.
	*/
	virtual void SetValues(const OME_SCALAR & val) = 0;
	/** Assign values from a primitive array.
		@param vals Pointer to primitive array containing values to assign.
		@param count The number of values in the primitive array.
	*/
	virtual void SetValues(const OME_SCALAR* vals, const size_t count)=0;
	/** Retrieve value at index.
		@param i The index to reference.
		@return the value at index i.
	*/
	virtual OME_SCALAR GetValue(const size_t & i) const=0;
	/** Safely Retrieve value at index.
		@param i The index to access.
		@param out The variable to store the value at i on successful reference.
		@return true If i is a valid index and out is populated; false otherwise.
	*/
	virtual bool GetValue(const size_t & i, OME_SCALAR & out)=0;
	/** Get value for specific multidimensional index.
	@param iv Collection of indices indicating what value to access.
	@return Value found at index derived from iv.
	*/
	virtual OME_SCALAR GetValue(const ListDims & iv) const = 0;
	/** Get value for specific multidimensional index. Safe accessor
	@param iv Collection of indices indicating what value to access.
	@param out On successful return, contains value found at index derived from iv.
	@return true if value was found; false otherwise.
	*/
	virtual bool GetValue(const ListDims & iv, OME_SCALAR & out) = 0;

	//default queries; overload as necesary
	virtual bool TestAny(const OME_SCALAR & testCase) const;
	virtual bool TestAll(const OME_SCALAR & testCase) const;
	virtual size_t GetMaxInd() const;
	virtual size_t GetMinInd() const;
	virtual OME_SCALAR GetProduct() const;
	virtual OME_SCALAR GetSum() const;
	virtual OME_SCALAR GetMax() const;
	virtual OME_SCALAR GetMin() const;

	/**@return Pointer to represented Evaluable object, or NULL if there is none.*/
	virtual Evaluable* GetRepEval() = 0;

	virtual const size_t GetSize() const;
	virtual size_t GetStepSize() const;
	virtual size_t GetLevel() const;
	virtual ListDims& GetDims();
	virtual ListDims GetDims() const;

	/** @return The number of internal dimensions. */
	virtual unsigned short GetDimCount() const;
	/** Retrieve a specific dim.
	@param ind The index of the dimension to retrieve.
	@return The dimension found at index ind.
	*/
	virtual size_t GetDim(const size_t & ind) const;


	virtual void RecalcStep();

	//default operators; overload as necesary
	virtual Listable& operator+(const OME_SCALAR & rhs);
	virtual Listable& operator-(const OME_SCALAR & rhs);
	virtual Listable& operator*(const OME_SCALAR & rhs);
	virtual Listable& operator/(const OME_SCALAR & rhs);
	virtual Listable& operator==(const OME_SCALAR & rhs);
	virtual Listable& operator!=(const OME_SCALAR & rhs);
	virtual Listable& operator<=(const OME_SCALAR & rhs);
	virtual Listable& operator< (const OME_SCALAR & rhs);
	virtual Listable& operator>=(const OME_SCALAR & rhs);
	virtual Listable& operator> (const OME_SCALAR & rhs);
	//virtual Listable& operator&&(const OME_SCALAR & rhs);
	//virtual Listable& operator||(const OME_SCALAR & rhs);

	virtual Listable& operator+(Listable & rhs);
	virtual Listable& operator-(Listable & rhs);
	virtual Listable& operator*(Listable & rhs);
	virtual Listable& operator/(Listable & rhs);
	virtual Listable& operator==(Listable & rhs);
	virtual Listable& operator!=(Listable & rhs);
	virtual Listable& operator<=(Listable & rhs);
	virtual Listable& operator< (Listable & rhs);
	virtual Listable& operator>=(Listable & rhs);
	virtual Listable& operator> (Listable & rhs);
	//virtual Listable& operator&&(Listable & rhs);
	//virtual Listable& operator||(Listable & rhs);

	virtual Listable& operator!();
	virtual Listable& operator-();

	virtual Listable& Negate();
	virtual Listable& Invert();
	virtual Listable& Reciprocate();

	/** Create a List with a subset of the calling list's contents.
		@param start The first position to start subset.
		@param length The length of the subset.
		@return Reference to Pointer to new list containing subset.
	*/
	virtual Listable& Subset(const size_t & start, const size_t & length)=0;
	/** Create a List with a subset of the calling list's contents using the list's step size.
		@param ind The index of the outermost dimension to start subset at.
		@return Reference to Pointer to new list containing subset.
	*/
	virtual Listable& SubsetFromStep(const size_t & ind)=0;
	/** Create a subset reference of the calling list's contents.
		@param start The first position to start subset.
		@param length The length of the subset.
		@return Reference to subset object.
	*/
	virtual Listable& RefSubset(const size_t & start, const size_t & length)=0;
	/** Create a subset reference of the calling list's contents.
		@param ind The index of the outermost dimension to start subset at.
		@return Reference to subset object.
	*/
	virtual Listable& RefSubsetFromStep(const size_t & ind)=0;
	/** Collect the contents of a subset of a list in another list using the list's step size.
		@param out The list to store subset data.
		@param offset The outermost-dimension index to start subset at
		@param outStart The first index to start subset values in out.
	*/
	virtual void SubCollect(Listable & out, const size_t & offset,const size_t & outStart=0)=0;
	virtual Listable* SubAggregate(AggFunc aggFunc,const size_t & initVal=0.0);
	virtual Listable* SubSum();

	//Listable& SubCollect();
	//Listable& SubCollect(WACollFunc collFunc, BWInitFunc initFunc);

	/** @return a new iterator pointing to the first value in the list. */
	virtual ListableSclrItr SclrBegin()=0;

	/** Derive dimensions from a 1D index.
		@param ind Index to modify.
		@param out ListDims that will contain the appropriate dimensions on return.
	*/
	inline void DeriveDims(size_t ind, ListDims & out)
	{
		while (out.GetSize() < m_dims.GetSize())
			out.AppendDim(0);
		unsigned int sub;

		for (unsigned int i = m_dims.GetSize()-1; i+1 > 0; i--)
		{
			sub = 1;
			for (unsigned int j = 0; j<i; j++)
				sub *= m_dims[j];

			out[i]= ind/sub;
			ind -= out[i] * sub;
		}
	}

	/** Derive dimensions from a 1D index.
		@param ind Index to modify.
		@param out ListDims that will contain the appropriate dimensions on return.
		@param count contains the number of dimensions on return.
	*/
	inline void DeriveDims(const size_t & ind, ListDims & out, size_t & count)
	{
		count = m_dims.GetSize();
		DeriveDims(ind, out);
	}

	/** Utility function for deriving an index from a series of coordinates.
	@param inds ListDims object containing the reference coordinates.
	@param dims ListDims object containing the dimensions for each level.
	@return The absolute index equivalent inds when dims is used as reference.
	*/
	static inline size_t DeriveIndex(const ListDims & inds, const ListDims & dims)
	{
		unsigned int ret = inds[0];
		unsigned int sub;
		for (unsigned int i = 1; i<dims.GetSize(); i++)
		{
			sub = 1;
			for (unsigned int j = 0; j<i; j++)
				sub *= dims[j];

			ret += inds[i] * sub;
		}

		return ret;
	}

	/** Utility function for deriving an index from a series of coordinates.
	@param inds array containing the reference coordinates.
	@param dims array containing the dimensions for each level.
	@param lim The length of both inds and dims (the lengths must be equal).
	@return The absolute index equivalent inds when dims is used as reference.
	*/
	static inline size_t DeriveIndex(const size_t* inds, const ListDims & dims,const size_t & lim=0)
	{
		unsigned int ret = inds[0];
		unsigned int sub;
		for (unsigned int i = 1; i<dims.GetSize() && (!lim || i<lim); i++)
		{
			sub = 1;
			for (unsigned int j = 0; j<i; j++)
				sub *= dims[j];

			ret += inds[i] * sub;
		}

		return ret;
	}
	/** Utility function for deriving an index from a series of coordinates.
	@param inds initializer_list object containing the reference coordinates.
	@param dims ListDims object containing the dimensions for each level.
	@return The absolute index equivalent inds when dims is used as reference.
	*/
	static inline size_t DeriveIndex(const initializer_list<size_t> & inds, const ListDims & dims)
	{
		initializer_list<size_t>::const_iterator dItr = inds.begin();
		size_t ret= *dItr;
		++dItr;
		size_t sub;
		for (unsigned int i = 1; i<dims.GetSize(); i++,++dItr)
		{
			sub = 1;
			for (unsigned int j = 0; j<i; j++)
				sub *= dims[j];

			ret += (*dItr) * sub;
		}

		return ret;
	}

protected:
	Listable(const bool & isPersist=false);
	Listable(const size_t & len, const bool & isPersist = false);
	Listable(const ListDims & dims, const bool & isPersist = false);

	ListDims m_dims;		///< Structure containing dimension data.
	size_t m_length;		///< The length of the list.
	size_t m_step;			///< The size of the stride of the outermost index.

	//for iterator access
	/** Function which is intended to be called by an iterator to retrieve the correct value.
		@param ref Pointer to list defined data; address can be modified.
		@param pos The current position whose data is being requested.
		@param posChanged true if the iterator has been incremented since the last call; false otherwise.
		@return Reference to the value being requested by the iterator.
	*/
	virtual OME_SCALAR& ValForRef(void*& ref,const size_t & pos,const bool & posChanged) = 0;
	//virtual Listable* ListForRef(void* ref,const size_t & pos) = 0;

private:
};

//global scalar operators
///@cond OME_NO_DOCX
__EXPORT__ Listable& operator+(const OME_SCALAR & lhs, Listable &rhs);
__EXPORT__ Listable& operator-(const OME_SCALAR & lhs, Listable &rhs);
__EXPORT__ Listable& operator*(const OME_SCALAR & lhs, Listable &rhs);
__EXPORT__ Listable& operator/(const OME_SCALAR & lhs, Listable &rhs);
 
__EXPORT__ Listable& operator==(const OME_SCALAR & lhs, Listable &rhs);
__EXPORT__ Listable& operator!=(const OME_SCALAR & lhs, Listable &rhs);
__EXPORT__ Listable& operator<=(const OME_SCALAR & lhs, Listable &rhs);
__EXPORT__ Listable& operator< (const OME_SCALAR & lhs, Listable &rhs);
__EXPORT__ Listable& operator>=(const OME_SCALAR & lhs, Listable &rhs);
__EXPORT__ Listable& operator>(const OME_SCALAR & lhs, Listable &rhs);
//Listable& operator&&(const OME_SCALAR & lhs, Listable &rhs);
//Listable& operator||(const OME_SCALAR & lhs, Listable &rhs);

///@endcond

/** Generalized Iterator class for parsing Listable-compliant containers.*/
class __EXPORT__ ListableSclrItr
{
public:
	ListableSclrItr();
	ListableSclrItr(const ListableSclrItr & i);
	ListableSclrItr(Listable* pSrc, const size_t & initPos, const size_t & len,void* initRef=NULL);
	ListableSclrItr& operator++();
	ListableSclrItr operator++(int dummy);
	ListableSclrItr& operator+=(size_t offset);
	ListableSclrItr operator+(const size_t & offset);
	bool operator==(const ListableSclrItr & rhs);
	bool operator!=(const ListableSclrItr & rhs);
	OME_SCALAR& operator*();

	/** Return the current position of the list within the parent list.*/
	inline size_t GetPos() const { if (!IsValid()) throw OMEFuncOpException("ListableSclrItr::GetPos","iterator is invalid"); return m_pos; }
	bool IsValid() const;

	ListableSclrItr& operator=(const ListableSclrItr & itr);

private:

	Listable* m_pSrc;		///< Pointer to source list.
	void* m_pRef;			///< Pointer to list-defined data.
	size_t m_pos;			///< Index of current position.
	size_t m_endPos;		///< Index of first invalid position.
	bool m_posChanged;		///< Flag indicating that position has changed.
};
