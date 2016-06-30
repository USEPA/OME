#include "LuaEvalSerializer.h"
#include "omeobjects/SimObjs.h"
#include "LuaOMEParser.h"

#include <stack>


#define SETCALL(x)	(!(x)->IsOMEType(OME_VARARRAY) ? "setEvalVal" : "setVarArrayVal")

static std::map<STLString, STLString> InitOpMap()
{
	std::map<STLString, STLString> ret;
	ret["=="] = "OMEWrapEq";
	ret["!="] = "OMEWrapNe";
	ret["<"]  = "OMEWrapLt";
	ret[">"]  = "OMEWrapGt";
	ret["<="] = "OMEWrapLe";
	ret[">="] = "OMEWrapGe";

	return ret;
}

std::map<STLString, STLString> LuaEvalSerializer::m_boolFuncs = InitOpMap();
std::map<STLString, STLString> LuaEvalSerializer::m_initFuncs;

/** Default Constructor. */
LuaEvalSerializer::LuaEvalSerializer()
	:BaseEvalSerializer(), m_pEvalParser(NULL), m_selfReferences(NULL), m_usingDebug(true), m_pRootModel(NULL)
{
}

/** Detailed Constructor.
	@param pParser The parser to associate with.
	@param useDbg Flag indicating if debugging code should be generated or not.
*/
LuaEvalSerializer::LuaEvalSerializer(LuaOMEParser* pParser,const bool & useDbg)
	:BaseEvalSerializer(), m_pEvalParser(pParser), m_selfReferences(NULL), m_usingDebug(useDbg), m_pRootModel(NULL)
{
	ExtractSelfReferencesFromParser();
	ExtractFunctionArgsAndInitsFromParser();
}

/** Copy Constructor.
	@param les The parser to copy.
*/
LuaEvalSerializer::LuaEvalSerializer(const LuaEvalSerializer & les)
	:BaseEvalSerializer(les), m_selfReferences(NULL),m_pRootModel(NULL)
{
	Duplicate(les);
}

/** Default destructor. */
LuaEvalSerializer::~LuaEvalSerializer()
{
	if(m_selfReferences)
		delete[] m_selfReferences;
}

bool LuaEvalSerializer::SerializeEval(Model* pRootModel)
{
	//create default breakpaths if necessary
	if(m_ibPath.empty())
		m_ibPath=pRootModel->GetExprName()+"-initBreaks.txt";
	if(m_ebPath.empty())
		m_ebPath=pRootModel->GetExprName()+"-evalBreaks.txt";

	//Serialize based on priorities
	m_convertedExprs.clear();
#ifndef OME_IBATCH_OVERRIDE
	
	//init first
	SerializeModel(pRootModel,true);
	
	//resort for eval

#else
	std::ifstream inFile(OME_IBATCH_OVERRIDE);
	if (inFile)
	{
		m_updateCache << inFile.rdbuf();
		inFile.close();
	}
	else
		cout << "Err: override file not opened." << endl;
#endif

#ifndef OME_EBATCH_OVERRIDE
	
	//then eval
	SerializeModel(pRootModel,false);
#else
	std::ifstream inFile2(OME_EBATCH_OVERRIDE);
	if(inFile)
	{
		m_updateCache<<inFile2.rdbuf();
		inFile2.close();
	}
	else
		cout<<"Err: override file not opened."<<endl;
#endif
	return true;
}

