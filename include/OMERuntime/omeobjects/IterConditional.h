#pragma once
#include "Variable.h"
#include "Model.h"

/** Class to support iteration loops in Model, provided conditions are met.
   Compatibility class for Simile's Alarm node.
   */
class __EXPORT__ IterConditional : public Variable
{

/// \question should IterConditional inherit from Variable or Evaluable?
public:
	IterConditional() : Variable() {};
   /** Basic Constructor.
      @param name Name of node.
      @param desc Description of node.
      @param exp Conditional expression.
   */
	IterConditional(STLString name, STLString desc, STLString exp) : Variable(name,desc,0.0) { m_expr=exp; };
   /** Copy constructor.
      @param ic The IterConditional to copy.
   */
	IterConditional(IterConditional & ic) { Duplicate(ic); };
   /** TiXML constructor
      @param pCurrElem The current Tiny XML element.
      @param tag The identifying tag.
      @param filename Path to the file currently being parsed.
   */
	IterConditional(TI2Element* pCurrElem,const STLString & tag, const STLString & filename) : Variable(pCurrElem,tag,filename) {};

	virtual ~IterConditional(void);

	virtual OME_SCALAR Initialize(const OME_SCALAR time,BaseManager* pBm);
	virtual void BatchInitialize(const OME_SCALAR time,BaseManager* pBm);

	virtual void GetXMLAttributes(StrArray & out) const;
	virtual STLString GetXMLRepresentation(const unsigned int indent=0,const unsigned int inc=4);
	virtual void GetXMLSubNodes(StrArray & out,const unsigned int indent=0,const unsigned int inc=4) { Evaluable::GetXMLSubNodes(out,indent,inc);};
	virtual const OMEChar* GetXMLTag() const { return "iterator"; }

	virtual void Reset(BaseManager* pBm);

	virtual OME_SCALAR Evaluate(const OME_SCALAR time,BaseManager* pBm);

	virtual OMETYPE GetOMEType() const { return OME_ITERCOND; };

	unsigned short GetIterations(BaseManager* pBm);

	/** @return true if the object's condition statement has been statisfied at some point during the simulation run; false otherwise. */
	inline bool GetTriggered() { return m_triggered; }; 
	 /** Assignment operator.
		@param rhs The IterConditional object to use in assignment.
		@return a reference to the IterConditional object.
   */
	IterConditional& operator=(const IterConditional & rhs) { Duplicate(rhs); return *this; };
	/** Equivalence operator.
		@param rhs The IterConditional to compare against.
		@return true if this and rhs are equivalent; false otherwise.
	*/
	bool operator==(const IterConditional & rhs) { return this==&rhs || Variable::operator==(rhs); }

	virtual IterConditional* Clone() { return new IterConditional(*this); }

protected:

	unsigned int m_iterations;				///< The number of iterations that have passed.
	bool m_triggered;						///< true if IterConditional has been evaluated during the present iterations; false otherwise.

	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename);

	void Duplicate(const IterConditional & ic);
};

