#include "LuaOMEParser.h"
#include "LuaFuncs.h"
#include "LuaMemPool.h"
#include <sstream>
#include <regex>
#include <iomanip>
#include <csignal>
#include "LuaHeaders.h"
#include "omeobjects/ModelInstance.h"
#include "LuaEvalSerializer.h"
#include "ISpatialDataProvider.h"
#include "ValCache.h"
#include "CompiledMdlFuncs.h"
#include "SDPLuaFuncs.h"

//lua header for MAXSTACK macro
#include"llimits.h"

///@cond DOX_NO_DOC
const struct luaL_Reg LuaOMEParser::OMEWrapLib[] = {

	{"__add",OMEWrapAdd},
	{"__sub",OMEWrapSub},
	{"__mul",OMEWrapMult},
	{"__div",OMEWrapDiv},
	{"__mod",OMEWrapMod},
	{"__pow",OMEWrapPow},
	{"__unm",OMEWrapUNM},

	//can't use these operators directly since 
	//lua won't return a list as part of a boolean comparison
	//{"__eq",OMEWrapEq},
	//{"__lt",OMEWrapLt},
	//{"__le",OMEWrapLe},
	//{"__concat",NULL},
	{"__len",OMEWrapLen},
	{"__tostring",OMEWrapToString},
	//{"__newindex",OMEWrapSetVal},
	{"__index",OMEWrapGetField},
	//{"__call",OMEWrapForwardCall},
	//{"__gc", OMEWrapDestroy},

	{NULL,NULL}
};

const struct luaL_Reg LuaOMEParser::mdlTblLib[] = {
	//{"__add",EvalAdd},
	//{"__sub",EvalSub},
	//{"__mul",EvalMult},
	//{"__div",EvalDiv},
	//{"__mod",EvalMod},
	//{"__pow",EvalPow},
	//{"__unm",EnumUNM},
	//{"__eq",EvalEq},
	//{"__lt",EvalLt},
	//{"__le",EvalLe},
	//{"__concat",NULL},
	//{"__len",NULL},
	{"__tostring",MTToString},
	//{"__newindex",EnumSetVal},
	{"__index",MTGetVal},
	//{"__call",EnumForwardCall},
	{NULL,NULL}
};

/// @endcond DOX_NO_DOC
namespace MEM_CACHES
{
	/** Global cache used to store scalar values used by the parser.*/
	ValCache<OME_SCALAR> m_valCache = ValCache<OME_SCALAR>();
};

/** Retrieve reference to global cache.
	@return Reference to global ValCache.
*/
ValCache<OME_SCALAR>& LuaOMEParser::GetCache()
{
	return MEM_CACHES::m_valCache;
}

LuaOMEParser::LuaOMEParser(void)
	:m_currExpr(NULL), m_pMakeDepthArray(NULL), m_pLState(NULL), m_pSerializer(NULL), m_ptrPool(10000)
	//, m_memPool(NULL), m_countAllocs(false), m_poolOffset(0), m_memAlloc(0)
{
	m_funcRegistry=DefaultLuaFuncs::GetDefaultRegistry();
	m_constRegistry=DefaultLuaFuncs::GetDefaultConstants();
	m_factories = DefaultLuaFuncs::GetFactories();
	Reset();
}

LuaOMEParser::LuaOMEParser(Model* mdl)
	:m_currExpr(NULL), m_pMakeDepthArray(NULL), m_pLState(NULL), m_pSerializer(NULL), m_ptrPool(10000)
	//, m_memPool(NULL), m_countAllocs(false), m_poolOffset(0), m_memAlloc(0)
{
	m_funcRegistry=DefaultLuaFuncs::GetDefaultRegistry();
	m_constRegistry=DefaultLuaFuncs::GetDefaultConstants();
	m_factories = DefaultLuaFuncs::GetFactories();
	Reset(mdl);

}

bool LuaOMEParser::Init(Model* pParentMdl)
{	
	if(m_pMakeDepthArray)
		delete m_pMakeDepthArray;
	m_pMakeDepthArray=new vector<int>();

	lua_pushlightuserdata(m_pLState,(void*)pParentMdl->GetManager());
	lua_setfield(m_pLState,LUA_REGISTRYINDEX,LuaDefines::MANAGER_ID);

	lua_pushlightuserdata(m_pLState,(void*)this);
	lua_setfield(m_pLState,LUA_REGISTRYINDEX,LuaDefines::PARSER_ID);

	lua_pushlightuserdata(m_pLState,(void*)&m_breaks);
	lua_setfield(m_pLState,LUA_REGISTRYINDEX,LuaDefines::BREAKS_ID);

	//load value
	lua_pushlightuserdata(m_pLState,(void*)m_pMakeDepthArray);
	//bind key-value;
	lua_setfield(m_pLState,LUA_REGISTRYINDEX,LuaDefines::MAKE_DEPTH_ID);

	if(m_currExpr)
		m_currExpr=NULL;

	//define funcs here
	std::for_each(m_funcRegistry.begin(),m_funcRegistry.end(),[this](OMELuaFunc* func){func->RegisterFunc(m_pLState);});

	//define ops
	lua_pushcfunction(m_pLState, LuaOMEParser::OMEWrapEq);
	lua_pushcfunction(m_pLState, LuaOMEParser::OMEWrapLt);
	lua_pushcfunction(m_pLState, LuaOMEParser::OMEWrapLe);
	lua_pushcfunction(m_pLState, LuaOMEParser::OMEWrapNe);
	lua_pushcfunction(m_pLState, LuaOMEParser::OMEWrapGt);
	lua_pushcfunction(m_pLState, LuaOMEParser::OMEWrapGe);
	lua_setglobal(m_pLState,"OMEWrapGe");
	lua_setglobal(m_pLState,"OMEWrapGt");
	lua_setglobal(m_pLState,"OMEWrapNe");
	lua_setglobal(m_pLState,"OMEWrapLe");
	lua_setglobal(m_pLState,"OMEWrapLt");
	lua_setglobal(m_pLState,"OMEWrapEq");

	std::for_each(m_constRegistry.begin(),m_constRegistry.end(),[this](std::pair<const STLString,OME_SCALAR> & entry){ DefineConstant(entry.first.c_str(),entry.second);});
	//we may want to cache function info elsewhere
	return true;
}

void LuaOMEParser::PrepTempDataHandler()
{
	//OMECFuncs::OverrideWrapPoolAllocator(LuaBWNew, LuaBWRelease, LuaBWClear, LuaBWRefit, this);
}

LuaOMEParser::~LuaOMEParser(void)
{
	lua_close(m_pLState);

	if(m_pMakeDepthArray)
		delete m_pMakeDepthArray;

	if(m_currExpr)
		delete[] m_currExpr;

	std::for_each(m_funcRegistry.begin(),m_funcRegistry.end(),[](OMELuaFunc* func){delete func;});
	DisposeStaticArrays();
	
}

bool LuaOMEParser::CompileExpression(const char* expr, OMEObject* pCaller)
{
	///todo modify expression modification to only be required once; cache with eval.
	int ret;
	if(m_batchStack.empty())
	{
		static const size_t resultLen=strlen(LuaDefines::EVAL_RESULT_RETURN);

		if(m_currExpr)
			delete[] m_currExpr;

		STLString dup(LuaOMEParser::LuaizeExpression(expr));
	
		m_currExpr=new char[resultLen+dup.length()+1];
		SAFE_STRCPY(dup.c_str(), m_currExpr, resultLen + dup.length() + 1);

		m_currExpr=strcat(m_currExpr,dup.c_str());

		//keep track of the currently calling object
		newOMERep(m_pLState,dynamic_cast<Evaluable*>(pCaller));
		//bind key-value;
		lua_setglobal(m_pLState,LuaDefines::EVAL_CALLER_ID);
		
		ret=luaL_loadstring(m_pLState,m_currExpr);

	#ifdef _DEBUG
		if(ret)
			std::cout<<"Err: "<<lua_tostring(m_pLState,-1);
	#endif
	}
	else
		ret=true;
	return !ret;
}

bool LuaOMEParser::TestExpression(const char* expr)
{
	//expression should be compiled before calling TestExpression; expr ignored.
	OME_SCALAR dummy;
   return Evaluate(dummy,true);
}

void LuaOMEParser::PopBatch() 
{
	if(!m_batchStack.empty())
	{
		luaL_unref(m_pLState,LUA_REGISTRYINDEX,m_batchStack.top());
		m_batchStack.pop();
	}
}

void LuaOMEParser::PushBatch(const OMEChar* batch)
{
	int err=luaL_loadstring(m_pLState,batch);
	if(err!=LUA_OK)
	{
		const STLString errType= err==LUA_ERRRUN ? "Runtime ": "Memory ";
		STLString details=lua_tostring(m_pLState,-1);

		lua_getfield(m_pLState,LUA_REGISTRYINDEX,LuaDefines::MANAGER_ID);
		BaseManager *pBm=(BaseManager*)lua_touserdata(m_pLState,-1);
		if(pBm)
			pBm->ReportError("Batch compile "+errType+" error: "+details,NULL,EC_CRITICAL);

		DBG_SETTXCOLOR(DBG_RED);
		DBG_COLOR(DBG_PRINT("Batch ERR: "+details),DBG_RED);
		DBG_SETTXCOLOR(DBG_DEFAULT);
	}
	//cache compiled statement
	m_batchStack.push(luaL_ref(m_pLState,LUA_REGISTRYINDEX));
}

bool LuaOMEParser::Evaluate(OME_SCALAR & value, const bool isTest,const SclrArgs & args)
{
	lua_pushboolean(m_pLState,isTest);
	lua_setfield(m_pLState,LUA_REGISTRYINDEX,LuaDefines::TEST_MODE);

	for(size_t i=0; i<args.size();i++)
		lua_pushnumber(m_pLState,args[i]);
	
	//load error handler
	lua_pushcfunction(m_pLState,ErrHandler);
	lua_insert(m_pLState,1);
	
	ActivatePool();
	int err=lua_pcall(m_pLState,args.size(),1,1);

	//grab the last item on the stack if non batch; else ignore any output (except errors)
	ProcessResult(err,m_batchStack.empty() ? -1 : 0,value);
	
	//clear stack
	lua_settop(m_pLState,0);
	DeactivatePool();
   return !err;
}

