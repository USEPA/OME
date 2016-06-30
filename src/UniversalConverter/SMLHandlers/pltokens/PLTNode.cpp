#include "SMLHandlers/pltokens/PLTNode.h"
#include "SMLHandlers/PrologParser.h"
#include "ModifierProcessor.h"
#include "DrawDetails.h"
#include <iostream>
#include <fstream>
#include <sstream>

PLTNode::PLTNode()
{
	//initialize function map. There should be one function for each node type
	funcMap["alarm"]=&buildAlarmObject;
	funcMap["cloud"]=&buildCloudObject;
	funcMap["compartment"]=&buildCompartmentObject;
	funcMap["function"]=&buildFunctionObject;
	funcMap["variable"]=&buildVariableObject;
	funcMap["submodel"]=&buildSubmodelObject;
	funcMap["border"]=&buildBorderObject;
	funcMap["immigration"]=&buildImmigrationObject;
	funcMap["reproduction"]=&buildReproductionObject;
	funcMap["loss"]=&buildLossObject;
	funcMap["creation"]=&buildCreationObject;
	funcMap["condition"]=&buildConditionObject;
	funcMap["text"]=&buildTextObject;

	//initialize type map
	tMap["alarm"]=OME_ITERCOND;
	tMap["cloud"]=OME_NULL;
	tMap["compartment"]=OME_STATEVAR;
	tMap["function"]=OME_NULL;
	tMap["variable"]=OME_VARIABLE;
	tMap["submodel"]=OME_MODEL;
	tMap["border"]=OME_SUBPORT;
	tMap["immigration"]=OME_SPAWNER;
	tMap["reproduction"]=OME_SPAWNER;
	tMap["loss"]=OME_SPAWNER;
	tMap["creation"]=OME_SPAWNER;
	tMap["condition"]=OME_ITERCOND;
	tMap["text"]=OME_LABEL;
}

PLTNode::~PLTNode()
{

}

//=============================== Sub Builders ====================================

/** Constructs a representation of Simile's cloud node.
	@param name the name of the object.
	@param subs list of strings of subitem ids (not used by this function).
	@param attrs regex match object of all possible node attributes.
	@param prsr The parser to use to evaluate node Condition.
	@return an OMEObject pointer referencing the OME-equivalent object (IterConditional).
*/
OMEObject* PLTNode::buildAlarmObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr)
{
	return new IterConditional(name,ConvertEscapes(attrs[ATTR_DESC]),ConvertExpression(attrs[ATTR_VALUE],NULL));
}

/** Constructs a representation of Simile's cloud node.
	@param name the name of the object.
	@param subs list of strings of subitem ids (not used by this function).
	@param attrs regex match object of all possible node attributes.
	@param prsr The parser to use to evaluate node Condition.
	@return an OMEObject pointer referencing the OME-equivalent object (NullCloud).
*/
OMEObject* PLTNode::buildCloudObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr)
{
	return new NullCloud();
}
/** Constructs a representation of Simile's compartment node.
	@param name the name of the object.
	@param subs list of strings of subitem ids (not used by this function).
	@param attrs regex match object of all possible node attributes.
	@param prsr The parser to use to evaluate node Condition.
	@return an OMEObject pointer referencing the OME-equivalent object (StateVar).
*/
OMEObject* PLTNode::buildCompartmentObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr)
{

		//STLString description= ConvertEscapes(attrs[ATTR_DESC]);
		return new StateVar(name,name);
}

