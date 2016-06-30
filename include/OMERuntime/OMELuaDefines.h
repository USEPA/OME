#pragma once
#include "LuaHeaders.h"
#include "omeobjects/OMETypes.h"
#include "omeobjects/VarArray.h"

#include "CompiledMdlWrappers.h"

//#include "Evaluable.h"

class Evaluable;

#define checkOMEWrap(L,i)	(BaseWrap*)luaL_testudata((L),(i),LuaDefines::WRAP_META_ID)
#define checkModelPtr(L,i)	(Model**)luaL_testudata((L),(i),LuaDefines::MT_META_ID)

//... is for a variable number of args, which replaces the __VA_ARGS__ macro; this is a c standard
//do ... while(0) is a scoping trick.
#define newOMERep(L, ...)												        \
do {																	        \
	using namespace OMECFuncs;                                                  \
	BaseWrap* rep=(BaseWrap*)lua_newuserdata(L,sizeof(BaseWrap));	            \
	LuaDefines::LuaWrap(rep,__VA_ARGS__);                                       \
	luaL_getmetatable(L,LuaDefines::WRAP_META_ID);						        \
	lua_setmetatable(L,-2);          										    \
} while(0)

#define newMdlPtr(L,pMdl)													   \
do 																			   \
{																			   \
	Model** rep = (Model**)lua_newuserdata(L, sizeof(Model*));				   \
	*rep = pMdl;															   \
	luaL_getmetatable(L, LuaDefines::MT_META_ID);							   \
	lua_setmetatable(L, -2);												   \
} while (0)																	   


/** Misc collection of Lua-related functions, variables, and constants.*/
namespace LuaDefines
{
	using namespace OMECFuncs;

	/** Signature for function which interprets an expression function call in a novel way.*/
	using LuaFactoryFunc = STLString(*)(Evaluable*, const STLString &, const bool &);
	/** Map to link expression function invocation to LuaFactoryFunc.*/
	typedef std::map<STLString, LuaFactoryFunc> LuaFactoryMap;

	static const char* WRAP_META_ID="OMEObjectWrap";			///< Lua Variable identifying an OMEWrapper metatable.
	static const char* MT_META_ID="OMEModelPtr";				///< Lua Variable identifying an OME Model object.
																
	static const char* EVAL_RESULT_RETURN="return ";			///< Lua return statement.
	static const char* EVAL_CALLER_ID="__OMECaller";			///< Variable defined before a function is called designating the component to act upon.
	static const char* MDL_COMP_TABLE = "__OMEModelComponents";	///< Table holding raw OME model components.
	static const char* MANAGER_ID="__OME_simulation_manager";	///< Pointer to present simulation manager.
	static const char* PARSER_ID="__OMEParser";					///< Pointer to present expression parser. 
	static const char* BREAKS_ID="__OMEBreakpoints";			///< List of breakpoint objects.
	static const char* MAKE_DEPTH_ID="__OME_makeDepth";			///< Value defined and used in nested makeArray() calls.
	static const char* MAKE_OFFSET = "__OME_makeOffset";		///< Value defined and used in nested makeArray() calls
	static const char* TABLE_MC_SRC="__OMEMCSrc";				///< Variable defined before a function is called containing the Parent model to act upon.
	static const char* TEST_MODE="__OMETestMode";				///< Flag indicating whether or not testing mode is active.

	//breakpoint struct
	///@todo update breakpoints with struct below.
	/** Simple details for representing a siimple breakpoint. */
	struct BreakPoint
	{
		bool enabled;			 ///< Flag indicating that the breakpoint should be evaluated.
		unsigned int hitCount;	 ///< The number of times the breakpoint has been hit during a session.
		unsigned int minHit;	 ///< The minimum number of times the breakpoint should be hit before it pauses execution.
		unsigned int lineNum;	 ///< The line at which the breakpoint breaks.
		STLString lineExpr;		 ///< The expression found at lineNum.

		/** Detailed constructor.
			@param lNum The line number.
			@param expr The line expression.
			@param mHit The optional minimum number of hits before break.
		*/
		BreakPoint(const unsigned int & lNum, const STLString & expr, const unsigned int & mHit = 0)
			:enabled(false), hitCount(0), minHit(mHit), lineNum(lNum), lineExpr(expr) {}
	};

