#include "OMELuaFunc.h"
#include "BaseManager.h"
#include "ValCache.h"

namespace LUA_MEM_CACHE
{
	ValCache<OME_SCALAR> cache=ValCache<OME_SCALAR>();
}

/** @return Reference to the global ValCache used with LuaFuncs.*/
ValCache<OME_SCALAR>& OMELuaFunc::GetLuaFuncValCache()
{
	return LUA_MEM_CACHE::cache;
}

ostream& operator<<(ostream& outStrm, lua_State* pState)
{
	for(int i=lua_gettop(pState); i>=1; i--)
	{
		outStrm<<i<<"\t";
		int t=lua_type(pState,i);
		switch(t)
		{
		case LUA_TSTRING:
			outStrm<<lua_tostring(pState,i);
			break;
		case LUA_TBOOLEAN:
			outStrm<<(lua_toboolean(pState,i) ? "true" : "false");
			break;
		case LUA_TNUMBER:
			outStrm<<lua_tonumber(pState,i);
			break;
		case LUA_TTABLE:
			outStrm<<"table("<<lua_objlen(pState,i)<<")";
			lua_pushnil(pState);
			//print keys
			while(lua_next(pState,i))
			{
				t=lua_type(pState,-2);
				if(t==LUA_TSTRING)
					outStrm<<endl<<"\t{ "<<lua_tostring(pState,-2)<<" }";
				else if(t==LUA_TNUMBER)
					outStrm<<endl<<"\t{ "<<lua_tonumber(pState,-2)<<" }";
				lua_pop(pState,1);
			}
			break;
		default:
			outStrm<<lua_typename(pState,t);
		};
		outStrm<<"\t"<<(lua_gettop(pState)-i+1)*-1<<std::endl;
	}
	return outStrm;
}

//initialize the random engine
RAN_ENGINE OMELuaFunc::m_sGen=RAN_ENGINE(SEED);

/** @return The maximum number of arguments that the function will evaluate.

   __NOTE__: by default, this function returns the same value as GetMinNumArgs().
   Children should override this function if their functions will accept any optional arguments.
*/
unsigned int OMELuaFunc::GetMaxNumArgs() const 
{ 
	return GetMinNumArgs(); 
}

/** Registers the represented function with the provided lua state. The function returned by GetFunc() is 
registered under the name returned by GetName().
   @param pState Pointer to the Lua state to register the function with.
*/
void OMELuaFunc::RegisterFunc(lua_State *pState)
{
	lua_pushcfunction(pState,this->GetFunc());
	lua_setglobal(pState,this->GetName());
}

/** Registers the represented function with the provided lua state. The function returned by GetFunc() is 
registered under the name returned by GetName().
   @param pState Pointer to the Lua state to register the function with.
   @param tblIdx The index of the table to register the function with.
*/
void OMELuaFunc::RegisterFunc(lua_State *pState, int tblIdx)
{
	lua_pushcfunction(pState,this->GetFunc());
	lua_setfield(pState,tblIdx,this->GetName());
}

/** Run a provided C-function for all items in a table. This is useful for some functions that 
   are assumed to operate over all items in a list if a list is passed in. The table is assumed
   to be at the top of the stack.
   @param pState Pointer to the lua State with the stack to evaluate.
   @param func The function to run on each item in the table.
   @return The number of new items placed on the stack.

*/
int OMELuaFunc::RunForTable(lua_State *pState, lua_CFunction func)
{
	
	//assume table is at the top of stack
	if(lua_istable(pState,-1))
	{
		int len=lua_objlen(pState,-1);
		//add new table
		lua_newtable(pState);
		for(int i=1; i<=len; i++)
		{
			lua_rawgeti(pState,-2,i);
			func(pState);
			lua_rawseti(pState,-2,i);
		}

		//clear old table
		lua_insert(pState,-2);
		lua_pop(pState,1);
	}
	return 1;
}
