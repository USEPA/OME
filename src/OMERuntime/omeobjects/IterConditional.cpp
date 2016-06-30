#include "omeobjects/Model.h"
#include "omeobjects/IterConditional.h"
#include "BaseManager.h"

IterConditional::~IterConditional(void)
{
}

/** Copy the contents of another IterConditional.
	@param ic The IterConditional to copy.
*/
void IterConditional::Duplicate(const IterConditional & ic)
{
	m_iterations=ic.m_iterations;
	m_triggered=ic.m_triggered;
	Variable::Duplicate(ic);
}

OME_SCALAR IterConditional::Initialize(const OME_SCALAR time,BaseManager* pBm)
{
	if(!IsInitialized())
	{
		Reset(pBm);
		m_mdlIndex=GetModelIndex();
		Evaluate(time,pBm);
		SetInitialized(true);
	}
	return Evaluable::GetValue();
}

void IterConditional::BatchInitialize(const OME_SCALAR time,BaseManager* pBm)
{
	if(!IsInitialized())
	{
		//Reset(pBm);
		//Evaluate and reset called in BatchInitExpr
		m_mdlIndex=GetModelIndex();
		SetInitialized(true);
	}
}


void IterConditional::GetXMLAttributes(StrArray & out) const
{
	//function attrs
	Variable::GetXMLAttributes(out);

	//nothing unique at the moment
}

STLString IterConditional::GetXMLRepresentation(const unsigned int indent,const unsigned int inc)
{
	/// \todo come up with a better name for this class than "iterator"
	return GetXMLForObject(indent,inc);
}

void IterConditional::Reset(BaseManager* pBm)
{
	m_iterations=0;
	m_triggered=false;
}

OME_SCALAR IterConditional::Evaluate(const OME_SCALAR time,BaseManager* pBm)
{
	if(!m_triggered)
	{
		m_iterations=((Model*)m_pParentModel)->GetIterationCount();
		if(Variable::Evaluate(time,pBm))
			m_triggered=true;
	}		

	return Variable::GetValue();
}

void IterConditional::GetXMLAttributeMap(XMLAttrVector & out)
{
	//currently no additional attributes
	Variable::GetXMLAttributeMap(out);
}

int IterConditional::PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
{
	return Variable::PopulateFromComplexElements(pCurrElem,tag,filename);

}

/** @return the number of iterations up until the Conditional Evaluated to true; otherwise, return current iteration count. */
unsigned short IterConditional::GetIterations(BaseManager* pBm) 
{ 
	Evaluate(pBm->GetPresentTime(),pBm); return m_iterations; 
};