bool LuaOMEParser::Evaluate(SclrResults & values, int & count, const bool isTest,const SclrArgs & args)
{
	lua_pushboolean(m_pLState,isTest);
	lua_setfield(m_pLState,LUA_REGISTRYINDEX,LuaDefines::TEST_MODE);

	for(size_t i=0; i<args.size();i++)
		lua_pushnumber(m_pLState,args[i]);

	//load error handler
	lua_pushcfunction(m_pLState,ErrHandler);
	lua_insert(m_pLState,1);
	ActivatePool();

	int err=lua_pcall(m_pLState,args.size(),LUA_MULTRET,1);

	//grab result?
	//remove c function from stack
	lua_remove(m_pLState, 1);
	count=lua_gettop(m_pLState);
	if (values.size() < count)
		values.resize(count);

	if(count)
	{
		for(int i=1; i<=count; i++)
			ProcessResult(err,i,values[i-1]);
		
	}
	else
	{
		OME_SCALAR dummy;
		ProcessResult(err,0,dummy); //just check for errors
	}

		//clear stack
	lua_settop(m_pLState,0);
	DeactivatePool();

	return !err;
}

bool LuaOMEParser::Evaluate(OME_SCALAR* & values, int & count, const bool isTest,const SclrArgs & args)
{
	lua_pushboolean(m_pLState,isTest);
	lua_setfield(m_pLState,LUA_REGISTRYINDEX,LuaDefines::TEST_MODE);

	for(size_t i=0; i<args.size();i++)
		lua_pushnumber(m_pLState,args[i]);

	//load error handler
	lua_pushcfunction(m_pLState,ErrHandler);
	lua_insert(m_pLState,1);
	ActivatePool();

	int err=lua_pcall(m_pLState,args.size(),LUA_MULTRET,1);
	
	//grab result?
	//remove c function from stack
	lua_remove(m_pLState, 1);
	//remove err arg from stack
	lua_remove(m_pLState, 1);

	int top=lua_gettop(m_pLState);

	if(count<0)
		values=NULL;
	if(count<0 || top<count)
		count=top;

	if(count)
	{
		//only allocate if count was negative
		if(!values)
			values=new OME_SCALAR[count];

		for(int i=1; i<=count; i++)
			ProcessResult(err,i,values[i-1]);

		lua_pop(m_pLState,count);
	}
	else
	{
		OME_SCALAR dummy;
		ProcessResult(err,0,dummy); //just check for errors
	}

		//clear stack
	lua_settop(m_pLState,0);
	DeactivatePool();

	return !err;
}

/** Process the results from a lua evaluation session.
	@param err Any error values that resulted from lua_pcall().
	@param index The stack index for the value we are interested in. An index of 0 means no return value is expected.
	@param value The variable to be populated by the value at index, unless index is equal to 0.
*/
void LuaOMEParser::ProcessResult(const int & err, int index, OME_SCALAR & value)
{
	Evaluable* repCast;
	if(err==LUA_OK)
	{
		if(index) //an index of zero means we are not interested in output.
		{
			repCast=LuaDefines::CheckWrapEval(m_pLState,OME_EVAL,index);
			//repCast=rep->GetEval();

			if(!repCast)
			{
				switch(lua_type(m_pLState,index))
				{
				case LUA_TNUMBER:
					value=(OME_SCALAR)lua_tonumber(m_pLState,index);
					break;
				case LUA_TBOOLEAN:
					value=(lua_toboolean(m_pLState,index)!=0);
					break;
				case LUA_TNIL:
					value=0.0; //treat nils as zeros, like C
					break;
				case LUA_TTABLE:
				case LUA_TSTRING:

				default:
					break;
				}
			}
			else
				value=repCast->GetValue();
		}
	}
	else
	{
		if(lua_isstring(m_pLState,-1))
		{
			const STLString errType= err==LUA_ERRRUN ? "Runtime ": "Memory ";
			STLString details=lua_tostring(m_pLState,-1);

			lua_getfield(m_pLState,LUA_REGISTRYINDEX,LuaDefines::MANAGER_ID);
			BaseManager *pBm=(BaseManager*)lua_touserdata(m_pLState,-1);
			if(pBm)
				pBm->ReportError("Expression "+errType+" error: "+details,NULL,EC_CRITICAL);

			DBG_COLOR(DBG_PRINT("ERR: "+details),DBG_RED);
			
         if(!m_pSerializer || m_pSerializer->GetDbgEnabled())
         {
			      OMEDBG_COLOR lastColor;
			      DBG_GETTXCOLOR(lastColor);
			      DBG_SETTXCOLOR(DBG_CYAN);
			      GoPrompt();
			      DBG_SETTXCOLOR(lastColor);
         }
		}
	}
}

bool LuaOMEParser::BatchEvaluate(const SclrArgs & args, const bool isTest)
{
	bool ret=false;
	OME_SCALAR dummy;
	if(!m_batchStack.empty())
	{
		lua_rawgeti(m_pLState,LUA_REGISTRYINDEX,m_batchStack.top());
		ret=Evaluate(dummy,isTest,args);
		//try 
		//{
		//	LUAP_MEM::repPool.RefitPool();	
		//}
		//catch(std::exception & e)
		//{
		//	DBG_COLOR(cout<<e.what()<<endl,DBG_RED);
		//}

	}
	else
		ret=false;

	return ret;
}

bool LuaOMEParser::BatchEvaluate(SclrResults & values, int & count, const SclrArgs & args, const bool isTest)
{
	bool ret=false;
	if(!m_batchStack.empty())
	{
		lua_rawgeti(m_pLState,LUA_REGISTRYINDEX,m_batchStack.top());
		ret=Evaluate(values,count,isTest,args);
		//try 
		//{
		//	LUAP_MEM::repPool.RefitPool();	
		//}
		//catch(std::exception & e)
		//{
		//	DBG_COLOR(cout<<e.what()<<endl,DBG_RED);
		//}
	}
	else
		ret=false;

	return ret;
}

bool LuaOMEParser::BatchEvaluate(OME_SCALAR* & values, int & count,const SclrArgs & args, const bool isTest)
{
	bool ret=false;
	if(!m_batchStack.empty())
	{
		//for error handler positioning
		lua_pushinteger(m_pLState, 0);
		
		lua_rawgeti(m_pLState,LUA_REGISTRYINDEX,m_batchStack.top());
		ret=Evaluate(values,count,isTest,args);
		//try 
		//{
		//	LUAP_MEM::repPool.RefitPool();	
		//}
		//catch(std::exception & e)
		//{
		//	DBG_COLOR(cout<<e.what()<<endl,DBG_RED);
		//}
	}
	else
		ret=false;

	return ret;
}


bool LuaOMEParser::DefineVar(const OMEChar* varName, Evaluable *pEval)
{
	bool useLast=lua_gettop(m_pLState);
	//if(pEval->GetIsConstant())
	//	lua_pushnumber(m_pLState,pEval->GetValue());
	//else
	//{
		newOMERep(m_pLState,pEval);
	//}
	//DBG_PRINT(StackDump());
	if(useLast)
		lua_setfield(m_pLState,-2,varName);
	else
		lua_setglobal(m_pLState,varName);

	return true;
}

bool LuaOMEParser::DefineVar(const OMEChar* varName, OME_SCALAR *pSclr)
{
	//disabled 

	//bool useLast=lua_gettop(m_pLState);
	////if(pEval->GetIsConstant())
	////	lua_pushnumber(m_pLState,pEval->GetValue());
	////else
	////{
	//	newOMERep(m_pLState,pSclr);
	////}
	////DBG_PRINT(StackDump());
	//if(useLast)
	//	lua_setfield(m_pLState,-2,varName);
	//else
	//	lua_setglobal(m_pLState,varName);

	return false;
}

bool LuaOMEParser::RedefineVar(const char* varName, Evaluable *pEval)
{
	// Disabled

	DefineVar(varName,pEval);
	//StrList path;
	//if(pEval->GetParentModel())
	//	pEval->GetParentModel()->GetUpPath(path);
	//path.push_back(varName);
	//
	//const char* useName=ConcatStrings(path.begin(),path.end(),'.').c_str();

	//lua_getfield(m_pLState,-1,useName);

	//if(!lua_isnil(m_pLState,-1))
	//{
	//	LuaOMERep* pRep=checkOMERep(m_pLState,-1);
	//	if(pRep)
	//		pRep->SetEvaluable(pEval);
	//}

	return false;
}

//bool LuaOMEParser::RegisterFunction(const char* funcName, int numArgs)
//{
//	//may need to subclass MTFunctoinI
//	return false;
//}

OMEParser* LuaOMEParser::Copy()
{
	LuaOMEParser* ret= new LuaOMEParser();
	/// \todo fill in copy()

	return ret;
}

void LuaOMEParser::DefineEnumVals(const ModelEnum & enumVals)
{
	//using namespace LuaDefines;

	//const char* enumType=enumVals.GetEnumClassName().c_str();
	////enumeration type; value is equal to total number of enum values.
	//EnumRep temp(enumType,enumVals.GetEnumValueCount(),enumType);

	//int topTable=lua_gettop(m_pLState);

	//int i;
	//temp.valCount = enumVals.GetEnumValueCount();
	//temp.valNames=new const char*[temp.valCount];
	//for (i = 0; i < temp.valCount; i++)
	//	temp.valNames[i] = enumVals.GetEnumValue(i).c_str();

	////load enum values into Lua space
	//EnumRep* repPtr= (EnumRep*)lua_newuserdata(m_pLState,sizeof(temp));
	//*repPtr=EnumRep();
	//*repPtr=temp;

	//
	////registeredNames.push_back(temp.name);

	//for(i=0; i<enumVals.GetEnumValueCount();i++)
	//{
	//	//add each enumerated value and assign their numerical value in order
	//	temp=EnumRep(enumVals.GetEnumValue(i).c_str(),i,enumType);
	//	repPtr= (EnumRep*)lua_newuserdata(m_pLState,sizeof(temp));
	//	*repPtr=temp;

	//	luaL_getmetatable(m_pLState,ENUM_META_ID);
	//	lua_setmetatable(m_pLState,-2);
	//	if(topTable)
	//		lua_setfield(m_pLState,topTable,temp.name);
	//	else
	//		lua_setglobal(m_pLState,temp.name);
	//	//registeredNames.push_back(temp.name);
	//}
}

void LuaOMEParser::RedefineEnumVals(const ModelEnum & enumVals)
{
	//for now
	DefineEnumVals(enumVals);
}

bool LuaOMEParser::DefineArray(const OMEChar* varName, SclrVArray* pArr)
{
	//bool useLast=lua_gettop(m_pLState);
	//newOMERep(m_pLState,pArr);
	//
	////DBG_PRINT(StackDump());
	//if(useLast)
	//	lua_setfield(m_pLState,-2,varName);
	//else
	//	lua_setglobal(m_pLState,varName);

	return true;
}

