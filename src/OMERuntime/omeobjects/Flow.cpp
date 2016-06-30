#include "omeobjects/Flow.h"
#include "omeobjects/Influence.h"
#include "DebugOutput.h"
#include "omeobjects/StateVar.h"
#include "omeobjects/SubmodelPort.h"

/** Concantenation operator.
	Replaces target info and role with that of rhs; averages control points.
	@param rhs right hand side argument
*/
Flow& Flow::operator+=(const Flow & rhs)
{
	//flow specific here
	//function info comes from source (lhs); no changes for function necessary

	//perform parent +=
	((Link)*this)+=(Link)rhs;
	return *this;
}

//Flow operator+ (const Flow & lhs, const Flow & rhs)
//{
//	Flow out=Flow((lhs.m_name+"-"+rhs.m_name),lhs.m_source,rhs.m_target,lhs.m_pSource,rhs.m_pTarget);
//	
//	//inherit Evaluable parts from LHS
//	out.m_expr=lhs.m_expr;
//	out.m_value=lhs.m_value;
//
//	//for now, just average control points
//	out.m_ctrlPoint0.x=(lhs.m_ctrlPoint0.x+rhs.m_ctrlPoint0.x)/2;
//	out.m_ctrlPoint0.y=(lhs.m_ctrlPoint0.y+rhs.m_ctrlPoint0.y)/2;
//
//	out.m_ctrlPoint1.x=(lhs.m_ctrlPoint1.x+rhs.m_ctrlPoint1.x)/2;
//	out.m_ctrlPoint1.y=(lhs.m_ctrlPoint1.y+rhs.m_ctrlPoint1.y)/2;
//
//	//boundary check
//	out.m_crossesModelBoundary=Link::HasCommonRoot(out.m_pSource, out.m_pTarget);
//		
//	return out;
//}


/** Setter for target pointer (Evaluable Object).
	@param trg new target pointer.
*/
void Flow::SetPTarget(OMEObject* trg)
{
	Link::SetPTarget(trg);
	if (trg)
	{
		if (trg->IsOMEType(OME_STATEVAR))
			((StateVar*)trg)->AddInput(this);
		else if (trg->GetOMEType() == OME_SUBPORT)
		{
			if (m_pParentModel)
			{
				((SubmodelPort*)trg)->GetParentModel() == m_pParentModel ? ((SubmodelPort*)trg)->AddSubmodelSource(this) : ((SubmodelPort*)trg)->AddOutmodelSource(this);

			}
		}
	}

	//Evaluable* evalCast=dynamic_cast<Evaluable*>(trg);
	//if(evalCast)
	//	evalCast->AddUpObject(this);
}

/** Setter for source pointer (Evaluable Object).
	@param src new source pointer.
*/
void Flow::SetPSource(OMEObject* src)
{
	Link::SetPSource(src);
	if (src)
	{
		if (src->IsOMEType(OME_STATEVAR))
			((StateVar*)src)->AddOutput(this);
		else if (src->GetOMEType() == OME_SUBPORT)
		{
			if (m_pParentModel)
			{
				((SubmodelPort*)src)->GetParentModel() == m_pParentModel ? ((SubmodelPort*)src)->AddSubmodelTarget(this) : ((SubmodelPort*)src)->AddOutmodelTarget(this);
			}
			m_flowObj = (Evaluable*)src;
		}
	}
	
	//Evaluable* evalCast=dynamic_cast<Evaluable*>(src);
	//if(evalCast)
	//	evalCast->AddDownObject(this);
}

STLString Flow::GetXMLRepresentation(unsigned int indent,unsigned int inc)
{
	return GetXMLForObject(indent,inc);
}

void Flow::GetXMLAttributes(StrArray & out) const
{
	//function attrs
	Evaluable::GetXMLAttributes(out);
	
	//flow attrs
	
	//link attrs
	Link::GetXMLAttributes(out);
	

}

void Flow::GetXMLAttributeMap(XMLAttrVector & out)
{
	Evaluable::GetXMLAttributeMap(out);
	Link::GetXMLAttributeMap(out);
}

int Flow::PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
{
	return Evaluable::PopulateFromComplexElements(pCurrElem,tag,filename) &&
		Link::PopulateFromComplexElements(pCurrElem,tag,filename);
}

void Flow::GetXMLSubNodes(StrArray & out,const unsigned int indent,const unsigned int inc)
{
	Evaluable::GetXMLSubNodes(out,indent,inc);
	Link::GetXMLSubNodes(out,indent,inc);
}

/** Copies values from another Flow.
	All Values are copied except up and down object arrays.
	m_name and m_id are append with underscores to maintain uniqueness (is there a better way to handle this?).
	@param flo the flow to be copied.
*/
void Flow::Duplicate(const Flow & flo)
{
	//duplicate parents
	Evaluable::Duplicate(flo);
	Link::Duplicate(flo);
	m_flowObj = flo.m_flowObj;
}

void Flow::Reset(BaseManager* pBm)
{
	Evaluable::Reset(pBm);
}

unsigned int Flow::Prioritize(unsigned int currPriority, const bool isInitial)
{
	unsigned int maxPrior= Evaluable::Prioritize(currPriority, isInitial);

	Evaluable* evalCast=dynamic_cast<Evaluable*>(m_pSource);
	if(evalCast && evalCast->GetOMEType()!=OME_STATEVAR)
	{
		unsigned int srcPrior=evalCast->Prioritize(currPriority+1,isInitial);
		maxPrior= srcPrior > maxPrior ? srcPrior : maxPrior;
	}

	return maxPrior;
}

/** Equivalence operator. 
	@param rhs The Flow object to compare against.
	@return true if this and rhs contain equivalent values.
*/
bool Flow::operator==(const Flow & rhs)
{
	return this==&rhs ||
		(
		Link::operator==(rhs) &&
		Evaluable::operator==(rhs)
		);
}

void Flow::Detach()
{
	//detach from state vars
	/*if(m_pSource && m_pSource->GetOMEType()==OME_STATEVAR)
		((StateVar*)m_pSource)->RemoveOutput(this);
	if(m_pTarget && m_pTarget->GetOMEType()==OME_STATEVAR)
		((StateVar*)m_pTarget)->RemoveInput(this);
*/
	Evaluable::Detach();
}

OME_SCALAR Flow::GetValue() const
{
	return !m_flowObj ? Evaluable::GetValue() : m_flowObj->GetValue();
}

OME_SCALAR Flow::GetValue(const size_t & inst) const
{
	return !m_flowObj ? Evaluable::GetValue(inst) : m_flowObj->GetValue(inst);
}