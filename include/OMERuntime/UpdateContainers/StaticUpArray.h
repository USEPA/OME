#pragma once

#include "Listable.h"

class VarArray;
class ValueArchive;
/** List representation of an Evaluable object referenced across multiple instances of a model.*/
class __EXPORT__ StaticUpArray : public Listable
{
	friend class ListableSclrItr;
public:
	StaticUpArray(Evaluable* pEval, const size_t & lvl);
	StaticUpArray(VarArray* pVa, const size_t & lvl);
	virtual ~StaticUpArray() {}

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

	virtual Listable& Subset(const size_t & start, const size_t & length);
	virtual Listable& SubsetFromStep(const size_t & ind);
	virtual Listable& RefSubset(const size_t & start, const size_t & length);
	virtual Listable& RefSubsetFromStep(const size_t & ind);
	virtual void SubCollect(Listable & out, const size_t & offset, const size_t & outStart = 0);
	
	void OffsetByStep(const size_t & numSteps);
	void OffsetByAbsolute(const size_t & offset);

	virtual ListableSclrItr SclrBegin();

	static void ClearTempPool();
	static void RefitTempPool();
	static StaticUpArray* NewSUA(Evaluable* pEval, const size_t & lvl);
	static StaticUpArray* NewSUA(VarArray* pVa, const size_t & lvl);
	static StaticUpArray* NewSUA(VarArray* pVa, const size_t & start, const size_t & len);

	static void ReleaseSUA(StaticUpArray* pSua);

	static StaticUpArray NewSUAFromStack(Evaluable* pEval, const size_t & lvl);

protected:
	StaticUpArray() :Listable(), m_start(OME_NOVAL), m_pArch(NULL), m_pRefEval(NULL) {};
	//for iterator access
	virtual OME_SCALAR& ValForRef(void*& ref, const size_t & pos,const bool & posChanged);
	
	size_t m_start;			///< Start index into list.
	ValueArchive* m_pArch;	///< Archive to reference values from.
	Evaluable* m_pRefEval;	///< Pointer to represented object.
	Model* m_pRefModel;		///< Pointer to parent Model.

};