bool LuaOMEParser::DefineParameter(Param* pParam)
{
	return false;
}

bool LuaOMEParser::RedefineParameter(Param* pParam)
{
	return false;
}

bool LuaOMEParser::DefineConstant(const OMEChar* constName, const int & val)
{
	bool ret=false;
	lua_getglobal(m_pLState,constName);

	if(lua_isnil(m_pLState,-1))
	{
		lua_pushinteger(m_pLState,val);
		lua_setglobal(m_pLState,constName);
		ret=true;
	}
	lua_pop(m_pLState,1);
	return ret;
}

bool LuaOMEParser::DefineConstant(const OMEChar* constName, const OME_SCALAR & val)
{
	bool ret=false;
	lua_getglobal(m_pLState,constName);

	if(lua_isnil(m_pLState,-1))
	{
		lua_pushnumber(m_pLState,val);
		lua_setglobal(m_pLState,constName);
		ret=true;
	}
	lua_pop(m_pLState,1);
	return ret;
}

bool LuaOMEParser::DefineConstant(const OMEChar* constName, const OMEChar* & val)
{
	bool ret=false;
	lua_getglobal(m_pLState,constName);

	if(lua_isnil(m_pLState,-1))
	{
		lua_pushstring(m_pLState,val);
		lua_setglobal(m_pLState,constName);
		ret=true;
	}
	lua_pop(m_pLState,1);
	return ret;
}

bool LuaOMEParser::RedefineConstant(const OMEChar* constName, const int & val)
{
	bool ret=false;
	lua_getglobal(m_pLState,constName);

	if(!lua_isnil(m_pLState,-1))
	{
		lua_pushinteger(m_pLState,val);
		lua_setglobal(m_pLState,constName);
		ret=true;
	}
	lua_pop(m_pLState,1);
	return ret;
}

bool LuaOMEParser::RedefineConstant(const OMEChar* constName, const OME_SCALAR & val)
{
	bool ret=false;
	lua_getglobal(m_pLState,constName);

	if(!lua_isnil(m_pLState,-1))
	{
		lua_pushnumber(m_pLState,val);
		lua_setglobal(m_pLState,constName);
		ret=true;
	}
	lua_pop(m_pLState,1);
	return ret;
}

bool LuaOMEParser::RedefineConstant(const OMEChar* constName, const OMEChar* & val)
{
	bool ret=false;
	lua_getglobal(m_pLState,constName);

	if(!lua_isnil(m_pLState,-1))
	{
		lua_pushstring(m_pLState,val);
		lua_setglobal(m_pLState,constName);
		ret=true;
	}
	lua_pop(m_pLState,1);
	return ret;
}

bool LuaOMEParser::DefineSubmodel(Model* pMdl)
{
	//EvalArray evals;
	//pMdl->GetAllEvals(evals, DT_SHALLOW);
	////register evals
	//for (auto itr = evals.begin(); itr != evals.end(); ++itr)
	//{
	//	lua_pushstring(m_pLState, (*itr)->GetExprName().c_str());
	//	newOMERep(m_pLState, (*itr));
	//}
	return true;
}

bool LuaOMEParser::DefineAll(Model* pMdl)
{
	/**
		
		1. register root in global space. as model metatable
		2. register all objects in global table:
			_Evaluables (as wrapper with name based on id)
			_Models (as ptr using id name)
			_Enum collections (as wrapper with name based on model id+label)
			_Enum values (as wrapper with name based on model id+enumtype+label)
		3. recurse into submodels and repeat steps 2 and 3.
	*/

	//add root model
	newMdlPtr(m_pLState, pMdl);
	lua_setglobal(m_pLState, pMdl->GetExprName().c_str());


	//place evaluable object on table;
	lua_getglobal(m_pLState, LuaDefines::MDL_COMP_TABLE);
	RegisterModel(pMdl, lua_gettop(m_pLState));

	lua_pop(m_pLState, 1);
	return true;
}

/** Register a model to its appropriate spot within the Model heirarchy.
	@param pMdl Pointer to the Model to register.
	@param valTblIdx The index of the Lua table to associate with pMdl.
*/
void LuaOMEParser::RegisterModel(Model* pMdl, const int & valTblIdx)
{
	lua_pushstring(m_pLState, pMdl->GetID().c_str());
	newMdlPtr(m_pLState, pMdl);

	lua_settable(m_pLState, valTblIdx);

	EvalArray evals;
	pMdl->GetAllEvals(evals, DT_SHALLOW | DT_ALIAS);

	//register evals
	for (auto itr = evals.begin(); itr != evals.end(); ++itr)
	{
		Evaluable* pCurr = (*itr);
		if (pCurr->IsOMEType(OME_ALIAS))
			pCurr = ((EvalAlias*)pCurr)->GetSrc();
		lua_pushstring(m_pLState, (*itr)->GetID().c_str());
		newOMERep(m_pLState, pCurr);
		lua_settable(m_pLState, valTblIdx);
	}

	//register enums
	for (int i = 0; i < pMdl->GetModelEnumCount(); ++i)
	{
		ModelEnum& me = pMdl->GetModelEnum(i);
		STLString meLbl = pMdl->GetID() + "_" + ScrubName(me.GetEnumClassName());
		lua_pushstring(m_pLState, meLbl.c_str());
		newOMERep(m_pLState, &me);
		lua_settable(m_pLState, valTblIdx);

		//register enum values
		for (int j = 0; j < me.GetEnumValueCount(); ++j)
		{
			lua_pushstring(m_pLState, (meLbl + "_" + me.GetEnumValue(i)).c_str());
			newOMERep(m_pLState, &me.GetEnumEntry(i));
			lua_settable(m_pLState, valTblIdx);
		}
	}

	//register and parse models
	for (int i = 0; i < pMdl->GetSubmodelCount(); ++i)
	{
		Model* pSub = pMdl->GetSubmodel(i);
		
		//register contents of submodel
		RegisterModel(pSub, valTblIdx);
	}
}

bool LuaOMEParser::UpdateSubmodelEntries(Model* pMdl, const int start,const int count)
{
	using namespace LuaDefines;

	//int mcInd=lua_gettop(m_pLState);
	//if(!mcInd)
	//{
	//	StrList path;
	//	pMdl->GetUpPath(path);

	//	//get parent table
	//	lua_getglobal(m_pLState,ConcatStrings(path.begin(),path.end(),'.').c_str());
	//	mcInd=lua_gettop(m_pLState);
	//}
	//EvalArray vars;
	//pMdl->GetVarsToRegister(vars);

	////add link to collection.
	//newOMERep(m_pLState,pMdl);
	//lua_setfield(m_pLState,mcInd,TABLE_MC_SRC);

	//std::for_each(vars.begin(),vars.end(),[this](Evaluable *pE){ DefineVar(pE->GetExprName().c_str(),pE); });

	return true;
}

bool LuaOMEParser::DefineAssoc(SubmodelAssoc* pSa)
{
	//newOMERep(m_pLState,pSa);
	//const char* name=pSa->GetExprName().c_str();
	//lua_setglobal(m_pLState,name);
	////registeredNames.push_back(name);
	
	return true;
}

/** Load Lua Metatable definitions used by LuaOMEParser. */
void LuaOMEParser::LoadMetatables()
{
	using namespace LuaDefines;
	//load interface for Evaluable objects
	if(!luaL_newmetatable(m_pLState,WRAP_META_ID))
		DBG_PRINT_FLAGGED("Wrapper metatable failure",DBG_CRIT);
	luaL_register(m_pLState,NULL,OMEWrapLib);

	if(!luaL_newmetatable(m_pLState,MT_META_ID))
		DBG_PRINT_FLAGGED("ModelTable metatable failure",DBG_CRIT);
	luaL_register(m_pLState,NULL,mdlTblLib);

	//metatables registered, remove from stack
	lua_pop(m_pLState,2);
}

/** Copy lua field from one state to another.
	@param pIn the lua_State to copy the field from.
	@param pOut the lua_State to copy the field to.
	@param ind index of the field to copy.
	@return 1 if copy was successful, 0 otherwise
*/
int LuaOMEParser::CopyLuaField(lua_State* pIn, lua_State* pOut, int ind)
{
	// solution found: http://lua-users.org/lists/lua-l/2010-03/msg00531.html
	int success=1;
	EnumRep* pEnumData,*pEnumCpy;
	
	switch(lua_type(pIn,ind))
	{
	case LUA_TNIL:
		lua_pushnil(pOut);
		break;
	case LUA_TBOOLEAN:
		lua_pushboolean(pOut,lua_toboolean(pIn,ind));
		break;
	case LUA_TNUMBER:
		lua_pushnumber(pOut,lua_tonumber(pIn,ind));
		break;
	case LUA_TSTRING:
		lua_pushstring(pOut,lua_tostring(pIn,ind));
		break;
	case LUA_TLIGHTUSERDATA:
	case LUA_TUSERDATA:
	//	//assume OMEEvaluable
	//	
	//	if((pEnumData=checkEnum(pIn,ind)))
	//	{
	//		pEnumCpy=(EnumRep*)lua_newuserdata(pOut,sizeof(*pEnumData));
	//		*pEnumCpy=*pEnumData;
	//		luaL_getmetatable(pOut,LuaDefines::ENUM_META_ID);
	//		lua_setmetatable(pOut,-2);
	//	}
	//	else if((pRepData=checkOMERep(pIn,ind)))
	//	{
	//		newOMERep(pOut,*pRepData);
	//	}
	//	else
	//	{
			DBG_PRINT_FLAGGED("UNKNOWN UDATA TYPE FOUND DURING COPY",DBG_WARN);
			success=0;
	//	}
		break;
	//case LUA_TUSERDATA:
	//	//assume OMEEnum
	//	udata=(EnumRep*)lua_touserdata(pIn,ind);
	//	cpy=(EnumRep*)lua_newuserdata(pOut,sizeof(*udata));
	//	*cpy=*udata;
	//	luaL_getmetatable(pOut,ENUM_META_ID);
	//	lua_setmetatable(pOut,-2);
	//	break;
	case LUA_TFUNCTION:
		lua_pushcfunction(pOut,lua_tocfunction(pIn,ind));

		break;
	default:
		success=0;
	};
	return success;
}