/** Constructs a representation of Simile's function node.
	@param name the name of the object.
	@param subs list of strings of subitem ids (not used by this function).
	@param attrs regex match object of all possible node attributes.
	@param prsr The parser to use to evaluate node Condition.
	@return an OMEObject pointer referencing the OME-equivalent object (NullFunc).
*/
OMEObject* PLTNode::buildFunctionObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr)
{
			/*! Maps to NullFunc
			Attrs:
				complete
				name
				spec
				table_data
				units
				value
				??
		*/
	NullFunc* newFunc=new NullFunc(name.c_str());

	newFunc->SetUnitStr(attrs[ATTR_UNITS]);
		
	//STLString spec=ConvertEscapes(attrs[ATTR_QSPEC].empty()==false ? attrs[ATTR_QSPEC] : attrs[ATTR_SPEC]);

	newFunc->SetExpr(/*spec.empty()==false ? spec :*/ attrs[ATTR_VALUE]);
	//newFunc->m_expr=attrs[ATTR_VALUE].str().c_str();

	if(attrs[ATTR_TABLE].length())
		newFunc->SetTableData(buildTableDataEntry(attrs[ATTR_TABLE]));
		
	newFunc->SetComplete(!attrs[ATTR_COMPLETE].empty());
	return newFunc;
}

/** Constructs a representation of Simile's variable node.
	@param name the name of the object.
	@param subs list of strings of subitem ids (not used by this function).
	@param attrs regex match object of all possible node attributes.
	@param prsr The parser to use to evaluate node Condition.
	@return an OMEObject pointer referencing the OME-equivalent object (Variable).
*/
OMEObject* PLTNode::buildVariableObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr)
{
			/*! Maps to Variable
			Attrs:
				comment
				complete
				description
				max_val
				min_val
				name
				param_type
				spec
				table_data
				units
				value
				??
		*/

		//STLString description= ConvertEscapes(attrs[ATTR_DESC]);
		STLString units=ConvertEscapes(attrs[ATTR_UNITS]);
		STLString val_expr=ConvertExpression(attrs[ATTR_VALUE],NULL);

		//set defaults
		Variable* newVar;
		if(units.substr(0,5)!="array")
		{
			//for now, assume vals with min/max will be applied toward time series
			//if(!attrs[ATTR_PARAM].empty() || (!attrs[ATTR_MAX].length() && !attrs[ATTR_MIN].length()))
				newVar=new Variable(name, name, 0.0);
				if(attrs[ATTR_PARAM]=="file")
					newVar->SetExpectsFile(true);
			//else
			//	newVar=new TimeSeriesVar(name,description);
		}
		else
			newVar=new VarArray(name,name);

		if (units.find("int") != STLString::npos)
			newVar->SetAsInt(true);
		//set additional
		/** \todo modify with detail wrapper
		newVar->m_unitStr=units;
		*/
		newVar->SetExpr(val_expr);
		if(attrs[ATTR_MAX].length())
		{
			OME_SCALAR temp=strtod(attrs[ATTR_MAX].c_str(),NULL);
			newVar->SetMaxValue(temp);
		}
		if(attrs[ATTR_MIN].length())
			newVar->SetMinValue(strtod(attrs[ATTR_MIN].c_str(),NULL));

		if (newVar->GetMinMaxLimit() != Variable::NO_LIMIT)
			newVar->SetAskVal(true);

		

		//dealloc of table will occur when parent model is dealloced
		if(attrs[ATTR_TABLE].length())
			newVar->SetPEvalTable(NullFunc::GenerateEvalTable(buildTableDataEntry(attrs[ATTR_TABLE])),0);

		////if double bounded and without param_type argument, assume it is a time series; (if its not, being a time series won't be harmful)
		//if (newVar->GetMinMaxLimit() == Variable::HAS_MINMAX && attrs[ATTR_PARAM].empty())
		//	newVar = TimeSeriesVar::ReplaceVariable(newVar);

		return newVar;
}

