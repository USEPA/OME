#pragma once
#include <random>
#include <sstream>

#include "omeobjects/Model.h"
#include "omeobjects/Spawner.h"
#include "omeobjects/IterConditional.h"
#include "OMELuaDefines.h"

#include "SimManager.h"

#include "OMELuaFunc.h"

#define NEWMAT3 new OME_SCALAR[9]
#define XY(x,y)		((x)*3)+(y)
namespace DefaultLuaFuncs
{

	using namespace LuaDefines;
	//====================================================================\\
		//                                                                    \\
		//                         Scalar Funcs                               \\
		//                                                                    \\
		//====================================================================\\

#define OME_LUASCALARFUNC1(FUNC_NAME,FUNC_SOURCE)								   \
	static int FUNC_NAME(lua_State *pState)										   \
			{																		   \
		PROFILE_FUNC();                                                            \
		int ret = 1;															   \
		OMECFuncs::BaseWrap* pRep;												   \
		if ((pRep = checkOMEWrap(pState, 1)))									   \
				{																		   \
			if (pRep->m_type == OMECFuncs::BaseWrap::LIST)						   \
				newOMERep(pState, FUNC_SOURCE((Listable&)(*pRep)));			       \
						else																   \
				lua_pushnumber(pState, FUNC_SOURCE((OME_SCALAR)(*pRep)));		   \
				}																		   \
				else																	   \
			lua_pushnumber(pState, FUNC_SOURCE(lua_tonumber(pState, 1)));          \
		return ret;																   \
		}

#define OME_LUASCALARFUNC2(FUNC_NAME,FUNC_SOURCE)								       \
	static int FUNC_NAME(lua_State *pState)										       \
		{                                                                              \
		PROFILE_FUNC();															       \
                                                                					   \
		lua_pushnumber(pState, FUNC_SOURCE(GetDouble(pState,1),GetDouble(pState,2)));  \
		return 1;																       \
	}


//================= abs( x ) ==========================
class OMELuaAbs : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "abs"; }
	virtual lua_CFunction GetFunc() const { return luaAbs; };
	virtual const OMEChar* GetDescription() const { return "abs(x) - Return absolute value of x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaAbs, OMECFuncs::abs)
};

//================= acos( x ) =========================
class OMELuaAcos : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "acos"; }
	virtual lua_CFunction GetFunc() const { return luaAcos; };
	virtual const OMEChar* GetDescription() const { return "acos(x) - Return arc-cosine of x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaAcos, OMECFuncs::acos)
};

//================= asin( x ) =========================
class OMELuaAsin : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "asin"; }
	virtual lua_CFunction GetFunc() const { return luaAsin; };
	virtual const OMEChar* GetDescription() const { return "asin(x) - Return arc-sine of x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaAsin, OMECFuncs::asin)
};

//================= at_init( x ) ======================
class OMELuaAtInit : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "at_init"; }
	virtual lua_CFunction GetFunc() const { return luaAtInit; };
	virtual const OMEChar* GetDescription() const { return "at_init(x) - Return the initial value of x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaAtInit(lua_State *pState)
	{
		int ret=1;
		OMECFuncs::BaseWrap* pRep;												   
		if ((pRep = checkOMEWrap(pState, 1)))
			newOMERep(pState, OMECFuncs::at_init(*pRep));
		else
			luaL_error(pState, "at_init: not a valid argument");
		return ret;
	}
};

//================= atan( x ) =========================
class OMELuaAtan : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "atan"; }
	virtual lua_CFunction GetFunc() const { return luaAtan; };
	virtual const OMEChar* GetDescription() const { return "atan(x) - Return arc-tangent of x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaAtan, OMECFuncs::atan)
};

//================= atan2( x, y ) ======================
class OMELuaAtan2 : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "atan2"; }
	virtual lua_CFunction GetFunc() const { return luaAtan2; };
	virtual const OMEChar* GetDescription() const { return "atan2(x,y) - Return arc-tangent between x and y."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaAtan2(lua_State *pState)
	{
		PROFILE_FUNC();
		OMECFuncs::BaseWrap* pRep;
		if (lua_gettop(pState) >= 2)
		{
			/// @question should this be a wrapped?
			lua_pushnumber(pState, OMECFuncs::atan2(GetDouble(pState,1), GetDouble(pState,2)));
		}
		else if ((pRep = checkOMEWrap(pState, 1)) && pRep->m_type==OMECFuncs::BaseWrap::LIST)
			newOMERep(pState, OMECFuncs::atan2((Listable&)(*pRep)));
		else
			luaL_error(pState, "atan2: invalid arguments");

		return 1;
	}
};

//================= binome( x, y ) ======================
class OMELuaBinom : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "binome"; }
	virtual lua_CFunction GetFunc() const { return luaBinom; };
	virtual const OMEChar* GetDescription() const { return "binome(x,y) - Return random value from binomal distribution."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaBinom(lua_State *pState)
	{
		PROFILE_FUNC();

		if (lua_gettop(pState) >= 2)
		{
			int arg1=(int)GetDouble(pState,1);
			OME_SCALAR arg2=GetDouble(pState,2);

			/// @question should this be a wrapped?
			lua_pushnumber(pState, OMECFuncs::binome(arg1, arg2));
		}
		else
			luaL_error(pState, "binome: not enough arguments");

		return 1;
	}
};

//================= ceil( x ) ==========================
class OMELuaCeil : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "ceil"; }
	virtual lua_CFunction GetFunc() const { return luaCeil; };
	virtual const OMEChar* GetDescription() const { return "ceil(x) - Return ceiling value of x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaCeil, OMECFuncs::ceil)
};

//================= channel_is( x )=====================
class OMELuaChannelIs : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "channel_is"; }
	virtual lua_CFunction GetFunc() const { return luaChannelIs; };
	virtual const OMEChar* GetDescription() const { return "channel_is(x) - Return True if current model instance was spawned by x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }
	virtual bool SelfReferencing() const{return true;}

	static int luaChannelIs(lua_State *pState)
	{
		PROFILE_FUNC();
		
		Spawner* pSpwn = (Spawner*)CheckWrapEval(pState, OME_SPAWNER,1);
		if (pSpwn)
			lua_pushboolean(pState, OMECFuncs::channel_is(*LuaDefines::GetCaller(pState), *pSpwn));
		else
			luaL_error(pState,"channel is: argument must be a spawner");
		return 1;
	}
};

//================= const_delay( x, t )=================
class OMELuaConstDelay : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "const_delay"; }
	virtual lua_CFunction GetFunc() const { return luaConstDelay; };
	virtual const OMEChar* GetDescription() const { return "const_delay(x,t) - Return value of x t units ago."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }
	virtual const OMEChar* GetAltInitFuncName() const { return "lookupInit_const_delay"; }

	static int luaConstDelay(lua_State *pState)
	{
		PROFILE_FUNC();
		
		if (lua_gettop(pState) >= 2)
		{
			OMECFuncs::BaseWrap* pRep = checkOMEWrap(pState, 1);

			if (pRep)
				lua_pushnumber(pState, const_delay(*pRep, lua_tonumber(pState, 2)));
			else
				luaL_error(pState, "const_delay: arg1 is not wrapped.");
		}
		else
			luaL_error(pState, "const_delay: not enough arguments");
		return 1;
	}
};

//================= cos( x ) ===========================
class OMELuaCos : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "cos"; }
	virtual lua_CFunction GetFunc() const { return luaCos; };
	virtual const OMEChar* GetDescription() const { return "cos(x) - Return cosine of x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaCos, OMECFuncs::cos)
};

//================= cosh( x ) ==========================
class OMELuaCosh : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "cosh"; }
	virtual lua_CFunction GetFunc() const { return luaCosh; };
	virtual const OMEChar* GetDescription() const { return "cosh(x) - Return cosineh of x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaCosh, OMECFuncs::cosh)
};

//================= dies_of( x ) =======================
class OMELuaDiesOf : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "dies_of"; }
	virtual lua_CFunction GetFunc() const { return luaDiesOf; };
	virtual const OMEChar* GetDescription() const { return "dies_of(x) - Returns true if object/model will die at end of step due to spawner x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }
	virtual bool SelfReferencing() const{return true;}

	static int luaDiesOf(lua_State *pState)
	{
		PROFILE_FUNC();
		Spawner* pSpwn = (Spawner*)CheckWrapEval(pState, OME_SPAWNER,1);
		if (pSpwn)
			lua_pushboolean(pState, OMECFuncs::dies_of(*LuaDefines::GetCaller(pState), *pSpwn));
		else
			luaL_error(pState, "dies_of: argument must be a spawner");
		return 1;
		return 1;
	}
};

//================= dt( x ) ============================
class OMELuaDt : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "dt"; }
	virtual lua_CFunction GetFunc() const { return luaDt; };
	virtual const OMEChar* GetDescription() const { return "dt(x) - Step Interval for model at step x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }
	virtual bool SelfReferencing() const{return true;}

	static int luaDt(lua_State *pState)
	{
		PROFILE_FUNC();
		OMECFuncs::BaseWrap* pRep=checkOMEWrap(pState,1);
		if (pRep)
		{
			if (pRep->m_type!=OMECFuncs::BaseWrap::LIST)
				lua_pushnumber(pState, OMECFuncs::dt(*GetCaller(pState), (OME_SCALAR)(*pRep)));
			else
				newOMERep(pState, OMECFuncs::dt(*GetCaller(pState), (Listable&)(*pRep)));
		}
		else
			lua_pushnumber(pState, OMECFuncs::dt(*GetCaller(pState), lua_tonumber(pState, 1)));
		return 1;
	}
};

//================= exp( x ) ===========================
class OMELuaExp : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "exp"; }
	virtual lua_CFunction GetFunc() const { return luaExp; };
	virtual const OMEChar* GetDescription() const { return "exp(x) - Return e raised to the power of x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaExp, OMECFuncs::exp)
};

//================= factorial( x ) =====================
class OMELuaFactorial : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "factorial"; }
	virtual lua_CFunction GetFunc() const { return luaFact; };
	virtual const OMEChar* GetDescription() const { return "factorial(x) - Return x!."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaFact, OMECFuncs::factorial)
};

//================= first( x ) =========================
class OMELuaFirst : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "first"; }
	virtual lua_CFunction GetFunc() const { return luaFirst; };
	virtual const OMEChar* GetDescription() const { return "first(x) - Return true if enumerated type is first in enumerated type."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaFirst(lua_State *pState)
	{
		EnumEntry* pEE = CheckWrapEnumEntry(pState, 1);
		if (pEE)
			lua_pushboolean(pState, OMECFuncs::first(pEE));
		else
			luaL_error(pState, "first: argument is not an enumerated value.");
		return 1;
	}
};

//================= floor( x ) =========================
class OMELuaFloor : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "floor"; }
	virtual lua_CFunction GetFunc() const { return luaFloor; };
	virtual const OMEChar* GetDescription() const { return "floor(x) - Return floor of x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaFloor, OMECFuncs::floor)
};

