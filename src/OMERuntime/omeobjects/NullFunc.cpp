#include "omeobjects/NullFunc.h"
#include "SMLHandlers/pltokens/PLToken.h"
#include "omeobjects/Evaluable.h"
#include "omeobjects/Influence.h"
#include "omeobjects/EvalInterpTable.h"
#include <regex>
#include <string>

/** Copy values from another NullFunc
	@param nf the NullFunc to copy
*/
void NullFunc::Duplicate(const NullFunc & nf)
{
	m_spec=nf.m_spec;
	m_tData=nf.m_tData;	
	m_expr=nf.m_expr;
	m_inArray.assign(nf.m_inArray.begin(),nf.m_inArray.end());
	m_out=nf.m_out;
	m_complete=nf.m_complete;
	m_isArrayFunc=nf.m_isArrayFunc;
	m_isConverted=nf.m_isConverted;
//	m_ghost=nf.m_ghost;

	OMENull::Duplicate(nf);
}

/** Add an influence to the NullFunc object's source collection.
	@param pInfl pointer to the desired Influence.
*/
void NullFunc::AddInfluence(Influence* pInfl)
{
	m_inArray.push_back(pInfl);

	//if(m_ghost)
	//	m_ghost->AddInLink(pInfl);

	if(m_out)
		pInfl->SetPTarget(m_out);
}

/** Add influence that points away from a function. This occurs under Simile 6.x for Flow funcs.
	@param pInf Pointer to Influence to add to the outbound list.
*/
void NullFunc::AddOutInfluence(Influence* pInf)
{
	m_outArray.push_back(pInf);

	if (m_out)
	{
		if (m_out->GetOMEType() == OME_FLOW)
			pInf->SetPSource(m_out);
		else
			throw OMEFuncOpException("Influence::AddOutInfluence","Function Influence not pointing away from Flow.");
	}
}

/** Set object that will eventually contain the function expression.
	@param pEval Pointer to target object.
*/
void NullFunc::SetOutObject(Evaluable* pEval)
{
	m_out=pEval;

	m_isArrayFunc= IsAppliedToArray();
	//const OMEChar* patterns[] = {"^\\[.*\\]$",
	//									"^\\{.*\\}$",
	//									"\0"};
	//if(m_isArrayFunc && BatchRegexMatchAny(m_expr,patterns,0))
	//	m_expr=m_expr.substr(1,m_expr.size()-2);

	if(!m_isConverted)
	{
		m_expr=PLToken::ConvertExpression(m_expr,NULL);
		m_isConverted=true;
	}

	for (auto itr = m_inArray.begin(); itr != m_inArray.end(); ++itr)
	{	
		(*itr)->SetPTarget(m_out);
	}
	if (pEval->GetOMEType() == OME_FLOW)
	{
		for (auto itr = m_outArray.begin(); itr != m_outArray.end(); ++itr)
		{
			(*itr)->SetPSource(m_out);
		}
	}
	m_out->SetExpr(m_expr);

	if (m_tData.dims[0] != -1)
		m_out->SetPEvalTable(GenerateEvalTable(),0);

	if (m_unitStr.find("int") == STLString::npos)
		m_out->SetAsInt(false);
}

/** @return Pointer to function's out object.*/
Evaluable* NullFunc::GetOutObject()
{
	return m_out;
}

/** Retrieve a specific Influence that is pointing at the NullFunc.
	@param i Index of Influence to retrieve.
	@return Pointer to Influence at i, or NULL if i is not a valid index.
*/
Influence* NullFunc::GetInfluence(const size_t i)
{
	Influence* ret=NULL;
	if(i<m_inArray.size())
		ret=m_inArray[i];

	return ret;
}

/** @return the number of influence who currently target this function. */
size_t NullFunc::GetInfCount() const
{
	return m_inArray.size();
}

/** @param expr Expression to assign to NullFunc.
*/
void NullFunc::SetExpr(const STLString & expr)
{

	m_expr=expr;
	if(m_out)
	{
		m_out->SetExpr(m_expr);
	}
}

