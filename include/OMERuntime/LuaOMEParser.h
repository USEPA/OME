#pragma once
#include "OMEParser.h"
#include "Summarizable.h"
#include "OMELuaFunc.h"
#include "EnumRep.h"
#include "omeobjects/SubmodelAssoc.h"
#include "LuaHeaders.h"

#include "LuaMemPool.h"
#include "OMELuaDefines.h"
#include "ValCache.h"

#include <stack>

class OMELuaFunc;
class Model;
class LuaEvalSerializer;
class ISpatialDataProvider;

/** Parses expressions using the LUA runtime language */
class __EXPORT__ LuaOMEParser : public OMEParser, public Summarizable
{
public:
	/** Default constructor */
	LuaOMEParser(void);

	/** Simple Constructor.
		@param mdl The model to associate with this parser.
	*/
	LuaOMEParser(Model* mdl);

	/** Simple Destructor.
	*/
	~LuaOMEParser(void);

	virtual bool Init(Model* pParentMdl=NULL);

	virtual void PrepTempDataHandler();

	virtual bool CompileExpression(const OMEChar* expr, OMEObject* pCaller=NULL);

	virtual bool TestExpression(const OMEChar* expr);

	//virtual int GetVarCount();
	//virtual LPCSTR GetVarName(const int i);
	//virtual void GetVarNames(StrArray & varList);
	
	virtual bool DefineVar(const OMEChar* varName, Evaluable *pEval);
	virtual bool DefineVar(const OMEChar* varName, OME_SCALAR *pSclr);
	virtual bool RedefineVar(const OMEChar* varName, Evaluable *pEval);

	virtual bool Evaluate(OME_SCALAR & value, const bool isTest=false,const SclrArgs & args=SclrArgs());
	virtual bool Evaluate(OME_SCALAR* & values, int & count, const bool isTest=false,const SclrArgs & args=SclrArgs());
	virtual bool Evaluate(SclrResults & values, int & count, const bool isTest=false,const SclrArgs & args=SclrArgs());
	virtual bool BatchEvaluate(const SclrArgs & args=SclrArgs(), const bool isTest=false);
	virtual bool BatchEvaluate(SclrResults & values, int & count, const SclrArgs & args=SclrArgs(), const bool isTest=false);
	virtual bool BatchEvaluate(OME_SCALAR* & values, int & count,const SclrArgs & args=SclrArgs(), const bool isTest=false);
	//virtual bool RegisterFunction(const OMEChar* funcName, int numArgs);

	virtual OMEParser* Copy();

	//virtual void RegisterModel(Model* mdl);
	//virtual void UnregisterModel(Model* mdl);

	virtual void DefineEnumVals(const ModelEnum & enumVals);
	virtual void RedefineEnumVals(const ModelEnum & enumVals);

	virtual bool DefineArray(const OMEChar* varName, SclrVArray* pArr);

	virtual bool DefineParameter(Param* pParam);
	virtual bool RedefineParameter(Param* pParam);

	virtual bool DefineConstant(const OMEChar* constName, const int & val);
	virtual bool DefineConstant(const OMEChar* constName, const OME_SCALAR & val);
	virtual bool DefineConstant(const OMEChar* constName, const OMEChar* & val);

	virtual bool RedefineConstant(const OMEChar* constName, const int & val);
	virtual bool RedefineConstant(const OMEChar* constName, const OME_SCALAR & val);
	virtual bool RedefineConstant(const OMEChar* constName, const OMEChar* & val);

	virtual bool DefineSubmodel(Model* pMdl);
	virtual bool DefineAssoc(SubmodelAssoc* pSa);

	virtual bool DefineAll(Model* pMdl);

	virtual bool UpdateSubmodelEntry(Model* pMdl, const int ind) {return false;};
	virtual bool UpdateSubmodelEntries(Model* pMdl, const int start,const int count);
	
	virtual void ConvertExpressionForEval(Evaluable* pEval) { pEval->SetExpr(LuaOMEParser::LuaizeExpression(pEval->GetExpr())); };
	virtual const OMEChar* ToString(const OMEChar tabChar=' ', const unsigned int indent=0, const unsigned int inc=4) const;