/** Constructs a representation of Simile's submodel node.
	@param name the name of the object.
	@param subs list of strings of subitem ids.
	@param attrs regex match object of all possible node attributes.
	@param prsr The parser to use to evaluate node Condition.
	@return an OMEObject pointer referencing the OME-equivalent object (Model).
*/
OMEObject* PLTNode::buildSubmodelObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr)
{
			/*! Maps to Model
				List of Node names
				Attrs:
					complete
					enum_types
					external_code
					fill_colour
					fix_math_arg
					image_posn
					multiplication_spec
					name
					separate
		*/
		Model* newMod = new Model();

		//parse subList
		STLString currStr;
		for(STLStringListIter it = subs.begin(); it != subs.end(); it++)
		{
			currStr=(*it);
			prsr.AddModelSubID(newMod,currStr);

		}

		//record any enumerated types
		if(attrs[ATTR_ENUM].length())
		{
			STLString enumStr=attrs[ATTR_ENUM];
			STLString valStr;
			STLRegex enumReg("'?(.*?)'?-\\[(.*?)\\],?");
			STLRegex valReg("'?(.+?)'?(?:,|$)");
			STLsmatch enumMatch;
			STLsmatch valMatch;
			
			unsigned int eInd;
			while(std::regex_search(enumStr,enumMatch,enumReg))
			{
				ModelEnum newEnums(enumMatch[1].str());
				valStr=enumMatch[2];

				while(std::regex_search(valStr,valMatch,valReg))
				{
					newEnums.AddEnumValue(valMatch[1].str());
					valStr=valMatch.suffix();
				}

				newMod->AddModelEnum(newEnums);
				enumStr=enumMatch.suffix();
			}

		}

		//default instanceCount should always be one unless otherwise specified
		size_t initNum=1;
		if(attrs[ATTR_MULTSPEC].length())
		{
			//for now, assume count is 1D; should add support for nD.
			static const OMEChar* patterns[]= {"count=\\[([0-9]+)",
									"type=[:alnum:]",
									"\0"
									};

			STLString multspec=attrs[ATTR_MULTSPEC];
			StrArray results=BatchRegex(multspec,patterns,1,true);			
			
			if(!results[1].empty())
			{
				std::stringstream(results[1])>>initNum;
			}
			else if(!results[2].empty())
			{
				if(results[2]=="records")
					newMod->SetInstMode(Model::RECORD);
				else if (results[2]=="population")
					newMod->SetInstMode(Model::DYNAMIC);
			}
		}
		newMod->SetInitNumInstances(initNum);
		newMod->SetName(name);
		

		return newMod;
}

/** Constructs a representation of Simile's creation node.
	@param name the name of the object.
	@param subs list of strings of subitem ids (not used by this function).
	@param attrs regex match object of all possible node attributes.
	@param prsr The parser to use to evaluate node Condition.
	@return an OMEObject pointer referencing the OME-equivalent object (Spawner).
*/
OMEObject* PLTNode::buildCreationObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr)
{
	//STLString description= ConvertEscapes(attrs[ATTR_DESC]);
	return Spawner::BuildInitSpawn(name,name);
}

/** Constructs a representation of Simile's immigration node.
	@param name the name of the object.
	@param subs list of strings of subitem ids (not used by this function).
	@param attrs regex match object of all possible node attributes.
	@param prsr The parser to use to evaluate node Condition.
	@return an OMEObject pointer referencing the OME-equivalent object (Spawner).
*/
OMEObject* PLTNode::buildImmigrationObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr)
{
	STLString description= ConvertEscapes(attrs[ATTR_DESC]);
	return Spawner::BuildAccumSpawn(name,description);
}

/** Constructs a representation of Simile's reproduction node.
	@param name the name of the object.
	@param subs list of strings of subitem ids (not used by this function).
	@param attrs regex match object of all possible node attributes.
	@param prsr The parser to use to evaluate node Condition.
	@return an OMEObject pointer referencing the OME-equivalent object (Spawner).
*/
OMEObject* PLTNode::buildReproductionObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr)
{
	//STLString description= ConvertEscapes(attrs[ATTR_DESC]);
	return Spawner::BuildReproSpawn(name,name);
}

/** Constructs a representation of Simile's loss node.
	@param name the name of the object.
	@param subs list of strings of subitem ids (not used by this function).
	@param attrs regex match object of all possible node attributes.
	@param prsr The parser to use to evaluate node Condition.
	@return an OMEObject pointer referencing the OME-equivalent object (Spawner).
*/
OMEObject* PLTNode::buildLossObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr)
{
	//STLString description= ConvertEscapes(attrs[ATTR_DESC]);

	return Spawner::BuildDeathSpawn(name,name);
}