/** @return Expression represented by NullFunc object.*/
STLString NullFunc::GetExpr() const
{
	return m_expr;
}

/** Set Table data for function.
	@param f Path to source file.
	@param dl Columns to access for data.
	@param il List of indices for access.
	@param c Last loaded values from file at f.
	@param d Pointer to a size 2 array containing table dimensions.
	@param u Units used by table.
*/
void NullFunc::SetTableData(const STLString & f, const StrArray & dl,const StrArray & il,const EvalTable::DataArray & c,const int* d, const STLString & u)
{
	m_tData.file=f;
	m_tData.data=dl;
	m_tData.indices=il;
	m_tData.current=c;
	m_tData.dims[0]=d[0];
	m_tData.dims[1]=d[1];
	m_tData.units=u;
}

/** Set Table data for function.
	@param data tableData object to use during assignment.
*/
void NullFunc::SetTableData(const NullFunc::TableData & data)
{
	m_tData=data;
}

/** 
@return tableData object equal to the internally stored tabledata object.
*/
NullFunc::TableData NullFunc::GetTableData() const
{
	return m_tData;
}

/** @param spec the spec to assign to this function.*/
void NullFunc::SetSpec(const STLString & spec)
{
	m_spec=spec;
}

/** @return internal spec currently assigned.*/
STLString NullFunc::GetSpec() const
{
	return m_spec;
}

/** @param comp If true, function is marked as "complete".*/
void NullFunc::SetComplete(const bool comp)
{
	m_complete=comp;
}

/**
	@return true if NullFunc is "complete", false otherwise.
*/
bool NullFunc::GetComplete() const
{
	return m_complete;
}

/** @return true If NullFunc is an array function; false otherwise.*/
bool NullFunc::GetIsArrayFunction() const
{
	return m_isArrayFunc;
}

/** Generate an EvalTable from internally stored tableData.
	@return Pointer to newly allocated EvalTable representing data stored in internal tableData.
*/
EvalTable* NullFunc::GenerateEvalTable() const
{

	return NullFunc::GenerateEvalTable(m_tData);
}

/** Generate an EvalTable from tableData.
	@param data The tableData object used to generate a new EvalTable.
	@return Pointer to newly allocated EvalTable representing data stored in data.
*/
EvalTable* NullFunc::GenerateEvalTable(TableData data)
{
	static unsigned int valNum=0;
	EvalTable* ret=NULL;
	
	
	//extract a subset of SIMILE table data; other pieces can be incorporated as needed.
 	IndVec dims;
	for(unsigned int i=0; i<2; i++)
	{
		if(data.dims[i]>=0)
			dims.push_back(data.dims[i]);
	}

	//generate unique id; all ids for tables should be derived from here in order to ensure uniqueness
	oSTLSStream id;
	id<<"table_"<<valNum++;

	if (!data.file.empty())
	{
		//is table generated for table() or graph() call?
		if (data.file != "/graph/")
		{
			ret = new EvalTable(id.str(), dims, data.current);
			//grab data loading stuff, if any
			ret->SetColumnName(data.colName);

			ret->SetFilePath(data.file);
		}
		else
		{
			//data. current values need to be transformed from how simile stores the values in .pls
			/* 
				value= (transform - currVal)/(transform/valCount) + ymin
				
				where:
					value = new value for data.current[x]
					transform = value at data.indices[2]
					currVal = original value at data.current[x]
					ymax = value at data.data[1]
					ymin = value at data.data[0]
					valCount= data.current.size() - 1 (to exclude the 0th value)
			*/
			//OME_SCALAR ymax = OME_SCLR_READSTR(data.data[0]);
			OME_SCALAR ymin = OME_SCLR_READSTR(data.data[1]);
			OME_SCALAR trans = OME_SCLR_READSTR(data.indices[2]);
			OME_SCALAR denom = trans/ (data.current.size()-1);
			for (auto itr = data.current.begin(); itr != data.current.end(); ++itr)
				*itr = (trans - *itr) / denom + ymin;
			EvalInterpTable* pIT = new EvalInterpTable(id.str(), data.current);

			//grab bounds;
			pIT->SetBounds(OME_SCLR_READSTR(data.indices[0]), OME_SCLR_READSTR(data.indices[1]));
			//apply modifiers to find actual values (requires bounds to be set)
			//pIT->NormalizeValsWithDenom(OME_SCLR_READSTR(data.indices[2]));
			
			ret = pIT;

			/*Flag values from SIMILE (determined imperically:
				0 = Interpolate, Truncate
				1 = Interpolate, Extrapolate
				2 = Interpolate, Wraparound
				3 = N/A
				4 = Round, Truncate
				5 = Round, Extrapolate
				6 = Round, Wraparound
			*/
			//get interp flag
			int flag = stoi(data.indices[3]);
			pIT->SetInterpMode(flag < 3 ? EvalInterpTable::INTERP : EvalInterpTable::ROUND);

			//get border flag
			switch (flag)
			{
			case 0:
			case 4:
				pIT->SetBoundMode(EvalInterpTable::CLAMP);
				break;
			case 1:
			case 5:
				pIT->SetBoundMode(EvalInterpTable::EXTRAP);
				break;
			case 2:
			case 6:
				pIT->SetBoundMode(EvalInterpTable::WRAP);
				break;
			}
		}
	}

	return ret;
}