/** Serialize a model and its subobjects.
	@param pRootModel Pointer to the root model to serialize.
	@param useInit If true, generate init statements; otherwise generate eval/update statements.
*/
void LuaEvalSerializer::SerializeModel(Model *pRootModel,const bool useInit)
{
	m_constArrayCount = 0;
	m_constArrays.clear();
	
	//m_spawnersAdded.clear();
	std::map<Model*, size_t> spwnsAdded;
	m_pRootModel = pRootModel;
	EvalArray evals;
	pRootModel->GetOrderedEvalsForUpdates(evals, useInit);
	oSTLSStream* pCache= useInit ? &m_initCache : &m_updateCache;
	
	//ofstream outFile("prioritybrief.txt");
	std::stack< std::pair<Model*, bool> > mdlStack;
	mdlStack.emplace(pRootModel,true);
	Model* currMdl;
	unsigned int execGrp=OME_NO_EXEC_GRP;
	//std::sort(evals.begin(),evals.end(), useInit ? CompareSerialInitPriority : CompareSerialEvalPriority);

	//main cache is populated slightly out of order; work with body cache. 
	oSTLSStream bodyCache;

	for(auto itr=evals.begin(); itr!=evals.end(); ++itr)
	{

		//subports need to be evaluated outside of model loop
		currMdl= (*itr)->GetOMEType()!=OME_SUBPORT ? (*itr)->GetParentModel() : ((SubmodelPort*)(*itr))->GetOuterModel();
		
		if (spwnsAdded.find(currMdl) == spwnsAdded.end())
			spwnsAdded[currMdl] = 0;

		//check for exec group change
		if ((*itr)->GetExecGroup() != execGrp)
		{
			if (execGrp != OME_NO_EXEC_GRP && (*itr)->GetExecGroup() != OME_NO_EXEC_GRP)
			{
				if (NeedsLoop(mdlStack.top().first))
				{
					AddModelLoopFooter(mdlStack.top().first, mdlStack.top().second, bodyCache);
					//outFile<<mdlStack.top()->GetName()<<" finish"<<endl;
				}
				mdlStack.pop();
			}
			execGrp = (*itr)->GetExecGroup();
		}

		if(mdlStack.top().first!=currMdl)
		{	
			ModelList path;
			while(!mdlStack.top().first->IsNested(currMdl,&path))
			{
				if (NeedsLoop(mdlStack.top().first))
				{
					AddModelLoopFooter(mdlStack.top().first, mdlStack.top().second, bodyCache);
					//outFile<<mdlStack.top()->GetName()<<" finish"<<endl;
				}
				mdlStack.pop();
			}

			for (auto pItr = path.begin(); pItr != path.end(); ++pItr)
			{
				bool needsSpawner = spwnsAdded[currMdl] == currMdl->GetSpawnerCount();
				if(NeedsLoop(*pItr))
				{
					AddModelLoopHeader((Model*)(*pItr), bodyCache, needsSpawner, useInit);
					//outFile<<(*pItr)->GetName()<<" start"<<endl;
				}
				mdlStack.emplace(*pItr,needsSpawner);
			}
		}
		
		
		ProcessEval((*itr),bodyCache,useInit);
		if ((*itr)->GetOMEType() == OME_SPAWNER)
		{
			++spwnsAdded[currMdl];

			if (spwnsAdded[currMdl] == currMdl->GetSpawnerCount())
			{
				//todo update for lua
				bodyCache << "killMarkedInstances("<<mdlStack.top().first->GetID()<<")" << endl;
				bodyCache << "if isModelActive(" << mdlStack.top().first->GetID() << ") then" << endl;
				mdlStack.top().second = true;
			}
		}
		//outFile<<(*itr)->GetEvalPriority()<<"\t"<<(*itr)->GetName()<<endl;
		
	}

	//close any open loops
	while(!mdlStack.empty() && mdlStack.top().first!=pRootModel)
	{
		if(NeedsLoop((Model*)mdlStack.top().first))
			AddModelLoopFooter(mdlStack.top().first, mdlStack.top().second, bodyCache);
		mdlStack.pop();
	}

	AddTailDeclarations(bodyCache);

	LoadBreaks((useInit ? m_ibPath : m_ebPath),useInit);

	//some information that goes into the headers comes from processing the body.
	//build header here
	AddHeaderDeclarations(*pCache,useInit);

	*pCache << bodyCache.str();
}

bool LuaEvalSerializer::SerializeEval(const EvalArray & entries,const bool forInit)
{
	oSTLSStream* pCache= forInit ? &m_initCache : &m_updateCache;
	std::for_each(entries.begin(),entries.end(),[&](Evaluable*e){ProcessEval(e,*pCache,forInit);});
	return true;
}

bool LuaEvalSerializer::SerializeEval(Evaluable* entry,const bool forInit)
{
	oSTLSStream* pCache= forInit ? &m_initCache : &m_updateCache;
	ProcessEval(entry,*pCache,forInit);
	return true;
}


STLString LuaEvalSerializer::GetInitStatements(const bool forPrint)
{
	return !forPrint ? AssignBreaks(m_initCache.str(),m_initBreaks) : m_initCache.str();
}

STLString LuaEvalSerializer::GetEvalStatements(const bool forPrint)
{
	//return  AssignBreaks(m_updateCache.str(), m_updateBreaks);
	return !forPrint ? AssignBreaks(m_updateCache.str(),m_updateBreaks) : m_updateCache.str();
}

void LuaEvalSerializer::ClearSerializer()
{
	m_initCache.clear();
	m_initCache.str(STLString());

	m_updateCache.clear();
	m_updateCache.str(STLString());
}