/** Constructs a representation of Simile's conditional node.
	@param name the name of the object.
	@param subs list of strings of subitem ids (not used by this function).
	@param attrs regex match object of all possible node attributes.
	@param prsr The parser to use to evaluate node Condition.
	@return an OMEObject pointer referencing the OME-equivalent object (Spawner).
*/
OMEObject* PLTNode::buildConditionObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr)
{
	//STLString description= ConvertEscapes(attrs[ATTR_DESC]);

	return Spawner::BuildCondSpawn(name,name);
}

/** Constructs a representation of Simile's border node.
	@param name the name of the object.
	@param subs list of strings of subitem ids (not used by this function).
	@param attrs regex match object of all possible node attributes.
	@param prsr The parser to use to evaluate node Condition.
	@return an OMEObject pointer referencing the OME-equivalent object (SubmodelPort).
*/
OMEObject* PLTNode::buildBorderObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr)
{
	//border object is equivalent to SubmodelPort

	//names are frequently reused with borders, but they are not directly referenced by name; ensure each port has a unique name
	static unsigned int num=0;

	oSTLSStream outStrm;
	outStrm<<name<<'_'<<num++;
	//for now, create a blank submodelPort with the appropriate id
	OMEObject* ret= new SubmodelPort(outStrm.str());

	return ret;
}

/** Constructs a representation of Simile's text node.
	@param name The name of the object.
	@param subs List of strings of subitem ids (not used by this function).
	@param attrs Regex match object of all possible node attributes.
	@param prsr The parser to use to evaluate node Condition.
	@return An OMEObject pointer referencing the OME-equivalent object (SimpleLabel).
*/
OMEObject* PLTNode::buildTextObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr)
{
	STLRegex sQuoteReg("''");
	STLString text=ConvertEscapes(std::regex_replace(attrs[ATTR_NAME],sQuoteReg,STLString("'")));
	OMEObject* ret= new SimpleLabel(name,text);
	return ret;
}