//================= fmod( x, y ) =======================
class OMELuaFmod : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "fmod"; }
	virtual lua_CFunction GetFunc() const { return luaFmod; };
	virtual const OMEChar* GetDescription() const { return "fmod(x,y) - Return x mod y."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	OME_LUASCALARFUNC2(luaFmod, fmod)
};

//================= following( x ) =====================
class OMELuaFollowing : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "following"; }
	virtual lua_CFunction GetFunc() const { return luaFollowing; };
	virtual const OMEChar* GetDescription() const { return "following(x) - Return the enumerated value following enumeration x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaFollowing(lua_State *pState)
	{
		PROFILE_FUNC();
		union
		{
			EnumEntry* pEe;
			OMECFuncs::BaseWrap* pRep;
		};

		if ((pRep = checkOMEWrap(pState, 1)) && pRep->m_type == OMECFuncs::BaseWrap::LIST)
		{
			Listable& lst = (Listable&)(*pRep);
			newOMERep(pState, *NewTempLambdaList(lst.GetSize(), [&lst](const size_t & i){return lst[i] + 1; }));
		}
		else if ((pEe = CheckWrapEnumEntry(pState, 1)))
		{
			if (pEe->ind != pEe->pCollection->GetEnumValueCount())
				newOMERep(pState, pEe->pCollection->AddrForIndex(pEe->ind + 1));
			else
				lua_pushnumber(pState, 0.0);
		}
		else
			luaL_error(pState, "following: argument is not an enumerated value or list of enumerated values.");

		return 1;
	}
};

//================= gaussian_var( x, y ) ===============
class OMELuaGuassianVar : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "gaussian_var"; }
	virtual lua_CFunction GetFunc() const { return luaGaussian; };
	virtual const OMEChar* GetDescription() const { return "gaussian_var(x,y) - Return random value from gaussian distribution."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	OME_LUASCALARFUNC2(luaGaussian,OMECFuncs::gaussian_var)
};

//================= graph( x ) =========================
class OMELuaGraph : public OMELuaFunc
{
public:

	// renamed to interpTable
	virtual const OMEChar* GetName() const { return "interpTable"; }
	virtual lua_CFunction GetFunc() const { return luaInterpTable; };
	virtual const OMEChar* GetDescription() const { return "interpTable(x) - Return interpolated value from stored table."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }
	virtual bool SelfReferencing() const { return true; }

	static int luaInterpTable(lua_State *pState)
	{
		PROFILE_FUNC();
		
		try
		{
			Evaluable* pEval = GetCaller(pState);
			BaseManager* pBm = GetManager(pState);
			lua_pushnumber(pState, OMECFuncs::interpTable(*pEval, pBm, GetDouble(pState, 1)));
		}
		catch (std::runtime_error* err)
		{
			luaL_error(pState, "interpTable: %s", err->what());
		}
		
		return 1;
	}
};

//================= hypergeom( x, y, z ) ===============
class OMELuaHyperGeom : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "hypergeom"; }
	virtual lua_CFunction GetFunc() const { return luaHyperGeom; };
	virtual const OMEChar* GetDescription() const { return "hypergeom(x,y,z) - Return random value from hypergeometric distribution distribution."; }
	virtual unsigned int GetMinNumArgs() const { return 3; }

	static int luaHyperGeom(lua_State *pState)
	{
		PROFILE_FUNC();

		lua_pushinteger(pState, OMECFuncs::hypergeom(GetDouble(pState, 1), GetDouble(pState, 2), GetDouble(pState, 3)));
		return 1;
	}
};

//================= hypot( x, y ) ======================
class OMELuaHypot : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "hypot"; }
	virtual lua_CFunction GetFunc() const { return luaHypot; };
	virtual const OMEChar* GetDescription() const { return "hypot(x, y) - Return hypoteneus of right triangle with legs x and y."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	OME_LUASCALARFUNC2(luaHypot,OMECFuncs::hypot)
};

//================= init_time( I ) =====================
class OMELuaInitTime : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "init_time"; }
	virtual lua_CFunction GetFunc() const { return luaInitTime; };
	virtual const OMEChar* GetDescription() const { return "init_time(x) - Time at which object was created"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }
	virtual bool SelfReferencing() const{return true;}

	static int luaInitTime(lua_State *pState)
	{
		PROFILE_FUNC();
		
		OMECFuncs::BaseWrap* pRep = checkOMEWrap(pState, 1);
		if (pRep)
		{
			if (pRep->m_type!=OMECFuncs::BaseWrap::LIST)
				lua_pushnumber(pState, OMECFuncs::init_time(*GetCaller(pState)));
			//else
				//newOMERep(pState, OMECFuncs::init_time(*GetCaller(pState), (Listable&)(*pRep)));
		}
		else
			lua_pushnumber(pState, OMECFuncs::init_time(*GetCaller(pState)));

		return 1;
	}
};

//================= in_preceding( x ) ==================
class OMELuaInPreceding : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "in_preceding"; }
	virtual lua_CFunction GetFunc() const { return luaInPreceding; };
	virtual const OMEChar* GetDescription() const { return "in_preceding(x) - find value of object x in preceding instance of submodel."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }
	virtual bool SelfReferencing() const{return true;}

	static int luaInPreceding(lua_State *pState)
	{
		/// @todo update once OMECFuncs::in_preceding is implemented.
		PROFILE_FUNC();
		luaL_error(pState,"InPreceding: Disabled for now");
		return 0;
	}
};

//================= in_progenitor( x ) =================
class OMELuaInProgenitor: public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "in_progenitor"; }
	virtual lua_CFunction GetFunc() const { return luaInProgenitor; };
	virtual const OMEChar* GetDescription() const { return "in_progenitor(x) - find value of object x in instance that spawned submodel."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }
	virtual bool SelfReferencing() const{return true;}

	static int luaInProgenitor(lua_State *pState)
	{
		PROFILE_FUNC();
		/// @todo update once OMECFuncs::in_progenitor is implemented.
		PROFILE_FUNC();
		luaL_error(pState, "InProgenitor: Disabled for now");
		return 0;
	}
};
//================= index( x ) =========================
class OMELuaIndex : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "index"; }
	virtual lua_CFunction GetFunc() const { return luaIndex; };
	virtual const OMEChar* GetDescription() const { return "index(x) - Return index of model instance for depth x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }
	virtual bool SelfReferencing() const{return true;}

	static int luaIndex(lua_State *pState)
	{
		PROFILE_FUNC();
		OMECFuncs::BaseWrap* pRep = checkOMEWrap(pState, 1);
		if (pRep)
		{
			if (pRep->m_type != OMECFuncs::BaseWrap::LIST)
				lua_pushnumber(pState, OMECFuncs::index(*GetCaller(pState), (unsigned int)(OME_SCALAR)(*pRep)));
			else
				newOMERep(pState, OMECFuncs::index(*GetCaller(pState), (Listable&)(*pRep)));
		}
		else
			lua_pushnumber(pState, OMECFuncs::index(*GetCaller(pState), lua_tointeger(pState, 1)));

		return 1;
	}
};

//================= int( x ) ===========================
class OMELuaInt : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "int"; }
	virtual lua_CFunction GetFunc() const { return luaInt; };
	virtual const OMEChar* GetDescription() const { return "int(x) - Return int value of x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaInt, OMECFuncs::toInt)
};

//================= last( x ) ==========================
class OMELuaLast : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "last"; }
	virtual lua_CFunction GetFunc() const { return luaLast; };
	virtual const OMEChar* GetDescription() const { return "last(x) - Return previous value of x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }
	virtual const OMEChar* GetAltInitFuncName() const { return "lookupInit_last"; }
	
	static int luaLast(lua_State *pState)
	{
		//add time argument, call const_delay.
		PROFILE_FUNC();
		OMECFuncs::BaseWrap* pRep=checkOMEWrap(pState,1);
		if (pRep)
			lua_pushnumber(pState, OMECFuncs::last(GetManager(pState), *pRep));
		else
			luaL_error(pState, "last: argument is not a model component");
		return 1;
	}
};

//================= log( x ) ===========================
class OMELuaLog : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "log"; }
	virtual lua_CFunction GetFunc() const { return luaLog;};
	virtual const OMEChar* GetDescription() const { return "log(x) - Return natural log of x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaLog, OMECFuncs::log)
};

//================= log10( x ) =========================
class OMELuaLog10 : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "log10"; }
	virtual lua_CFunction GetFunc() const { return luaLog10;};
	virtual const OMEChar* GetDescription() const { return "log10(x) - Return base-10 log of x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaLog10, OMECFuncs::log10)
};

//================= max( x, y ) ========================
class OMELuaMax : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "max"; }
	virtual lua_CFunction GetFunc() const { return luaMax; };
	virtual const OMEChar* GetDescription() const { return "max(x, y) - Return The maximum value of pair x and y."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC2(luaMax,OMECFuncs::OMEMax)
};

//================= min( x, y ) ========================
class OMELuaMin : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "min"; }
	virtual lua_CFunction GetFunc() const { return luaMin; };
	virtual const OMEChar* GetDescription() const { return "min(x, y) - Return The minimum value of pair x and y."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	OME_LUASCALARFUNC2(luaMin, OMECFuncs::OMEMin)
};

//================= parent() ===========================
class OMELuaParent: public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "parent"; }
	virtual lua_CFunction GetFunc() const { return luaParent; };
	virtual const OMEChar* GetDescription() const { return "parent() - return the negated instance number of the parent."; }
	virtual unsigned int GetMinNumArgs() const { return 0; }
	virtual bool SelfReferencing() const{return true;}

	static int luaParent(lua_State *pState)
	{
		PROFILE_FUNC();
		lua_pushnumber(pState, OMECFuncs::parent(*GetCaller(pState)));
		return 1;
	}
};

//================= poidev( x ) ========================
class OMELuaPoidev : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "poidev"; }
	virtual lua_CFunction GetFunc() const { return luaPoidev; };
	virtual const OMEChar* GetDescription() const { return "poidev(x) - Return random value from poisson distribution."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaPoidev(lua_State *pState)
	{
		PROFILE_FUNC();
		lua_pushnumber(pState, OMECFuncs::poidev(GetDouble(pState,1)));
		return 1;
	}
};

//================= pow( x, y ) ========================
class OMELuaPower : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "pow"; }
	virtual lua_CFunction GetFunc() const { return luaPow; };
	virtual const OMEChar* GetDescription() const { return "pow(x,y) - Return x raised to the power of y."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaPow(lua_State *pState)
	{
		PROFILE_FUNC();
		enum { SS = 0, LS, SL, LL };
		unsigned short mode = 0;
		OME_SCALAR x, y;
		OMECFuncs::BaseWrap* pRep1, *pRep2;

		if ((pRep1 = checkOMEWrap(pState, 1)))
		{
			if (pRep1->m_type == OMECFuncs::BaseWrap::LIST)
				mode += 1;
			else
				x = (OME_SCALAR)(*pRep1);
		}
		else
			x = lua_tonumber(pState, 1);

		if ((pRep2 = checkOMEWrap(pState, 2)))
		{
			if (pRep2->m_type == OMECFuncs::BaseWrap::LIST)
				mode += 2;
			else
				y = (OME_SCALAR)(*pRep2);
		}
		else
			y = lua_tonumber(pState, 2);

		switch (mode)
		{
		case LL:
			luaL_error(pState, "pow: both arguments are lists.");
			break;
		case SL:
			newOMERep(pState, OMECFuncs::pow((OME_SCALAR)x, (Listable&)(*pRep2)));
			break;
		case LS:
			newOMERep(pState, OMECFuncs::pow((Listable&)(*pRep1), (OME_SCALAR)y));
			break;
		case SS:
			lua_pushnumber(pState, OMECFuncs::pow(x, y));
		}

		return 1;
	}
};

