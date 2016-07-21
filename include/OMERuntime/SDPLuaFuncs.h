#pragma once
#include "OMELuaFunc.h"
#include "ISpatialDataProvider.h"
#include "OMEDefines.h"
#include "OMELuaDefines.h"
#include "LuaOMEParser.h"

class SpatialLuaFunc : public OMELuaFunc
{
public:
	SpatialLuaFunc(ISpatialDataProvider* sdp) : OMELuaFunc()
      {
         m_sdp=sdp;
      }

	//pass on pure virtuals from OMELuaFunc to subclasses; this is just an intermediate interface
protected:
	static ISpatialDataProvider* m_sdp;

private:
	SpatialLuaFunc() : OMELuaFunc() {};
};

ISpatialDataProvider* SpatialLuaFunc::m_sdp=NULL;

//====================================================================\\
//                                                                    \\
//                         Spatial Data Funcs                         \\
//                                                                    \\
//====================================================================\\

class SDPGetBooleanData : public SpatialLuaFunc
{
public:
	SDPGetBooleanData(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPGetBooleanData"; }
	virtual lua_CFunction GetFunc() const { return luaGetBoolData; };
	virtual const OMEChar* GetDescription() const { return "SDPGetBooleanData(x,y) - Return boolean value for column y in record x, or nil if it doesn't exist."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaGetBoolData(lua_State *pState)
	{
		int rec=-1,col=-1;
		if(m_sdp)
		{
			rec=LuaDefines::GetDouble(pState,1,-1);
			col=LuaDefines::GetDouble(pState,2,-1);

			lua_pop(pState,2);

			if(rec!=-1 && col!=-1 )
				lua_pushboolean(pState, OMECFuncs::SDPGetBooleanData(m_sdp, rec, col));
			else
				lua_pushnil(pState);
			
		}
		else
		{
			lua_pop(pState,2);
			lua_pushnil(pState);
		}
		return 1;
	}

private:
	SDPGetBooleanData();
};

class SDPGetCapabilities : public SpatialLuaFunc
{
public:
	SDPGetCapabilities(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPGetCapabilityFlags"; }
	virtual lua_CFunction GetFunc() const { return luaGetCapabilites; };
	virtual const OMEChar* GetDescription() const { return "SDPGetCapabilityFlags() - Return The flag markers for capabilities of SPD."; }
	virtual unsigned int GetMinNumArgs() const { return 0; }

	static int luaGetCapabilites(lua_State *pState)
	{
		int out=NONE;
		if(m_sdp)
			out=m_sdp->GetCapabilities();
		lua_pushinteger(pState,out);
		return 1;
	}

private:
	SDPGetCapabilities();
};

class SDPGetCellSize : public SpatialLuaFunc
{
public:
	SDPGetCellSize(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPGetCellSize"; }
	virtual lua_CFunction GetFunc() const { return luaGetCellDims; };
	virtual const OMEChar* GetDescription() const { return "SDPGetCellSize() - Return 2 values in order: width, height."; }
	virtual unsigned int GetMinNumArgs() const { return 0; }

	static int luaGetCellDims(lua_State *pState)
	{
		OME_SCALAR width=0.0,height=0.0;
		if(m_sdp)
			m_sdp->GetCellSize(width,height);
		lua_pushnumber(pState,width);
		lua_pushnumber(pState,height);

		return 2;
	}

private:
	SDPGetCellSize();
};

class SDPGetColumnCount : public SpatialLuaFunc
{
public:
	SDPGetColumnCount(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPGetColumnCount"; }
	virtual lua_CFunction GetFunc() const { return luaGetColumns; };
	virtual const OMEChar* GetDescription() const { return "SDPGetColumnCount() - Return number of columns in spatial data provider."; }
	virtual unsigned int GetMinNumArgs() const { return 0; }

	static int luaGetColumns(lua_State *pState)
	{
		int cols=0;
		if(m_sdp)
			cols=m_sdp->GetColumnCount();
		lua_pushinteger(pState,cols);

		return 1;
	}

private:
	SDPGetColumnCount();
};

class SDPGetExtents : public SpatialLuaFunc
{
public:
	SDPGetExtents(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPGetExtents"; }
	virtual lua_CFunction GetFunc() const { return luaGetExtents; };
	virtual const OMEChar* GetDescription() const { return "SDPGetExtents() - Return 4 values in order: xMin, xMax, yMin, yMax."; }
	virtual unsigned int GetMinNumArgs() const { return 0; }

	static int luaGetExtents(lua_State *pState)
	{
		OME_SCALAR xMin=0.0, xMax=0.0, yMin=0.0, yMax=0.0;
		if(m_sdp)
			m_sdp->GetExtents(xMin,xMax,yMin,yMax);
		lua_pushnumber(pState,xMin);
		lua_pushnumber(pState,xMax);
		lua_pushnumber(pState,yMin);
		lua_pushnumber(pState,yMax);

		return 4;
	}

private:
	SDPGetExtents();
};

class SDPGetFieldCol : public SpatialLuaFunc
{
public:
	SDPGetFieldCol(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPGetFieldCol"; }
	virtual lua_CFunction GetFunc() const { return luaGetCols; };
	virtual const OMEChar* GetDescription() const { return "SDPGetFieldCol(x) - Return index of column with label x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaGetCols(lua_State *pState)
	{
		int ind=-1;
		STLString out;
		if(m_sdp)
		{
			out=lua_tostring(pState,-1);
			lua_pop(pState,1);
			if(out.length())
				ind=OMECFuncs::SDPGetFielCol(m_sdp,out.c_str());
		}
		lua_pushinteger(pState,ind);

		return 1;
	}

private:
	SDPGetFieldCol();
};

class SDPGetDataMinMax : public SpatialLuaFunc
{
public:
	SDPGetDataMinMax(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPGetDataMinMax"; }
	virtual lua_CFunction GetFunc() const { return luaGetMinMax; };
	virtual const OMEChar* GetDescription() const { return "SDPGetDataMinMax(x) - Return min and max values for column at index x (or all if x is -1)."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaGetMinMax(lua_State *pState)
	{
		OME_SCALAR min=0.0, max=0.0;
		if(m_sdp)
		{
			int col=LuaDefines::GetDouble(pState,2,-2);
			//int outVal;
			//this should fall back to float accessor if double accessor is absent; lua doesn't really care.
			if(col!=-2)
				m_sdp->GetDataMinMax(col,min,max);
			
		}
		
		lua_pop(pState,1);
		lua_pushnumber(pState,min);
		lua_pushnumber(pState,max);

		return 2;
	}

private:
	SDPGetDataMinMax();
};

class SDPGetIntData : public SpatialLuaFunc
{
public:
	SDPGetIntData(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPGetIntData"; }
	virtual lua_CFunction GetFunc() const { return luaGetDoubleData; };
	virtual const OMEChar* GetDescription() const { return "SDPGetIntData(x,y) - Return integer value for column y in record x, or nil if it doesn't exist."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaGetDoubleData(lua_State *pState)
	{
		int rec=-1,col=-1;
		if(m_sdp)
		{
			rec=LuaDefines::GetDouble(pState,1,-1);
			col=LuaDefines::GetDouble(pState,2,-1);

			lua_pop(pState,2);

			int outVal;
			//this should fall back to float accessor if OME_SCALAR accessor is absent; lua doesn't really care.
			if(rec!=-1 && col!=-1 && m_sdp->GetData(rec,col,outVal))
				lua_pushinteger(pState,outVal);
			else
				lua_pushnil(pState);
			
		}
		else
		{
			lua_pop(pState,2);
			lua_pushnil(pState);
		}
		return 1;
	}

private:
	SDPGetIntData();
};

class SDPGetNeighborCount : public SpatialLuaFunc
{
public:
	SDPGetNeighborCount(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPGetNeighborCount"; }
	virtual lua_CFunction GetFunc() const { return luaGetNeighborCount; };
	virtual const OMEChar* GetDescription() const { return "SDPGetNeighborCount(x) - Return the number of neighbors for record x."; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaGetNeighborCount(lua_State *pState)
	{
		int ret = 0;
		if (m_sdp)
		{
			int poly = lua_tointeger(pState, 1);
			lua_pop(pState, 1);

			ret = OMECFuncs::SDPGetNeighborCount(m_sdp,poly);
		}
		lua_pushinteger(pState, ret);

		return 1;
	}

private:
	SDPGetNeighborCount();
};



class SDPGetNeighborForDirection : public SpatialLuaFunc
{
public:
	SDPGetNeighborForDirection(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPGetNeighborForDirection"; }
	virtual lua_CFunction GetFunc() const { return luaGetNeighborForDirection; };
	virtual const OMEChar* GetDescription() const { return "SDPGetNeighborForDirection(x,y) - Return index of neighbor of x in direction y."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaGetNeighborForDirection(lua_State *pState)
	{
		int ret = 0;
		if (m_sdp)
		{
			int poly = lua_tointeger(pState,1);
			
			GRID_DIR dir= (GRID_DIR) lua_tointeger(pState, 2);
			lua_pop(pState, 2);

			ret = OMECFuncs::SDPGetNeighborForDirection(m_sdp, poly, dir);
		}
		lua_pushinteger(pState, ret);

		return 1;
	}

private:
	SDPGetNeighborForDirection();
};

class SDPGetNumberData : public SpatialLuaFunc
{
public:
	SDPGetNumberData(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPGetNumberData"; }
	virtual lua_CFunction GetFunc() const { return luaGetNumberData; };
	virtual const OMEChar* GetDescription() const { return "SDPGetNumberData(x,y) - Return floating point value for column y in record x, or nil if it doesn't exist."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaGetNumberData(lua_State *pState)
	{
		OME_SCALAR rec=-1.,col=-1.;
		if(m_sdp)
		{
            rec=LuaDefines::GetDouble(pState,1,-1);
            col=LuaDefines::GetDouble(pState,2,-1);

			lua_pop(pState,2);

			OME_SCALAR outVal;
			//this should fall back to float accessor if OME_SCALAR accessor is absent; lua doesn't really care.
			if(rec!=-1. && col!=-1.)
				lua_pushnumber(pState,OMECFuncs::SDPGetNumberData(m_sdp,rec,col));
			else
				lua_pushnil(pState);
			
		}
		else
		{
			lua_pop(pState,2);
			lua_pushnil(pState);
		}
		return 1;
	}

private:
	SDPGetNumberData();
};

class SDPGetRowCount : public SpatialLuaFunc
{
public:
	SDPGetRowCount(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPGetRowCount"; }
	virtual lua_CFunction GetFunc() const { return luaGetRows; };
	virtual const OMEChar* GetDescription() const { return "SDPGetRowCount() - Return number of rows in spatial data provider."; }
	virtual unsigned int GetMinNumArgs() const { return 0; }

	static int luaGetRows(lua_State *pState)
	{
		int rows=0;
		if(m_sdp)
			rows=m_sdp->GetRowCount();
		lua_pushinteger(pState,rows);

		return 1;
	}

private:
	SDPGetRowCount();
};

class SDPGetStringData : public SpatialLuaFunc
{
public:
	SDPGetStringData(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPGetStringData"; }
	virtual lua_CFunction GetFunc() const { return luaGetStringData; };
	virtual const OMEChar* GetDescription() const { return "SDPGetStringData(x,y,z) - Return string value for column y in record x (up to z chars), or nil if it doesn't exist."; }
	virtual unsigned int GetMinNumArgs() const { return 3; }

	static int luaGetStringData(lua_State *pState)
	{
		int rec=-1,col=-1;
		if(m_sdp)
		{
			rec=LuaDefines::GetDouble(pState,1,-1);
			col=LuaDefines::GetDouble(pState,2,-1);
			int maxChars=LuaDefines::GetDouble(pState,3);

			lua_pop(pState,3);

			OMEChar* outVal= new OMEChar[maxChars]();
			
			//this should fall back to float accessor if OME_SCALAR accessor is absent; lua doesn't really care.
			if(rec!=-1 && col!=-1 && m_sdp->GetData(rec,col,outVal,maxChars))
				lua_pushstring(pState,outVal);
			else
				lua_pushnil(pState);
			//DBG_PRINTC(outVal, DBG_GREEN);
			delete[] outVal;
		}
		else
		{
			lua_pop(pState,2);
			lua_pushnil(pState);
		}
		return 1;
	}

private:
	SDPGetStringData();
};

class SDPHasNeighborForDirection : public SpatialLuaFunc
{
public:
	SDPHasNeighborForDirection(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPHasNeighborForDirection"; }
	virtual lua_CFunction GetFunc() const { return luaHasNeighborForDirection; };
	virtual const OMEChar* GetDescription() const { return "SDPHasNeighborForDirection(x,y) - Return true if x has a neighbor in the direction of y."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaHasNeighborForDirection(lua_State *pState)
	{
		bool ret = false;
		if (m_sdp)
		{
			int poly = lua_tointeger(pState, 1);

			GRID_DIR dir = (GRID_DIR)lua_tointeger(pState, 2);
			lua_pop(pState, 2);

			ret = OMECFuncs::SDPHasNeighborForDirection(m_sdp, poly, dir);
		}
		lua_pushboolean(pState, ret);

		return 1;
	}

private:
	SDPHasNeighborForDirection();
};

class SDPListCapabilities : public SpatialLuaFunc
{
public:
	SDPListCapabilities(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPListCapabilities"; }
	virtual lua_CFunction GetFunc() const { return luaListCapabilites; };
	virtual const OMEChar* GetDescription() const { return "SDPListCapabilities() - Return A string listing all flagged capabilities."; }
	virtual unsigned int GetMinNumArgs() const { return 0; }

	static int luaListCapabilites(lua_State *pState)
	{
		int flags=NONE;

		STLString out;
		if(m_sdp)
			flags=m_sdp->GetCapabilities();

		if(flags==NONE)
			out=STLString("NONE");
		else
		{
			if(flags & POINT_SUPPORT)
				out+="POINT_SUPPORT\n";
			if(flags & HEX_SUPPORT)
				out+="HEX_SUPPORT\n";
			if(flags & GRID_SUPPORT)
				out+="GRID_SUPPORT\n";
			if(flags & POLY_SUPPORT)
				out+="POLY_SUPPORT\n";
			if(flags & QUERY_SUPPORT)
				out+="QUERY_SUPPORT\n";
			if(flags & NEIGHBOR_SUPPORT)
				out+="NEIGHBOR_SUPPORT\n";
			if(flags & ITR_SUPPORT)
				out+="ITR_SUPPORT\n";
			if(flags & BI_ITR_SUPPORT)
				out+="BI_ITR_SUPPORT\n";
			if(flags & READ_ACCESS)
				out+="READ_ACCESS\n";
			if(flags & WRITE_ACCESS)
				out+="WRITE_ACCESS\n";
		}
		lua_pushstring(pState,out.c_str());
		return 1;
	}

private:
	SDPListCapabilities();
};

class SDPNextTo : public SpatialLuaFunc
{
public:
	SDPNextTo(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPNextTo"; }
	virtual lua_CFunction GetFunc() const { return luaNextTo; };
	virtual const OMEChar* GetDescription() const { return "SDPNextTo(x,y) - Return true if any neighbors next to polygon at x validate query y."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaNextTo(lua_State *pState)
	{
		bool ret=false;
		if(m_sdp)
		{
			int poly=LuaDefines::GetDouble(pState,1,-1);
			STLString query;//= GetString(pState, 2);
			lua_pop(pState,2);

			ret = OMECFuncs::SDPNextTo(m_sdp,poly,query);// query.c_str());
		}
		lua_pushboolean(pState,ret);

		return 1;
	}

private:
	SDPNextTo();
};


class SDPNextToArea : public SpatialLuaFunc
{
public:
	SDPNextToArea(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPNextToArea"; }
	virtual lua_CFunction GetFunc() const { return luaNextToArea; };
	virtual const OMEChar* GetDescription() const { return "SDPNextToArea(x,y) - Return the total area of all neighbors next to polygon at x that validate query y."; }
	virtual unsigned int GetMinNumArgs() const { return 2; }

	static int luaNextToArea(lua_State *pState)
	{
		float ret=0.0;
		if(m_sdp)
		{
			int poly=LuaDefines::GetDouble(pState,1,-1);
			STLString query;//=GetString(pState,2);
			lua_pop(pState,2);

			ret = OMECFuncs::SDPNextToArea(m_sdp,poly, query);// query.c_str());
		}
		lua_pushnumber(pState,ret);

		return 1;
	}

private:
	SDPNextToArea();
};

class SDPNextToIDs : public SpatialLuaFunc
{
public:
	SDPNextToIDs(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPNextToIDs"; }
	virtual lua_CFunction GetFunc() const { return luaNextToIDs; };
	virtual const OMEChar* GetDescription() const { return "SDPNextToIDs(x) - Return array of row ids for all neighbors of x"; }
	virtual unsigned int GetMinNumArgs() const { return 1; }

	static int luaNextToIDs(lua_State *pState)
	{
		if (m_sdp)
		{

			int poly = LuaDefines::GetDouble(pState, 1, 0) - 1;

			lua_pop(pState, 1);
			newOMERep(pState, OMECFuncs::SDPNextToID(m_sdp, poly));
		}
		else
			lua_pushnil(pState);
		return 1;
	}
	
private:
	SDPNextToIDs();
};

class SDPNextToValues : public SpatialLuaFunc
{
public:
	SDPNextToValues(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPNextToValues"; }
	virtual lua_CFunction GetFunc() const { return luaNextToValues; };
	virtual const OMEChar* GetDescription() const { return "SDPNextToValues(x,y) - Return array of values for field y for all neighbors of x"; }
	virtual unsigned int GetMinNumArgs() const { return 2; }
	
	static int luaNextToValues(lua_State *pState)
	{
		if (m_sdp)
		{
			int poly = LuaDefines::GetDouble(pState, 1, -1);
			STLString field = lua_tostring(pState, 2);

			lua_pop(pState, 2);
			newOMERep(pState, OMECFuncs::SDPNextToValues(m_sdp, poly, field));
		}
		else
			lua_pushnil(pState);

		return 1;
	}
	
private:
	SDPNextToValues();
};

class SDPSetData : public SpatialLuaFunc
{
public:
	SDPSetData(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPSetData"; }
	virtual lua_CFunction GetFunc() const { return luaSetData; };
	virtual const OMEChar* GetDescription() const { return "SDPSetData(x,y,z) - Set value of column y in record x to value z"; }
	virtual unsigned int GetMinNumArgs() const { return 3; }

	static int luaSetData(lua_State *pState)
	{
		if(m_sdp)
		{
			int rec=LuaDefines::GetDouble(pState,1,-1);
			int col=LuaDefines::GetDouble(pState,2,-1);
			
			if(rec!=-1 && col!=-1)
			{
				using namespace LuaDefines;

				Evaluable* evalCast;
				EnumRep* enumCast;

				switch(lua_type(pState,3))
				{
				case LUA_TNUMBER:
					m_sdp->SetData(rec,col,(OME_SCALAR)lua_tonumber(pState,3));
					break;
				case LUA_TSTRING:
					m_sdp->SetData(rec,col,lua_tostring(pState,3));
					break;
				default:
						m_sdp->SetData(rec,col,LuaDefines::GetDouble(pState,3));
				};
		
				lua_pop(pState,3);
			}
		}
		
		return 0;
	}

private:
	SDPSetData();
};


class SDPWithin : public SpatialLuaFunc
{
public:
	SDPWithin(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPWithin"; }
	virtual lua_CFunction GetFunc() const { return luaWithin; };
	virtual const OMEChar* GetDescription() const { return "SDPWithin(x,y,z) - Return true if any polygons within a distance of z from polygon at x validate query y."; }
	virtual unsigned int GetMinNumArgs() const { return 3; }

	static int luaWithin(lua_State *pState)
	{
		bool ret=false;
		if(m_sdp)
		{
			int poly=LuaDefines::GetDouble(pState,1,-1);
			STLString query;//=GetString(pState,2);
			OME_SCALAR dist = LuaDefines::GetDouble(pState, 2);// 3);
			lua_pop(pState,3);

			if(dist)
				ret=OMECFuncs::SDPWithin(m_sdp,poly,query,dist);
		}
		lua_pushboolean(pState,ret);

		return 1;
	}

private:
	SDPWithin();
};

class SDPWithinArea : public SpatialLuaFunc
{
public:
	SDPWithinArea(ISpatialDataProvider* sdp) : SpatialLuaFunc(sdp) {}
	virtual const OMEChar* GetName() const { return "SDPWithinArea"; }
	virtual lua_CFunction GetFunc() const { return luaWithinArea; };
	virtual const OMEChar* GetDescription() const { return "SDPWithinArea(x,y,z) - Return area of all polygons within a distance of z from polygon at x validate query y."; }
	virtual unsigned int GetMinNumArgs() const { return 3; }

	static int luaWithinArea(lua_State *pState)
	{
		float ret=0.0;
		if(m_sdp)
		{
			int poly=LuaDefines::GetDouble(pState,1,-1);
			STLString query;//=GetString(pState,2);
			OME_SCALAR dist = LuaDefines::GetDouble(pState, 2);// 3);
			lua_pop(pState,3);

			if(dist)
				ret=OMECFuncs::SDPWithinArea(m_sdp,poly,query,dist);
		}
		lua_pushnumber(pState,ret);

		return 1;
	}

private:
	SDPWithinArea();
};

//====================================================================\\
//                                                                    \\
//                  Registration Func for Lua Stuff                   \\
//                                                                    \\
//====================================================================\\

void RegisterCoreSDPFuncs(ISpatialDataProvider* sdp,LuaOMEParser* pParser)
{
	/// \todo create custom lua namespace for SPD
	//add identifiers for capabilities if they don't already exist
	pParser->RegisterExternalConstant("SDP_NONE",NONE);
	pParser->RegisterExternalConstant("SDP_POINT_SUPPORT",POINT_SUPPORT);
	pParser->RegisterExternalConstant("SDP_HEX_SUPPORT",HEX_SUPPORT);
	pParser->RegisterExternalConstant("SDP_GRID_SUPPORT",GRID_SUPPORT);
	pParser->RegisterExternalConstant("SDP_POLY_SUPPORT",POLY_SUPPORT);
	pParser->RegisterExternalConstant("SDP_QUERY_SUPPORT",QUERY_SUPPORT);
	pParser->RegisterExternalConstant("SDP_NEIGHBOR_SUPPORT",NEIGHBOR_SUPPORT);
	pParser->RegisterExternalConstant("SDP_ITR_SUPPORT",ITR_SUPPORT);
	pParser->RegisterExternalConstant("SDP_BI_ITR_SUPPORT",BI_ITR_SUPPORT);
	pParser->RegisterExternalConstant("SDP_READ_ACCESS",READ_ACCESS);
	pParser->RegisterExternalConstant("SDP_WRITE_ACCESS",WRITE_ACCESS);
	pParser->RegisterExternalConstant("SDP_RW_ACCESS",RW_ACCESS);

	//cardinal directions
	pParser->RegisterExternalConstant("SDP_NORTH", GD_N);
	pParser->RegisterExternalConstant("SDP_EAST", GD_E);
	pParser->RegisterExternalConstant("SDP_SOUTH", GD_S);
	pParser->RegisterExternalConstant("SDP_WEST", GD_W);
	pParser->RegisterExternalConstant("SDP_NORTHEAST", GD_NE);
	pParser->RegisterExternalConstant("SDP_SOUTHEAST", GD_SE);
	pParser->RegisterExternalConstant("SDP_SOUTHWEST", GD_SW);
	pParser->RegisterExternalConstant("SDP_NORTHWEST", GD_NW);

	//register base functions
	pParser->RegisterExternalFunction(new SDPGetBooleanData(sdp));
	pParser->RegisterExternalFunction(new SDPGetCapabilities(sdp));
	pParser->RegisterExternalFunction(new SDPGetCellSize(sdp));
	pParser->RegisterExternalFunction(new SDPGetColumnCount(sdp));
	pParser->RegisterExternalFunction(new SDPGetDataMinMax(sdp));
	pParser->RegisterExternalFunction(new SDPGetExtents(sdp));
	pParser->RegisterExternalFunction(new SDPGetFieldCol(sdp));
	pParser->RegisterExternalFunction(new SDPGetNeighborForDirection(sdp));
	pParser->RegisterExternalFunction(new SDPGetNumberData(sdp));
	pParser->RegisterExternalFunction(new SDPGetRowCount(sdp));
	pParser->RegisterExternalFunction(new SDPGetStringData(sdp));
	pParser->RegisterExternalFunction(new SDPGetNeighborCount(sdp));
	pParser->RegisterExternalFunction(new SDPHasNeighborForDirection(sdp));
	pParser->RegisterExternalFunction(new SDPListCapabilities(sdp));
	pParser->RegisterExternalFunction(new SDPNextTo(sdp));
	pParser->RegisterExternalFunction(new SDPNextToArea(sdp));
	pParser->RegisterExternalFunction(new SDPNextToIDs(sdp));
	pParser->RegisterExternalFunction(new SDPNextToValues(sdp));
	pParser->RegisterExternalFunction(new SDPSetData(sdp));
	pParser->RegisterExternalFunction(new SDPWithin(sdp));
	pParser->RegisterExternalFunction(new SDPWithinArea(sdp));
}