/** Copy lua Table from one state to another.
	@param pIn the lua_State to copy the table from.
	@param pOut the lua_State to copy the table to.
	@param ind index of the table to copy.
	@return 1 if the copy was successful, 0 otherwise.
*/
int LuaOMEParser::CopyLuaTable(lua_State* pIn, lua_State* pOut, int ind)
{
	// solution found: http://lua-users.org/lists/lua-l/2010-03/msg00531.html
	int success=1;
	int curr;
	lua_newtable(pOut);
	curr=lua_gettop(pOut);

	lua_pushnil(pIn);	
	while(lua_next(pIn,ind) != 0 && success)
	{
		success=CopyLuaField(pIn,pOut,-2);
		if(success)
		{
			if(lua_type(pIn,-1)==LUA_TTABLE)
				success=CopyLuaTable(pIn,pOut,lua_gettop(pIn));
			else
				success=CopyLuaField(pIn,pOut,-1);
			lua_settable(pOut,curr);
		}
		lua_pop(pIn,1);
		
	}
	return success;
}

STLString LuaOMEParser::GetModeID() const
{ 
	return !m_ehStack.empty() ? m_ehStack.top() : STLString(); 
}

void LuaOMEParser::PushModeID(const STLString & eh)
{ 
	m_ehStack.push(eh);
}

STLString LuaOMEParser::PopModeID()
{
	STLString out;
	if(!m_ehStack.empty())
	{
		out=m_ehStack.top();
		m_ehStack.pop();
	}
	return out;
}

void LuaOMEParser::ClearModeIDs()
{
	while(!m_ehStack.empty())
		m_ehStack.pop();
}

/// @cond DOX_NO_DOC
bool LuaOMEParser::CheckMultiVal(lua_State* L,const int ind)
{
	//
	using namespace LuaDefines;

	bool ret;
	OMECFuncs::BaseWrap *pRep=checkOMEWrap(L,ind);
	ret=((pRep && pRep->m_type==OMECFuncs::BaseWrap::LIST) || lua_istable(L,ind));

	return ret;
}

/* NOTE: the following two defines are bad practice,
	But I wanted the repititious code to be inlined without
	having to write it out for each operator.
	Don't do this.
	-PCW

*/
#define DEFINE_LUAOP(FNAME,OP)                                 \
int LuaOMEParser::FNAME(lua_State* L)					       \
{															   \
	using namespace OMECFuncs;								   \
															   \
	SclrWrap wRhs(0.0), wLhs(0.0);							   \
	BaseWrap* lhs = checkOMEWrap(L, 1);						   \
	if (!lhs)												   \
		lhs = new(&wLhs)SclrWrap(lua_tonumber(L, 1));		   \
															   \
	BaseWrap* rhs = checkOMEWrap(L, 2);						   \
	if (!rhs)												   \
		rhs = new(&wRhs)SclrWrap(lua_tonumber(L, 2));		   \
															   \
	using namespace OMECFuncs;																		                        \
		enum {SS,SE,SL,SA,SM,SC,                                                                                                \
            ES,EE,EL,EA,EM,EC,                                                                                                  \
            LS,LE,LL,LA,LM,LC,                                                                                                  \
            AS,AE,AL,AA,AM,AC,                                                                                                  \
            MS,ME,ML,MA,MM,MC,                                                                                                  \
            CS,CE,CL,CA,CM,CC};							                                                                        \
		unsigned short action = 0;										                                                        \
		action = lhs->m_type*BaseWrap::TYPECOUNT;					                                                            \
		action += rhs->m_type;											                                                        \
																				                                                \
		switch (action)													                                                        \
      {									                                                                                        \
		case SS: newOMERep(L,lhs->m_scalar OP rhs->m_scalar); break;						                                        \
		case SE: newOMERep(L,lhs->m_scalar OP rhs->m_pEval->GetValue()); break;		                                            \
		case SL: newOMERep(L,lhs->m_scalar OP *rhs->m_pList); break;						                                        \
		case SA: newOMERep(L,lhs->m_scalar OP *((AliasWrap*)rhs)->m_pRepVal); break;	                                            \
      case SM: newOMERep(L,lhs->m_scalar OP rhs->m_pEe->ind); break;                                                              \
      case SC: newOMERep(L,lhs->m_scalar OP rhs->m_pEColl->GetEnumValueCount()); break;                                           \
		case ES: newOMERep(L,lhs->m_pEval->GetValue() OP rhs->m_scalar); break;			                                        \
		case EE: newOMERep(L,lhs->m_pEval->GetValue() OP rhs->m_pEval->GetValue()); break;                                        \
		case EL: newOMERep(L,lhs->m_pEval->GetValue() OP *rhs->m_pList); break;			                                        \
		case EA: newOMERep(L,lhs->m_pEval->GetValue() OP *((AliasWrap*)rhs)->m_pRepVal); break;	                                \
      case EM: newOMERep(L,lhs->m_pEval->GetValue() OP rhs->m_pEe->ind); break;                                                   \
      case EC: newOMERep(L,lhs->m_pEval->GetValue() OP rhs->m_pEColl->GetEnumValueCount()); break;                                \
		case LS: newOMERep(L,*lhs->m_pList OP rhs->m_scalar); break;						                                        \
		case LE: newOMERep(L,*lhs->m_pList OP rhs->m_pEval->GetValue()); break;			                                        \
		case LL: newOMERep(L,*lhs->m_pList OP *rhs->m_pList); break;						                                        \
		case LA: newOMERep(L,*lhs->m_pList OP *((AliasWrap*)rhs)->m_pRepVal); break;			                                    \
      case LM: newOMERep(L,*lhs->m_pList OP rhs->m_pEe->ind); break;                                                              \
      case LC: newOMERep(L,*lhs->m_pList OP rhs->m_pEColl->GetEnumValueCount()); break;                                           \
		case AS: newOMERep(L,*((AliasWrap*)lhs)->m_pRepVal OP rhs->m_scalar); break;			                                    \
		case AE: newOMERep(L,*((AliasWrap*)lhs)->m_pRepVal OP rhs->m_pEval->GetValue()); break;			                        \
		case AL: newOMERep(L,*((AliasWrap*)lhs)->m_pRepVal OP *rhs->m_pList); break;						                        \
		case AA: newOMERep(L,*((AliasWrap*)lhs)->m_pRepVal OP *((AliasWrap*)rhs)->m_pRepVal); break;	                            \
      case AM: newOMERep(L,*((AliasWrap*)lhs)->m_pRepVal OP rhs->m_pEe->ind); break;                                              \
      case AC: newOMERep(L,*((AliasWrap*)lhs)->m_pRepVal OP rhs->m_pEColl->GetEnumValueCount()); break;                           \
      case MS: newOMERep(L,lhs->m_pEe->ind OP rhs->m_scalar); break;						                                        \
      case ME: newOMERep(L,lhs->m_pEe->ind OP rhs->m_pEval->GetValue()); break;		                                            \
      case ML: newOMERep(L,lhs->m_pEe->ind OP *rhs->m_pList); break;						                                        \
      case MA: newOMERep(L,lhs->m_pEe->ind OP *((AliasWrap*)rhs)->m_pRepVal); break;	                                            \
      case MM: newOMERep(L,lhs->m_pEe->ind OP rhs->m_pEe->ind); break;                                                            \
      case MC: newOMERep(L,lhs->m_pEe->ind OP rhs->m_pEColl->GetEnumValueCount()); break;                                         \
      case CS: newOMERep(L,lhs->m_pEColl->GetEnumValueCount() OP rhs->m_scalar); break;						                    \
      case CE: newOMERep(L,lhs->m_pEColl->GetEnumValueCount() OP rhs->m_pEval->GetValue()); break;		                        \
      case CL: newOMERep(L,lhs->m_pEColl->GetEnumValueCount() OP *rhs->m_pList); break;						                    \
      case CA: newOMERep(L,lhs->m_pEColl->GetEnumValueCount() OP *((AliasWrap*)rhs)->m_pRepVal); break;	                        \
      case CM: newOMERep(L,lhs->m_pEColl->GetEnumValueCount() OP rhs->m_pEe->ind); break;                                         \
      default:/*case CC:*/ newOMERep(L,lhs->m_pEColl->GetEnumValueCount() OP rhs->m_pEColl->GetEnumValueCount()); break;          \
      };		                                    											                                \
	return 1;												   \
}															   \