//================= preceding( T ) =====================
class OMELuaPreceding : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "preceding"; }
	virtual lua_CFunction GetFunc() const { return luaPreceding; };
	virtual const OMEChar* GetDescription() const { return "preceding(x) - Return the enumerated value preceeding enumeration x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaPreceding(lua_State *pState)
	{
		union
		{
			EnumEntry* pEe;
			OMECFuncs::BaseWrap* pRep;
		};

		if ((pRep = checkOMEWrap(pState, 1)) && pRep->m_type == OMECFuncs::BaseWrap::LIST)
		{
			Listable& lst = (Listable&)(*pRep);
			newOMERep(pState, *NewTempLambdaList(lst.GetSize(), [&lst](const size_t & i){return lst[i] - 1; }));
		}
		else if ((pEe = CheckWrapEnumEntry(pState, 1)))
		{
			if (pEe->ind >1)
				newOMERep(pState, pEe->pCollection->AddrForIndex(pEe->ind - 1));
			else
				lua_pushnumber(pState, 0.0);
		}
		else
			luaL_error(pState, "preceding: argument is not an enumerated value or list of enumerated values.");

		return 1;
	}
};

//================= prev( x ) ==========================
class OMELuaPrev : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "prev"; }
	virtual lua_CFunction GetFunc() const { return luaPrev; };
	virtual const OMEChar* GetDescription() const { return "prev(x) - Return the previous value of the calling object."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }
	virtual bool SelfReferencing() const{return true;}
	virtual const OMEChar* GetAltInitFuncName() const { return "lookupInit_prev"; }

	static int luaPrev(lua_State *pState)
	{
		PROFILE_FUNC();
		Evaluable* pCaller = GetCaller(pState);

		OME_SCALAR x=GetDouble(pState,1);
		
		switch (pCaller->GetOMEType())
		{
		case OME_VARARRAY:
			newOMERep(pState, *OMECFuncs::prev(*((VarArray*)pCaller), x));
			break;
		default:
			lua_pushnumber(pState, OMECFuncs::prev(*pCaller, x));
		}

		return 1;
	}
};

//================= pulse( x,y[,z] ) ===================
class OMELuaPulse : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "pulse"; }
	virtual lua_CFunction GetFunc() const { return luaPulse; };
	virtual const OMEChar* GetDescription() const { return "pulse(x,y[,z]) - Send out value x at time y, optionally repeating on an interval z"; }
	virtual unsigned int GetMinNumArgs() const { return 2; }
	virtual unsigned int GetMaxNumArgs() const { return 3; }

	static int luaPulse(lua_State *pState)
	{
		PROFILE_FUNC();
		
		OME_SCALAR mag = GetDouble(pState, 1);
		OME_SCALAR start = GetDouble(pState, 2);

		if (lua_gettop(pState)<=2)
		{
			lua_pushnumber(pState, OMECFuncs::pulse(mag, start, GetManager(pState)));
		}
		else
		{
			OME_SCALAR interval = GetDouble(pState, 3);
			lua_pushnumber(pState, OMECFuncs::pulse(mag, start,interval, GetManager(pState)));

		}
		return 1;
	}
};

//================= ramp( x,y ) ========================
class OMELuaRamp : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "ramp"; }
	virtual lua_CFunction GetFunc() const { return luaRamp; };
	virtual const OMEChar* GetDescription() const { return "ramp(x,y) - Linearly increase value by x starting at time y"; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaRamp(lua_State *pState)
	{
		PROFILE_FUNC();

		OME_SCALAR slope = GetDouble(pState, 1);
		OME_SCALAR start = GetDouble(pState, 2);
		lua_pushnumber(pState, OMECFuncs::ramp(slope, start, GetManager(pState)));
		
		return 1;
	}
};

//================= rand_var( x, y ) ===================
class OMELuaRandVar : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "rand_var"; }
	virtual lua_CFunction GetFunc() const { return luaRandVar; };
	virtual const OMEChar* GetDescription() const { return "rand_var(x,y) - Return a random value between x and y."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	OME_LUASCALARFUNC2(luaRandVar, OMECFuncs::rand_var);
};

//================= rand_const( x, y ) =================
class OMELuaRandConst : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "rand_const"; }
	virtual lua_CFunction GetFunc() const { return luaRandConst; };
	virtual const OMEChar* GetDescription() const { return "rand_const(x,y) - Return a random value between x and y; returns same value after initial call."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }
	virtual bool SelfReferencing() const{return true;}

	static int luaRandConst(lua_State *pState)
	{
		//disabled
		luaL_error(pState, "rand_const: disabled for now");
		return 0;
	}
};


//================= round( x ) =========================
class OMELuaRound : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "round"; }
	virtual lua_CFunction GetFunc() const { return luaRound; };
	virtual const OMEChar* GetDescription() const { return "round(x) - Return value of x rounded to the nearest whole number."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaRound, OMECFuncs::round)
};

//================= self( ) ============================
class OMELuaSelf : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "self"; }
	virtual lua_CFunction GetFunc() const { return luaSelf; };
	virtual const OMEChar* GetDescription() const { return "self() - Return reference to caller."; }
	virtual unsigned int GetMinNumArgs() const { return 0; }

	static int luaSelf(lua_State *pState)
	{
		lua_getglobal(pState, LuaDefines::EVAL_CALLER_ID);
		return 1;
	}
};

//================= sgn( x ) ===========================
class OMELuaSgn : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "sgn"; }
	virtual lua_CFunction GetFunc() const { return luaSgn; };
	virtual const OMEChar* GetDescription() const { return "sgn(x) - Return value that represents sign of x (1 or -1)"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaSgn, OMECFuncs::sgn)
};

//================= sin( x ) ===========================
class OMELuaSin : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "sin"; }
	virtual lua_CFunction GetFunc() const { return luaSin; };
	virtual const OMEChar* GetDescription() const { return "sin(x) - Return sine of x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaSin, OMECFuncs::sin)
};

//================= sinh( x ) ==========================
class OMELuaSinh : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "sinh"; }
	virtual lua_CFunction GetFunc() const { return luaSinh; };
	virtual const OMEChar* GetDescription() const { return "sinh(x) - Return sinh of x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaSinh, OMECFuncs::sinh)
};

//================= sqrt( x ) ==========================
class OMELuaSqrt : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "sqrt"; }
	virtual lua_CFunction GetFunc() const { return luaSqrt; };
	virtual const OMEChar* GetDescription() const { return "sqrt(x) - Return the square root of x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaSqrt, OMECFuncs::sqrt)
};

//================= step( x,y ) ========================
class OMELuaStep : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "step"; }
	virtual lua_CFunction GetFunc() const { return luaStep; };
	virtual const OMEChar* GetDescription() const { return "step(x,y) - Step value from 0 to x at time y."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaStep(lua_State *pState)
	{
		PROFILE_FUNC();

		OME_SCALAR height = GetDouble(pState, 1);
		OME_SCALAR start = GetDouble(pState, 2);
		lua_pushnumber(pState, OMECFuncs::step(height, start, GetManager(pState)));

		return 1;
	}
};

//================= stop( x ) ==========================
class OMELuaStop : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "stop"; }
	virtual lua_CFunction GetFunc() const { return luaStop; };
	virtual const OMEChar* GetDescription() const { return "stop(x) - Terminate simulation with err code x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaStop(lua_State *pState)
	{
		PROFILE_FUNC();

		OME_SCALAR errCode = GetDouble(pState, 1);
		
		OMECFuncs::stop(errCode,GetManager(pState));
		luaL_error(pState, "Execution halted with user code: %d", errCode);
		return 0;
	}
};

//================= table( x ) =========================
class OMELuaTable : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "table"; }
	virtual lua_CFunction GetFunc() const { return luaValTable; };
	virtual const OMEChar* GetDescription() const { return "table(x...) - Return value stored in internal table of caller at indicated indices."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }
	virtual unsigned int GetMaxNumArgs() const { return UINT_MAX; /*variadic */ }
	virtual bool SelfReferencing() const { return true; }

	static int luaValTable(lua_State *pState)
	{
		PROFILE_FUNC();

		OME_SCALAR arg1 = GetDouble(pState, 1);

		if (lua_gettop(pState) > 1)
		{
			OME_SCALAR arg2=GetDouble(pState,2);
			lua_pushnumber(pState, OMECFuncs::table(*GetCaller(pState), GetManager(pState), (unsigned int)arg1, (unsigned int)arg2));
		}
		else
			lua_pushnumber(pState, OMECFuncs::table(*GetCaller(pState), GetManager(pState), (unsigned int)arg1));
		return 1;
	}
};

//================= tan( x ) ===========================
class OMELuaTan : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "tan"; }
	virtual lua_CFunction GetFunc() const { return luaTan; };
	virtual const OMEChar* GetDescription() const { return "tan(x) - Return tangent of x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaTan, OMECFuncs::tan)

};

//================= tanh( x ) ==========================
class OMELuaTanh : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "tanh"; }
	virtual lua_CFunction GetFunc() const { return luaTanh; };
	virtual const OMEChar* GetDescription() const { return "tanh(x) - Return tangenth of x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASCALARFUNC1(luaTanh, OMECFuncs::tanh)
};

//================= time() =============================
class OMELuaTime : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "time"; }
	virtual lua_CFunction GetFunc() const { return luaTime; };
	virtual const OMEChar* GetDescription() const { return "time() - Return the current simulation time."; }
	virtual unsigned int GetMinNumArgs() const { return 0; }
	virtual unsigned int GetMaxNumArgs() const { return 1; }

	static int luaTime(lua_State *pState)
	{
		PROFILE_FUNC();
		if (!lua_gettop(pState))
			lua_pushnumber(pState, OMECFuncs::currenttime(GetManager(pState)));
		else
			lua_pushnumber(pState, OMECFuncs::currenttime(GetManager(pState), GetDouble(pState, 1)));

		return 1;
	}
};

//================= var_delay( x, t ) ==================
class OMELuaVarDelay : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "var_delay"; }
	virtual lua_CFunction GetFunc() const { return luaVarDelay; };
	virtual const OMEChar* GetDescription() const { return "var_delay(x,y) - Return the value of x y units ago."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }
	virtual const OMEChar* GetAltInitFuncName() const { return "lookupInit_var_delay"; }

	static int luaVarDelay(lua_State *pState)
	{
		PROFILE_FUNC();
		OMECFuncs::BaseWrap* pRep=checkOMEWrap(pState,1);
		if (pRep)
			lua_pushnumber(pState, OMECFuncs::var_delay(*pRep, GetDouble(pState, 2)));
		else
			luaL_error(pState, "var_delay: not called on model component.");
	
		return 1;
	}
};