//=============================== Main Build Func =================================
OMEObject* PLTNode::buildOMEObject(STLString args, PrologParser & parser, STLString & outLabel)
{
	//node = a number of possibilities

	//define regex searches (use preprossessor concantenation to make it easier to read)
	static const STLRegex argReg(	"^"
									"([a-zA-Z0-9]*),"
									"([a-zA-Z0-9]*),"
									"\\[(.*?)\\],"
									"(\\[.*\\]),"
									"\\[(.*)\\]"
									"$"
								);

	static const OMEChar* patterns[] = {		"^.*comment='(.*?)'(?:,|\\]).*$",
											"^.*complete=(.*?)(?:,|\\]).*$",
											"^.*description='?(.*?)'?(?:,|\\]).*$",
											"^.*enum_types=\\[((?:.*?-\\[.*?\\],?)+)\\](?:,|\\]).*$",
											"^.*external_code=\\[(.*?)\\](?:,|\\]).*$",
											"^.*fill_colour='?(.*?)'?(?:,|\\]).*$",
											"^.*fix_math_arg=(.*?)(?:,|\\]).*$",
											"^.*image_posn=(.*?)(?:,|\\]).*$",
											"^.*max_val=(.*?)(?:,|\\]).*$",
											"^.*min_val=(.*?)(?:,|\\]).*$",
											"^.*multiplication_spec=\\[(.*?)\\](?:,|\\]).*$",
											"^.*name='?(.*?)'?(?:,|\\]).*$",
											"^.*param_type='?(.*?)'?(?:,|\\]).*$",
											"^.*separate=(.*?)(?:,|\\]).*$",
											"^.*spec='(.*?)',.*$",
											"^.*spec=([^'].*?[^']),.*$",
											"^.*table_data=\\[(.*dims=(?:\\[|').*?(?:\\]|'))\\](?:,|\\]).*$",
											"^.*units=(array\\(.*?),value.*$",
											"^.*value=(.*?)\\]$",
											"\0"};

	
	static const STLRegex units2Reg("^.*units=(.*?)(?:,|\\]).*$");
	static const STLRegex subReg("([a-zA-Z0-9]+?)(?:,|$)");
	static const STLRegex wsReg("[ \\t]");
	static const STLRegex qtReg("[\"']");
	static const OMEChar* graphPatterns[] = {
									"^.*bounding_box=\\[(.*?,.*?,.*?,.*?)\\].*$",
									"^.*caption_offset=\\[(.*?,.*?)\\].*$",
									"^.*centre=\\[(.*?,.*?)\\].*$",
									"^.*internal_extent=\\[(.*?,.*?,.*?,.*?)\\].*$",
									"\0"};

		OMEObject* ret=NULL;

	STLsmatch argMatch, subMatch;
	STLStringList subList;

	//split initial arg list
	std::regex_match(args,argMatch,argReg);

	//split graphic info
	STLString graphStr=argMatch[PLTN_LOC].str();

	//grab type
	STLString type=argMatch[PLTN_TYPE];

	//build subnode list (if present)
	STLString subStr=argMatch[PLTN_SUB];
	if(subStr.length())
	{
		while(std::regex_search(subStr,subMatch,subReg))
		{
			STLString result=subMatch[1];
			subList.push_back(result);
			subStr=subMatch.suffix();
		}
	}

	//pull out attributes
	STLString attributes=STLString(argMatch[PLTN_ATTR]);
	StrArray attrMatches=BatchRegex(attributes,patterns);
	STLsmatch tempMatch;
	//do secondary search for units if first did not find anything;
	if(attrMatches[ATTR_UNITS].empty())
	{
		std::regex_match(attributes,tempMatch,units2Reg);
		attrMatches[ATTR_UNITS]=tempMatch[1];
	}
	
	//STLString name=std::regex_replace(attrMatches[ATTR_NAME],wsReg,STLString("_"));
	//remove newlines from names, ids
	STLString name = attrMatches[ATTR_NAME];
	//name=std::regex_replace(name,qtReg,STLString(""));

	//call relevant static build function based on type
	if(!attrMatches[ATTR_COMPLETE].empty() || tMap[type]==OME_SUBPORT || type=="function")
	{
		buildFunction goFunc=funcMap[type];
		ret=goFunc(ConvertEscapes(name),subList,attrMatches,parser);
	}
	else //if(tMap[type]!=OME_NULL)//ghostNode
	{
		ret=new EvalAlias(name);
	}


	if(ret)
	{
		//grab OME expression modifier statement if present
		parser.AddModifier(ret,ModifierProcessor::ExtractExpression(attrMatches[ATTR_COMMENT]));
		

		outLabel=argMatch[PLTN_ID].str();
		
		//evaluate graphic stuff
		DrawDetails* pDD=new DrawDetails(ret);
		pDD->SetDescription(ConvertEscapes(attrMatches[ATTR_COMMENT]));
		StrArray drawMatches=BatchRegex(graphStr,graphPatterns);
		OMEPoint tmp;

		iSTLSStream buff;
		//caption offset
		if(!drawMatches[GRAPH_CAPT].empty())
		{
			buff.str(drawMatches[GRAPH_CAPT]);
			if(buff>>tmp.x>>tmp.y)
				pDD->SetLabelOrigin(tmp);
			tmp=OMEPoint();
		}

		//bounding box / centre
		if(!drawMatches[GRAPH_CENTER].empty())
		{
			buff.str(drawMatches[GRAPH_CENTER]);
			if(buff>>tmp.x>>tmp.y)
				pDD->SetOrigin(tmp);
			tmp=OMEPoint();
		}
		else if(!drawMatches[GRAPH_BB].empty())
		{
			OMEPoint tmpOrigin;
			buff.str(drawMatches[GRAPH_BB]);
			if(buff>>tmpOrigin.x>>tmpOrigin.y)
				pDD->SetOrigin(tmpOrigin);

			if(buff>>tmp.x>>tmp.y)
			{
				tmp.x-=tmpOrigin.x;
				tmp.y-=tmpOrigin.y;
				pDD->SetExtents(tmp);
			}
			tmp=OMEPoint();
		}

		//internal extents
		if(!drawMatches[GRAPH_INTEXT].empty())
		{
			OMEBox bx;
			buff.str(drawMatches[GRAPH_INTEXT]);
			if(buff>>bx.tl.x>>bx.tl.y>>bx.br.x>>bx.br.y)
				pDD->SetInternalBox(bx);
		}

		switch(ret->GetOMEType())
		{
		case OME_MODEL:
			parser.m_submodelMap[outLabel]=(Model*)ret;
		break;
		case OME_SUBPORT:
			parser.m_subPortList.push_back((SubmodelPort*)ret);
			break;
		};
				
	}
	return ret;
}