#define DEFINE_LUABOOLOP(FNAME,OP)                                 \
int LuaOMEParser::FNAME(lua_State* L)					       \
{															   \
	using namespace OMECFuncs;								   \
															   \
	SclrWrap wRhs(0.0), wLhs(0.0);							   \
	BaseWrap* lhs = checkOMEWrap(L, 1);						   \
	if (!lhs)												   \
		lhs = new(&wLhs)SclrWrap(lua_tonumber(L, 1));		   \
															   \
	BaseWrap* rhs = checkOMEWrap(L, 2);						   \
	if (!rhs)												   \
		rhs = new(&wRhs)SclrWrap(lua_tonumber(L, 2));		   \
															   \
	using namespace OMECFuncs;																		                        \
		enum {SS,SE,SL,SA,SM,SC,                                                                                                \
            ES,EE,EL,EA,EM,EC,                                                                                                  \
            LS,LE,LL,LA,LM,LC,                                                                                                  \
            AS,AE,AL,AA,AM,AC,                                                                                                  \
            MS,ME,ML,MA,MM,MC,                                                                                                  \
            CS,CE,CL,CA,CM,CC};							                                                                        \
		unsigned short action = 0;										                                                        \
		action = lhs->m_type*BaseWrap::TYPECOUNT;					                                                            \
		action += rhs->m_type;											                                                        \
																				                                                \
		switch (action)													                                                        \
      {									                                                                                        \
		case SS: lua_pushboolean(L,lhs->m_scalar OP rhs->m_scalar); break;						                                        \
		case SE: lua_pushboolean(L,lhs->m_scalar OP rhs->m_pEval->GetValue()); break;		                                            \
		case SL: newOMERep(L,lhs->m_scalar OP *rhs->m_pList); break;						                                        \
		case SA: lua_pushboolean(L,lhs->m_scalar OP *((AliasWrap*)rhs)->m_pRepVal); break;	                                            \
      case SM: lua_pushboolean(L,lhs->m_scalar OP rhs->m_pEe->ind); break;                                                              \
      case SC: lua_pushboolean(L,lhs->m_scalar OP rhs->m_pEColl->GetEnumValueCount()); break;                                           \
		case ES: lua_pushboolean(L,lhs->m_pEval->GetValue() OP rhs->m_scalar); break;			                                        \
		case EE: lua_pushboolean(L,lhs->m_pEval->GetValue() OP rhs->m_pEval->GetValue()); break;                                        \
		case EL: newOMERep(L,lhs->m_pEval->GetValue() OP *rhs->m_pList); break;			                                        \
		case EA: lua_pushboolean(L,lhs->m_pEval->GetValue() OP *((AliasWrap*)rhs)->m_pRepVal); break;	                                \
      case EM: lua_pushboolean(L,lhs->m_pEval->GetValue() OP rhs->m_pEe->ind); break;                                                   \
      case EC: lua_pushboolean(L,lhs->m_pEval->GetValue() OP rhs->m_pEColl->GetEnumValueCount()); break;                                \
		case LS: newOMERep(L,*lhs->m_pList OP rhs->m_scalar); break;						                                        \
		case LE: newOMERep(L,*lhs->m_pList OP rhs->m_pEval->GetValue()); break;			                                        \
		case LL: newOMERep(L,*lhs->m_pList OP *rhs->m_pList); break;						                                        \
		case LA: newOMERep(L,*lhs->m_pList OP *((AliasWrap*)rhs)->m_pRepVal); break;			                                    \
      case LM: newOMERep(L,*lhs->m_pList OP rhs->m_pEe->ind); break;                                                              \
      case LC: newOMERep(L,*lhs->m_pList OP rhs->m_pEColl->GetEnumValueCount()); break;                                           \
		case AS: lua_pushboolean(L,*((AliasWrap*)lhs)->m_pRepVal OP rhs->m_scalar); break;			                                    \
		case AE: lua_pushboolean(L,*((AliasWrap*)lhs)->m_pRepVal OP rhs->m_pEval->GetValue()); break;			                        \
		case AL: newOMERep(L,*((AliasWrap*)lhs)->m_pRepVal OP *rhs->m_pList); break;						                        \
		case AA: lua_pushboolean(L,*((AliasWrap*)lhs)->m_pRepVal OP *((AliasWrap*)rhs)->m_pRepVal); break;	                            \
      case AM: lua_pushboolean(L,*((AliasWrap*)lhs)->m_pRepVal OP rhs->m_pEe->ind); break;                                              \
      case AC: lua_pushboolean(L,*((AliasWrap*)lhs)->m_pRepVal OP rhs->m_pEColl->GetEnumValueCount()); break;                           \
      case MS: lua_pushboolean(L,lhs->m_pEe->ind OP rhs->m_scalar); break;						                                        \
      case ME: lua_pushboolean(L,lhs->m_pEe->ind OP rhs->m_pEval->GetValue()); break;		                                            \
      case ML: newOMERep(L,lhs->m_pEe->ind OP *rhs->m_pList); break;						                                        \
      case MA: lua_pushboolean(L,lhs->m_pEe->ind OP *((AliasWrap*)rhs)->m_pRepVal); break;	                                            \
      case MM: lua_pushboolean(L,lhs->m_pEe->ind OP rhs->m_pEe->ind); break;                                                            \
      case MC: lua_pushboolean(L,lhs->m_pEe->ind OP rhs->m_pEColl->GetEnumValueCount()); break;                                         \
      case CS: lua_pushboolean(L,lhs->m_pEColl->GetEnumValueCount() OP rhs->m_scalar); break;						                    \
      case CE: lua_pushboolean(L,lhs->m_pEColl->GetEnumValueCount() OP rhs->m_pEval->GetValue()); break;		                        \
      case CL: newOMERep(L,lhs->m_pEColl->GetEnumValueCount() OP *rhs->m_pList); break;						                    \
      case CA: lua_pushboolean(L,lhs->m_pEColl->GetEnumValueCount() OP *((AliasWrap*)rhs)->m_pRepVal); break;	                        \
      case CM: lua_pushboolean(L,lhs->m_pEColl->GetEnumValueCount() OP rhs->m_pEe->ind); break;                                         \
      default:/*case CC:*/ lua_pushboolean(L,lhs->m_pEColl->GetEnumValueCount() OP rhs->m_pEColl->GetEnumValueCount()); break;          \
      };		                                    											                                \
	return 1;												   \
}															   \


DEFINE_LUAOP(OMEWrapAdd, +)
DEFINE_LUAOP(OMEWrapSub, -)
DEFINE_LUAOP(OMEWrapMult, *)
DEFINE_LUAOP(OMEWrapDiv, / )
		 
DEFINE_LUABOOLOP(OMEWrapEq, == )
DEFINE_LUABOOLOP(OMEWrapLt, < )
DEFINE_LUABOOLOP(OMEWrapLe, <= )
DEFINE_LUABOOLOP(OMEWrapNe, !=)
DEFINE_LUABOOLOP(OMEWrapGt, > )
DEFINE_LUABOOLOP(OMEWrapGe, >=)

int LuaOMEParser::OMEWrapMod(lua_State* L)
{
	using namespace OMECFuncs;

	SclrWrap wRhs(0.0), wLhs(0.0);
	BaseWrap* lhs = checkOMEWrap(L, 1);
	if (!lhs)
		lhs = new(&wLhs)SclrWrap(lua_tonumber(L, 1));

	BaseWrap* rhs = checkOMEWrap(L, 2);
	if (!rhs)
		rhs = new(&wRhs)SclrWrap(lua_tonumber(L, 2));

	enum { SS = 0, LS, SL, LL };

	unsigned short mode = 0;
	if (lhs->m_type == BaseWrap::LIST || rhs->m_type == BaseWrap::LIST)
	{
		//return error
		return 0;
	}

	newOMERep(L, OMEFMOD((OME_SCALAR)(*lhs), (OME_SCALAR)(*rhs)));

	return 1;
}

int LuaOMEParser::OMEWrapPow(lua_State* L)
{
	using namespace OMECFuncs;								   
											
	SclrWrap wRhs(0.0), wLhs(0.0);
	BaseWrap* lhs = checkOMEWrap(L, 1);
	if (!lhs)
		lhs = new(&wLhs)SclrWrap(lua_tonumber(L, 1));

	BaseWrap* rhs = checkOMEWrap(L, 2);
	if (!rhs)
		rhs = new(&wRhs)SclrWrap(lua_tonumber(L, 2));

	enum { SS = 0, LS, SL, LL };

	unsigned short mode = 0;
	if (lhs->m_type == BaseWrap::LIST)
		mode += 1;
	if (rhs->m_type == BaseWrap::LIST)
		mode += 2;

	switch (mode)
	{
	case LL:
		throw OMESupportException("Cannot perform operation with 2 lists.");
		return 0;
		break;
	case SL:
		newOMERep(L,pow((OME_SCALAR)(*lhs), (Listable&)(*rhs)));
		break;
	case LS:
		newOMERep(L, pow((Listable&)(*lhs), (OME_SCALAR)(*rhs)));
		break;
	case SS:
		newOMERep(L, OMEPOW((OME_SCALAR)(*lhs), (OME_SCALAR)(*rhs)));
	}

	return 1;
}
												         					   
int LuaOMEParser::OMEWrapUNM(lua_State* L)
{
	using namespace OMECFuncs;

	SclrWrap wRhs(0.0), wLhs(0.0);
	BaseWrap* lhs = checkOMEWrap(L, 1);
	if (!lhs)
		lhs = new(&wLhs)SclrWrap(lua_tonumber(L, 1));

	BaseWrap* rhs = checkOMEWrap(L, 2);
	if (!rhs)
		rhs = new(&wRhs)SclrWrap(lua_tonumber(L, 2));
	switch (lhs->m_type)
	{
	case BaseWrap::SCALAR: newOMERep(L,-lhs->m_scalar); break;
	case BaseWrap::EVAL: newOMERep(L,-lhs->m_pEval->GetValue()); break;
	case BaseWrap::LIST: newOMERep(L,-*lhs->m_pList); break;
	case BaseWrap::ALIAS: newOMERep(L,-*((AliasWrap*)lhs)->m_pRepVal); break;
	case BaseWrap::MENUM: newOMERep(L,-lhs->m_pEe->ind); break;
	default:/*case BaseWrap::ECOLL:*/ newOMERep(L,-lhs->m_pEColl->GetEnumValueCount()); break;
	};
	
	return 1;
}

int LuaOMEParser::OMEWrapToString(lua_State* L)
{
	using namespace OMECFuncs;
	BaseWrap* pRep;
	oSTLSStream outStrm;
	if((pRep=checkOMEWrap(L,-1)))
	{
		switch (pRep->m_type)
		{
		case BaseWrap::SCALAR:
			outStrm << ((SclrWrap*)pRep)->m_scalar << endl;
			break;
		case BaseWrap::ALIAS:
			outStrm << "Alias Index: " << ((AliasWrap*)pRep)->m_ind<<endl;
			outStrm << "Alias RepValue:" << *((AliasWrap*)pRep)->m_pRepVal << endl;
			outStrm << "Aliase Source Details:" << endl;
		case BaseWrap::EVAL:
			outStrm << ((EvalWrap*)pRep)->m_pEval->ToString();
			break;
		case BaseWrap::LIST:
			outStrm << "List Level: " << ((ListWrap*)pRep)->m_pList->GetLevel() << endl << "List dimensions: (";
			for (unsigned int i = 0; i<((ListWrap*)pRep)->m_pList->GetDimCount(); ++i)
				outStrm <<" "<< ((ListWrap*)pRep)->m_pList->GetDim(i);
			outStrm << " )"<<endl<<"List values: {";
			for (unsigned int i = 0; i<((ListWrap*)pRep)->m_pList->GetSize(); ++i)
				outStrm << " " << (*((ListWrap*)pRep)->m_pList)[i];
			outStrm << " }";
			break;
		
		case BaseWrap::MENUM:
			outStrm << "Enumerated Value: " << ((EnumWrap*)pRep)->m_pEe->label << " (" << ((EnumWrap*)pRep)->m_pEe->ind << ")" << endl;
			outStrm << "Belongs to ";
		case BaseWrap::ECOLL:
			outStrm << "Enum Type: " << ((ECollWrap*)pRep)->m_pEColl->GetEnumClassName()<<endl;
			outStrm << "Total Enum Vals: " << ((ECollWrap*)pRep)->m_pEColl->GetEnumValueCount();
			break;
		};
	}
	else if(lua_istable(L,-1))
	{
		outStrm<<"Object is table";
	}
	else
		outStrm<<"Unidentified object";
	
	lua_pushstring(L,outStrm.str().c_str());
	return 1;
}