//====================================================================\\
//																	  \\
//                         Array Funcs                                \\
//																	  \\
//====================================================================\\


/// \todo re-enabled lua list stuff once compiled coe refactoring is finished
//#define OME_LUASIMPLELISTFUNC1(FUNC_NAME,SOURCE_FUNC,STACK_CALL)	\
//static int FUNC_NAME(lua_State *pState)									\
//{																		\
//	return 0;		\
//	}
//#define OME_LUASIMPLELISTFUNC2(FUNC_NAME,SOURCE_FUNC,STACK_CALL)	\
//static int FUNC_NAME(lua_State *pState)									\
//{																		\
//	return 0;		\
//	}
#define OME_LUASIMPLELISTFUNC1(FUNC_NAME,SOURCE_FUNC,STACK_CALL)		\
static int FUNC_NAME(lua_State *pState)									\
{																		\
	PROFILE_FUNC();														\
	OMECFuncs::BaseWrap* pRep = checkOMEWrap(pState, 1);				\
	if (pRep)															\
		STACK_CALL(pState, OMECFuncs::SOURCE_FUNC(*pRep));				\
	else																\
		luaL_error(pState, #SOURCE_FUNC": argument is not a list.");	\
	return 1;															\
}

#define OME_LUASIMPLELISTFUNC2(FUNC_NAME,SOURCE_FUNC,STACK_CALL)		\
static int FUNC_NAME(lua_State *pState)									\
{																		\
	PROFILE_FUNC();														\
	OMECFuncs::BaseWrap* pRep1 = checkOMEWrap(pState, 1);				\
	OMECFuncs::BaseWrap* pRep2 = checkOMEWrap(pState, 2);				\
	if (pRep1 && pRep2)													\
		STACK_CALL(pState, OMECFuncs::SOURCE_FUNC(*pRep1,*pRep2));		\
		else																\
		luaL_error(pState, #SOURCE_FUNC": one or more arguments are not lists");	\
	return 1;															\
}

#define OME_LUALISTAGGFUNC(FUNC_NAME,SRC_FUNC,AGG_FUNC,INIT_VAL)						 \
static int FUNC_NAME(lua_State *pState)													 \
{																						 \
	PROFILE_FUNC();																		 \
																						 \
	Listable* pList = CheckList(pState, 1);												 \
	if (pList)																			 \
	{																					 \
		if (pList->GetLevel() == 1)														 \
			lua_pushnumber(pState, OMECFuncs::SRC_FUNC(*pList));					     \
		else																			 \
		{																				 \
			Listable* tmp = pList->SubAggregate([](Listable* pDest, Listable* pSrc, 	 \
				const size_t & dInd, const size_t & sInd, const size_t & rInd)			 \
			 AGG_FUNC , INIT_VAL);				     									 \
			newOMERep(pState, *tmp);													 \
		}																				 \
	}																					 \
	else																				 \
		luaL_error(pState, #SRC_FUNC": argument must be list");					         \
	return 1;																			 \
}


//================= all([X]) ===========================
class OMELuaAll : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "all"; }
	virtual lua_CFunction GetFunc() const { return luaAll; };
	virtual const OMEChar* GetDescription() const { return "all([X]) - return true if all elements in array X are true"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASIMPLELISTFUNC1(luaAll, allSclr, lua_pushboolean)
};

//================= any([X]) ===========================
class OMELuaAny : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "any"; }
	virtual lua_CFunction GetFunc() const { return luaAny; };
	virtual const OMEChar* GetDescription() const { return "any([X]) - return true if any elements in array X are true"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASIMPLELISTFUNC1(luaAny, anySclr, lua_pushboolean)
};

//================= colin([X]) =========================
class OMELuaColin : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "colin"; }
	virtual lua_CFunction GetFunc() const { return luaColin; };
	virtual const OMEChar* GetDescription() const { return "colin([X]) - return an index chosen by probability of entry"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASIMPLELISTFUNC1(luaColin, colin, lua_pushinteger)
};

//================= count([X]) =========================
class OMELuaCount : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "count"; }
	virtual lua_CFunction GetFunc() const { return luaCount; };
	virtual const OMEChar* GetDescription() const { return "count([X]) - return the number of elements in array X"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASIMPLELISTFUNC1(luaCount, count, lua_pushnumber)
};

//================= element([X],I) =====================
class OMELuaElement : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "element"; }
	virtual lua_CFunction GetFunc() const { return luaElement; };
	virtual const OMEChar* GetDescription() const { return "element([X],I) - return the Ith item in array X"; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaElement(lua_State *pState)
	{
		PROFILE_FUNC();
		OME_SCALAR idx = -1;
		Listable* pList = CheckList(pState, 1);

		if (pList)
		{
			try
			{
				Listable* pIndList = CheckList(pState, 2);
				if (pIndList)
					newOMERep(pState, OMECFuncs::element(*pList, *pIndList));
				else
				{
					idx = GetDouble(pState, 2) - 1;
					if (pList->GetLevel() == 1)
						newOMERep(pState, pList->GetRepEval(), &(*pList)[idx], idx);
					else
						newOMERep(pState, pList->RefSubsetFromStep(idx));
				}
			}
			catch (...)
			{
				Evaluable* pEval = pList->GetRepEval();
				if (pEval)
					luaL_error(pState, "element: error looking up index value for element %s at index %i", pEval->GetName().c_str(), (int)idx);
				else
					luaL_error(pState, "element: error looking up index value for unnamed list at index %i", (int)idx);
			}
		}
		else
			luaL_error(pState, "element: first argument must be list");

		return 1;
	}
};

//================= firsttrue([B]) =====================
class OMELuaFirstTrue : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "firsttrue"; }
	virtual lua_CFunction GetFunc() const { return luaFirstTrue; };
	virtual const OMEChar* GetDescription() const { return "firsttrue([B]) - return the index of the first true element in array B"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }
	virtual bool TakesBoolTable() const { return true; }

	OME_LUALISTAGGFUNC(luaFirstTrue, firsttrueSclr, { if (!(*pDest)[dInd] && (*pSrc)[sInd] != 0.0) (*pDest)[dInd] = rInd + 1; }, 0.0)
};

//================= greatest([X]) ======================
class OMELuaGreatest : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "greatest"; }
	virtual lua_CFunction GetFunc() const { return luaGreatest; };
	virtual const OMEChar* GetDescription() const { return "greatest([X]) - return the element with the largest value in array X"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUALISTAGGFUNC(luaGreatest, greatestSclr, { if ((*pDest)[dInd] < (*pSrc)[sInd])(*pDest)[dInd] = (*pSrc)[sInd]; }, OME_SCALAR_MIN)
};

//================= howmanytrue([B]) ===================
class OMELuaHowManyTrue : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "howmanytrue"; }
	virtual lua_CFunction GetFunc() const { return luaHowManyTrue; };
	virtual const OMEChar* GetDescription() const { return "howmanytrue([X]) - return the number of elements in [X] that evaluate to true"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }
	virtual bool TakesBoolTable() const { return true; }

	OME_LUASIMPLELISTFUNC1(luaHowManyTrue, howmanytrue, lua_pushnumber)
};

//================= least([X]) =========================
class OMELuaLeast : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "least"; }
	virtual lua_CFunction GetFunc() const { return luaLeast; };
	virtual const OMEChar* GetDescription() const { return "least([X]) - return the element with the smallest value in array X"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUALISTAGGFUNC(luaLeast, leastSclr, { if ((*pDest)[dInd] > (*pSrc)[sInd]) (*pDest)[dInd] = (*pSrc)[sInd]; }, OME_SCALAR_MAX)
};

//================= makearray(X,N) =====================
class OMELuaMakeArray : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "makearray"; }
	virtual lua_CFunction GetFunc() const { return luaMakeArray; };
	virtual const OMEChar* GetDescription() const { return "makearray(X,N) - Initializes varArray as with args"; }
	virtual unsigned int GetMinNumArgs() const { return 2; }
	virtual bool SelfReferencing() const{ return true; }
	virtual unsigned int TakesFuncArg() const { return 1; }

	static int luaMakeArray(lua_State *pState)
	{
		BaseManager* bm = LuaDefines::GetManager(pState);
		if (bm->GetExecMode() == "Init")
		{
			PROFILE_FUNC();
			size_t offset=0;
			Evaluable* pEv = GetCaller(pState);
			if (!pEv->IsOMEType(OME_VARARRAY))
				luaL_error(pState, "makeArray:Object '%s' is not a VarArray", pEv->GetName().c_str());

			VarArray* pVa = (VarArray*)pEv;
			lua_getfield(pState, LUA_REGISTRYINDEX, LuaDefines::MAKE_DEPTH_ID);
			vector<int>* pMRDepth = (vector<int>*)lua_touserdata(pState, -1);
			pMRDepth->push_back(0);
			lua_getfield(pState, LUA_REGISTRYINDEX, LuaDefines::MAKE_OFFSET);
			if (!lua_isnil(pState, -1))
				offset = lua_tointeger(pState, -1);

			size_t count = (size_t)GetDouble(pState, 2);
			if (pVa->GetDimCount()<pMRDepth->size())
				pVa->AddDimFront(count);
			pVa->Allocate();
			for (size_t i = 0; i < count; i++)
			{
				pMRDepth->back() = i;
				if (lua_isfunction(pState, 1))
				{
					lua_pushinteger(pState, (offset * count) + i);
					lua_setfield(pState, LUA_REGISTRYINDEX, LuaDefines::MAKE_OFFSET);
					lua_pushvalue(pState, 1);
					if (lua_pcall(pState, 0, 1, 0) == 0)
					{
						if (!lua_isnil(pState, -1))
							pVa->SetValue((offset*count)+i, GetDouble(pState, -1));
						//else values assigned in subdimension
					}
					else
						luaL_error(pState, "makeArray: Problem calling first argument on object '%s", pEv->GetName().c_str());
				}
			}
			pMRDepth->pop_back();
			//reset offset
			lua_pushinteger(pState, offset);
			lua_setfield(pState, LUA_REGISTRYINDEX, LuaDefines::MAKE_OFFSET);

		}
		
		return 0;
	}
};

//================= place_in(X) ========================
class OMELuaPlaceIn : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "place_in"; }
	virtual lua_CFunction GetFunc() const { return luaPlaceIn; };
	virtual const OMEChar* GetDescription() const { return "place_in(X) - Find place in makearray contruct at depth X"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaPlaceIn(lua_State *pState)
	{
		PROFILE_FUNC();
		int ret=0;
		int depth=lua_tointeger(pState,-1);
		lua_pop(pState,1);
		lua_getfield(pState,LUA_REGISTRYINDEX,LuaDefines::MAKE_DEPTH_ID);
		vector<int>* pMRDepth=(vector<int>*)lua_touserdata(pState,-1);
		lua_pop(pState,1);

		if(pMRDepth->size() && pMRDepth->size()>=depth)
			ret=pMRDepth->at(pMRDepth->size()-depth);

		lua_pushnumber(pState,ret);
		return 1;
	}
};


