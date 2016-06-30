#include "omeobjects/Variable.h"
#include "omeobjects/Influence.h"
#include "DebugOutput.h"
#include "omeobjects/Model.h"
/** Copy the contents of another Variable.
	@param var The Variable to copy.
*/
void Variable::Duplicate(const Variable & var)
{
	m_minValue=var.m_minValue;
	m_maxValue=var.m_maxValue;
	m_minMaxLimit=var.m_minMaxLimit;
	m_expectsFile=var.m_expectsFile;
	m_initOnly=var.m_initOnly;

	Evaluable::Duplicate(var);
	
}

void Variable::BatchInitialize(const OME_SCALAR time, BaseManager* pBm)
{
	//if (!IsInitialized())
	{
		//evaluate should be called by batchInitExpr
		m_mdlIndex = GetModelIndex();
		//try param first, then load
		if (!m_pInitializer || !m_pInitializer->InitializeEval(this))
		{
			if (m_expr.empty() && m_minMaxLimit == HAS_MINMAX)
			{
				//if limits but no arg, choose mid-range value. This is a Simile Behavior
				SetValue((m_maxValue + m_minValue) / 2.0);
				//SetInitialized(true);
			}
			else
			//if (!IsInitialized())
				Evaluable::BatchInitialize(time, pBm);
		}
		//else
			SetInitialized(true);
	}

}

/** Setter for Value.
	Sets value, taking into account any max or min limits. If value is outside of bounds, the value is not set, unless clamp is true; in this case the value is clamped ot the nearest bound.
	@param value new value to set
	@param clamp if true, value is clamp to nearest bound if it lies outside the acceptable range
	@return true if value is in acceptable range, false otherwise.
*/
bool Variable::SetValue(const OME_SCALAR value, bool clamp)
{
	bool ret=true;

	/// \todo renable and modify as needed once the true roles of min/max limitations are understood.
	//OME_SCALAR checkVal=value;
	//ret=CheckBounds(checkVal,clamp);
	//if(ret)

	Evaluable::SetValue(value/*checkVal*/);
	return ret;
}

/**Setter for min value.
	Sets the min value. If min is greater than the max value, the min value is not set and the function returns false.
	@param min the min bound.
	@return true if min is less than or equal to the max value (if available); false otherwise
*/
bool Variable::SetMinValue(OME_SCALAR min)
{
	bool ret=false;
	if((m_minMaxLimit < HAS_MAX) || (min<=m_maxValue))
	{
		ret=true;
		m_minValue=min;

		//if value is NO_LIMIT (0) or HAS_MAX (2)
		//   inc to HAS_MIN (1) or HAS_MINMAX (3)
		if(!(m_minMaxLimit % 2))
			m_minMaxLimit++;
	}
	return ret;
}

/**Setter for max value.
	Sets the max value. If max is less than the min value, the max value is not set and the function returns false.
	@param max the max bound.
	@return true if max is greater than or equal to the min value (if available); false otherwise
*/
bool Variable::SetMaxValue(OME_SCALAR max)
{
	bool ret=false;
	if(!(m_minMaxLimit % 2) || max >= m_minValue)
	{
		ret=true;
		m_maxValue=max;

		//if value is NO_LIMIT (0) or HAS_MIN (1)
		//   inc by 2 to HAS_MAX (2) or HAS_MINMAX (3)
		if(m_minMaxLimit < HAS_MAX)
			m_minMaxLimit+=2;
	}
	return ret;
}

/**Setter for both min and max values.
	Sets the min and max values. If max is less than the min value, neither bound is set and the function returns false.
	@param min the min bound.
	@param max the max bound.
	@return true if max is greater than or equal to the min value; false otherwise
*/
bool Variable::SetMinMaxValue(OME_SCALAR min, OME_SCALAR max)
{
	bool ret= false;
	if(min<=max)
	{
		ret=true;
		m_minValue=min;
		m_maxValue=max;

		m_minMaxLimit= HAS_MINMAX;
	}
	return ret;
}

/**Removes min limit if present. */
void Variable::ClearMin()
{
	//if value is HAS_MIN (1) or HAS_MINMAX (3)
	//   dec to NO_LIMIT (0) or HAS_MIN (2)

	if(m_minMaxLimit % 2)
		m_minMaxLimit--;
}

