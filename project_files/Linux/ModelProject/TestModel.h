#pragma once
#include "OMEDefines.h"
#include "CompiledInterface.h"
#include "CompiledMdlFuncs.h"
#define OME_MDL_CLASS TestModel
#define OME_MDL_NAME "TestModel"

class __MDL_EXPORT__ OME_MDL_CLASS : public CompiledInterface
{
public:
	virtual OMEChar* GetCompiledName() { return OME_MDL_NAME; }
	virtual Model* LoadModel(BaseManager*); //script generated

protected:

	virtual void DoInit(const OME_SCALAR &, BaseManager*, ISpatialDataProvider*); //script generated
	virtual void DoUpdate(const OME_SCALAR &, BaseManager*, ISpatialDataProvider*); //script generated


};