	virtual STLString GetModeID() const;
	virtual void PushModeID(const STLString & eh);
	virtual STLString PopModeID();
	virtual void ClearModeIDs();

	/** Bypass model structure and directly execute lua expression in current context. 
		@param expr The lua string to execute.
	*/
	void ExecuteLuaString(const STLString & expr) { luaL_dostring(m_pLState,expr.c_str()); }
	
	/** Dumps current lua stack summary to string; for debugging purposes.
		@return STLString containing summary of current stack.
	*/
	STLString StackDump() const;

	void RegisterExternalFunction(const STLString & name, lua_CFunction func);
	void RegisterExternalFunction(OMELuaFunc* luaFunc);

	void RegisterExternalConstant(const STLString name, const OME_SCALAR value);

	virtual void PushBatch(const OMEChar* batch);
	virtual void PopBatch();
	virtual void ClearBatch() {while (!m_batchStack.empty()) PopBatch();};
	virtual bool InBatchMode() {return !m_batchStack.empty();}
	virtual void SetBreaks(const BreakSet & breaks) {m_breaks=breaks;};
	virtual void Reset(Model* pMdl=NULL);

	/** Activate memory pool-based allocator for lua objects. This is intended for predominantly temporary objects.*/
	inline void ActivatePool() { lua_setallocf(m_pLState, LuaOMEParser::PoolAlloc, this); }
	/** Deactivate memory pool-based allocator and instead use a conventional allocator. This is intended for long-lived objects.*/
	inline void DeactivatePool() { lua_setallocf(m_pLState, LuaOMEParser::SimpleAlloc, this); }
	
	SimpleArrayWrapper* AddStaticArray(const size_t & len);
	void DisposeStaticArrays();
	/** Set the serializer that is used to create batch statements. The serializer is primarily used to provide breakpoint info to command prompt.
		@param pSerial Pointer to the serializer used in creating batch statements.
	*/
	inline void SetLuaSerializer(LuaEvalSerializer* pSerial) {m_pSerializer=pSerial;}
	void SetDebugState(const bool & enabled);
	/** Immediately launch prompt.
		@return Error code from prompt session.
	*/
	inline int GoPrompt() { unsigned int dummy=0; return GoPrompt(dummy,0);}
	int GoPrompt(unsigned int & skipCount,const size_t & startLine=0);

	/** @return constant pointer to internal function registry. */
	inline const ARRAY_TYPE<OMELuaFunc*>* GetLuaFuncRegistry() const {return &m_funcRegistry;}

	LuaDefines::LuaFactoryFunc GetFactoryForFunc(const STLString & key) const;

	static STLString LuaizeExpression(const STLString & in);
	/** Reset offset into global cache.*/
	inline void ResetCacheOffset() { GetCache().ResetCache(); }
	static ValCache<OME_SCALAR>& GetCache();

	virtual BaseEvalSerializer* GenerateSerializer();
	virtual bool AssociateWithSerializer(BaseEvalSerializer* pS);
	virtual void LoadSDPFuncs(ISpatialDataProvider* pSDP);

	/** Resize memory pool to account for any overflows/underflows. */
	inline void RefitMemPool() { m_ptrPool.ClearPool();  m_ptrPool.RefitPool(0, true); }
private:

	///@cond DOX_NO_DOC
	LuaOMEParser(const LuaOMEParser &);		//disable copying
	///@endcond

	lua_State* m_pLState;				///< Current expression to execute.
	vector<int>* m_pMakeDepthArray;		///< Depth of call for MakeArray.

	std::stack<int> m_batchStack;	 ///< Registry index of cached batch statement, if one exists.

	OMEChar* m_currExpr;               ///< The currently loaded expression.