//=============================== Support Func ====================================
/** Support function for buildFunction object.
	Donstructs and returns a tabledata struct used to represent some of Simile's function info.
	@param args String of information defining the tabledata struct.
	@return A populated tabledata struct.
*/
NullFunc::TableData PLTNode::buildTableDataEntry(STLString args)
{
	NullFunc::TableData ret;
	enum {ALL=0,FILE,DATA_TABLE,DATA_COLUMN,INDICES,CURRENT,UNITS,BOUNDS,DIMS, SUBATTR_COUNT};

	static const OMEChar* patterns[] = {
													"^.*file='?(.*?)'?(?:,|\\]).*$",
													"^.*data=\\[(.*?)\\](?:,|\\]).*$",
													"^.*data='?(.*?)'?(?:,|\\]).*$",
													"^.*indices=\\[(.*?)\\](?:,|\\]).*$",
													"^.*current=\\[+(.*?)\\],units.*$",
													"^.*units=(.*?)(?:,|\\]).*$",
													"^.*bounds=\\[(.*?)\\].*$",
													"^.*dims=\\[(.*?)\\].*$",
													"\0" };

	StrArray baseMatches=BatchRegex(args,patterns);

	STLString tempStr,subStr;
	std::smatch subMatch;
	static const STLRegex subReg("(?:[\\[,']+|^)(.+?)(?:[\\],']|$)");
	static const STLRegex qReg("'?(.+?)'?(?:[\\[,\\]]+|$)");

	//parse out attribute data

	ret.file=baseMatches[FILE];

	//data
	tempStr=baseMatches[DATA_TABLE];
	if (!tempStr.empty())
	{
		while (tempStr.length())
		{
			std::regex_search(tempStr, subMatch, qReg);
			ret.data.push_back(subMatch[1]);

			tempStr = subMatch.suffix().str();
		}
	}
	else
		ret.colName = baseMatches[DATA_COLUMN];

	//indices
	tempStr=baseMatches[INDICES];

	while(tempStr.length())
	{
		std::regex_search(tempStr,subMatch,qReg);
		ret.indices.push_back(subMatch[1]);

		tempStr=subMatch.suffix().str();
	}


	tempStr.clear();

	//current
	tempStr=baseMatches[CURRENT];

	while(tempStr.length())
	{
		std::regex_search(tempStr,subMatch,subReg);
		ret.current.push_back(OME_SCLR_READSTR(subMatch[1]));

		tempStr=subMatch.suffix();
	}
	tempStr.clear();

	//units
	ret.units=baseMatches[UNITS];

	//bounds
	tempStr=baseMatches[BOUNDS];
	
	while(tempStr.length())
	{
		std::regex_search(tempStr,subMatch,subReg);
		ret.bounds.push_back(subMatch[1]);

		tempStr=subMatch.suffix();
	}
	tempStr.clear();
	
	//dims (assume int for now)
	tempStr=baseMatches[DIMS];
	
	unsigned int i;
	for(i=0; i<2 && tempStr.length();i++)
	{
		std::regex_search(tempStr,subMatch,subReg);
		ret.dims[i]=(std::stoi(subMatch[1]));

		tempStr=subMatch.suffix();
	}
	tempStr.clear();
	if (i == 2)
	{
		//invert 2D dims
		int tmp = ret.dims[0];
		ret.dims[0] = ret.dims[1]; 
		ret.dims[1] = tmp;
	}
	return ret;
}