/** Determine if a NullFunc instance is intended to be applied to an array.
	@return true if NullFunc expression is intended to be applied to an array object; false otherwise.
*/	
bool NullFunc::IsAppliedToArray()
{
	bool ret=false;
	static const STLRegex reg("^array\\(.*\\)$");
	STLsmatch match;
	if(std::regex_match(m_unitStr,match,reg))
		ret=true;
	//if(!m_expr.empty())
	//{

	//	//first, find index of leftmost bracket
	//	size_t i;
	//	for(i=0; i<m_expr.size() && m_expr[i]!='[' && m_expr[i]!='{'; i++);

	//	if(i==0)
	//		ret=true;
	//	else if(i<m_expr.size())
	//	{
	//		//then determine if the statement is returning from a function that provides an array
	//		static const OMEChar* patterns[]={"^[-+/*^0-9\\s]*makeArray\\(.*$",
	//										"^[-+/*^0-9\\s]*rankings\\(.*$",
	//										"^[-+/*^0-9\\s]*subtotals\\(.*$",
	//										"^[-+/*^0-9\\s]*ordinals\\(.*$",
	//										"^[-+/*^0-9\\s]*product3\\(.*$",
	//										"^[-+/*^0-9\\s]*transform3\\(.*$",
	//										"\0"};
	//		if(BatchRegexMatchAny(m_expr.substr(0,i),patterns,0))
	//			ret=true;
	//	}
	//}
	return ret;
}

/** Determine array dimensions from a prolog units string.
	@param units The string to parse for array dimension information.
	@param out Indvec which will contain resulting dimensions on success.
	@param mdls List of models for resolving enumerated values
	@return true if dimensions were found; false otherwise.
*/
bool NullFunc::FindArrayDims(const STLString & units, IndVec & out,std::list<Model*> & mdls)
{
	bool ret=false;

	const STLRegex dimReg("array\\((.*),(.*)\\)");
	STLsmatch match;
	if(std::regex_match(units,match,dimReg))
	{
		size_t ind;
		FindArrayDims(match[1],out,mdls);
		
		STLString dimStr = match[2].str();
		iSTLSStream inStrm(dimStr);
		if (inStrm >> ind)
		{
			out.push_back(ind);
			ret = true;
		}
		else
		{
			const STLRegex enumReg("^\\s*['\"]+(.*)['\"]+\\s*$");
			STLsmatch eMatch;
			if (std::regex_match(dimStr, eMatch, enumReg))
			{
				STLString eLbl = eMatch[1].str();
				short enumVal = -1;
				for (auto itr = mdls.begin(); itr != mdls.end() && enumVal == -1; ++itr)
					enumVal = (*itr)->ModelEnumValueForName(eLbl);

				if (enumVal != -1)
				{
					out.push_back(enumVal);
					ret = true;
				}
			}
		}
	}
	

	return ret;
}
