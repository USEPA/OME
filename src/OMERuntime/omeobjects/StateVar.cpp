#include "omeobjects/StateVar.h"
#include "omeobjects/Flow.h"
#include "DebugOutput.h"

STLString StateVar::GetXMLRepresentation(const unsigned int indent, const unsigned int inc)
{
	return GetXMLForObject(indent,inc);
}

void StateVar::GetXMLAttributes(StrArray & out) const
{
	//function attrs
	Evaluable::GetXMLAttributes(out);
	
	//stateVar attrs
}

void StateVar::GetXMLSubNodes(StrArray & out,const unsigned int indent,const unsigned int inc)
{
	Evaluable::GetXMLSubNodes(out,indent,inc);
}

void StateVar::GetXMLAttributeMap(XMLAttrVector & out)
{
		XmlAttr attrs[] = {
		// attr				type		   address                          isReq
		
		{ NULL,				XML_NULL,		NULL,							false } };

		//drop null
	 int count;
	 for(count=0; attrs[count].name!=NULL; count++)
		  out.push_back(attrs[count]);

	Evaluable::GetXMLAttributeMap(out);
}

int StateVar::PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
{
	int ret= Evaluable::PopulateFromComplexElements(pCurrElem,tag,filename);

	return ret;

}

/** Copy another StateVar object
	@param sv the StateVar to copy
*/
void StateVar::Duplicate(const StateVar & sv)
{

	Evaluable::Duplicate(sv);
}

// /** Returns the derivatives associated with the state variable.
//	The derivative is calculated by adding the values of inbound flows
//	and subtracting the values of outbound flows.
//	@return The calculated derivative.
//*/
//OME_SCALAR StateVar::GetDerivative()
//{
//	int i=0;
//	OME_SCALAR out=0.0;
//
//	//find derivative
//
//	//add inputs
//	for(i=0; i< m_upObjects.size();i++)
//	{	
//		if(m_upObjects[i]->GetOMEType()==OME_FLOW)
//			out+=m_upObjects[i]->GetValue();
//	}
//
//	//subtract outputs
//	for(i=0; i< m_downObjects.size();i++)
//	{
//		if(m_downObjects[i]->GetOMEType()==OME_FLOW)
//			out-=m_downObjects[i]->GetValue();
//	}
//
//
//	return out;
//}

void StateVar::Reset(BaseManager* pBm) 
{ 
	Evaluable::Reset(pBm); 

	//reset to initial condition (may be overridden by equation in Validate())
}


unsigned int StateVar::Prioritize(unsigned int currPriority, const bool isInitial)
{
	//unsigned int maxPriority=0, tempPriority;
	unsigned int compPriority;// = isInitial ? m_initPriority : m_evalPriority;

	
	//StateVars are updated separately, so they have a minimum priority, unless isInitial.
	if (isInitial)
	{
		compPriority = Evaluable::Prioritize(currPriority, isInitial);
	}
	else
		compPriority = 0;

	return compPriority;
}


/** Comparison operator.
	@param sv The StateVar to compare values against.
	@return true If StateVar values are equal; false otherwise.
*/
bool StateVar::operator==(const StateVar & sv)
{
	return this==&sv ||
		(
		Evaluable::operator==(sv) 
		);
}

/** Remove an integration input.
	@param pFlow Pointer to outbound flow to remove.
*/
void StateVar::RemoveInput(Flow* pFlow)
{
	auto fItr=std::find(m_inputs.begin(),m_inputs.end(),pFlow);
	if(fItr!=m_inputs.end())
		m_inputs.erase(fItr);
}

/** Remove an integration output.
	@param pFlow Pointer to outbound flow to remove.
*/
void StateVar::RemoveOutput(Flow* pFlow)
{
	auto fItr=std::find(m_outputs.begin(),m_outputs.end(),pFlow);
	if(fItr!=m_outputs.end())
		m_outputs.erase(fItr);
}

void StateVar::Detach()
{
	std::for_each(m_inputs.begin(),m_inputs.end(),[](Evaluable* pVal){ ((Flow*)pVal)->SetPTarget(NULL);});
	std::for_each(m_outputs.begin(),m_outputs.end(),[](Evaluable* pVal){ ((Flow*)pVal)->SetPSource(NULL);});
	Evaluable::Detach();
}

/** Add an integration input.
	@param pFlow Pointer to inbound flow to add.
*/
void StateVar::AddInput(Flow* pFlow) 
{ 
	m_inputs.push_back(pFlow); 
}

/** Add an integration output.
	@param pFlow Pointer to outbound flow to add.
*/
void StateVar::AddOutput(Flow* pFlow) 
{
	m_outputs.push_back(pFlow); 
}


/** Update Flows based on StateVar state.
	@param inst The instance affected.
*/
void StateVar::AdjustFlows(const size_t & inst)
{
	//Give subclasses the opportunity to modify their flows before integration.
	//regular state vars will not make adjustments.
}