/** Derive a Lua statement for a specific Evaluable object.
	@param pEval Pointer to an Evaluable object to be evaluated.
	@param cache ostringstream that will incorporate the derived statement.
	@param isInit If true process pEval for an initialization statement; otherwise, process for update statement.
*/
void LuaEvalSerializer::ProcessEval(Evaluable* pEval, oSTLSStream & cache,const bool & isInit)
{
	using namespace std;
	bool initOnly=false;
	
	bool dynMod;
	
	STLString name=pEval->GetID();
	STLString expr;
	if (isInit || pEval->GetInitExpr()!=pEval->GetExpr() || m_convertedExprs.find(pEval->GetID()) == m_convertedExprs.end())
	{
		//modify and cache expression on init pass
		expr = LuaOMEParser::LuaizeExpression(isInit ? pEval->GetInitExpr() : pEval->GetExpr());
		expr = ConvertExprNamesToIds(expr);
		expr = RegisterConstArrays(expr);
		expr = ApplyFactories(pEval, expr, isInit);
		expr = ProcessLists(expr);
		expr = ConvertBools(expr);

		m_convertedExprs[pEval->GetID()] = expr;
		//init pass has some special function substitutions; add after caching
		expr = ConvertInitFuncs(expr);
	}
	else
		expr = m_convertedExprs[pEval->GetID()];

	OMETYPE type;
	
	switch(pEval->GetOMEType())
	{
	case OME_STATEVAR:
	case OME_CONVEYOR:
		dynMod = !isInit && pEval->GetParentModel()->IsDynamic();
		if (isInit || dynMod)
		{
			if (dynMod)
				cache << "if isInitialized(" << name << ") then" << endl;
			cache << "resetEval(" << name << ')' << endl;
			cache << SETCALL(pEval) << "(" << name << "," << expr << ")" << endl;
			cache << "initEval(" << name << ')' << endl;
			if (dynMod)
				cache << "end" << endl;
		}
		break;
	case OME_VARIABLE:
	case OME_VARARRAY:
		initOnly=((Variable*)pEval)->GetInitOnly();
	case OME_FLOW:

		//if value is constant, don't do update
		type=pEval->GetOMEType();
		//if(!pEval->GetIsConstant() || type==OME_STATEVAR || type==OME_VARARRAY)
		//{
			//insert linking of calling object for self-referencing functions
			if(BatchRegexMatchAny(expr,m_selfReferences,0,true))
					cache<<"_G.__OMECaller="<<name<<endl;

			//convert statements to functions for specific functions
			for (auto mItr = m_funcArgReferences.begin(); mItr != m_funcArgReferences.end(); ++mItr)
				expr = UpdateFuncArgs((*mItr).first, (*mItr).second, expr);

			if(!isInit && !initOnly)
			{
				if(!pEval->GetIsConstant())
					cache<<SETCALL(pEval)<<"("<<name<<","<<expr<<")"<<endl;
								
			}
			else if (isInit)
			{
				if (!pEval->IsOMEType(OME_VARARRAY))
				cache<<"resetEval("<<name<<')'<<endl;
				STLString initExpr=pEval->GetInitExpr();
				if(!initExpr.empty())
				{
					cache << SETCALL(pEval)<<"(" << name << "," << expr << ")" << " -- " << pEval->GetInitPriority() << endl;
					//if(!FindIf(cache,initExpr))
					//	cache<<"__OMELastResult="<<initExpr<<endl;
					//cache<<"setEvalVal("<<name<<",__OMELastResult)"<<endl;
				}
				cache<<"initEval("<<name<<')'<<endl;
			}
			
		//}
//		else
//			cache<<name<<'='<<pEval->GetValue()<<endl;

		break;
	case OME_SUBPORT:
		cache<<"updatePort("<<name<<')'<<" -- "<<pEval->GetInitPriority()<<endl;
		break;
	case OME_TSVAR:
		if (&cache == &m_initCache)
			cache << "initEval(" << name << ')' << endl;
		cache<<"updateTimeSeries("<<name<<')'<<endl;
		break;
	case OME_SPAWNER:
		initOnly = initOnly || ((Spawner*)pEval)->RunsOnce();
		if (&cache == &m_initCache || !initOnly)
		{
			if (BatchRegexMatchAny(expr, m_selfReferences, 0, true))
				cache << "__OMECaller=" << name << endl;

			for (auto mItr = m_funcArgReferences.begin(); mItr != m_funcArgReferences.end(); ++mItr)
				expr = UpdateFuncArgs((*mItr).first, (*mItr).second, expr);

			//spawner needs to be initialized before update
			if (&cache == &m_initCache)
				cache << "initEval(" << name << ')' << endl;
			cache << "updateSpawner(" << name << ',' << expr << ')' << endl;
			
		}
		
		break;
	
	case OME_INFLUENCE:
	case OME_MODEL:
	//case OME_EXCHANGER:
	//case OME_INTERBRIDGE:
	case OME_ITERCOND:	
	case OME_SUBASSOC:
	default:
		break;
	}
   pEval->SetIsConstant(false);
}

/** Generate header for model loop.
	@param pMdl The model to build the header from.
	@param cache ostringstream to give the header to.
	@param useActive Add statements needed for evaluating a Model's active level.
	@param forInit If true, process model components for initialization statements.
*/
void LuaEvalSerializer::AddModelLoopHeader(Model* pMdl, oSTLSStream & cache,const bool & useActive,const bool & forInit)
{
	STLString name = pMdl->GetID();

	cache << "do" << endl;
   /*cache<<"print(string.format(\""<<name<<" start: %s\",os.date(\"%X\")))"<<endl;
   cache<<"local "<<name<<"_start=os.clock()"<<endl;*/

	cache<<"local "<<name<<"_count=getMCInstanceCount("<<name<<")"<<endl;
	cache<<"local "<<name<<"_i"<<endl;
	//cache<<"local __OMECurrentIteration"<<endl;
	cache<<"for "<<name<<"_i=1,"<<name<<"_count do"<<endl;
	//next line is debug
	//cache << "print(" << '"'<<name << "\","<<name<<"_i)" << endl;
	//cache<<"__OMECurrentIteration="<<name<<"_i"<<endl;
	cache<<"setActiveInstance("<<name<<","<<name<<"_i)"<<endl;
	if (pMdl->IsDynamic())
	{
		//only evaluate spawners at the beginning of the first model loop
		if (std::find(m_spawnersAdded.begin(), m_spawnersAdded.end(), pMdl) == m_spawnersAdded.end())
		{
			//sort spawners, then evaluate at top of model block
			EvalSet sortedSpawns(&cache == &m_initCache ? OMEObject::CompareInitPriority : OMEObject::CompareEvalPriority);
			for (int i = 0; i < pMdl->GetSpawnerCount(); i++)
				sortedSpawns.insert(pMdl->GetSpawner(i));
			for (auto itr = sortedSpawns.begin(); itr != sortedSpawns.end(); ++itr)
				ProcessEval(*itr, cache,forInit);
			m_spawnersAdded.push_back(pMdl);
		}
		cache << "if isModelDead(" << name << ")~=true then" << endl;
	}
	if (pMdl->IsConditional() && useActive)
		cache << "if isModelActive("<<name << ")==true then" << endl;
}

