#pragma once
#include "OMEDefines.h"
#include "omeobjects/SimObjs.h"
#include "BaseManager.h"
#include "ISpatialDataProvider.h"

/** Base class for C++/compiled versions of models. */
class __EXPORT__ CompiledInterface
{
public:
	/** Modes of execution that can be requested by outside programs.*/
	enum EXECMODES { NONE=0,	///< No execution mode set.
					INIT,		///< Execute initialization code.
					EVAL,		///< Execute update code.
					//SPAWN,		///< Execute Model spawning code.
					VALIDATE	///< Execute Validation code, if any.
	};

	/** @return String containing the name of the compiled model.*/
	virtual OMEChar* GetCompiledName() { return NULL; }
	
	/** Register model object with internal record. Can be used to override existing model objects.
		@param pObj Pointer to the object to register.
		@return true if an object with the same id exists and was overwritten by pObj; false otherwise.
	*/
	bool Register(OMEObject* pObj)
	{
		bool ret=false;
		const OMEChar* rawID = pObj->GetID().c_str();
		if (m_mdlKeys.find(rawID) != m_mdlKeys.end())
		{
			delete m_mdlKeys[rawID];
			m_mdlKeys[rawID] = pObj;
			ret=true;
		}
		return ret;
	}

	/** Execute the requested model code.
		@param exec The execution mode requested by the caller.
		@param pBm Pointer to a BaseManager object to use during evaluation.
		@param pSDP pointer to a SpatialDataProvider to use during evaluation.
		@return true if the mode requested by exec is supported and executed; false otherwise.
	*/
	virtual bool Execute(const EXECMODES & exec,BaseManager* pBm,ISpatialDataProvider* pSDP)
	{
		bool ret=true;
		switch(exec)
		{
		case INIT:
			DoInit(pBm->GetPresentTime(),pBm,pSDP);
			break;
		case EVAL:
			DoUpdate(pBm->GetPresentTime(),pBm,pSDP);
			break;
		/*case SPAWN:
			UpdateSpawners(pBm->GetPresentTime(), pBm, pSDP);*/
		default:
			ret=false;
		}
		return ret;
	}
	
	/** Construct a model object representation from the compiled construction code.
		@param pBm Pointer to BaseManager to bind to Model object.
		@return Pointer to newly constructed model object.
	*/
	virtual Model* LoadModel(BaseManager* pBm) { return NULL; }; //script generated

protected:
	std::map<const OMEChar*,OMEObject*> m_mdlKeys;		///< Record of all objects in the model, sorted by id. 

	/** Entry point for subclasses to execute Initialization code.
		@param time Current simulation time for code execution.
		@param pBm Pointer to a BaseManager object to use during evaluation.
		@param pSDP pointer to a SpatialDataProvider to use during evaluation.
	*/
	virtual void DoInit(const OME_SCALAR & time, BaseManager* pBm,ISpatialDataProvider* pSDP) {};
	/** Entry point for subclasses to execute Update code.
	@param time Current simulation time for code execution.
	@param pBm Pointer to a BaseManager object to use during evaluation.
	@param pSDP pointer to a SpatialDataProvider to use during evaluation.
	*/
	virtual void DoUpdate(const OME_SCALAR & time, BaseManager* pBm,ISpatialDataProvider* pSDP) {};

	//virtual void UpdateSpawners(const float & time, BaseManager* pBm, ISpatialDataProvider* pSDP) {};
};