#pragma once

#include "OMEDefines.h"
#include <random>
#include "EnumRep.h"

#include "LuaHeaders.h"
#include "OMELuaDefines.h"
#include "omeobjects/Evaluable.h"
#include "ValCache.h"

#pragma warning(push)
#pragma warning(disable : 4800)		
//convenience overload for dumping the contents of a lua_State stack
ostream& operator<<(ostream& outStrm, lua_State* pState);

typedef std::default_random_engine	RAN_ENGINE;
#ifndef OME_RAND_DEBUG
	#define SEED time(0)
#else
	#define SEED 1
#endif

//================= base class ========================
/** Convenience base class for managing lua function mapping and associated metadata.*/
class OMELuaFunc
{ 
public:
   /** @return The name of the function to be registered*/
	virtual const OMEChar* GetName() const=0;
   /** @return Pointer to the C-function to be registered with Lua. */
	virtual lua_CFunction GetFunc() const=0;
   /** @return A description of what the function does. */
	virtual const OMEChar* GetDescription() const=0;
   /** @return The minimum number of arguments required by the function. */
	virtual unsigned int GetMinNumArgs() const=0;
	virtual unsigned int GetMaxNumArgs() const;
	
	/** @return the number of arguments a passed in function will take.*/
	virtual unsigned int TakesFuncArg() const { return 0; }
	/** @return true if the function expects a list/table of boolean values.*/
	virtual bool TakesBoolTable() const { return false; }
	/** @return the associated boolean operator, if any.*/
	virtual const OMEChar* GetBoolOp() const { return NULL; }

	/** @return the name of the function to use when in init mode.*/
	virtual const OMEChar* GetAltInitFuncName() const { return NULL; }
	/** @return true If the function needs to know the calling object; false otherwise. */
	virtual bool SelfReferencing() const {return false;};
	void RegisterFunc(lua_State *pState);
	void RegisterFunc(lua_State *pState, int tblIdx);

   /** Dump the contents of the Lua-C exchange stack to a string.
	  @param pState Pointer to the state whose stack will be dumped.
	  @return A String representation of pState's stack.
   */
	inline static STLString DebugDump(lua_State* pState)
	{
		ostringstream out;
		out<<"Stack dump:"<<std::endl;
		out<<pState;
		return out.str();
	}
protected:

	static RAN_ENGINE m_sGen;     ///< C++11 engine used for random number generation.

	static ValCache<OME_SCALAR>& GetLuaFuncValCache();

	static int RunForTable(lua_State *pState, lua_CFunction func);

	//static void ReportErrorToManager(lua_State* pState,const OMEChar* msg,BaseManager* pBm=NULL);
};

#pragma warning(pop)