int LuaOMEParser::OMEWrapLen(lua_State* L)
{
	using namespace OMECFuncs;

	BaseWrap* pRep;
	oSTLSStream outStrm;

	pRep = checkOMEWrap(L, 1);
	if (pRep)
	{
		switch (pRep->m_type)
		{
		case BaseWrap::LIST:
			lua_pushnumber(L, (((ListWrap*)pRep)->m_pList)->GetSize());
			break;
		case BaseWrap::ECOLL:
			lua_pushnumber(L, (((ECollWrap*)pRep)->m_pEColl)->GetEnumValueCount());
			break;
		default:
			lua_pushstring(L, "object is not a listable type, and has no length");
		}
	}
	else
		lua_pushstring(L, "OME Wrapper metamethod called on a non-OME object.");

	return 1;
}


//===
int LuaOMEParser::MTToString(lua_State* L)
{
	Model** hMdl;
	if((hMdl=checkModelPtr(L,1)))
	{
		lua_settop(L,0);
		lua_pushstring(L, (*hMdl)->ToString());
	}
	else
	{
		lua_settop(L,0);
		lua_pushstring(L,"This is not a model pointer");
	}
	return 1;
}

int LuaOMEParser::MTGetVal(lua_State* L)
{
	Model** hMdl;
	int ret = 0;
	if ((hMdl = checkModelPtr(L, 1)))
	{
		const char* lbl;
		if ((lbl = luaL_checkstring(L, 2)))
		{
			STLString lblStr(lbl);
			//search for object by name;
			OMEObject* pObj; 
			if (pObj = (*hMdl)->ObjectForName(lblStr))
			{
				//wrap eval
				if (!ret && pObj->IsOMEType(OME_EVAL))
				{
					newOMERep(L, (Evaluable*)pObj);
					ret=1;
				}

				//if something else or non existant, return nil;
				if (!ret)
				{
					lua_pushnil(L);
					ret = 1;
				}
			}
			else if (pObj = (*hMdl)->SubmodelForName(lblStr))
			{
				//wrap model
				newMdlPtr(L, (Model*)pObj);
				ret = 1;
			}
			//search for enum by name;
			ModelEnum* pMe;
			if (pMe = (*hMdl)->ModelEnumForName(lblStr))
			{
				newOMERep(L,pMe);
				ret=1;
			}
		}
		
		if (!ret)
		{
			lua_pushnil(L);
			ret = 1;
		}
	}
	else
	{
		lua_settop(L, 0);
		lua_pushstring(L, "This is not a model pointer");
		ret = 1;
	}
	return ret;
}
//===
 int LuaOMEParser::OMEWrapGetField(lua_State* L)
 {
	 using namespace LuaDefines;
	 using namespace OMECFuncs;

	 BaseWrap* pRep;
	 //for now, just use on listables
	 if ((pRep = checkOMEWrap(L, 1)))
	 {
		 const char* strVal = luaL_checkstring(L,2);
		 if (strVal && STLString(strVal) == "value")
			 return LuaOMEParser::OMEGetValue(L);
		 else
		 {

			 switch (pRep->m_type)
			 {
			 case BaseWrap::LIST:
			 {
					int idx = luaL_checkinteger(L, 2);

					if (idx > 0)
					{
						--idx;		//from 1-based to 0-based;
						if (idx < ((ListWrap*)pRep)->m_pList->GetSize())
						{
							lua_pushnumber(L, (*((ListWrap*)pRep)->m_pList)[idx]);
							return 1;
						}
					}
					else if (idx < 0)
					{
						idx = ((ListWrap*)pRep)->m_pList->GetSize() - idx;
						if (idx >= 0)
						{
							lua_pushnumber(L, (*((ListWrap*)pRep)->m_pList)[idx]);
							return 1;
						}
					}
			 }
				 break;
			 case BaseWrap::ECOLL:
			 {
				const OMEChar* lbl;
				int idx;
				if ((lbl = luaL_checkstring(L, 2)))
				{
					EnumEntry* pEnum;
					if ((pEnum = ((ECollWrap*)pRep)->m_pEColl->AddrForLabel(lbl)))
					{
						newOMERep(L, pEnum);
						return 1;
					}
				}
				idx = luaL_checkinteger(L, 2);
				if (idx > 0)
				{
					--idx;		//from 1-based to 0-based;
					if (idx < ((ECollWrap*)pRep)->m_pEColl->GetEnumValueCount())
					{
						newOMERep(L, ((ECollWrap*)pRep)->m_pEColl->AddrForIndex(idx));
						return 1;
					}
				}
				else if (idx < 0)
				{
					idx = ((ECollWrap*)pRep)->m_pEColl->GetEnumValueCount() - idx;
					if (idx >= 0)
					{
						newOMERep(L, ((ECollWrap*)pRep)->m_pEColl->AddrForIndex(idx));
						return 1;
					}
				}
			 }
				 break;
			 }
		 }
	 }
	 lua_pushnil(L);
	
	 ///// \todo unionize pointers that are exclusive.
	 //Model* pMdl=NULL;
	 //EvalCollection* pEc=NULL;
	 //SubmodelAssoc* pSa;
	 //STLString key=lua_tostring(L,2);
	 //VarArray* pVa;
	 //SclrVArray* pArray;

	 //bool success=false;

	 //if((pSa=CheckRepAssoc(L,1)))
	 //{
		// /// \todo update for new instance setup.
		// //reference from submodel assoc.
		// /*
		// OMEObject* pCaller=LuaDefines::GetCaller(L);
		// if(pCaller)
		// {
		//	 //find caller, and grab appropriate instances based on whether parent model is sub or base
		//	 if(pSa->IsSub((ModelCollection*)pCaller->GetParentModel()->GetSpawningParent()))
		//		pMdl=pSa->GetBaseModelInstance(pCaller->GetParentModel());
		//	 else
		//	 {
		//		 //grab cached evals from sub
		//		 int assocInd=pCaller->GetParentModel()->GetAssocIndex(pSa);
		//		 pEc=pSa->GetSubValues(key,assocInd >=0 ? assocInd : pCaller->GetParentModel()->m_instance);
		//		 if(pEc)
		//		{
		//			//push collection onto stack
		//			lua_settop(L,0);
		//			newOMERep(L,pEc);

		//			success=true;
		//			pEc=NULL;
		//		}
		//	 }
		// }
		// */
	 //}
	 //else
		// pMdl=CheckRepSubmodel(L,1);

	 //if(!success)
	 //{
		// if(pMdl)
		// {
		//	 /// \todo update for new instance setup
		//	 //grab object or collection from model, depending on what type of model
		//	 //pEval=dynamic_cast<Evaluable*>(pMdl->ObjectForName(key));
		//	 //if(!(pEval) || pMdl->GetModelType()!=MD_COLL || pEval->GetOMEType()==OME_SUBPORT) 
		//	 //{
		//		// //find var type, push on to stack
		//		// newOMERep(L,pEval);
		//		//success=true;
		//	 //}
		//	 //else if(pMdl->GetModelType()==MD_COLL)
		//	 //{
		//		// EvalCollection* pColl=((ModelCollection*)pMdl)->GetCollection(key);

		//		// if(pColl)
		//		// {
		//		//	newOMERep(L,pColl);
		//		//	success=true;
		//		// }
		//	 //}
		// }
		// else 
		// {
		//	 //we could be dealing with a collection, which shoud be using [] syntax instead of . syntax
		//	 if(!pEc)
		//		pEc=CheckRepCollection(L,1);

		//	 int index=lua_tointeger(L,2)-1;

		//	 lua_pop(L,2);
		//	 if(pEc)
		//	 {
		//		 if(pEc->GetCount() > index && index >= 0)
		//		 {
		//			/// \todo Test this code
		//			newOMERep(L,&(*pEc)[index]);
		//			success=true;
		//		 }
		//	 }
		//	 else if((pVa=CheckRepVarArray(L,1)))
		//	 {
  //              index=lua_tointeger(L,2)-1;
		//			
		//		if(pVa->GetSize() > index && index >=0)
		//		{
		//			lua_pushnumber(L,pVa->operator[](index));
		//			success=true;
		//		}
		//	 }
		//	 else if((pArray=CheckRepArray(L,1)))
		//	 {
		//		 index=lua_tointeger(L,2)-1;
		//		 lua_pushnumber(L,(*pArray)[index]);
		//		 success=true;
		//	 }
		// }
	 //}

	 ///// \todo figure out nested namespaces in lua
	 ////if(!success && checkOMERep(L,1))
	 ////{
		//// BaseModel* pBm=((LuaOMERep*)lua_touserdata(L,1))->GetParentModel();
		//// if(pBm)
		////	 //...
	 ////}
	 //if(!success)
		// lua_pushnil(L);
	 return 1;
 }

 int LuaOMEParser::OMEGetValue(lua_State* L)
 {
	 using namespace LuaDefines;
	 Evaluable* pEval = CheckWrapEval(L, OME_EVAL, 1);
	 size_t count = 1;
	 if (pEval)
	 {
		 if (!pEval->IsOMEType(OME_VARARRAY))
			lua_pushnumber(L, pEval->GetValue());
		 else
		 {
			 VarArray* pVa = (VarArray*)pEval;
			 count = pVa->GetSize();

			 for (size_t i = count; i > 0; --i)
				 lua_pushnumber(L, pVa->GetValue(i - 1));
		 }
	 }
	 else
		 lua_pushnil(L);

	 return (int)count;
 }

int LuaOMEParser::OMEWrapDestroy(lua_State* L)
{
	//disabled
	return 0;
}

int LuaOMEParser::OMEWrapForwardCall(lua_State* L)
{
	//disabled
	return 0;
	// /// \todo test call forwards to global scope after different model-level scopes are fully enabled.
	//LuaOMERep* rep=checkOMERep(L,1);
	//lua_remove(L,1);
	//int ret=0;
	//if(rep)
	//{
	//	lua_getglobal(L,rep->GetName().c_str());
	//	lua_insert(L,1);
	//	lua_call(L,lua_gettop(L)-1,LUA_MULTRET);
	//	ret=lua_gettop(L);
	//}
	//
	//return ret;
}
/// @endcond DOX_NO_DOC

//===
STLString LuaOMEParser::StackDump() const
{
	return OMELuaFunc::DebugDump(m_pLState);
}

const char* LuaOMEParser::ToString(const char tabChar, const unsigned int indent, const unsigned int inc) const
{
	STLString stack=StackDump();
	int i;
	char* out=new char[stack.length()+1];

	for(i=0; i < stack.length(); i++)
		out[i]=stack.c_str()[i];
	out[i]='\0';

	return out;
}