	ARRAY_TYPE<OMELuaFunc*> m_funcRegistry;		///< List of function descriptors of all custom functions incorporated into this parser.
	std::map<STLString,OME_SCALAR> m_constRegistry; ///< Map of default constants to register with new parser.
	LuaDefines::LuaFactoryMap m_factories;			///< Function pointers for expression functions that are generated completely differently than just an expression call.
	BreakSet m_breaks;									///< Break point maps for various batch Scripts.
	std::stack<STLString> m_ehStack;					///< Labels used when querying lua batch state.
	LuaEvalSerializer* m_pSerializer;				///< For loading and saving breakpoint paths.
	LuaMemPool m_ptrPool;							///< Block-based memory allocation pool.

	std::vector<SimpleArrayWrapper*> m_staticArrays;	///< Record of static arrays allocated for use within the Lua Environment.

	///@endcond DOX_NO_DOC
//	size_t m_memAlloc;									///< The amount of Lua memory allocated (in bytes).
//	size_t m_largestAlloc;								///< The largest amount of Lua memory allocated at one time (in bytes).

	//for metatable definitions
	static const struct luaL_Reg OMEWrapLib[];		///< Evaluable metatable info.
	static const struct luaL_Reg mdlTblLib[];		///< Model table info.
	//static const struct luaL_Reg modelLib[];	///< Model metatable info.
	//static const struct luaL_Reg collectionLib[];	///< EvalCollection metatable info.
	void LoadMetatables();
	void RegisterModel(Model* pMdl, const int & valTblIdx);
	int CopyLuaField(lua_State* pIn, lua_State* pOut, int ind);
	int CopyLuaTable(lua_State* pIn, lua_State* pOut, int ind);

	void ProcessResult(const int & err, int index, OME_SCALAR & value);

	//static void* DebugAlloc(void* ud, void* ptr, size_t osize, size_t nsize);
	static void* PoolAlloc(void* ud, void* ptr, size_t osize, size_t nsize);
	static void* SimpleAlloc(void* ud, void* ptr, size_t osize, size_t nsize);
	static int ErrHandler(lua_State *pState);

	///** Extract OME_SCALAR values from custom types, or builtin.
	//	@param L the lua_State which has the two values sitting on its stack.
	//	@param op1 populated with the OME_SCALAR value of the first value when found.
	//	@param op2 populated with the OME_SCALAR value of the second value when found.
	//*/
	//inline static void FindDoubles(lua_State* L, OME_SCALAR & op1, OME_SCALAR & op2)
	//{
	//	using namespace LuaDefines;
	//		
	//	if(!GetRepSingleValue(L,1,op1)) 
	//		op1=lua_tonumber(L,1);
	//	
	//	
	//	if(!GetRepSingleValue(L,2,op2))  
	//		op2=lua_tonumber(L,2);

	//}

	///@cond DOX_NO_DOC
	

	//metatable functions for Evaluables
	static int OMEWrapAdd(lua_State* L);
	static int OMEWrapSub(lua_State* L);
	static int OMEWrapMult(lua_State* L);
	static int OMEWrapDiv(lua_State* L);
	static int OMEWrapMod(lua_State* L);
	static int OMEWrapPow(lua_State* L);
	static int OMEWrapUNM(lua_State* L);
	static int OMEWrapEq(lua_State* L);
	static int OMEWrapLt(lua_State* L);
	static int OMEWrapLe(lua_State* L);
	static int OMEWrapNe(lua_State* L);
	static int OMEWrapGt(lua_State* L);
	static int OMEWrapGe(lua_State* L);
	
	static int OMEWrapToString(lua_State* L);
	static int OMEWrapLen(lua_State* L);
	static int OMEWrapGetField(lua_State* L);
	static int OMEWrapDestroy(lua_State* L);

	static int OMEGetValue(lua_State* L);
	static int OMEWrapToBool(lua_State*L);