/** Generate footer for model loop.
	@param pMdl The model to build the footer from.
	@param wasActive If true, add statements relative to active condition.
	@param cache ostringstream to give the footer to.
*/
void LuaEvalSerializer::AddModelLoopFooter(Model* pMdl,const bool & wasActive,oSTLSStream & cache)
{	
	STLString name=pMdl->GetID();

	if (pMdl->IsDynamic())
		cache << "end" << endl;
//	cache<<"collectgarbage(\"collect\")"<<endl;
	cache<<"end"<<endl;
	if (pMdl->IsConditional() && wasActive)
		cache << "end" << endl;
	cache<<"setActiveInstance("<<name<<",1)"<<endl;
	// if (pMdl->IsDynamic())
	//	cache << "killMarkedInstances(" << name << ')' << endl;
   //cache<<"print(string.format(\""<<name<<" finish. Total time: %d seconds.\", os.clock()-"<<name<<"_start))"<<endl;
	cache<<"end"<<endl;
}

/** Copy another LuaEvalSerializer.
	@param les The LuaEvalSerializer to copy.
*/
void LuaEvalSerializer::Duplicate(const LuaEvalSerializer & les)
{
	m_pEvalParser=les.m_pEvalParser;
	m_initCache.str(les.m_initCache.str());
	m_updateCache.str(les.m_updateCache.str());

	ExtractSelfReferencesFromParser();
}


/** Add declarations that appear at the beginning of any given batch script.
	@param cache The ostringtream to add the declarations to.
	@param forInit If true, apply initialization statements to cache.
*/
void LuaEvalSerializer::AddHeaderDeclarations(oSTLSStream & cache,const bool & forInit)
{
	//cache<<"local "<<m_pRootModel->GetExprName()<<"= "<<m_pRootModel->GetExprName()<<endl;
	if (forInit)
	{
		//add const arrays before switching global scope so that they persist beyond script run.
		for (auto itr = m_constArrays.begin(); itr != m_constArrays.end(); ++itr)
			cache <<"_G."<< (*itr).first << "=omeStaticList(" << (*itr).second <<')'<< endl;

	}

	LocalizeFuncs(cache);

	cache << "local env=" << LuaDefines::MDL_COMP_TABLE << endl;
	cache << "setmetatable(env, {__index=_G})" << endl;
	cache << "_ENV=env" << endl;
	

	//disabled for now since Lua has a hard limit on local variable count; there are some other techniques we can try in the future
	//LocalizeModels(m_pRootModel, cache);
	//LocalizeModelComponents(cache);

	//write out VaAllocs
	if (&cache==&m_initCache)
		AllocateVarArrays(m_pRootModel, cache);
	
}

/** Insert functions allocate space for VarArrays associated with model.
	@param pMdl Pointer to current Model to parse.
	@param cache stream to populate with allocation statements.
*/
void LuaEvalSerializer::AllocateVarArrays(Model* pMdl, oSTLSStream & cache)
{
	VarArray* pVa;
	for (size_t i = 0; i < pMdl->GetVarCount(); ++i)
	{
		if ((pVa = dynamic_cast<VarArray*>(pMdl->GetVariable(i))))
			cache << "allocVarArray(" << pVa->GetID() << ")" << endl;
	}

	for (size_t i = 0; i < pMdl->GetSubmodelCount(); ++i)
		AllocateVarArrays(pMdl->GetSubmodel(i), cache);
}

/** Add declarations that appear at the end of any given batch script.
	@param cache The ostringtream to add the declarations to.
*/
void LuaEvalSerializer::AddTailDeclarations(oSTLSStream & cache)
{
	//cache<<"print(\"BEFORE PURGE: \",collectgarbage(\"count\"))"<<endl;
	cache<<"collectgarbage(\"collect\")"<<endl;
	//cache<<"print(\"AFTER PURGE: \",collectgarbage(\"count\"))"<<endl;
	cache<<"omeCleanup()"<<endl;
}