	/**Convenience function for packing a scalar into a pre-allocated wrapper pointer.
		@param pBw Pointer to the pre-allocated wrapper space.
		@param scalar The scalar value to wrap.
	*/
	inline void LuaWrap(BaseWrap* pBw, const OME_SCALAR & scalar) { new(pBw)SclrWrap(scalar); }
	/**Convenience function for packing an Evaluable Pointer into a pre-allocated wrapper pointer.
		@param pBw Pointer to the pre-allocated wrapper space.
		@param pEval The Evaluable pointer to wrap.
	*/
	inline void LuaWrap(BaseWrap* pBw, Evaluable* pEval) { new(pBw)EvalWrap(pEval); };
	/**Convenience function for packing a VarArray Pointer into a pre-allocated wrapper pointer.
		@param pBw Pointer to the pre-allocated wrapper space.
		@param pVa The VarArray pointer to wrap.
	*/
	inline void LuaWrap(BaseWrap* pBw, VarArray* pVa) { new(pBw)ListWrap(*pVa); }
	/**Convenience function for packing a Listable into a pre-allocated wrapper pointer.
		@param pBw Pointer to the pre-allocated wrapper space.
		@param list The listable to wrap.
	*/
	inline void LuaWrap(BaseWrap* pBw, Listable& list) { new(pBw)ListWrap(list); }
	/**Convenience function for packing an EnumEntry pointer into a pre-allocated wrapper pointer.
		@param pBw Pointer to the pre-allocated wrapper space.
		@param pEe The EnumEntry pointer to wrap.
	*/
	inline void LuaWrap(BaseWrap* pBw, EnumEntry* pEe) { new(pBw)EnumWrap(pEe); }
	/**Convenience function for packing a ModelEnum pointer into a pre-allocated wrapper pointer.
		@param pBw Pointer to the pre-allocated wrapper space.
		@param pMe The ModelEnum pointer to wrap.
	*/
	inline void LuaWrap(BaseWrap* pBw, ModelEnum* pMe) { new(pBw)ECollWrap(pMe); }
	
	/**Convenience function for generating an EvalAlias and packing it in a wrapper.
		@param pBw Pointer to the pre-allocated wrapper space.
		@param pEval Pointer to the Evaluable to alias.
		@param pRep Pointer to the scalar value that is represented by the alias.
		@param index The index being represented by the alias.
	*/
	inline void LuaWrap(BaseWrap* pBw, Evaluable* pEval, OME_SCALAR* pRep, const size_t & index) 
	{ new(pBw)AliasWrap(pEval,pRep,index); }

	/**Convenience function for copying the contents of a BaseWrap.
		@param pBw Pointer to the pre-allocated wrapper space.
		@param bw The BaseWrap to duplicate.
	*/
	inline void LuaWrap(BaseWrap* pBw, BaseWrap& bw) 
	{ 
		switch (bw.m_type)
		{
		case BaseWrap::SCALAR:
			new(pBw)SclrWrap(bw);
			break;
		case BaseWrap::EVAL:
			new(pBw)EvalWrap(bw);
			break;
		case BaseWrap::LIST:
			new(pBw)ListWrap(bw);
			break; 
		case BaseWrap::ALIAS:
			new(pBw)AliasWrap(bw);
			break;
		case BaseWrap::MENUM:
			new(pBw)EnumWrap(bw);
			break;
		case BaseWrap::ECOLL:
			new(pBw)ECollWrap(bw);
			break;
		}
	}
	//inline void LuaWrap(BaseWrap* pBw, BaseWrap& bw) { return bw; };

	/** Retrieve the pointer of the component "calling" the function.
		@param pState Pointer to the lua state to query.
		@return Pointer to the Evaluable object acting as the caller.
	*/
	inline Evaluable* GetCaller(lua_State* pState)
	{
		lua_getglobal(pState,LuaDefines::EVAL_CALLER_ID);
		Evaluable* pEval=(Evaluable*)(*checkOMEWrap(pState,-1));
		lua_pop(pState,1);
		return pEval;
	}

	/** Retrieve the pointer of the manager running the simulation.
		@param pState Pointer to the lua state to query.
		@return Pointer to the BaseManager object that is managing the parent simulation.
	*/
	inline BaseManager* GetManager(lua_State* pState)
	{
		lua_getfield(pState,LUA_REGISTRYINDEX,LuaDefines::MANAGER_ID);
		BaseManager* pBm=(BaseManager*)lua_touserdata(pState,-1);
		lua_pop(pState,1);
		return pBm;
	}

	/** Check to see if the Lua script is being run in "test mode".
		@param pState Pointer to the lua state to query.
		@return true if the lua script is being run in test mode, false otherwise.
	*/
	inline bool InTestMode(lua_State *pState)
	{
		lua_getfield(pState,LUA_REGISTRYINDEX,LuaDefines::TEST_MODE);
		bool ret=lua_toboolean(pState,-1);
		lua_pop(pState,1);
		return ret;
	}

