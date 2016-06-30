#include "omeobjects/Influence.h"
#include "DebugOutput.h"
/** Concantenation operator.
	Replaces target info and role with that of rhs; averages control points.
	@param rhs right hand side argument
*/
Influence & Influence::operator+=(const Influence & rhs)
{
	//influence specific here

	//perform parent +=
	((Link)*this)+=(Link)rhs;

	return *this;
}

/** Setter for target pointer (Evaluable Object).
	@param trg new target pointer.
*/
void Influence::SetPTarget(OMEObject* trg)
{
	Evaluable* evalCast=dynamic_cast<Evaluable*>(m_pTarget);
	if(evalCast)
		this->RemoveDownObject(evalCast);
	Link::SetPTarget(trg);
	evalCast=dynamic_cast<Evaluable*>(trg);
	if(evalCast)
		evalCast->AddUpObject(this);
		//evalCast->GetOMEType()!=OME_SUBPORT ? evalCast->AddUpObject(this) : this->AddDownObject(evalCast);
}

/** Setter for source pointer (Evaluable Object).
	@param src new source pointer.
*/
void Influence::SetPSource(OMEObject* src)
{
	Evaluable* evalCast=dynamic_cast<Evaluable*>(m_pSource);
	if(evalCast)
		this->RemoveUpObject(evalCast);
	Link::SetPSource(src);
	evalCast=dynamic_cast<Evaluable*>(src);
	if(evalCast)
		evalCast->AddDownObject(this);
		//evalCast->GetOMEType()!=OME_SUBPORT ? evalCast->AddDownObject(this) : this->AddUpObject(evalCast);
}

STLString Influence::GetXMLRepresentation(const unsigned int indent, const unsigned int inc)
{
	return GetXMLForObject(indent,inc);
}

void Influence::GetXMLAttributes(StrArray & out) const
{
	//function attrs
	Evaluable::GetXMLAttributes(out);
	
	//influence attrs
   OMEObject::AddAttributeValue("use_curr_val",m_useCurrentVal,out);

	//link attrs
	Link::GetXMLAttributes(out);
	
}


void Influence::GetXMLAttributeMap(XMLAttrVector & out)
{
	XmlAttr attrs[] = {
	// attr				type		         address		                isReq
	{ "use_curr_val",	XML_BOOL,		   &(this->m_useCurrentVal),	false },
	{ NULL,				XML_NULL,		   NULL,							   false } };

	//drop null
	 int count=0;
	 for(; attrs[count].name!=NULL;count++)
		  out.push_back(attrs[count]);

	Evaluable::GetXMLAttributeMap(out);
	Link::GetXMLAttributeMap(out);	
}

int Influence::PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
{
	return Evaluable::PopulateFromComplexElements(pCurrElem,tag,filename) &&
		Link::PopulateFromComplexElements(pCurrElem,tag,filename);
}

void Influence::GetXMLSubNodes(StrArray & out,const unsigned int indent,const unsigned int inc)
{
	Evaluable::GetXMLSubNodes(out,indent,inc);
	Link::GetXMLSubNodes(out,indent,inc);
}


/** Copies values from another influence.
	All Values are copied except up and down object arrays.
	m_name and m_id are append with underscores to maintain uniqueness (is there a better way to handle this?).
	@param infl the influence to be copied.
*/
void Influence::Duplicate(const Influence & infl)
{
	//copy local fields
	m_useCurrentVal=infl.m_useCurrentVal;
	//dup parents
	Evaluable::Duplicate(infl);
	Link::Duplicate(infl);
}

OME_SCALAR Influence::Evaluate(const OME_SCALAR time,BaseManager* pBm)
{
	return m_pSource ? GetPEvalSource()->GetValue() : 0.0;
}

void Influence::Reset(BaseManager* pBm)
{
	Evaluable::Reset(pBm);
}

unsigned int Influence::Prioritize(unsigned int currPriority, const bool isInitial)
{
	//influences are parsed differently if initial priority
	if(m_pSource && m_pTarget)
	{
		(isInitial ? m_initPriority : m_evalPriority)=currPriority;
		Evaluable* evalCast;
		
		bool usingTarget=(!isInitial && m_useCurrentVal);
		evalCast=( usingTarget ? GetPEvalTarget() : GetPEvalSource());
		//if stepping back to iterconditional/alarm, stop! it could create an infinite loop!
		if(usingTarget || evalCast->GetOMEType()!= OME_ITERCOND)
			return evalCast->Prioritize(currPriority+1,isInitial);
	}
	return isInitial ? m_initPriority : m_evalPriority;
}

OME_SCALAR Influence::GetValue() const
{
	Evaluable* evalCast=(Evaluable*)m_pSource;
	return evalCast->GetValue();
}

/** Equivalence operator.
	@param rhs The influence to compare against.
	@return true if this and rhs are equivalent; false otherwise.
*/
bool Influence::operator==(const Influence & rhs)
{
	return this==&rhs ||
		(
		Link::operator==(rhs) &&
		Evaluable::operator==(rhs)
		);
}

void Influence::Detach()
{
	Evaluable::Detach();
	SetPTarget(NULL);
	SetPSource(NULL);
}