/** Find functions which require a reference from the calling object.
	These values are then cached and used to efficiently condense the batch script.
*/
void LuaEvalSerializer::ExtractSelfReferencesFromParser()
{
	if(m_pEvalParser)
	{
		if(m_selfReferences)
			delete[] m_selfReferences;


		ARRAY_TYPE<OMELuaFunc*> srFuncs;
		ARRAY_TYPE<OMELuaFunc*> btFuncs;
		for(auto itr=m_pEvalParser->GetLuaFuncRegistry()->begin(); itr!=m_pEvalParser->GetLuaFuncRegistry()->end();++itr)
		{
			if((*itr)->SelfReferencing())
				srFuncs.push_back(*itr);
			if ((*itr)->TakesBoolTable())
				btFuncs.push_back(*itr);
		}

		m_selfReferences=new const OMEChar*[srFuncs.size()+1];

		int i;
		for(i=0; i<srFuncs.size(); i++)
			m_selfReferences[i]=srFuncs[i]->GetName();
		m_selfReferences[i]="\0";

	}
}

/** Takes functions args from Function registry.*/
void LuaEvalSerializer::ExtractFunctionArgsAndInitsFromParser()
{
	if (m_pEvalParser)
	{

		if (!m_funcArgReferences.empty())
			m_funcArgReferences.clear();
		if (!m_initFuncs.empty())
			m_initFuncs.clear();
		
		for (auto itr = m_pEvalParser->GetLuaFuncRegistry()->begin(); itr != m_pEvalParser->GetLuaFuncRegistry()->end(); ++itr)
		{
			if ((*itr)->TakesFuncArg())
				m_funcArgReferences[(*itr)->GetName()] = (*itr)->TakesFuncArg();

			const char* initFunc = (*itr)->GetAltInitFuncName();
			if (initFunc)
				m_initFuncs[(*itr)->GetName()] = initFunc;
			
		}

	}
}


STLString LuaEvalSerializer::ConvertExprNamesToIds(const STLString & inExpr)
{
	const STLRegex pathReg("[a-zA-Z_][a-zA-Z0-9_.]*(?![(])\\b");
	const OMEChar* excludes[] = {	"^\\s*if\\s*$", 
									"^\\s*then\\s*$", 
									"^\\s*else\\s*$", 
									"^\\s*elseif\\s*$", 
									"^\\s*and\\s*$", 
									"^\\s*or\\s*$", 
									"^\\s*end\\s*$", 
									"^\\s*return\\s*$",
									"^.*\\b[0-9]+[eE][+-]?[0-9]+\\b.*$",
									"^\\s*SDP_(?:NORTH|SOUTH)?(?:EAST|WEST)?\\s*$",
									"\0" };

	typedef std::pair<size_t, size_t> Block;
	std::list<Block> hitList;
	 
	size_t offset = 0;
	STLsmatch matches;
	STLString subExpr = inExpr;
	while (std::regex_search(subExpr, matches, pathReg))
	{
		//DBG_PRINTC(matches[0].str(),DBG_GREEN);
		if ((matches.position(0)+offset == 0 || (inExpr[matches.position(0) +offset- 1] != ':' && !std::isdigit(inExpr[matches.position(0) +offset- 1]))) && (matches.position(0) + matches.length(0) >= inExpr.length() || inExpr[matches.position(0) + matches.length()] != ':'))
		{
			if (!BatchRegexMatchAny(matches[0].str(), excludes, 0))
			{
				hitList.emplace_back(matches.position(0) + offset, matches.length(0));

			}
		}
		offset += matches.position(0) + matches.length(0);
		subExpr = matches.suffix().str();
	}

	//if no replacements necessary; exit here
	if (hitList.empty())
		return inExpr;

	oSTLSStream convExpr;

	//...
	size_t currPos = 0;
	for (auto itr = hitList.begin(); itr != hitList.end(); ++itr)
	{
		//grab good part of expr
		convExpr << inExpr.substr(currPos, (*itr).first - currPos);
		//do lookup with nested path
		
		/// @todo fix for Seagrass.

		convExpr << m_pRootModel->NestedObjectForName(inExpr.substr((*itr).first, (*itr).second))->GetID();
		currPos = (*itr).first + (*itr).second;
	}
	convExpr << inExpr.substr(currPos);
	return convExpr.str();
}

/** Overwrite function invocations with their initialization counterparts.
	@param inExpr The expression to parse.
	@return The converted expression.
*/
STLString LuaEvalSerializer::ConvertInitFuncs(STLString inExpr)
{
	const STLRegex funcReg("\\b([a-zA-Z_][a-zA-Z0-9_]*)([(])");
	STLsmatch mtch;

	oSTLSStream outExpr;

	while (std::regex_search(inExpr, mtch, funcReg))
	{
		outExpr << inExpr.substr(0, mtch.position(1));
		auto itr = m_initFuncs.find(mtch[1].str());
		outExpr << (itr != m_initFuncs.end() ? (*itr).second : mtch[1].str());
		inExpr = mtch[2].str()+mtch.suffix().str();
	}
	outExpr << inExpr;

	return outExpr.str();
}