//================= posgreatest([X]) ===================
class OMELuaPosGreatest : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "posgreatest"; }
	virtual lua_CFunction GetFunc() const { return luaPosGreatest; };
	virtual const OMEChar* GetDescription() const { return "posgreatest([X]) - Return the position of the greatest value in array X"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaPosGreatest(lua_State *pState)
	{
		PROFILE_FUNC();
		
		OMECFuncs::BaseWrap* pRep = checkOMEWrap(pState, 1);
		if (pRep)
			lua_pushinteger(pState, OMECFuncs::posgreatest((Listable&)(*pRep)));
		else
			luaL_error(pState,"posgreatest: argument is not a list.");
		return 1;
	}
};

//================= posleast([X]) ======================
class OMELuaPosLeast : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "posleast"; }
	virtual lua_CFunction GetFunc() const { return luaPosLeast; };
	virtual const OMEChar* GetDescription() const { return "posleast([X]) - Return the position of the Smallest value in array X"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaPosLeast(lua_State *pState)
	{
		PROFILE_FUNC();

		OMECFuncs::BaseWrap* pRep = checkOMEWrap(pState, 1);
		if (pRep)
			lua_pushinteger(pState, OMECFuncs::posleast((Listable&)(*pRep)));
		else
			luaL_error(pState, "posleast: argument is not a list.");
		return 1;
	}
};

//================= product([X]) =======================
class OMELuaProduct : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "product"; }
	virtual lua_CFunction GetFunc() const { return luaProduct; };
	virtual const OMEChar* GetDescription() const { return "product([X]) - Return the product of all values in array X"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUALISTAGGFUNC(luaProduct, productSclr, { (*pDest)[dInd] *= (*pSrc)[sInd]; }, 1.0)
};

//================= ranking([X]) =======================
class OMELuaRankings : public OMELuaFunc
{
public:
	typedef std::pair<int,OME_SCALAR> SortPair;

	virtual const OMEChar* GetName() const { return "rankings"; }
	virtual lua_CFunction GetFunc() const { return luaRankings; };
	virtual const OMEChar* GetDescription() const { return "rankings([X]) - Return an array with corresponding ranks for each value"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASIMPLELISTFUNC1(luaRankings,rankings,newOMERep)
	
};

//================= size(S,I) ============================
class OMELuaSize : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "size"; }
	virtual lua_CFunction GetFunc() const { return luaSize; };
	virtual const OMEChar* GetDescription() const { return "size(S,I) - Return number of instances of submodel S (I is optional depth scoping)"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }
	virtual unsigned int GetMaxNumArgs() { return 2; }

	static int luaSize(lua_State *pState)
	{
		PROFILE_FUNC();

		Model** hMdl = checkModelPtr(pState, 1);

		if (hMdl)
		{
			if (lua_gettop(pState) == 1)
				lua_pushnumber(pState, OMECFuncs::size(*hMdl));
			else
				lua_pushnumber(pState, OMECFuncs::size(*hMdl, (unsigned int)GetDouble(pState, 2)));
		}
		else
			luaL_error(pState, "size: first argument must be model.");

		return 1;
	}
};

//================= subtotals([X]) =====================
class OMELuaSubtotals : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "subtotals"; }
	virtual lua_CFunction GetFunc() const { return luaSubtotals; };
	virtual const OMEChar* GetDescription() const { return "subtotals([X]) - Return an array listing a running total for each value"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	OME_LUASIMPLELISTFUNC1(luaSubtotals, subtotals, newOMERep);
};

//================= sum([X]) ===========================
class OMELuaSum : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "sum"; }
	virtual lua_CFunction GetFunc() const { return luaSum; };
	virtual const OMEChar* GetDescription() const { return "sum([X]) - Return the sum of values in array X"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	//OME_LUALISTAGGFUNC(luaSum, sumSclr, { (*pDest)[dInd] += (*pSrc)[sInd]; }, 0.0)
	static int luaSum(lua_State *pState)									
	{																		
	PROFILE_FUNC();															
	\
	Listable* pList = CheckList(pState, 1);									
	if (pList)																
	{																		
	if (pList->GetLevel() == 1)												
		lua_pushnumber(pState, OMECFuncs::sumSclr(*pList));				
	else																	
	{																		
	Listable* tmp = pList->SubAggregate([](Listable* pDest, Listable* pSrc, 
	const size_t & dInd, const size_t & sInd, const size_t & rInd)			
	{ (*pDest)[dInd] += (*pSrc)[sInd]; }, 0.0);
	newOMERep(pState, *tmp);												
	}																		
	}																		
	else																	
	luaL_error(pState,": argument must be list");					
	return 1;																			 
	}


};

//================= with_colin({X},{Y}) ================
class OMELuaWithColin : public OMELuaFunc
{
	virtual const OMEChar* GetName() const { return "with_colin"; }
	virtual lua_CFunction GetFunc() const { return luaWithColin; };
	virtual const OMEChar* GetDescription() const { return "with_colin({X},{Y}) - return value from {Y} chosen by probability of entry in {X}"; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	OME_LUASIMPLELISTFUNC2(luaWithColin, with_colin, lua_pushnumber)
};

//================= with_greatest([X],[Y]) =============
class OMELuaWithGreatest : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "with_greatest"; }
	virtual lua_CFunction GetFunc() const { return luaWithGreatest; };
	virtual const OMEChar* GetDescription() const { return "with_greatest([X],[Y]) - Return the value in [Y] that corresponds with the greatest value in [X]"; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	OME_LUASIMPLELISTFUNC2(luaWithGreatest, with_greatest, lua_pushnumber)
};

//================= with_least([X],[Y]) ================
class OMELuaWithLeast : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "with_least"; }
	virtual lua_CFunction GetFunc() const { return luaWithLeast; };
	virtual const OMEChar* GetDescription() const { return "with_least([X],[Y]) - Return the value in [Y] that corresponds with the smallest value in [X]"; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	OME_LUASIMPLELISTFUNC2(luaWithLeast, with_least, lua_pushnumber)
};

//====================================================================\\
//																	  \\
//                         Undocumented Funcs                         \\
//																	  \\
//====================================================================\\

/// \todo modify undocumented funcs to work with collections

//================= default(X) =========================
class OMELuaDefault : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "default"; }
	virtual lua_CFunction GetFunc() const { return luaDefault; };
	virtual const OMEChar* GetDescription() const { return "default(X) - Return the default value for variable of type X"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaDefault(lua_State *pState)
	{
		PROFILE_FUNC();
		EnumEntry* pEe = CheckWrapEnumEntry(pState, 1);
		if (pEe)
		{
			if (pEe->pCollection->GetEnumValueCount())
				newOMERep(pState, &pEe->pCollection->GetEnumEntry(0));
			else
				lua_pushnumber(pState, 0.0);
		}
		else
			lua_pushnumber(pState, 0.0);
		return 1;
	}
};

//================= for_members_of_type(X,[Y]) =========
class OMELuaForMembers : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "for_members_of_type"; }
	virtual lua_CFunction GetFunc() const { return luaForMembers; }
	virtual const OMEChar* GetDescription() const { return "for_members_of_type() - Return a list of values from [Y] base on enumerated values of type X."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaForMembers(lua_State *pState)
	{
		PROFILE_FUNC();
		luaL_error(pState, "for_members_of_type: Disabled");
		return 0;

	}
};

//================= interpolate(X,[Y],[Z]) =============
class OMELuaInterpolate : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "interpolate"; }
	virtual lua_CFunction GetFunc() const { return luaInterpolate; }
	virtual const OMEChar* GetDescription() const { return "interpolate(X,[Y],[Z]) - returns the equivalent value in [Z] as X would be found in [Y]."; }
	virtual unsigned int GetMinNumArgs() const { return 3; }

	static int luaInterpolate(lua_State *pState)
	{
		PROFILE_FUNC();
		
		OME_SCALAR x = GetDouble(pState, 1);
		OMECFuncs::BaseWrap* pRepY = checkOMEWrap(pState, 2);
		OMECFuncs::BaseWrap* pRepZ = checkOMEWrap(pState, 3);

		if (pRepY && pRepZ)
			lua_pushnumber(pState, OMECFuncs::interpolate(x, *pRepY, *pRepZ));
		else
			luaL_error(pState, "interpolate: one or both of the last arguments are not lists.");

		return 1;
	}
};

//================= iterations( x ) ====================
class OMELuaIterations : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "iterations"; }
	virtual lua_CFunction GetFunc() const { return luaIterations; };
	virtual const OMEChar* GetDescription() const { return "iterations(x) - ."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaIterations(lua_State *pState)
	{
		PROFILE_FUNC();

		IterConditional* pIc = (IterConditional*)CheckWrapEval(pState, OME_ITERCOND, 1);
		if (pIc)
			lua_pushnumber(pState, OMECFuncs::iterations(pIc, GetManager(pState)));
		else
			luaL_error(pState,"iterations: argument is not an Iterconditional.");

		return 1;
	}
};


//================= ordinals(X) ========================
class OMELuaOrdinals : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "ordinals"; }
	virtual lua_CFunction GetFunc() const { return luaOrdinals; }
	virtual const OMEChar* GetDescription() const { return "ordinals(X) - returns the ordered values of Enumerated Type X."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaOrdinals(lua_State *pState)
	{
		PROFILE_FUNC();
		
		ModelEnum* pMe = CheckWrapModelEnumColl(pState, 1);
		if (pMe)
		{
			OMECFuncs::ECollWrap eWrap(pMe);
			newOMERep(pState, OMECFuncs::ordinals(eWrap));
		}
		else
			luaL_error(pState, "ordinals: argument is not a Enumarated value collection.");

		return 1;
	}
};

//================= pi() ===============================
class OMELuaPi : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "pi"; }
	virtual lua_CFunction GetFunc() const { return luaPi; };
	virtual const OMEChar* GetDescription() const { return "pi() - returns value of pi"; }
	virtual unsigned int GetMinNumArgs() const { return 0; }

	static int luaPi(lua_State *pState)
	{
		static const OME_SCALAR PI  =3.141592653589793238462;

		lua_pushnumber(pState,PI);

		return 1;
	}
};

//================= product3([[X]],[[Y]]) ==============
class OMELuaProd3 : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "product3"; } 
	virtual lua_CFunction GetFunc() const { return luaProd3; }
	virtual const OMEChar* GetDescription() const { return "product3([[X]], [[Y]]) - multiplies 3x3 matrix [[X]] with 3x3 matrix [[Y]]"; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaProd3(lua_State *pState)
	{
		PROFILE_FUNC();
		
		luaL_error(pState, "product3: disabled.");
		return 1;
	}
};