	/** Check if a stack entry is a wrapper of a specific Evaluable type.
		@param pState Pointer to the lua state to query.
		@param t The OMEType identifier to check for.
		@param idx The index in the lua stack to check.
		@return A pointer to the Evaluable contained in the EvalWrapper if
		it is present and its type is of t; NULL otherwise.
	*/
	inline Evaluable* CheckWrapEval(lua_State *pState,const OMETYPE & t,const int & idx)
	{
		BaseWrap* pRep = checkOMEWrap(pState, idx);
		Evaluable* ret = NULL;
		if (pRep && (Evaluable*)(*pRep) && ((Evaluable*)(*pRep))->IsOMEType(t))
			ret = (Evaluable*)(*pRep);
		return ret;
	}

	/** Check if a stack entry is an enumerated value, and return if it is.
	@param pState Pointer to the lua state to query.
	@param idx The index in the lua stack to check.
	@return A pointer to the EnumEntry object if the entry is
	an EnumEntry, or NULL otherwise.
	*/
	inline EnumEntry* CheckWrapEnumEntry(lua_State *pState, const int & idx)
	{
		BaseWrap* pRep = checkOMEWrap(pState, idx);
		EnumEntry* ret = (EnumEntry*)(*pRep);

		return ret;
	}

	/** Check if a stack entry is an enumerated collection, and return if it is.
		@param pState Pointer to the lua state to query.
		@param idx The index in the lua stack to check.
		@return A pointer to the ModelEnum object if the entry is
		a ModelEnum, or NULL otherwise.
	*/
	inline ModelEnum* CheckWrapModelEnumColl(lua_State *pState, const int & idx)
	{
		BaseWrap* pRep = checkOMEWrap(pState, idx);
		ModelEnum* ret = (ModelEnum*)(*pRep);

		return ret;
	}

	/** Check if a stack entry is a Listable, and return if it is.
		@param pState Pointer to the lua state to query.
		@param idx The index in the lua stack to check.
		@return A pointer to the Listable object if the entry is 
				a Listable, or NULL otherwise.
	*/
	inline Listable* CheckList(lua_State *pState, const int & idx)
	{
		BaseWrap* pRep = checkOMEWrap(pState, idx);
		Listable* ret = NULL;
		
		if (pRep && pRep->m_type == BaseWrap::LIST)
			ret = &(Listable&)(*pRep);
		return ret;
	}

	/** Attempt to convert value on Lua stack to Scalar value.
		@param L Pointer to the lua state to query.
		@param ind The index in the lua stack to convert.
		@param defaultVal The default value to pass on conversion failure.
		@return The converted scalar value, or defaultVal if the conversion fails.
	*/
	inline OME_SCALAR GetDouble(lua_State* L, const int & ind, OME_SCALAR defaultVal)
	{
		OME_SCALAR ret=defaultVal;
		BaseWrap* pRep;
		switch (lua_type(L, ind))
		{
		case LUA_TNUMBER:
			ret = (OME_SCALAR)lua_tonumber(L, ind);
			break;
		case LUA_TBOOLEAN:
			ret = (lua_toboolean(L, ind) != 0);
			break;
		case LUA_TUSERDATA:

			pRep = checkOMEWrap(L, ind);
			if (pRep)
			{
				ret = (OME_SCALAR)(*pRep);
				break;
			}
		}

		return ret;
	}

	/** Attempt to convert value on Lua stack to Scalar value. 
		@param L Pointer to the lua state to query.
		@param ind The index in the lua stack to convert.
		@return The converted scalar value. A lua error is pushed 
		        on to the stack if the conversion fails.
	*/
	inline OME_SCALAR GetDouble(lua_State* L, const int & ind)
	{
		OME_SCALAR ret;
		BaseWrap* pRep;
		switch (lua_type(L, ind))
		{
		case LUA_TNUMBER:
			ret = (OME_SCALAR)lua_tonumber(L, ind);
			break;
		case LUA_TBOOLEAN:
			ret = (lua_toboolean(L, ind) != 0);
			break;
		case LUA_TUSERDATA:

			pRep = checkOMEWrap(L, ind);
			if (pRep)
			{
				ret = (OME_SCALAR)(*pRep);
				break;
			}
			//else drop in to error

		default:
			luaL_error(L, "Double-precision number lookup failed.");
		}
		return ret;
	}
}