/**Removes max limit if present. */
void Variable::ClearMax()
{
	//if value is HAS_MAX (2) or HAS_MINMAX (3)
	//   dec by 2 to NO_LIMIT (0) or HAS_MIN (1)
	if(m_minMaxLimit >= HAS_MAX)
		m_minMaxLimit-=2;
}

/**Removes all present limits. */
void Variable::ClearMinMax()
{
	m_minMaxLimit=NO_LIMIT;
}

STLString Variable::GetXMLRepresentation(const unsigned int indent,const unsigned int inc)
{
	return GetXMLForObject(indent,inc);
}

void Variable::GetXMLAttributes(StrArray & out) const
{
	//function attrs
	Evaluable::GetXMLAttributes(out);
	
	//variable attrs
  
	if(m_minMaxLimit % 2)
	  AddAttributeValue("min_value",m_minValue,out);

	if(m_minMaxLimit>=HAS_MAX)
	   AddAttributeValue("max_value",m_maxValue,out);

	if(m_expectsFile)
	   AddAttributeValue("external_init",m_expectsFile,out);
	if(m_initOnly)
		AddAttributeValue("init_only",m_initOnly,out);
}

void Variable::GetXMLSubNodes(StrArray & out,const unsigned int indent,const unsigned int inc)
{
	Evaluable::GetXMLSubNodes(out,indent,inc);
}

void Variable::GetXMLAttributeMap(XMLAttrVector & out)
{
	
	 XmlAttr attrs[] = {
	  // attr				type		   address                          isReq
	  { "min_value",		XML_SCALAR,	&(this->m_minValue),			false },
	  { "max_value",		XML_SCALAR,	&(this->m_maxValue),			false },
	  { "external_init",	XML_BOOL,	&(this->m_expectsFile),		false },
	  { "init_only",		XML_BOOL,   &(this->m_initOnly),		false },
	  { NULL,				XML_NULL,		NULL,							false } };
	  
	  //drop null
	 int count;
	 for(count=0; attrs[count].name!=NULL; count++)
		  out.push_back(attrs[count]);

	Evaluable::GetXMLAttributeMap(out);
}

int Variable::PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
{
		  //set minMaxLimit
	  m_minMaxLimit=NO_LIMIT;

	  double dummy;
	  if(pCurrElem->QueryDoubleAttribute("min_value",&dummy) == tinyxml2::XML_SUCCESS)
		  m_minMaxLimit++;
	  if(pCurrElem->QueryDoubleAttribute("max_value",&dummy) == tinyxml2::XML_SUCCESS)
		  m_minMaxLimit+=2;

	return Evaluable::PopulateFromComplexElements(pCurrElem,tag,filename);

}



void Variable::Reset(BaseManager* pBm)
{
	Evaluable::Reset(pBm);


}


/** Walk influences and other variables, looking for a loop.
	@param start the starting point for the walk, and the variable to look for in all subsequent steps
	@return true if no loop is found, false otherwise
*/
bool Variable::WalkInfluences(Variable *start)
{
	Influence* infl;
	Variable* var;
	bool ret=true;

	for(int i=0; i< m_upObjects.size() && ret; i++)
	{
		infl=dynamic_cast<Influence*>(m_upObjects[i]);
		if(infl)
		{
			var=dynamic_cast<Variable*>(infl->GetPEvalSource());
			if(var)
			{
				if(var!=start)
					ret=var->WalkInfluences(start);
				else
					ret=false;
			}
		}
	}
	return ret;
}

/** Equivalence operator.
	@param var The variable to compare against.
	@return true if this and var contain equivalent values; false otherwise.
*/
bool Variable::operator==(const Variable & var)
{
	return this==&var ||
		(
		Evaluable::operator==(var) &&
		m_minValue==var.m_minValue &&
		m_maxValue==var.m_maxValue &&
		m_minMaxLimit==var.m_minMaxLimit
		);
}

void Variable::SetInitOnly(const bool & io)
{
	m_initOnly = GetIsConstant() ? true : io; 
}

void Variable::CheckConstant()
{
	Evaluable::CheckConstant();
	if (GetIsConstant())
		m_initOnly=true;
}