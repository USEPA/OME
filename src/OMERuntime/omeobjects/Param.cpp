#include "omeobjects/Param.h"
#include "OMEParser.h"
#include "omeobjects/Evaluable.h"
#include "omeobjects/TimeSeriesVar.h"
#include "SDPMapEntry.h"
#include "omeobjects/Model.h"

/** Equality comparison operator.
	@param parm The Param to compare against.
	@return true if the Param objects are equivalent; false otherwise.
*/
bool Param::operator==(const Param & parm) const
{
	bool ret = m_isPublic == parm.m_isPublic &&
		m_targetName == parm.m_targetName &&
		m_paramType == parm.m_paramType;

	if (ret)
	{
		switch (m_paramType)
		{
		case PARAM_CONST:
			ret = m_value == parm.m_value;
			break;
		case PARAM_EXPR:
			ret = m_expr == parm.m_expr;
			break;
		case PARAM_SPATIAL:
			ret = m_pME == parm.m_pME;
			break;
		}

	}
	return ret;
}

/** Copies values from another param.
	All Values are copied except for m_validated and up and down object arrays.
	m_name and m_id are append with underscores to maintain uniqueness (is there a better way to handle this?).
	@param parm the object to be copied.
*/
void Param::Duplicate(const Param & parm)
{
	//copy fields
	m_value=parm.m_value;
	m_isPublic=parm.m_isPublic;
	m_paramType=parm.m_paramType;

}

/** Set the Value of this Param.
	This operation will fail if the Param is not of the NULL or CONST type,
	unless oride is set to true;
	@param val The value to assign.
	@param oride If true, The previous type flag is overridden.
	@return true If the assignment was successful; false otherwise.
*/
bool Param::SetConstValue(const OME_SCALAR & val, const bool & oride)
{
	bool ret=false;
	if(oride || m_paramType==PARAM_NULL || m_paramType==PARAM_CONST)
	{
		ClearParam();
		m_value=val;
		m_paramType=PARAM_CONST;
		ret=true;
	}
	return ret;
}

/** Set the expression stored in this Param.
	This operation will fail if the Param is not of the NULL or EXPR type,
	unless oride is set to true;
	@param expr string containing expression to assign.
	@param oride If true, The previous type flag is overridden.
	@return true If the assignment was successful; false otherwise.
*/
bool Param::SetExpression(const STLString & expr, const bool & oride)
{
	bool ret=false;
	if(oride || m_paramType==PARAM_NULL || m_paramType==PARAM_EXPR)
	{
		ClearParam();
		m_expr=new char[expr.length()];
		SAFE_STRCPY(expr.c_str(),m_expr,expr.length());
		m_paramType=PARAM_EXPR;
		ret=true;
	}
	return ret;
}


/** Retrieve stored expression.
	This will only be successful if the Param is of type POINT.
	@param expr Reference to a string object to store the expression in.
	@return true if the value was successfully retrieved, false otherwise.
*/
bool Param::GetExpression(STLString & expr)
{
	bool ret=false;
	if(m_paramType==PARAM_EXPR)
	{
		expr=m_expr;
		ret=true;
	}
	return ret;
}

/** Clear the contents of the Param and set the type to NULL.
	Currently, the DATA type _will not_ dealloc the internal data pointer,
	potentially creating a memory leak if the object is not being tracked elsewhere.
*/
void Param::ClearParam()
{
	switch(m_paramType)
	{
	case PARAM_CONST:
		m_value=0.0;
		break;
	case PARAM_EXPR:
		delete[] m_expr;
		m_expr=NULL;
		break;
	case PARAM_SPATIAL:
		m_pME = NULL;
		break;
    default:
        break;
//	case PARAM_DATA:
		//for now, not responsible for allocation
//		m_dataProvider=NULL;
	};

	m_paramType=PARAM_NULL;
}

/** Use a Param object to initialize an Evaluable object.
	@param pEval Pointer to the Evaluable to initialize.
	@return true if the initialization was successful, false otherwise.
*/
bool Param::InitializeEval(Evaluable* pEval)
{
	bool ret = true;
	OME_SCALAR tmp;
	switch (m_paramType)
	{
	case PARAM_CONST:
		if (pEval->GetOMEType() != OME_TSVAR)
			pEval->SetValue(m_value);
		else
			((TimeSeriesVar*)pEval)->SetFixed(m_value);
		break;
	case PARAM_DATA:
		ret=pEval->GetParentModel()->InitializeEvalFromDataSource(pEval);
		
		break;
	case PARAM_SPATIAL:

		//JIT initialization.
		m_pME->ApplyEntryForCurrInstEval(SDPMapSubEntry::INIT_COMPONENT, pEval);
		break;
	default:
		ret = false;
	}
	return ret;
}

