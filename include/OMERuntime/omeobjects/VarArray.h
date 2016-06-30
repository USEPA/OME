#pragma once
#include "Variable.h"
#include "../UpdateContainers/Listable.h"
#include <valarray>

class SubmodelPort;
typedef SclrArgs VALVECTOR;

/** Variant of the Variable class that stores multiple doubles */
class __EXPORT__ VarArray : public Variable,public Listable
{
	friend class SubVAList;
public:
	/*VarArray() :Variable(), Listable(true){};
	VarArray(const STLString n,const STLString d, const OME_SCALAR* vals, const size_t count);
	VarArray(const STLString n,const STLString d, const SclrVArray vals);*/

	VarArray(TI2Element* pCurrElem,const STLString & tag, const STLString & filename);
	VarArray(const VarArray & va);
	VarArray(const Variable & v);
	VarArray(Variable & v, const bool & isReplace=false);
	VarArray(const STLString & n, const STLString & desc);
	VarArray();

	virtual OME_SCALAR& operator[](size_t pos);
	virtual const OME_SCALAR operator[](size_t pos) const;
	VarArray& operator=(const SubmodelPort& rhs);
	/** Assignment operator.
		@param va VarArray to copy.
		@return reference to this.
	*/
	VarArray& operator=(const VarArray & va) { Duplicate(va); return *this; }

	virtual ~VarArray() {};

	virtual OMETYPE GetOMEType( void ) const { return OME_VARARRAY; }
	virtual bool IsOMEType(OMETYPE type) const { return type==OME_VARARRAY ? true : Variable::IsOMEType(type); }

	bool SetValue(const size_t index, const OME_SCALAR value, const bool & clamp);
	virtual bool SetValue(const size_t & index, const OME_SCALAR & value);

	virtual bool SetValue(const size_t & index, ModelEnum * value);
	virtual bool SetValue(const size_t & index, EnumEntry * value);

	/** Set The first value in a VarArray 
		@param val The value to assign.
		@param clamp If true, enforce clamping behavior.
		@return true if value was successfully assigned.
	*/
	virtual bool SetValue(const OME_SCALAR val, const bool & clamp) { return SetValue(0,val,clamp); }; 
	virtual void SetValue(const OME_SCALAR val) { SetValue(0,val,false); };

	virtual OME_SCALAR GetValue(const size_t & i) const;
	virtual ModelEnum* GetEnumClass(const size_t & i);
	virtual EnumEntry* GetEnumEntry(const size_t & i);
	virtual bool IsEnumClass(const size_t & i) const;
	virtual bool IsEnumValue(const size_t & i) const;

	virtual bool GetValue(const size_t & i, OME_SCALAR & out);
	//virtual OME_SCALAR& GetValue(const ListDims & iv) { return operator[](Listable::DeriveIndex(iv, m_dims)); }
//	virtual OME_SCALAR GetValue(const initializer_list<unsigned int> & iv) const { return operator[](Listable::DeriveIndex(iv, m_dims)); }
	virtual OME_SCALAR GetValue(const ListDims & iv) const { return operator[](Listable::DeriveIndex(iv,m_dims)); }
	virtual bool GetValue(const ListDims & iv,OME_SCALAR & out) { return GetValue(DeriveIndex(iv,m_dims),out); }

	virtual OME_SCALAR GetValue() const { return GetValue((size_t)0); };

	
	/** Recalculate the total length of th VarArray based on internal dimensions.*/
	inline void RecalcSize() { m_length = m_dims.CalcRepLength(); }
	void Allocate();//allocate on existing dims;
	void Allocate(const size_t newSize);
	void Allocate(const IndVec & newSizes);

	virtual void SetValues(const OME_SCALAR* vals, const size_t count);
	virtual void SetValues(const OME_SCALAR* vals, const size_t count, const bool & resize);
	virtual void SetValues(const VALVECTOR vals, const bool & resize=false);
	virtual void SetValues(const OME_SCALAR & val);