//================= transform3([[X]],[Y]) ============
class OMELuaTrans3 : public OMELuaFunc
{
	virtual const OMEChar* GetName() const { return "transform3"; } 
	virtual lua_CFunction GetFunc() const { return luaTrans3; }
	virtual const OMEChar* GetDescription() const { return "transform3([[X]], [Y]) - multiplies 3x3 matrix [[X]] with 3 vector [Y]"; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaTrans3(lua_State *pState)
	{
		PROFILE_FUNC();

		luaL_error(pState, "transform3: disabled.");
		return 1;
	}
};

//====================================================================\\
//																	  \\
//                         OME unique funcs                           \\
//																	  \\
//====================================================================\\

//================= valueFromInstance(x,y) ===========
class OMELuaValueFromInstance : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "valueFromInstance"; }
	virtual lua_CFunction GetFunc() const { return luaValueFromInstance; }
	virtual const OMEChar* GetDescription() const { return "valueFromInstance(x,y) - Return value of field x from submodel instance y."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }
	virtual bool SelfReferencing() const{ return true; }

	static int luaValueFromInstance(lua_State *pState)
	{
		PROFILE_FUNC();

		if (lua_gettop(pState)<2)
			luaL_error(pState, "valueFromInstance: too few arguments.");
		OMECFuncs::BaseWrap* pRep1 = checkOMEWrap(pState, 1);
		OME_SCALAR inst = GetDouble(pState, 2);


		lua_pushnumber(pState, OMECFuncs::valueFromInstance(*((Evaluable*)(*pRep1)),inst));


		return 1;

	}
};


//================= valuesFromInstances(x,[y]) =======
class OMELuaValuesFromSubmodels : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "valuesFromInstances"; }
	virtual lua_CFunction GetFunc() const { return luaValuesFromSubs; }
	virtual const OMEChar* GetDescription() const { return "valuesFromInstances(x,[y]) - Return a list of values of field x from list of submodels [y]."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }
	virtual bool SelfReferencing() const{return true;}

	static int luaValuesFromSubs(lua_State *pState)
	{
		PROFILE_FUNC();

		OMECFuncs::BaseWrap* pRep1 = checkOMEWrap(pState, 1);
		OMECFuncs::BaseWrap* pRep2 = checkOMEWrap(pState, 2);

		if (pRep1 && pRep2)
			newOMERep(pState, OMECFuncs::valuesFromInstances(*((Evaluable*)(*pRep1)), (Listable&)(*pRep2)));
		else
			luaL_error(pState, "valuesFromInstances: arguments are incorrect.");

		return 1;

	}
};

//================= setEvalVal(x,y) ==================
class OMELuaSetEvalVal : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "setEvalVal"; }
	virtual lua_CFunction GetFunc() const { return luaSetEvalVal; }
	virtual const OMEChar* GetDescription() const { return "setEvalVal(x,y) - Set the value of Evaluable x to y"; }
	virtual unsigned int GetMinNumArgs() const { return 2; }
	virtual unsigned int GetMaxNumArgs() const { return UINT_MAX; }	//veriadic

	static int luaSetEvalVal(lua_State *pState)
	{
		PROFILE_FUNC();

		OMECFuncs::BaseWrap* pRep1 = checkOMEWrap(pState, 1);
		OMECFuncs::BaseWrap* pRep2 = checkOMEWrap(pState, 2);

		if (pRep1)
		{
			try
			{
				if (pRep2)
					OMECFuncs::setvalue(*((Evaluable*)(*pRep1)), (OME_SCALAR)(*pRep2));
				else
					OMECFuncs::setvalue(*((Evaluable*)(*pRep1)), lua_tonumber(pState, 2));
			}
			catch (...)
			{
				luaL_error(pState, "setEvalVal: Bad value dereference.");
			}
		}
		else
			luaL_error(pState, "setEvalVal: arguments are incorrect.");

		return 0;

	}
};

//================= setVarArrayVal(x,y) ==============
class OMELuaSetVarArrayVal : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "setVarArrayVal"; }
	virtual lua_CFunction GetFunc() const { return luaSetVarArrayVal; }
	virtual const OMEChar* GetDescription() const { return "setVarArrayVal(x,y...) - Set varArray value to following values"; }
	virtual unsigned int GetMinNumArgs() const { return 2; }
	virtual unsigned int GetMaxNumArgs() const { return UINT_MAX; }	//veriadic

	static int luaSetVarArrayVal(lua_State *pState)
	{
		PROFILE_FUNC();

		OMECFuncs::BaseWrap* pRep1 = checkOMEWrap(pState, 1);
		OMECFuncs::BaseWrap* pRep2 = checkOMEWrap(pState, 2);

		if (pRep1)
		{
			try
			{
				VarArray* pVa = dynamic_cast<VarArray*>((Evaluable*)(*pRep1));
				if (!pVa)
					luaL_error(pState, "setVarArrayVal: first argument must be a VarArray.");

				if (lua_gettop(pState) == 2)
				{

					if (pRep2)
						OMECFuncs::setvararray(*pVa, *pRep2);
					else
						OMECFuncs::setvararray(*pVa, lua_tonumber(pState, 2));
				}
				else
				{
					size_t count = lua_gettop(pState) - 1;
					for (size_t i = 0; i + 2 <= count && i < pVa->GetSize(); ++i)
						pVa->SetValue(i, GetDouble(pState, i + 2));
				}
			}
			catch (...)
			{
				luaL_error(pState, "setVarArray: VarArray access or allocation error.");
			}
		}
		else
			luaL_error(pState, "setVarArrayVal: arguments are incorrect.");


		return 0;

	}
};

//================= allocVarArray(x) =================
class OMEAllocVarArray : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "allocVarArray"; }
	virtual lua_CFunction GetFunc() const { return luaAllocVarArray; }
	virtual const OMEChar* GetDescription() const { return "allocVarArray(x) - allocate space for VarArray x"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaAllocVarArray(lua_State *pState)
	{
		int ret = 0;

		VarArray* pVa = (VarArray*)CheckWrapEval(pState, OME_VARARRAY, 1); 
		if (!pVa)
			luaL_error(pState, "allocVarArray: argument must be a VarArray.");

		pVa->Allocate();

		return 0;

	}
};

//================= initEval(x) ======================
class OMELuaInitEval : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "initEval"; }
	virtual lua_CFunction GetFunc() const { return luaInitEval; }
	virtual const OMEChar* GetDescription() const { return "initEval(x) - Initialize the value of Evaluable x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaInitEval(lua_State *pState)
	{
		PROFILE_FUNC();
		int ret = 0;

		Evaluable* pEval = (Evaluable*)(*checkOMEWrap(pState, 1));

		if (pEval)
		{
			BaseManager* bm = LuaDefines::GetManager(pState);
			if (bm)
				pEval->BatchInitialize(bm->GetStartTime(), bm);
		}
		return ret;

	}
};

//================= isInitialized(x) =================
class OMELuaIsInitialized : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "isInitialized"; }
	virtual lua_CFunction GetFunc() const { return luaIsInitialized; }
	virtual const OMEChar* GetDescription() const { return "isInitialized(x) - Queries Evaluable x to determine if its present instance has been initialized."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaIsInitialized(lua_State *pState)
	{
		PROFILE_FUNC();
		
		bool isInitialized = false;
		Evaluable* pEval = (Evaluable*)(*checkOMEWrap(pState, 1));
		 
		if (pEval)
		{
			isInitialized = true;
		}
		lua_pushboolean(pState, isInitialized);
		return 1;

	}
};

//================= resetEval(x) =====================
class OMELuaResetEval : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "resetEval"; }
	virtual lua_CFunction GetFunc() const { return luaResetEval; }
	virtual const OMEChar* GetDescription() const { return "resetEval(x) - Reset Evaluable x"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaResetEval(lua_State *pState)
	{
		int ret=0;
		
		Evaluable* pEval = (Evaluable*)(*checkOMEWrap(pState, 1));

		if(pEval)
		{
			BaseManager* bm = LuaDefines::GetManager(pState);
			
			pEval->Reset(bm);
			
		}
	
		return ret;

	}
};


//================= updateTimeSeries(x) ==============
class OMELuaUpdateTimeSeries : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "updateTimeSeries"; }
	virtual lua_CFunction GetFunc() const { return luaUpdateTS; }
	virtual const OMEChar* GetDescription() const { return "updateTimeSeries(x) - Update TimeSeriesVar x for current time"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaUpdateTS(lua_State *pState)
	{
		PROFILE_FUNC();
		int ret=0;

		Evaluable* pEval=(Evaluable*)(*checkOMEWrap(pState,1));
		
		if(pEval)
		{
			if(pEval->GetOMEType()==OME_TSVAR)
			{
				lua_pop(pState, 1);
				BaseManager* bm = LuaDefines::GetManager(pState);
				pEval->Evaluate(bm->GetPresentTime(),bm);
			}
			else
				luaL_error(pState,"updateTimeSeries: object %s is not a TimeSeriesVar",pEval->GetName().c_str());
		}
		else
			luaL_error(pState,"updateTimeSeries: nil object passed in");
		return ret;

	}
};

//================= updatePort(x) ====================
class OMELuaUpdatePort : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "updatePort"; }
	virtual lua_CFunction GetFunc() const { return luaUpdatePort; }
	virtual const OMEChar* GetDescription() const { return "updatePort(x) - Update port x to reflect dependencies"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaUpdatePort(lua_State *pState)
	{
		PROFILE_FUNC();
		int ret=0;

		SubmodelPort* pPort = (SubmodelPort*)CheckWrapEval(pState, OME_SUBPORT, 1);
		
		if(pPort)
		{
			BaseManager* bm = LuaDefines::GetManager(pState);
			pPort->Evaluate(bm->GetPresentTime(),bm);
		}
		return ret;

	}
};

//================= updateSpawner(x) =================
class OMELuaUpdateSpawner : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "updateSpawner"; }
	virtual lua_CFunction GetFunc() const { return luaUpdateSpawner; }
	virtual const OMEChar* GetDescription() const { return "updateSpawner(x,y) - Takes action with Spawner x with value y"; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaUpdateSpawner(lua_State *pState)
	{
		PROFILE_FUNC();

		if (lua_gettop(pState) >= 2)
		{
			Spawner* pSpawn = dynamic_cast<Spawner*>(CheckWrapEval(pState, OME_SPAWNER, 1));
			
			OME_SCALAR argVal = GetDouble(pState, 2);

			if (pSpawn)
				pSpawn->EvalWithVal(argVal);
			else
				luaL_error(pState, "First argument must be a spawner.");
		}
		else
			luaL_error(pState, "updateSpawner: not enough arguments");
		return 0;

	}
};

//================= killMarkedInstances(x) ====================
class OMELuaKillMarkedInstances : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "killMarkedInstances"; }
	virtual lua_CFunction GetFunc() const { return luaKillMarked; }
	virtual const OMEChar* GetDescription() const { return "killMarkedInstances(x) - Kill or deactivate any instances marked in x"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaKillMarked(lua_State *pState)
	{
		PROFILE_FUNC();
		int ret = 0;

		if (lua_type(pState, -1) == LUA_TTABLE)
			lua_getfield(pState, -1, TABLE_MC_SRC);
		Model** hMdl = checkModelPtr(pState, -1);

		if (hMdl)
			(*hMdl)->KillMarked();
		
		return ret;

	}
};