bool LuaEvalSerializer::WriteSerialExpressions(const STLString & initPath, const STLString & evalPath)
{
	bool result=true;
	std::ofstream outFile;

	if(!initPath.empty())
	{
		outFile.open(initPath);

		result=outFile.good();
		if(result)
			outFile<<GetInitStatements(true);
		outFile.close();
	}

	if(result && !evalPath.empty())
	{
		outFile.open(evalPath);
		result= result && outFile.good();
		if(result)
			outFile<<GetEvalStatements(true);
		outFile.close();
	}
	return result;
}

/** Query if model needs loop in batch representation.
	@param pBm Pointer to base model to query against.
	@return true if model needs a loop in batch representation; false otherwise.
*/
bool LuaEvalSerializer::NeedsLoop(Model* pBm)
{
	bool ret = (pBm->IsDynamic() || (pBm->GetInitNumInstances()!=1) || pBm->IsConditional() || pBm->ExpectsSpatial());
	return ret;
}

/** Insert breakpoint syntax for the current batch statement.
	@param orig String containing the unaltered batch statements.
	@param breaks The breakpoint set to query against.
	@return A string containing the batch statements modified for debugging breakpoints.
*/
STLString LuaEvalSerializer::AssignBreaks(const STLString & orig,const std::set<size_t> & breaks)
{
   
//	if(m_usingDebug )//&& !breaks.empty())
//	{
		StrArray lines=SplitString(orig);
		unsigned int ind;
		for(ind=0; ind<lines.size(); ind++)
		{
	//		if((*itr)>0 && (*itr)<=lines.size())
	//		{

				oSTLSStream buff;
#if (defined OME_SHOW_ELAPSED || defined OME_SCRIPT_ELAPSED)
			buff<<"print(string.format(\"%i: %s\","<<ind+1<<",os.date(\"%X\"))) ";
#endif
				buff<<"omeLineInit("<<ind+1<<")";
				lines[ind]=buff.str()+" "+lines[ind];
	//		}
		}
		return ConcatStrings(lines,'\n');
//	}
//	else 
//		return orig;

}

/** Load breakpoints from a file.
	@param path Path to breakpoint file.
	@param forInit true if serializing for initialization instructions; false otherwise.
*/
void LuaEvalSerializer::LoadBreaks(const STLString & path,const bool forInit)
{
	std::ifstream inFile(path);
	std::set<size_t>* currSet;
	currSet=forInit ? &m_initBreaks : &m_updateBreaks;
	size_t line;
	if(inFile.good())
	{
		while(inFile>>line)
			currSet->insert(line);
	}

	inFile.close();
}

bool LuaEvalSerializer::SetBreak(const size_t & line, const bool & enabled,const bool & forInit)
{
	std::set<size_t>* currSet;
	currSet = forInit ? &m_initBreaks : &m_updateBreaks;

	if (enabled)
		currSet->insert(line);
	else
		currSet->erase(line);

	return true;
}

size_t LuaEvalSerializer::GetLineCount(const bool forInit)
{
	STLString statements= forInit ? m_initCache.str() : m_updateCache.str();

	return SplitString(statements).size();
}

bool LuaEvalSerializer::PopulateBreaks(BreakSet & breaks,const bool forInit)
{
	breaks.resize(GetLineCount(forInit));
	std::set<size_t> *pBreaks= forInit ? &m_initBreaks : &m_updateBreaks;

	for_each(pBreaks->begin(),pBreaks->end(),[&breaks](size_t ind){ if(ind>0 && ind<=breaks.size()) breaks[ind-1]=true;});
	return true;
}

/** Create local representations of OMELuaFuncs.
	@param out std::ostringstream to write localization definitions to.
*/
void LuaEvalSerializer::LocalizeFuncs(oSTLSStream & out)
{
	for(auto itr=m_pEvalParser->GetLuaFuncRegistry()->begin(); itr!=m_pEvalParser->GetLuaFuncRegistry()->end();++itr)
	{
		out<<"local "<<(*itr)->GetName()<<"="<<(*itr)->GetName()<<endl;
	}
	
	
	for (auto itr = m_boolFuncs.begin(); itr != m_boolFuncs.end(); ++itr)
	{
		//release is inserting a blank KV pair somewhere, so remove here
		if ((*itr).second[0] != '\0')
		out << "local " << (*itr).second << "=" << (*itr).second << endl;
	}
}

/** Mark models with "local" keyword.
	@param pMdl Pointer to the model to mark.
	@param out Stream to hold the resulting statement.
*/
void LuaEvalSerializer::LocalizeModels(Model* pMdl, oSTLSStream & out)
{
	out << "local " << pMdl->GetID() << "=" << LuaDefines::MDL_COMP_TABLE << "." << pMdl->GetID() << endl;
	for (size_t i = 0; i < pMdl->GetSubmodelCount(); ++i)
		LocalizeModels(pMdl->GetSubmodel(i), out);
}

/** Mark individual Model components with "local" keyword.
	@param out Stream to hold resulting statement.
*/
void LuaEvalSerializer::LocalizeModelComponents(oSTLSStream & out)
{
	EvalArray evals;
	m_pRootModel->GetAllEvals(evals, DT_FULL);
	for (auto itr = evals.begin(); itr != evals.end();++itr)
		out << "local " << (*itr)->GetID() << "=" << LuaDefines::MDL_COMP_TABLE << "." << (*itr)->GetID() << endl;
}

