#pragma once

#include "OMEDefines.h"
#include "OMEParser.h"
#include "CompiledInterface.h"
#include <stack>

class OMEObject;
class Evaluable;
class Param;
class Model;
class SubmodelAssoc;
class BaseEvalSerializer;
class CPPSerializer;


/** Shim for compiled model updates. */
class __EXPORT__ CompiledMdlParser : public OMEParser
{
public:
	CompiledMdlParser(void);
	virtual ~CompiledMdlParser(void);
	virtual void PrepTempDataHandler();
	virtual bool Init(Model* pParentMdl=NULL);
	virtual bool Init(const STLString & args, BaseManager* pBm);
	virtual bool Init(CompiledInterface* pInterface,BaseManager* pBm);
	virtual bool CompileExpression(const OMEChar* expr, OMEObject* pCaller=NULL);
	virtual bool TestExpression(const OMEChar* expr);
	virtual bool DefineVar(const OMEChar* varName, Evaluable *pEval);
	virtual bool DefineVar(const OMEChar* varName, OME_SCALAR *pSclr);
	virtual bool RedefineVar(const OMEChar* varName, Evaluable *pEval);
	virtual bool Evaluate(OME_SCALAR & value, const bool isTest=false,const SclrArgs & args=SclrArgs());
	virtual bool Evaluate(OME_SCALAR* & values, int & count, const bool isTest=false,const SclrArgs & args=SclrArgs());
	virtual bool Evaluate(SclrResults & values, int & count, const bool isTest=false,const SclrArgs & args=SclrArgs());
	virtual bool BatchEvaluate(const SclrArgs & args=SclrArgs(), const bool isTest=false);
	virtual bool BatchEvaluate(SclrResults & values, int & count, const SclrArgs & args=SclrArgs(), const bool isTest=false);
	virtual bool BatchEvaluate(OME_SCALAR* & values, int & count,const SclrArgs & args=SclrArgs(), const bool isTest=false);
	virtual OMEParser* Copy();
	virtual void DefineEnumVals(const ModelEnum & enumVals);
	virtual void RedefineEnumVals(const ModelEnum & enumVals);
	
	virtual bool DefineArray(const OMEChar* varName, SclrVArray* pArr);
	
	virtual bool DefineParameter(Param* pParam);
	virtual bool RedefineParameter(Param* pParam);
	virtual bool DefineConstant(const OMEChar* constName, const int & val) { return false; }
	virtual bool DefineConstant(const OMEChar* constName, const OME_SCALAR & val) { return false; }
	virtual bool DefineConstant(const OMEChar* constName, const OMEChar* & val) { return false; }
	virtual bool RedefineConstant(const OMEChar* constName, const int & val) { return false; }
	virtual bool RedefineConstant(const OMEChar* constName, const OME_SCALAR & val) { return false; }
	virtual bool RedefineConstant(const OMEChar* constName, const OMEChar* & val) { return false; }
	virtual bool DefineSubmodel(Model* pMdl);
	virtual bool DefineAssoc(SubmodelAssoc* pSa);
	virtual bool DefineAll(Model* pMdl) {return false;}
	virtual bool UpdateSubmodelEntry(Model* pMdl, const int ind);
	virtual bool UpdateSubmodelEntries(Model* pMdl, const int start,const int count);
	virtual void ConvertExpressionForEval(Evaluable* pEval);
	virtual void PushBatch(const OMEChar* batch);
	virtual void ClearBatch();
	virtual void PopBatch();
	virtual bool InBatchMode();
	virtual void SetBreaks(const BreakSet & breaks);
	virtual void Reset(Model* pMdl=NULL);
	virtual STLString GetModeID() const;
	virtual void PushModeID(const STLString & eh);
	virtual STLString PopModeID();
	virtual void ClearModeIDs();

	virtual BaseEvalSerializer* GenerateSerializer();
	virtual bool AssociateWithSerializer(BaseEvalSerializer* pS);

	virtual void LoadSDPFuncs(ISpatialDataProvider* pSDP){ /* unsupported*/};

	/** @return Pointer to root Model object, if it has been loaded; NULL otherwise. */
	inline Model* GetRootModel() {return m_pRootMdl;};

	virtual bool RequiresRuntimePrioritization() const { return false; }
protected:

	typedef CompiledInterface* (PASCAL *GenFunc)();				///< Function prototype for retrieving CompiledInterface from external lib.
	typedef void (PASCAL *DisposeFunc)(CompiledInterface*);		///< Function prototype for retrieving model cleanup code from external lib.

	GenFunc GenModel;											///< Pointer to function to call for creating instance of compiled model.
	DisposeFunc DisposeModel;									///< Pointer to function to call for cleaning up after compiled model.

	std::stack<CompiledInterface::EXECMODES> m_modeStack;		///< Stack for tracking requested execution modes.
	Model* m_pRootMdl;											///< Pointer to Root model object.
	CompiledInterface* m_pCompiled;								///< Pointer to instance of CompiledInterface subclass which generates m_pRootModel.
	CPPSerializer* m_pSerializer;								///< Pointer to associated CPPSerializer.

    void* m_dlHandle;											///< Handle for dynamic library used in POSIX environments.

};