//================= getAsTable(x...) =================
class OMELuaGetAsTable : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "getAsTable"; }
	virtual lua_CFunction GetFunc() const { return luaToTable; }
	virtual const OMEChar* GetDescription() const { return "getAsTable(x) - Get values from object x as a table"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }
	virtual unsigned int GetMaxNumArgs() const { return UINT_MAX; } //variadic

	static int luaToTable(lua_State *pState)
	{
		PROFILE_FUNC();
		size_t count = lua_gettop(pState);
		if (count == 1 && lua_istable(pState, 1))
		{
			return 1;
		}
		Listable* result = OMECFuncs::NewTempList(count);
		ListDims addDims;

		Listable* pInList;
		size_t i = 0;

		for (size_t k = 1; k <= lua_gettop(pState); ++k, ++i)
		{
			pInList =CheckList(pState,i+1);
			if (pInList)
			{
				if (k == 1 && !addDims.GetSize())
				{
					addDims = pInList->GetDims();
					ListDims newDims(addDims);
					newDims.AppendDimFront(count);
					count = newDims.CalcRepLength();
					OMECFuncs::DeletePoolPtr(result);
					result = OMECFuncs::NewTempList(newDims, newDims.GetSize());

				}
				if (pInList->GetDims() == addDims)
				{
					for (size_t j = 0; j < pInList->GetSize(); ++j)
						result->SetValue(i++, pInList->GetValue(j));
					--i;
				}
				else
					luaL_error(pState, "BuildList: sub lists do not have matching dimensions.");
			}
			else
				result->SetValue(i, GetDouble(pState, k));
		}

		newOMERep(pState, *result);
		return 1;

	}
	
};

//================= getMCInstanceCount(x) ============
class OMELuaGetMCInstanceCount : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "getMCInstanceCount"; }
	virtual lua_CFunction GetFunc() const { return luaGetInstCount; }
	virtual const OMEChar* GetDescription() const { return "getMCInstanceCount(x) - Get the number of live instances in Model x"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaGetInstCount(lua_State *pState)
	{
		PROFILE_FUNC();
		Model** hM=checkModelPtr(pState,1);
		
		int count=0;
		if(hM)
			count=(*hM)->GetNumInstances();
		lua_pushinteger(pState,count);
		
		return 1;

	}
};

//================= isModelDead(x) ===================
class OMELuaIsModelDead : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "isModelDead"; }
	virtual lua_CFunction GetFunc() const { return luaIsModelDead; }
	virtual const OMEChar* GetDescription() const { return "isModelDead(x) - Test to see if current instance of model x is \"Dead\""; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaIsModelDead(lua_State *pState)
	{
		PROFILE_FUNC();
		int ret = 1;

		if (lua_type(pState, -1) == LUA_TTABLE)
			lua_getfield(pState, -1, TABLE_MC_SRC);

		Model** hM = checkModelPtr(pState, -1);

		bool isDead = true;
		if (hM)
			isDead = !(*hM)->GetCurrentInstanceDead();
		lua_pushboolean(pState, isDead);

		return ret;

	}
};

//================= isModelActive(x) =================
class OMELuaIsModelActive : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "isModelActive"; }
	virtual lua_CFunction GetFunc() const { return luaIsModelActive; }
	virtual const OMEChar* GetDescription() const { return "isModelActive(x) - Test to see if current instance of model x is \"Active\""; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaIsModelActive(lua_State *pState)
	{
		PROFILE_FUNC();
		int ret = 1;

		if (lua_type(pState, -1) == LUA_TTABLE)
			lua_getfield(pState, -1, TABLE_MC_SRC);

		Model** hM = checkModelPtr(pState, -1);

		bool isActive = true;
		if (hM)
			isActive = (*hM)->GetCurrentInstanceActive();
		lua_pushboolean(pState, isActive);

		return ret;

	}
};

//================= setActiveInstance(x) =============
class OMELuaSetActiveInstance : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "setActiveInstance"; }
	virtual lua_CFunction GetFunc() const { return luaSetActive; }
	virtual const OMEChar* GetDescription() const { return "setActiveInstance(x,y) - Select instance y to be the current instance of x"; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaSetActive(lua_State *pState)
	{
		PROFILE_FUNC();
		//swap arg places.
		
		Model** hM = checkModelPtr(pState, 1);

		int ind=((int)GetDouble(pState,2))-1;
		if(!hM)
			luaL_error(pState,"setActiveInstance: Argument 1 is not a model");
		if(ind<0 || ind>=(*hM)->GetNumInstances())
			luaL_error(pState,"setActiveInstance: Argument 2 is not a valid index; valid range: [1,%d]",(*hM)->GetActiveInstanceCount());
		
		(*hM)->SetCurrentInstance(ind);
		return 0;

	}
};

//================= lookupInit_const_delay( x, t ) =====
class OMELuaInitConstDelay : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "lookupInit_const_delay"; }
	virtual lua_CFunction GetFunc() const { return luaConstDelay; };
	virtual const OMEChar* GetDescription() const { return "lookupInit_const_delay(x,t) - Initialize record for const_delay."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaConstDelay(lua_State *pState)
	{
		PROFILE_FUNC();

		if (lua_gettop(pState) >= 2)
		{
			OMECFuncs::BaseWrap* pRep = checkOMEWrap(pState, 1);
			
			if (pRep)
				lua_pushnumber(pState, OMECFuncs::lookupInit_const_delay(GetManager(pState),*(Evaluable*)(*pRep), 0.0,0.0));
			else
				luaL_error(pState, "lookupInit_const_delay: arg1 is not wrapped.");
		}
		else
			luaL_error(pState, "lookupInit_const_delay: not enough arguments");
		return 1;
	}
};

//================= lookupInit_last( x ) ===============
class OMELuaInitLast : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "lookupInit_last"; }
	virtual lua_CFunction GetFunc() const { return luaLast; };
	virtual const OMEChar* GetDescription() const { return "lookupInit_last(x) - Initialize record for const_delay() call."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }
	static int luaLast(lua_State *pState)
	{
		//add time argument, call const_delay.
		PROFILE_FUNC();
		OMECFuncs::BaseWrap* pRep = checkOMEWrap(pState, 1);
		if (pRep)
			lua_pushnumber(pState, OMECFuncs::lookupInit_last(GetManager(pState), *(Evaluable*)(*pRep)));
		else
			luaL_error(pState, "lookupInit_last: argument is not a model component");
		return 1;
	}
};

//================= lookupInit_prev( x ) ===============
class OMELuaInitPrev : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "lookupInit_prev"; }
	virtual lua_CFunction GetFunc() const { return luaPrev; };
	virtual const OMEChar* GetDescription() const { return "lookupInit_prev(x) - Initialize record for prev() call."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }
	virtual bool SelfReferencing() const{ return true; }

	static int luaPrev(lua_State *pState)
	{
		PROFILE_FUNC();
		Evaluable* pCaller = GetCaller(pState);

		OME_SCALAR x = GetDouble(pState, 1);
		lua_pushnumber(pState, OMECFuncs::lookupInit_prev(GetManager(pState),*pCaller, x));
		
		return 1;
	}
};

//================= var_delay( x, t ) ==================
class OMELuaInitVarDelay : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "lookupInit_var_delay"; }
	virtual lua_CFunction GetFunc() const { return luaVarDelay; };
	virtual const OMEChar* GetDescription() const { return "lookupInit_var_delay(x,y) - Initialize record for var_delay() call."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaVarDelay(lua_State *pState)
	{
		PROFILE_FUNC();
		OMECFuncs::BaseWrap* pRep = checkOMEWrap(pState, 1);
		if (pRep)
			lua_pushnumber(pState, OMECFuncs::lookupInit_var_delay(GetManager(pState), *(Evaluable*)(*pRep), 0.0));
		else
			luaL_error(pState, "lookupInit_var_delay: not called on model component.");

		return 1;
	}
};


//================= omeLineInit() =======================
class OMELuaLineInit : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "omeLineInit"; }
	virtual lua_CFunction GetFunc() const { return luaOMEBreak; }
	virtual const OMEChar* GetDescription() const { return "omeLineInit(x) - break if line x is in break list; also resets EvalCollection Cache."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaOMEBreak(lua_State *pState)
	{
		PROFILE_FUNC();
		static lua_State *pLastState=NULL;
		static BreakSet *pBreakCache=NULL;
		static unsigned int skipStep=0;


		///@todo update caches resets below for new scheme:

		//reset val cache index
		//EvalCollection::ResetCacheIndex();
		//reset parser Index
		OMECFuncs::ResetLine();
		lua_getfield(pState,LUA_REGISTRYINDEX,LuaDefines::PARSER_ID);
		LuaOMEParser* pPrsr = (LuaOMEParser*)lua_touserdata(pState, -1);
		//pPrsr->ResetCacheOffset();
		//GetLuaFuncValCache().ResetCache();


		//maintain local link to breakpoint markers so they don't have to be retrieved every time from Lua
		//breakpoint line check should be O(1) if using vector.

		if(pState!=pLastState)
		{
			skipStep=0;
			pLastState=pState;
			lua_getfield(pState,LUA_REGISTRYINDEX,LuaDefines::BREAKS_ID);
			pBreakCache=(BreakSet*)lua_touserdata(pState,-1);
			lua_pop(pState,1);
		}

		int ret=0;

		// Begin debug-specific
		int line=lua_tointeger(pState,1);
		
		bool stepHit=skipStep-1==0;
		if(skipStep)
			skipStep--;
		if((!pBreakCache->empty() && (*pBreakCache)[line-1]) || stepHit)
		{
			lua_getfield(pState,LUA_REGISTRYINDEX,LuaDefines::PARSER_ID);
			LuaOMEParser* pPrsr=(LuaOMEParser*)lua_touserdata(pState,-1);

			OMEDBG_COLOR lastColor;
			DBG_GETTXCOLOR(lastColor);
			DBG_SETTXCOLOR(DBG_CYAN);
			cout<<endl<<"-- ";
			DBG_COLOR(cout<< pPrsr->GetModeID()<<' '<<line<<": ",DBG_WHITE);
			DBG_COLOR(cout<<"Break point triggered",DBG_YELLOW);
			cout<<" --"<<endl;

			int flag=pPrsr->GoPrompt(skipStep,line);
			if (flag)
			{
				pPrsr->SetDebugState(false);
				luaL_error(pState, "User triggered Abort with flag %d", flag);

			}
			DBG_SETTXCOLOR(lastColor);

			lua_settop(pState,0);
		}
		//end debug-specific
		return ret;

	}
};