/** Modify function args for serialization.
	@param funcName The name of the function to search for.
	@param argNum The number of arguments to expect.
	@param expr The expression to evaluate.
	@return A copy of expr with properly formatted function args.
*/
STLString LuaEvalSerializer::UpdateFuncArgs(const STLString & funcName, const unsigned int & argNum, const STLString & expr)
{
	oSTLSStream out;
	STLString ret;
	size_t pos = 0;
	size_t argLen;
	size_t found = expr.find(funcName, pos);
	while ( found != STLString::npos)
	{
		FindArgPos(argNum, expr, found, pos, argLen);
		out << expr.substr(found, pos - found);
		out << "function() return " << UpdateFuncArgs(funcName,argNum,expr.substr(pos, argLen)) << " end ";
		pos += argLen;
		found = expr.find(funcName, pos);
	}

	out << expr.substr(pos);
	return out.str();
}

/** Find position of specified argument in provided expression.
	@param argNum The argument to search for.
	@param expr The expression to parse.
	@param found length of found expression.
	@param pos On success, contains position of the beginning of the argument.
	@param argLen On success, contains length of specified argument.
	@return true if the specified argument is found; false otherwise.
*/
bool LuaEvalSerializer::FindArgPos(const unsigned int argNum, const STLString & expr, const size_t & found, size_t & pos, size_t & argLen)
{
	//start at first open paren
	unsigned int argCount = 0;
	unsigned short depth=0;
	pos = expr.find('(', found)+1;
	
	for (size_t i = pos; i < expr.length() && argCount < argNum; i++)
	{
		switch (expr[i])
		{
		case '(':
		case '[':
		case '{':
			depth++;
			break;
		case ')':
		case ']':
		case '}':
			if (depth == 0)
			{
				argLen = i - pos;
				argCount++;
			}
			depth--;
			break;
		case ',':
			if (depth == 0)
			{
				argCount++;
				if (argCount < argNum)
					pos = i + 1;
				else
					argLen = i - pos;
			}
			break;
		}
	}

	return argCount >= argNum;
}

/** Scrape out any getAsTable functions with only static values and assign them to global static arrays.
	@param expr the Expression to parse.
	@return The expr modified with the placeholder for the static array.
	@throws OMESupportException execption if parentheses are not balenced in expr.
*/
STLString LuaEvalSerializer::RegisterConstArrays(STLString expr)
{
	static const STLString FUNC_NAME = "[";
	static const STLString ARRAY_PREFIX = "constArray_";
	size_t pos = expr.find(FUNC_NAME);
	size_t argStart;
	size_t argEnd;
	size_t checkStart;
	size_t checkEnd;
	size_t nextStart;
	oSTLSStream newExpr;

	while (pos != STLString::npos)
	{
		newExpr << expr.substr(0, pos);
		//find arguments
		argStart = pos;
		argEnd=FindMatching(expr, argStart,'[', ']');

		if (argEnd == STLString::npos)
			throw OMESupportException("LuaEvalSerializer::RegisterConstArrays:Parentheses are not properly matched.");
		
		nextStart = argEnd+1;

		//check for lua table declaration, just to be safe {}
		checkStart=FindMatching(expr, argStart, '[', '{');
		if (checkStart != STLString::npos)
			checkEnd = FindMatching(expr, checkStart, '{', '}');
		else
		{
			checkStart = argStart;
			checkEnd = argEnd;
		}

		//slightly shrink bounds
		++checkStart;
		--checkEnd;
		if (CheckConstArray(expr.substr(checkStart, checkEnd - checkStart)))
		{
			STLString arrayName = ARRAY_PREFIX + std::to_string(m_constArrayCount++);
			m_constArrays[arrayName] = expr.substr(checkStart, (checkEnd - checkStart));
			newExpr << arrayName;
		}
		else
			newExpr << expr.substr(pos, nextStart - pos);
		//continue looking for table spots.
		expr = expr.substr(nextStart);
		pos = expr.find(FUNC_NAME);
	}

	newExpr << expr;
	return newExpr.str();

}

/** Check to see if all values in the expr are static numeric values.
	@param expr The Expression to be evaluated.
	@return true if all values are numeric; false otherwise.
*/
bool LuaEvalSerializer::CheckConstArray(const STLString & expr)
{

	StrArray entries = SplitString(expr.substr(0,expr.length()), ',');

	try
	{
		//check to see if all values are primitive numbers
		for (auto itr = entries.begin(); itr != entries.end(); ++itr)
			OME_SCLR_READSTR((*itr));
	}
	catch (std::invalid_argument & notNum)
	{
		// if the above test fails return false 
		return false;
	}
	return true;
}

