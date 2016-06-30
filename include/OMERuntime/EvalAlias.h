#pragma once

#include "omeobjects/Evaluable.h"
#include "omeobjects/VarArray.h"
#include "OMEException.h"

/** Alias object for reporting value; analogous to Simile's "Ghost" object
*/
class __EXPORT__ EvalAlias : public Evaluable 
{
public:
	/** Detailed Constructor.
	@param pEval Pointer to represented Evaluable.
	@param alias The name to apply to the alias object
	*/
	EvalAlias(Evaluable* pEval,const STLString & alias) :Evaluable(alias,0.0),m_pSrcEval(pEval),m_srcID(pEval->GetID()) {}

	/** Constructs an unassigned alias.
		@param alias The name to give to the constructed alias.
	*/
	EvalAlias(const STLString & alias=STLString()) :Evaluable(alias, 0.0), m_pSrcEval(NULL) {}

	/** Copy constructor.
		@param eval EvalAlias to copy.
	*/
	EvalAlias(const EvalAlias & eval) :m_pSrcEval(eval.m_pSrcEval),m_srcID(eval.m_srcID) {}

	/**	XML Constructor.
	This constructor instantiates the object based on the arguments passed in.
	@param pCurrElem  the current Tiny XML Element.
	@param tag   the name of the desired tag containing the info.
	@param filename  the filename associated with the XML info.
	@see	GetXMLForObject()
	*/
	EvalAlias(TI2Element* pCurrElem, const STLString & tag, const STLString & filename)
		:m_pSrcEval(NULL)
	{
		OMEObject::PopulateFromXML(pCurrElem, tag, filename);
		CheckConstant();
	}

	virtual ~EvalAlias(){};

	virtual OME_SCALAR GetValue() const { return m_pSrcEval->GetValue(); };
	virtual OME_SCALAR GetValue(const size_t & inst) const 
	{ return m_pSrcEval->GetValue(inst); };
	virtual OME_SCALAR GetValueForRawInst(const size_t & inst) const
	{ return m_pSrcEval->GetValueForRawInst(inst); };

	virtual void SetValue(const OME_SCALAR val) { throw OMESupportException("SetValue with out index unsupportin for VarArray."); };
	virtual void SetValue(const OME_SCALAR val, const size_t & inst) { throw OMESupportException("SetValue with out index unsupportin for VarArray."); };

	virtual const OME_SCALAR* GetValueAddr() { return m_pSrcEval->GetValueAddr(); };
	virtual const OME_SCALAR* GetValueAddr(const size_t & inst) { return m_pSrcEval->GetValueAddr(inst); };
	virtual EvalTable* GetPEvalTable(const size_t & ind) const { return m_pSrcEval->GetPEvalTable(ind); };
	virtual bool HasTable() const { return m_pSrcEval->HasTable(); };
	virtual bool GetTableValue(OME_SCALAR & val, const unsigned int* inds, const unsigned int indCount, const size_t & tblInd = 0) { return m_pSrcEval->GetTableValue(val, inds, indCount,tblInd); }

	virtual bool GetInterpTableValue(const OME_SCALAR & inVal, OME_SCALAR & outVal,const size_t & ind) { return m_pSrcEval->GetInterpTableValue(inVal, outVal,ind); };

	virtual size_t GetModelIndex() const { return m_pSrcEval->GetModelIndex(); };
	virtual bool GetModelIndex(size_t & out) const { return m_pSrcEval->GetModelIndex(out); };
	virtual bool IsInitialized() const {return m_pSrcEval && m_pSrcEval->IsInitialized();};

	virtual EvalAlias* Clone() { return new EvalAlias(m_pSrcEval,m_name);};

	virtual OMETYPE GetOMEType() const { return OME_ALIAS; }
	virtual bool IsOMEType(OMETYPE type) const { return type == OME_ALIAS ? true : Evaluable::IsOMEType(type);  }

	virtual void GetXMLAttributes(StrArray & out) const;
	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual const OMEChar* GetXMLTag() const { return "alias"; }

	virtual const OMEChar* ToString(const OMEChar tabChar = ' ', const unsigned int indent = 0, const unsigned int inc = 4) const;

	/** Set the represented Evaluable object. This will also set the ID field.
		@param pSrc Pointer to the Evaluable to represent.
	*/
	inline void SetSrc(Evaluable* pSrc) { m_pSrcEval = pSrc; m_srcID = pSrc->GetID(); }
	/** Set the represented Evaluable ID. This will clear the pointer to any previously represented Evaluable.
	@param src The ID of the represented object to assign.
	*/
	inline void SetSrcID(const STLString& src) { m_pSrcEval = NULL; m_srcID = src; }
	/** @return Pointer to represented Evaluable object. */
	inline Evaluable* GetSrc() const { return m_pSrcEval; }
	/** @return The unique ID of the represented Evaluable object. */
	inline STLString GetSrcID() const { return m_srcID; }

protected:
	STLString m_srcID;			///< ID of represented value object
	Evaluable* m_pSrcEval;		///< Pointer to represented Evaluable object.
private:

};