	static inline OMECFuncs::BaseWrap* LuaBWNew(void* extra) { return (OMECFuncs::BaseWrap*)(((LuaOMEParser*)extra)->m_ptrPool.NewPtr()); }
	static inline void LuaBWRelease(OMECFuncs::BaseWrap* ptr, void* extra) { ((LuaOMEParser*)extra)->m_ptrPool.ReleasePtr(ptr); }
	static inline void LuaBWClear(void* extra){ ((LuaOMEParser*)extra)->m_ptrPool.ClearPool(); }
	static inline void LuaBWRefit(void* extra){ ((LuaOMEParser*)extra)->m_ptrPool.RefitPool(); }
	//metatable functions for Model pointers
	static int MTToString(lua_State* L);
	static int MTGetVal(lua_State* L);
	//if var called as function, pass up to function to avoid namespace collision
	static int OMEWrapForwardCall(lua_State* L);

	static bool CheckMultiVal(lua_State* L,const int ind);

	///@endcond
};


/**
Legend:
	---  unimplemented
		 unimplemented (blank spaces)
	***  implemented
	   ! tested
	???  questions about implementation
	*--	 partially implemented

Expected Formula/Equation functions
Single Value Functions:
abs(X)						***!
acos(X)						***!
asin(X)						***!
at_init(X)					***!
atan(X)						***!
atan2(X,Y)					***!
binome(P, N)				***!
ceil(X)						***!
channel_is(X)				***
const_delay(X, T)			***
cos(X)						***!
cosh(X)						***!
dies_of(X)					***
dt(I)						***!
exp(X)						***!
first(T)					***
floor(X)					***!
fmod(X,Y)					***!
following(T)				***
gaussian_var(X,Y)			***!
hypergeom(P, M, S)			***!
hypot(X,Y)					***!
index(I)					***
init_time(1)				***
in_preceding(X)				***
in_progenitor(X)			***
int(X)						***!
last(X)						***
log(X)						***!
log10(X)					***!
max(X,Y)					***!
min(X,Y)					***!
parent(I)					***
poidev(X)					***!
pow(X,Y)					***!
preceding(T)				***
prev(N)						***
rand_const(X,Y)				***!
rand_var(X,Y)				***!
round(X)					***!
sgn(X)						***!
sin(X)						***!
sinh(X)						***!
sqrt(X)						***!
stop(X)						***
tan(X)						***!
tanh(X)						***!
time()						***
var_delay(X,T)				***

Lists/Array functions;

[X] = array
{X} = set
all([X])					***!
all({X})					***!
any([X])					***!
any({X})					***!
colin([X])					***!
count([X])					***!
element([X],I)				*--!
firsttrue([B])				***!
greatest([X])				***!
greatest({X})				***!
howmanytrue([B])			***!
least([X])					***!
least({X})					***!
makearray(X,N)				***!
place_in(I)					***!
posgreatest([X])			***!
posleast([X])				***!
product([X])				***!
product({X})				***!
rankings([X])				***!
size(S)						***
size(S,I)					***
subtotals([X])				***!
sum([X])					***!
with_colin({N},{X})			***!
with_greatest([N],[X])		***!
with_greatest({N},{X})		***!
with_least([N],[X])			***!
with_least({N},{X})			***!

Undocumented / Additional Functions:
at_phase(x)							????	-- looks like it queries to see if at a given step
default(X)					***
factorial(X)				***		Procs.cpp -- same as x!
for_members_of_type()		***		List Handling.pl --
gaussian_const()					Statistics.pl --
gaussian_var()						Statistics.pl --
hypergeom_const()					Statistics.pl --
hypergeom_var()						Statistics.pl --
interpolate()				***!	List Handling.pl -- interpolate(X,Xarray,Yarray): result is the value of Y obtained by interpolation between the tabulated values of Xarray and Yarray, using input X.
iterations()				*--		Model Properties.pl -- 
latency(X)							????	-- Takes immigration/reproduction node, returns current fraction toward new individual
ordinals(x)					***!	List Handling.pl -- 
pi()						***!
product3()					***!	Matrix Operations.pl -- Mult two 3x3 mats
poidev_const()						Statistics.pl --
poidev_var()						Statistics.pl --
quantize()							Statistics.pl --
ready(X)
stateArray(x, y, [z])
transform3()				***!	Matrix Operations.pl -- Mult vec3 by 3x3 mat
trigger_magnitude()
tweakArray(x, y, z)
ways_to_pick()						Statistics.pl --

*/