/** Process non-const array values*/
STLString LuaEvalSerializer::ProcessLists(const STLString & exprArgs)
{
	oSTLSStream outBuff;
	size_t lastPos = 0;
	size_t pos = 0;


	for (; pos < exprArgs.length(); ++pos)
	{
		//have we hit a list marker?
		if (exprArgs[pos] == '[')
		{
			size_t listStart = pos;
			outBuff << exprArgs.substr(lastPos, pos - lastPos);
			//count open brackets
			unsigned short depthCount = 0;
			for (; pos < exprArgs.length(); ++pos)
			{
				if (std::isspace(exprArgs[pos]))
					continue;
				if (exprArgs[pos] == '[')
					++depthCount;
				else //any other character hit; done counting brackets
					break;
			}
			
			pos = FindMatching(exprArgs, listStart, '[', ']');
			listStart += depthCount;
			
			STLRegex idReg("[\\[\\s]*([_a-zA-Z][_0-9a-zA-Z]*)[\\]\\s]*");
			STLsmatch mtch;
			//if this is an eval entry, we can just us upgroup
			STLString srchRange = exprArgs.substr(listStart, pos - listStart);
			if (std::regex_match(srchRange, mtch, idReg))
			{

				outBuff << "upgroup(" << ProcessLists(mtch[1].str())<<','<<depthCount<<')';
			}
			else
			{
				//we have a temporary list.
				outBuff << "getAsTable(" << ProcessLists(srchRange) << ')';
			}
			lastPos = pos + 1;
		}
	}
	outBuff << exprArgs.substr(lastPos, pos - lastPos);

	return outBuff.str();
}

/** Convert Boolean statements.
	@param exprArgs The expression to modify.
	@return a modified copy of exprArgs with booleans converted.
*/
STLString LuaEvalSerializer::ConvertBools(STLString exprArgs)
{
	const STLRegex opReg("[<>][=]?|[!=][=]");
	const STLRegex leftPrtReg("[)]\\s*?$");
	const STLRegex rightPrtReg("^\\s*[a-zA-Z0-9._]+[(]");
	const STLRegex leftReg("(?:[0-9]*[.]?[0-9]+e[+-][0-9]+|[a-zA-Z0-9._]+)\\s*$");
	const STLRegex rightReg("^\\s*(?:[0-9]*[.]?[0-9]+e[+-][0-9]+|[-]?[a-zA-Z0-9._]+)");

	STLsmatch opMatch,sideMatch;

	STLString leftSub, rightSub,currOp;
	size_t subPos, subLen;

	oSTLSStream outExpr;
	//first, find boolean operator
	while (std::regex_search(exprArgs, opMatch, opReg))
	{
		//grab operator
		currOp = opMatch[0].str();

		//split based on operator
		leftSub = exprArgs.substr(0, opMatch.position(0));
		rightSub = exprArgs.substr(opMatch.position(0)+opMatch.length(0));

		//find left side of argument
		if (std::regex_search(leftSub, sideMatch, leftPrtReg))
		{
			subPos = FindMatching(leftSub, sideMatch.position(0), ')', '(', true);
			//count function name as well
			for (; subPos > 0 && (std::isalnum(leftSub[subPos - 1]) || leftSub[subPos - 1] == '_'); --subPos);
		}
		else
		{
			std::regex_search(leftSub, sideMatch, leftReg);
			subPos = sideMatch.position(0);
		}

		//copy statement up to start of left match
		outExpr << exprArgs.substr(0, subPos);

		//begin function invocation
		outExpr << ' ' << m_boolFuncs[currOp] << '(' << exprArgs.substr(subPos, sideMatch.position(0)-subPos+sideMatch.length(0));

		//find right side of argument
		if (std::regex_search(rightSub, sideMatch, rightPrtReg))
			subLen = FindMatching(rightSub, sideMatch.position(0) + sideMatch.length(0)-1, '(', ')') + 1;
		else
		{
			std::regex_search(rightSub, sideMatch, rightReg);
			subLen = sideMatch.position(0) + sideMatch.length(0);
		}
		subPos = opMatch.position(0) + opMatch.length(0);

		//finish function call
		outExpr << ',' << exprArgs.substr(subPos, subLen) << ')';
		//advance beyond match.
		exprArgs = exprArgs.substr(subPos + subLen);
	}
	
	outExpr << exprArgs;
	return outExpr.str();


}

STLString LuaEvalSerializer::ApplyFactories(Evaluable* pCaller, STLString inExpr,const bool & inInit)
{
	using namespace LuaDefines;

	const STLRegex funcReg("\\b([a-zA-Z_][a-zA-Z0-9_]*)([(])");
	STLsmatch mtch;

	oSTLSStream outExpr;

	while (std::regex_search(inExpr, mtch, funcReg))
	{
		LuaFactoryFunc fctFunc = m_pEvalParser->GetFactoryForFunc(mtch[1].str());
		if (fctFunc)
			inExpr = fctFunc(pCaller, inExpr.substr(mtch.position(2) + 1, FindMatching(inExpr, mtch.position(2), '(', ')') - mtch.position(2) - 1), inInit);
		else
		{
			outExpr << mtch.prefix();
			outExpr << mtch[0].str();
			inExpr = mtch.suffix();
		}
	}
	outExpr << inExpr;

	return outExpr.str();
}