bool LuaOMEParser::AssociateWithSerializer(BaseEvalSerializer* pS) 
{ 
	bool ret=false; 
	if((m_pSerializer=dynamic_cast<LuaEvalSerializer*>(pS)))
		ret=true;
	return ret;
}


void LuaOMEParser::LoadSDPFuncs(ISpatialDataProvider* pSDP) 
{ 
	if(pSDP) 
      RegisterCoreSDPFuncs(pSDP, this);  //include in SDPLuaFuncs.h
}

BaseEvalSerializer* LuaOMEParser::GenerateSerializer()
{ 
	return new LuaEvalSerializer(this); 
}

/** Register an external function for use within the parser environment.
	@param name The name of used to reference the function.
	@param func The underlying function implementation, which should have the prototype:

	static int __name__(lua_state*)

	and return the number of arguments placed back into the lua stack.
*/
void LuaOMEParser::RegisterExternalFunction(const STLString & name, lua_CFunction func)
{
	//bare minimum needed
	lua_pushcfunction(m_pLState,func);
	lua_setglobal(m_pLState,name.c_str());
}


/** Register an external function stored in an OMELuaFunc subclass for use within the parser environment.
	@param luaFunc The OMELuaFunc object that contains the function to be added.
*/
void LuaOMEParser::RegisterExternalFunction(OMELuaFunc* luaFunc)
{
	//using the OMELuaFunc class can easily allow for future access to bundled info.
	m_funcRegistry.push_back(luaFunc);
	luaFunc->RegisterFunc(m_pLState);
}

/** Register an external constant that will persist across resets 
	@param name Identifier of constant.
	@param value The value of the constant.
*/
void LuaOMEParser::RegisterExternalConstant(const STLString name, const OME_SCALAR value)
{
	m_constRegistry[name]=value;
	DefineConstant(name.c_str(),value);
}

/** Convert a general expression statement into a lua-friendly statement.
	@param in a string containing the expression to convert.
	@return A copy of in modified to work in Lua.
*/
STLString LuaOMEParser::LuaizeExpression(const STLString & in)
{
	//static const size_t resultLen=strlen(LuaDefines::EVAL_RESULT_RETURN);
	/*	if we have if/then/else statements, we will need to modify things
		so that results will return properly.
		This statement will not work:

		return if ... then ... else ... end

		so this statement must be used instead

		if ... then return ... else return ... end
	*/
		//insert the return statement after the word if, else, and elseif
	static const OMEChar* patterns[]= {   "\\s*\\bif\\b", " (function() if omeToBool(",
									"\\s*\\belseif\\b", " elseif omeToBool(",
									"(\\s*)\\bthen\\b", ")$1then return ",
									"\\s*\\belse\\b", " else return ",
									"\\s*\\bend\\b", " end end)()",
									/* Patterns for misc operators*/
									"\\b[!][=]\\b", "~=",
									"\\s*(\\w+)\\s+xor\\s+(\\w+)", " $1 or $2 and $1 ~= $2",
									"\\s*[!][^=]\\s*", " not ",
									"\\bgraph\\s*[(]", " interpTable(",
								"\0"};
	

	STLString out=BatchRegexReplace(in,patterns); //std::regex_replace(STLString(expr),condReg,STLString("$&")+STLString(EVAL_RESULT_RETURN));

	//a function argument list in Lua is limited to at most either MAXSTACK or 256 (whichever is smaller).
	//if we have a getAsTable() call with > 250 entries, use a table constructor instead.
	//getAsTable is preferable when possible since it is a custom function that allows us to do some potential optimizations.

	const STLString gatStr=STLString("getAsTable(");
	const size_t ARG_LIM = 100; //a more practical limit 
	STLString tempStr;
	size_t found = out.find(gatStr);
	size_t argCount;
	size_t lvl = 0;
	size_t endInd;
	size_t i;
	while (found != STLString::npos)
	{
		argCount = 0;
		i = found + gatStr.length();
		do
			i++;
		while (i < out.length() && std::isspace(out[i]));

		if (out[i] != ')')
		{
			//at least one arg
			argCount++;
			endInd = 0;
			for (i = found + gatStr.length(); i < out.length() && !endInd; i++)
			{
				switch (out[i])
				{
				case ',':
					if (!lvl)
						argCount++;
					break;
				case '{':
				case '(':
					lvl++;
					break;
				case ')':
				case '}':
					if (!lvl)
						endInd = i;
					else
						lvl--;
				}
			}

			if (argCount >= ARG_LIM)
			{
				//convert getAsTable(...) to getAsTable({...})
				out.insert(endInd,1,'}');
				out.insert( found + gatStr.length(),1,'{');
			}
		}
		
		found = out.find(gatStr,found+1);
	}
	return out;
}

/** Pool-based memory allocator for Lua objects
	@param ud pointer to user data (in our case a LuaOMEParser).
	@param ptr Pointer to original memory block, if any.
	@param osize Old size of the memory block pointed to by ptr.
	@param nsize Request size of new memory block; if nsize is 0 then a deallocation of memory at Ptr is being requested.
	@return Pointer to new memory block of at least size nsize, or NULL if nsize is 0.
*/
void* LuaOMEParser::PoolAlloc(void* ud, void* ptr, size_t osize, size_t nsize)
{
	//return SimpleAlloc(ud, ptr, osize, nsize);

	LuaOMEParser* pPrsr = (LuaOMEParser*)ud;

	if (nsize == 0)
	{
		if (!pPrsr->m_ptrPool.ReleaseBlockPtr(ptr))
			operator delete(ptr);
		return NULL;
	}
	//convert sizes
	//DBG_COLOR(cout << nsize << endl, DBG_YELLOW);
	osize = LuaMemPool::BytesToBlockCount(osize);
	nsize = LuaMemPool::BytesToBlockCount(nsize);

	void* ret= pPrsr->m_ptrPool.ReallocBlockPtr(ptr, osize, nsize);
	//DBG_COLOR(cout << ret << endl, DBG_CYAN);
	return ret;
	
}

/**Generic memory allocator for Lua objects
@param ud pointer to user data (in our case a LuaOMEParser).
@param ptr Pointer to original memory block, if any.
@param osize Old size of the memory block pointed to by ptr.
@param nsize Request size of new memory block; if nsize is 0 then a deallocation of memory at Ptr is being requested.
@return Pointer to new memory block size nsize, or NULL if nsize is 0.
*/
void* LuaOMEParser::SimpleAlloc(void* ud, void* ptr, size_t osize, size_t nsize)
{
	LuaOMEParser* pPrsr = (LuaOMEParser*)ud;
	//standard allocator
	if (nsize == 0)
	{
		//DBG_COLOR(cout << ptr << endl, DBG_MAGENTA);
		if (!pPrsr->m_ptrPool.ReleaseBlockPtr(ptr))
			operator delete(ptr);

		return NULL;
	}

	//realloc
	void* outPtr = operator new(nsize);
	//DBG_COLOR(cout << outPtr << endl, DBG_BLUE);

	if (ptr)
	{
		//DBG_COLOR(cout << ptr << endl, DBG_MAGENTA);
		memcpy(outPtr, ptr, std::min(osize, nsize));
		if (!pPrsr->m_ptrPool.ReleaseBlockPtr(ptr))
			operator delete(ptr);
	}
	return outPtr;


}

/** Handler for Error reporting calls from within Lua.
	@param pState The current state posting an error.
	@return The number of arguments returned to the stack; should always be 1.
*/
int LuaOMEParser::ErrHandler(lua_State *pState)
{
	static const STLRegex brkReg("omeLineInit[(].*?[)]\\s*");
	lua_getfield(pState,LUA_REGISTRYINDEX,LuaDefines::PARSER_ID);
	LuaOMEParser* pPrsr=(LuaOMEParser*)lua_touserdata(pState,-1);

	STLString err=(pPrsr ? pPrsr->GetModeID() : STLString())+": "+std::regex_replace(STLString(lua_tostring(pState,1)),brkReg,STLString());

	lua_pop(pState,2);
	lua_pushstring(pState,err.c_str());
	return 1;
}