//================= upgroup(x,y) =====================
class OMELuaUpGroup : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "upgroup"; }
	virtual lua_CFunction GetFunc() const { return luaOMEUpGroup; }
	virtual const OMEChar* GetDescription() const { return "upgroup(x,y) - return the collection of x at model level y."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaOMEUpGroup(lua_State *pState)
	{
		PROFILE_FUNC();
		
		OMECFuncs::BaseWrap* pRep = checkOMEWrap(pState, 1);
		if (pRep)
		{
			Evaluable* pEval = (Evaluable*)(*pRep);
			if (pEval)
			{
				try
				{
					newOMERep(pState, OMECFuncs::upgroup(*pEval, GetDouble(pState, 2)));
					return 1;
				}
				catch (std::out_of_range* err)
				{
					luaL_error(pState, err->what());
				}
			}
			luaL_error(pState, "upgroup: argument is not a model component");
		}
		luaL_error(pState, "upgroup: argument is not a model component");
		return 1;
	}
};

//================= omeStaticList(...) ===============
class OMELuaStaticList : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "omeStaticList"; }
	virtual lua_CFunction GetFunc() const { return luaOMEStaticList; }
	virtual const OMEChar* GetDescription() const { return "omeStaticList(...) - return a statically stored using the arguments or table passed in"; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaOMEStaticList(lua_State *pState)
	{
		PROFILE_FUNC();

		//get parser, swap memory mode to non-pool
		lua_getfield(pState, LUA_REGISTRYINDEX, LuaDefines::PARSER_ID);
		LuaOMEParser* pPrsr = (LuaOMEParser*)lua_touserdata(pState, -1);

		lua_pop(pState, 1);
		//DBG_PRINTC(DebugDump(pState), DBG_YELLOW);
		pPrsr->DeactivatePool();

		size_t valCount;

		SimpleArrayWrapper* pArrWrap=NULL;
		OME_SCALAR* vals=NULL;

		if (lua_gettop(pState) == 1 && lua_istable(pState, 1))
		{
			//populate with table
			valCount=lua_objlen(pState, 1);

			pArrWrap=pPrsr->AddStaticArray(valCount);
			vals = pArrWrap->GetRawArray();
			//populate vals list from table;
			for (int i = 0; i < valCount; ++i)
			{
				lua_rawgeti(pState, 1, i + 1);
				vals[i] = GetDouble(pState, -1);
				lua_pop(pState, 1);
			}
		}
		else
		{
			//populate with all arguments
			valCount = lua_gettop(pState);
			
			pArrWrap = pPrsr->AddStaticArray(valCount);
			vals = pArrWrap->GetRawArray();

			//populate vals list;
			for (int i = 0; i < valCount; ++i)
				vals[i] = GetDouble(pState, i + 1);
			
		}

		newOMERep(pState, *pArrWrap);

		pPrsr->ActivatePool();

		return 1;
	}
};

//================= omeToBool(x) ===================
class OMELuaToBool : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "omeToBool"; }
	virtual lua_CFunction GetFunc() const { return toBool; }
	virtual const OMEChar* GetDescription() const { return "omeToBool(x) - ensure that x is a valid lua boolean"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int toBool(lua_State*L)
	{
		using namespace OMECFuncs;
		BaseWrap* pWrap = checkOMEWrap(L, 1);
		if (pWrap)
			lua_pushboolean(L, (bool)(*pWrap));
		return 1;
	}
};

//================= omeUseMemPool(x) ===============
//class OMELuaUseMemPool : public OMELuaFunc
//{
//public:
//	virtual const OMEChar* GetName() const { return "omeUseMemPool"; }
//	virtual lua_CFunction GetFunc() const { return luaUsePool; }
//	virtual const OMEChar* GetDescription() const { return "omeUseMemPool(x) - Enable or disable memory pool usage."; }
//	virtual unsigned int GetMinNumArgs() const { return 2; }
//
//	static int luaUsePool(lua_State *pState)
//	{
//		bool use = lua_toboolean(pState, 1);
//
//		LuaOMEParser* pPrsr = (LuaOMEParser*)GetManager(pState)->GetPEvalParser();
//
//		use ? pPrsr->ActivatePool() : pPrsr->DeactivatePool();
//		return 0;
//	}
//};


//================= omecleanup() =====================
class OMELuaCleanup : public OMELuaFunc
{
public:
	virtual const OMEChar* GetName() const { return "omeCleanup"; }
	virtual lua_CFunction GetFunc() const { return luaOMECleanup; }
	virtual const OMEChar* GetDescription() const { return "omeCleanup() - Cleanup some of the OMECaches; should be called at end of script."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaOMECleanup(lua_State *pState)
	{
      //LuaOMEParser* pPrsr=(LuaOMEParser*)GetManager(pState)->GetPEvalParser();
	  //pPrsr->RefitMemPool();

		//EvalCollection::OptimizeCache();
		//EvalCollection::ResetCacheIndex();

		return 0;
	}
};


//====================================================================\\
//                                                                    \\
//                         OME Registry Function                      \\
//                                                                    \\
//====================================================================\\

static ARRAY_TYPE<OMELuaFunc*> GetDefaultRegistry()
{
	ARRAY_TYPE<OMELuaFunc*> out;

	//======= Simile compatiblity (single args) ===========
	out.push_back(new OMELuaAbs());
	out.push_back(new OMELuaAcos());
	out.push_back(new OMELuaAsin());
	out.push_back(new OMELuaAtInit());
	out.push_back(new OMELuaAtan());
	out.push_back(new OMELuaAtan2());
	out.push_back(new OMELuaBinom());
	out.push_back(new OMELuaCeil());
	out.push_back(new OMELuaChannelIs());
	out.push_back(new OMELuaConstDelay());
	out.push_back(new OMELuaCos());
	out.push_back(new OMELuaCosh());
	out.push_back(new OMELuaDiesOf());
	out.push_back(new OMELuaDt());
	out.push_back(new OMELuaExp());
	out.push_back(new OMELuaFactorial());
	out.push_back(new OMELuaFirst());
	out.push_back(new OMELuaFloor());
	out.push_back(new OMELuaFmod());
	out.push_back(new OMELuaFollowing());
	out.push_back(new OMELuaGraph());
	out.push_back(new OMELuaGuassianVar());
	out.push_back(new OMELuaHyperGeom());
	out.push_back(new OMELuaHypot());
	out.push_back(new OMELuaIndex());
	out.push_back(new OMELuaInitTime());
	out.push_back(new OMELuaInPreceding());
	out.push_back(new OMELuaInProgenitor());
	out.push_back(new OMELuaInt());
	out.push_back(new OMELuaLast());
	out.push_back(new OMELuaLog());
	out.push_back(new OMELuaLog10());
	out.push_back(new OMELuaMax());
	out.push_back(new OMELuaMin());
	out.push_back(new OMELuaParent());
	out.push_back(new OMELuaPoidev());
	out.push_back(new OMELuaPower());
	out.push_back(new OMELuaPreceding());
	out.push_back(new OMELuaPrev());
	out.push_back(new OMELuaPulse());
	out.push_back(new OMELuaRamp());
	out.push_back(new OMELuaRandConst());
	out.push_back(new OMELuaRandVar());
	out.push_back(new OMELuaRound());
	out.push_back(new OMELuaSelf());
	out.push_back(new OMELuaSgn());
	out.push_back(new OMELuaSin());
	out.push_back(new OMELuaSinh());
	out.push_back(new OMELuaSqrt());
	out.push_back(new OMELuaStop());
	out.push_back(new OMELuaStep());
	out.push_back(new OMELuaTable());
	out.push_back(new OMELuaTan());
	out.push_back(new OMELuaTanh());
	out.push_back(new OMELuaTime());
	out.push_back(new OMELuaVarDelay());

	//======= Simile compatiblity (multi args) ============
	out.push_back(new OMELuaAll());
	out.push_back(new OMELuaAny());
	out.push_back(new OMELuaColin());
	out.push_back(new OMELuaCount());
	out.push_back(new OMELuaElement());
	out.push_back(new OMELuaFirstTrue());
	out.push_back(new OMELuaGreatest());
	out.push_back(new OMELuaHowManyTrue());
	out.push_back(new OMELuaLeast());
	out.push_back(new OMELuaMakeArray());
	out.push_back(new OMELuaPlaceIn());
	out.push_back(new OMELuaPosGreatest());
	out.push_back(new OMELuaPosLeast());
	out.push_back(new OMELuaProduct());
	out.push_back(new OMELuaRankings());
	out.push_back(new OMELuaSize());
	out.push_back(new OMELuaSubtotals());
	out.push_back(new OMELuaSum());
	out.push_back(new OMELuaWithColin());
	out.push_back(new OMELuaWithGreatest());
	out.push_back(new OMELuaWithLeast());

	//======= Simile compatiblity (other) =================
	out.push_back(new OMELuaDefault());
	out.push_back(new OMELuaInterpolate());
	out.push_back(new OMELuaIterations());
	out.push_back(new OMELuaForMembers());
	out.push_back(new OMELuaOrdinals());
	out.push_back(new OMELuaPi());
	out.push_back(new OMELuaProd3());
	out.push_back(new OMELuaTrans3());

	//======= OME Specific ================================
	out.push_back(new OMELuaValueFromInstance());
	out.push_back(new OMELuaValuesFromSubmodels());
	out.push_back(new OMELuaSetEvalVal());
	out.push_back(new OMELuaSetVarArrayVal());
	out.push_back(new OMEAllocVarArray());
	out.push_back(new OMELuaUpdateTimeSeries());
	out.push_back(new OMELuaUpdatePort());
	out.push_back(new OMELuaUpdateSpawner());
	out.push_back(new OMELuaKillMarkedInstances());
	out.push_back(new OMELuaGetMCInstanceCount());
	out.push_back(new OMELuaIsModelDead());
	out.push_back(new OMELuaIsModelActive());
	out.push_back(new OMELuaInitEval());
	out.push_back(new OMELuaIsInitialized());
	out.push_back(new OMELuaResetEval());
	out.push_back(new OMELuaGetAsTable());
	out.push_back(new OMELuaSetActiveInstance());
	out.push_back(new OMELuaLineInit());
	out.push_back(new OMELuaUpGroup());
	out.push_back(new OMELuaStaticList());
	out.push_back(new OMELuaCleanup());
	out.push_back(new OMELuaInitConstDelay());
	out.push_back(new OMELuaInitLast());
	out.push_back(new OMELuaInitPrev());
	out.push_back(new OMELuaInitVarDelay());
	out.push_back(new OMELuaToBool());
	return out;
}

static std::map<STLString, OME_SCALAR> GetDefaultConstants()
{
	std::map<STLString, OME_SCALAR> out;

	//register 'Pi' as a value
	out["Pi"]=3.141592653589793238462;
	return out;
}

//====================================================================\\
//                                                                    \\
//               OME Factory Funcs and Registration                   \\
//                                                                    \\
//====================================================================\\

STLString GenInitOnly(Evaluable* pCaller, const STLString & args, const bool & inInit)
{
	return inInit ? args : STLString();
}


static LuaFactoryMap GetFactories()
{
	LuaFactoryMap factories;
	factories["initOnly"] = GenInitOnly;

	return factories;
}

};