	virtual void SetValues(ModelEnum* pMe);
	virtual void SetValues(EnumEntry* pEe);

	virtual OME_SCALAR* GetValues(size_t & outCount) const;
	VALVECTOR GetValues() const;
	SclrPtrArray GetValueAddrs() const;
	
	virtual void Reset(BaseManager* pBm);
	

	virtual STLString GetXMLRepresentation(const unsigned int indent=0,const unsigned int inc=4);
	virtual void GetXMLAttributes(StrArray & out) const;
	virtual void GetXMLSubNodes(StrArray & out,const unsigned int indent=0,const unsigned int inc=4);
	virtual const OMEChar* GetXMLTag() const { return "vararray"; }

	virtual const OME_SCALAR* GetValueAddr() {return NULL;}
	virtual OME_SCALAR Evaluate(const OME_SCALAR time,BaseManager* pBm);
	virtual OME_SCALAR Initialize(const OME_SCALAR time,BaseManager* pBm);

	virtual size_t GetInstValCount() const;
	/** Test to see if any entries are nonzero.
		@return true if any contained value is nonzero; false otherwise.
	*/
	inline bool TestAny() const { return !TestAll(0.0); };
	virtual bool TestAny(const OME_SCALAR & testCase) const;

	/** Test to see if all entries are nonzero.
		@return true if all contained value is nonzero; false otherwise.
	*/
	inline bool TestAll() const { return !TestAny(0.0); };
	virtual bool TestAll(const OME_SCALAR & testCase) const;

	virtual size_t GetMaxInd() const;
	virtual size_t GetMinInd() const;

	virtual OME_SCALAR GetProduct() const;
	virtual OME_SCALAR GetSum() const;

	virtual VarArray* Clone() { return new VarArray(*this);};

	/** Replace internal dimensions.
		@param dims The new dimensions to use.
	*/
	inline void OverrideDims(const ListDims & dims) { m_dims=dims;};
	/** Replace internal dimensions.
		@param dims The new dimensions to use.
	*/
	inline void OverrideDims(const IndVec & dims) { m_dims = dims; };
	/** Replace internal dimensions.
	@param dims The new dimensions to use.
	*/
	void OverrideDims(const std::initializer_list<unsigned int> & dims);
	virtual ListDims& GetDims(){ return m_dims; }
	virtual ListDims GetDims() const { return m_dims; }
	virtual unsigned short GetDimCount() const { return m_dims.GetSize(); }
	virtual size_t GetDim(const size_t & ind) const { return m_dims[ind]; }
	/** Prepend a new dimension to the VarArray.
		@param d Extent of the dimension to prepend.
	*/
	void AddDimFront(const size_t & d){ m_dims.AppendDimFront(d); }
	
	virtual OME_SCALAR GetMax() const; 
	virtual OME_SCALAR GetMin() const;
	virtual const OMEChar* ToString(const OMEChar tabChar=' ', const unsigned int indent=0, const unsigned int inc=4) const;

	void GetDimsForRawIndex(size_t ind, IndVec & outDims) const;
    void GetDimsForRawIndex(size_t ind, ListDims & outDims) const;
    
	//Additional Listable overrides
	virtual size_t GetStepSize() const;
	virtual size_t GetLevel() const;

	virtual Evaluable* GetRepEval() { return this; };
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
	//virtual void SubAggregate(Listable & out, AggFunc aggFunc);

	virtual ListableSclrItr SclrBegin();
	virtual size_t GetRequestedSlotCount() const { return GetSize(); }
protected:
	SclrVArray m_fileVals; ///< values loaded from file.

	void Duplicate(const VarArray & va);
	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename);
	virtual void CheckConstant() {SetIsConstant(false); };

	//for ListableIterator
	virtual OME_SCALAR& ValForRef(void*& ref, const size_t & pos,const bool & posChanged);
	//virtual Listable* ListForRef(void* ref,const size_t & pos) = 0;

	
};