/** Launch interactive prompt.
	@return non-zero value if an interrupt error was generated during the interactive setting.
*/
int LuaOMEParser::GoPrompt(unsigned int & skipCount,const size_t & startLine)
{
	int ret=0; //no err;

	static const char* ESC_GO="\\g";
	static const char* ESC_QUIT="\\q";
	static const char* ESC_HELP="\\h";
	static const char* ESC_BRKS="\\b";
	static const char* ESC_ADD="\\a";
	static const char* ESC_REM="\\r";
	static const char* ESC_STEP="\\s";
	static const char* ESC_DUMP = "\\d";
	static const char* ESC_LINE = "\\l";

	static const char* HLP_COMMS="commands";
	static const char* HLP_FUNCS="functions";
	static const char* HLP_LINES="lines";
	static const char* DISP_ODENT="\t\t:   ";
	static const char* DISP_OHEAD="\t\t:  Options: ";
	static const char* DISP_BREAK="\t\t:\n";

	cout<<"Engaging Lua Prompt:"<<endl<<"-- type ";
	DBG_COLOR(cout<<ESC_GO<<" ",DBG_WHITE);
	cout<<"to continue, ";
	DBG_COLOR(cout<<ESC_QUIT<<" ",DBG_WHITE);
	cout<<"to quit, or ";
	DBG_COLOR(cout<<ESC_HELP<<" ",DBG_WHITE);
	cout<<"for more commands";
	cout<<" --"<<endl;
	char buffer[1025]={};
	size_t line;
	char option;

	bool go=true;
	while(go)
	{
		line=0;
		option='\0';
		cout<<">>> ";
		DBG_SETTXCOLOR(DBG_WHITE);
		cin.get(buffer,1024);
		cin.clear();
		cin.ignore(2,'\n');
		
		DBG_SETTXCOLOR(DBG_CYAN);

		//commands with multiple args
		iSTLSStream subBuff(LTrimString(buffer));
		STLString command;

		subBuff>>command;
		
		if(command==ESC_HELP)
		{
			STLString subOpt;
			subBuff>>subOpt;
			if(subOpt.empty() || subOpt==HLP_COMMS)
			{
				cout<<"Commands:"<<endl;
				DBG_COLOR(cout<<ESC_HELP<<" [<options>]",DBG_WHITE);
				cout<<"\t: Display this help text, if no argument passed."<<endl;
				cout<<DISP_OHEAD<<endl<<DISP_ODENT;
				DBG_COLOR(cout<<HLP_COMMS,DBG_WHITE);
				cout<<" - Display this help text."<<endl<<DISP_ODENT;
				DBG_COLOR(cout<<HLP_LINES,DBG_WHITE);
				cout<<" - Display the total number of loaded lines."<<endl<<DISP_ODENT;
				DBG_COLOR(cout<<HLP_FUNCS,DBG_WHITE);
				cout<<" - List all queryable OME functions."<<endl<<DISP_ODENT;
				DBG_COLOR(cout<<"<function>",DBG_WHITE);
				cout<<" - Show description of specific OME function."<<endl<<DISP_BREAK;
				DBG_COLOR(cout<<ESC_GO<<" [<options>]\t",DBG_WHITE);
				cout<<": Continue execution of model."<<endl;
				cout<<DISP_OHEAD<<endl<<DISP_ODENT;
				DBG_COLOR(cout<<'r',DBG_WHITE);
				cout<<" - continue execution without activating further breaks."<<endl<<DISP_BREAK;
				DBG_COLOR(cout << ESC_LINE << " \t\t", DBG_WHITE);
				cout << ": Print current line to screen." << endl << DISP_BREAK;
				DBG_COLOR(cout << ESC_DUMP << " [<start>[<stop>]]\t", DBG_WHITE) ;
				cout << ": Print current script to screen, optionally filtering out lines before <start> and after <stop>." << endl << DISP_BREAK;
				DBG_COLOR(cout<<ESC_STEP<<" <count=1>\t",DBG_WHITE);
				cout<<": Break after <count> steps (execution break only)."<<endl<<DISP_BREAK;
				DBG_COLOR(cout<<ESC_QUIT<<" <flag=0>\t",DBG_WHITE);
				cout<<": Abort execution of model with optional (nonzero) int <flag>."<<endl<<DISP_BREAK;
				DBG_COLOR(cout<<ESC_BRKS<<" [<options>]\t",DBG_WHITE);
				cout<<": Print current breakpoints if no <option> provided."<<endl;
				cout<<DISP_OHEAD<<endl<<DISP_ODENT;
				DBG_COLOR(cout<<"a <line>",DBG_WHITE);
				cout<<" - Add a break point at <line>."<<endl<<DISP_ODENT;
				DBG_COLOR(cout<<"r <line>",DBG_WHITE);
				cout<<" - Remove a break point at <line>."<<endl<<DISP_ODENT;
				DBG_COLOR(cout<<"s <path>",DBG_WHITE);
				cout<<" - Save current breakpoints to new file at <path>."<<endl<<DISP_ODENT;
				DBG_COLOR(cout<<"l <path>",DBG_WHITE);
				cout<<" - Load breakpoints from file at <path>."<<endl;
			}
			else
			{
				if(subOpt==HLP_FUNCS)
				{
					cout<<"OME functions:"<<endl;
					for	(auto itr=m_funcRegistry.begin(); itr!=m_funcRegistry.end(); ++itr)
						cout<<'\t'<<(*itr)->GetName()<<endl;
				}
				else if(subOpt==HLP_LINES)
					cout<<"There are "<<m_breaks.size()<<" lines in the current script."<<endl;
				else
				{
					auto itr=m_funcRegistry.begin();
					for(; itr!=m_funcRegistry.end(); ++itr)
					{
						if((*itr)->GetName()==subOpt)
						{
							cout<<(*itr)->GetDescription()<<endl;
							break;
						}
					}
					if(itr==m_funcRegistry.end())
						cout<<"OME function '"<<subOpt<<"' not found."<<endl;
				}
			}
			cout<<endl;
		}
		else if(command==ESC_GO)
		{
			go=false;

			if(subBuff>>option)
			{
				switch(option)
				{
				case 'r':
					for(auto itr=m_breaks.begin(); itr!=m_breaks.end(); ++itr)
						(*itr)=false;
					break;
				}
			}
		}
		else if(command==ESC_QUIT)
		{
			ret=0;

			//catch optional flag
			subBuff>>ret;
			if (ret < 1)
				ret = 1;

			lua_getfield(m_pLState,LUA_REGISTRYINDEX,LuaDefines::MANAGER_ID);

			BaseManager *pBm=(BaseManager*)lua_touserdata(m_pLState,-1);
			if(pBm)
				pBm->ReportError("Run Prompt-terminated",NULL,EC_CRITICAL);

			for(auto itr=m_breaks.begin(); itr!=m_breaks.end(); ++itr)
				(*itr)=false;
			go=false;
		}
		else if(command==ESC_STEP)
		{
			if(!subBuff>>skipCount || skipCount<=0)
				skipCount=1;
			go=false;
		}
		else if(command==ESC_BRKS)
		{
			size_t i;
			std::ifstream inFile;
			std::ofstream outFile;

			subBuff>>option;
			switch(option)
			{
			case 'a':
			case 'r':
				if(subBuff>>line)
				{
					if(line>0 && line<=m_breaks.size())
					{
						cout<<"Break point at line "<<line;
						if(option=='a')
						{
							m_breaks[line-1]=true;
							cout<<" added."<<endl;
						}
						else
						{
							m_breaks[line-1]=false;
							cout<<" removed."<<endl;
						}
					}
					else
						DBG_COLOR(cout<<"ERR: Line "<<line<<" exceeds the number of statements ("<<m_breaks.size()<<")."<<endl,DBG_RED);
				}
				else
					DBG_COLOR(cout<<"ERR: Line argument to "<<ESC_BRKS<<' '<<option<<" missing."<<endl,DBG_RED);
				break;
			case 's':
				command.clear();
				subBuff>>command;
				if(m_pSerializer && command.empty())
					command= m_ehStack.top() == "Init" ? m_pSerializer->GetInitBreakPath() : m_pSerializer->GetEvalBreakPath();
				outFile.open(command);
				if(outFile.good())
				{
					for(i=0; i<m_breaks.size(); i++)
					{
						if(m_breaks[i])
							outFile<<i+1<<endl;
					}
					cout<<"Breakpoints saved to '"<<command<<"'."<<endl;
				}
				else
					DBG_COLOR(cout<<"Could not save breakpoints to '"<<command<<"'."<<endl,DBG_RED);
				break;
			case 'l':
				command.clear();
				subBuff>>command;
				if(m_pSerializer && command.empty())
					command= m_ehStack.top() == "Init" ? m_pSerializer->GetInitBreakPath() : m_pSerializer->GetEvalBreakPath();
				inFile.open(command);

				if(inFile.good())
				{
					//clear current breaks
					for(i=0; i<m_breaks.size(); i++)
						m_breaks[i]=false;

					while(inFile>>i)
						m_breaks[i-1]=true;
					cout<<"Breakpoints loaded from '"<<command<<"':"<<endl;
					//omit break to show new breakpoints
				}
				else
				{
					DBG_COLOR(cout<<"Could not load breakpoints from '"<<command<<"'"<<endl,DBG_RED);
					break;
				}
			default:
				cout<<"Current break lines:"<<endl;
				for(i=0; i<m_breaks.size(); i++)
				{
					if(m_breaks[i])
						cout<<'\t'<<i+1<<endl;
				}
			}

		}
		else if (command == ESC_DUMP || command == ESC_LINE)
		{
			STLString outStr;
			if (m_ehStack.top() == "Init")
				outStr=m_pSerializer->GetInitStatements(true);
			else if (m_ehStack.top() == "Update")
				outStr=m_pSerializer->GetEvalStatements(true);
			else if (m_currExpr)
				outStr=m_currExpr;

			StrArray statements = SplitString(outStr);

			if (command == ESC_LINE)
			{
				size_t specificLine=0;
				subBuff >> specificLine;
				if (specificLine==0 || specificLine>statements.size())
					specificLine= startLine;
				cout << (specificLine ? statements[specificLine - 1] : STLString("No line loaded")) << endl;
			}
			else
			{
				int start = 1;
				int stop = statements.size();

				//check to see if there are line restrictions
				//these will not affect start/stop if arguments are absent
				if (subBuff >> start)
					subBuff >> stop;

				for (size_t i = start-1; i < stop; i++)
				{
					DBG_COLOR(cout << std::setw(4) << i + 1 << ":  ", DBG_GREEN);
					cout << statements[i] << endl;
				}
			}
		}
		else if (buffer[0] != '\0' && buffer[0] != '\n' && luaL_dostring(m_pLState,buffer))
		{
			//evaluate command in lua
			DBG_COLOR(cout<<"ERR: "<<lua_tostring(m_pLState,-1)<<endl,DBG_RED);
		}
		
	}
	return ret;
}

void LuaOMEParser::Reset(Model* pMdl)
{
	delete m_pLState;
	DisposeStaticArrays();

//#ifdef OME_LUA_ALLOC
	m_pLState= lua_newstate(LuaOMEParser::SimpleAlloc,this);
//#else
//	m_pLState = luaL_newstate();
//#endif
	luaL_openlibs(m_pLState);
	LoadMetatables();
	lua_newtable(m_pLState);
	lua_setglobal(m_pLState, LuaDefines::MDL_COMP_TABLE);

	if(pMdl)
		Init(pMdl);
	
	GetCache().ResetCache();
	ClearModeIDs();
}

/** Allocate a new static array for use in Lua environment.
	@param len The length of the array to allocate.
	@return Pointer to the wrapper to the newly allocated array.
*/
SimpleArrayWrapper* LuaOMEParser::AddStaticArray(const size_t & len)
{
	OME_SCALAR* vals = new OME_SCALAR[len]();

	SimpleArrayWrapper* pWrap=new SimpleArrayWrapper(vals, len);
	m_staticArrays.push_back(pWrap);
	return pWrap;
}

/** Dispose of any memory allocated through calls to AddStaticArray().*/
void LuaOMEParser::DisposeStaticArrays()
{
	for (auto itr = m_staticArrays.begin(); itr != m_staticArrays.end(); ++itr)
	{
		delete[](*itr)->GetRawArray();
		delete (*itr);
	}
	m_staticArrays.clear();
}

/** Enable or disable Debug statements.
	@param enabled If true, enable debug checks; otherwise disable debug checks.
*/
void LuaOMEParser::SetDebugState(const bool & enabled)
{
	if (m_pSerializer)
		m_pSerializer->SetDbgEnabled(enabled);
}

/** Retrieve a factory function for a given name.
	@param key The name of the function to search for.
	@param ret Contains a function pointer if found, or NULL if not.
*/
LuaDefines::LuaFactoryFunc LuaOMEParser::GetFactoryForFunc(const STLString & key) const
{
	using namespace LuaDefines;
	LuaFactoryFunc ret = NULL;

	auto itr = m_factories.find(key);
	if (itr != m_factories.end())
		ret = (*itr).second;

	return ret;
}