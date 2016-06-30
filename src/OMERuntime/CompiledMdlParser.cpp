#include "CompiledMdlParser.h"
#include "CPPSerializer.h"
#include "CompiledMdlFuncs.h"

#if !defined WIN32 && !defined _WIN64
#include <dlfcn.h>
#endif
CompiledMdlParser::CompiledMdlParser(void)
	:m_pCompiled(NULL), m_pRootMdl(NULL), m_pSerializer(NULL),m_dlHandle(NULL)
{
	GenModel = NULL;
	DisposeModel = NULL;
}

CompiledMdlParser::~CompiledMdlParser(void)
{
	if(m_pCompiled && DisposeModel)
    {
		DisposeModel(m_pCompiled);
#if !defined WIN32 && !defined _WIN64
        if(m_dlHandle)
            dlclose(m_dlHandle);
#endif
    }

}

void CompiledMdlParser::PrepTempDataHandler()
{
	OMECFuncs::UseDefaultWrapAllocator();
}

bool CompiledMdlParser::Init(Model* pParentMdl)
{
	m_pRootMdl=pParentMdl;
	return true;
}

bool CompiledMdlParser::Init(const STLString & args,BaseManager* pBm)
{
#if defined WIN32 || defined _WIN64
	//assume args is path to dynamic lib containing compiled library.
	CString dllName(args.c_str());
	dllName += ".dll";
	HINSTANCE hDLL = AfxLoadLibrary( dllName.AllocSysString() ); 
	if (hDLL == NULL)
	{
		DBG_PRINTC(std::to_string(GetLastError()), DBG_RED);
		return false;
	}
	//bind function calls
	GenModel=NULL;
	DisposeModel=NULL;
	 GenModel  = (GenFunc) GetProcAddress( hDLL, "GenModel" );
	 DisposeModel = (DisposeFunc) GetProcAddress( hDLL, "DisposeModel" );
	 if(!GenModel || !DisposeModel)
		 return false;
	 
#else
	//other environments here
#ifdef __APPLE__
#define LIB_EXT ".dylib"
#else
#define LIB_EXT ".so"
#endif

    //unix/linux/osx
    char* errStr=NULL;
    m_dlHandle=dlopen((args+LIB_EXT).c_str(),RTLD_LAZY);
    if ((errStr=dlerror()))
    {
        DBG_PRINTC(errStr,DBG_RED);
        dlclose(m_dlHandle);
        return false;
    }
    GenModel = (GenFunc)dlsym(m_dlHandle, "GenModel");
    if ((errStr=dlerror()))
    {
        DBG_PRINTC(errStr,DBG_RED);
        dlclose(m_dlHandle);
        return false;
    }
    DisposeModel=(DisposeFunc)dlsym(m_dlHandle, "DisposeModel");
    if ((errStr=dlerror()))
    {
        DBG_PRINTC(errStr,DBG_RED);
        dlclose(m_dlHandle);
        return false;
    }
    
    //TODO: ensure factories are exported correctly

#endif
    
    
    //grab interface
    m_pCompiled=GenModel();
    //grab model
    m_pRootMdl=m_pCompiled->LoadModel(pBm);

	return m_pRootMdl;
}

/** Alternate CompiledMdlParser-specific Initialization function.
@param pInterface Pointer to CompiledInterface to associate with Parser.
@param pBm Pointer to BaseManager object to associate with initialization.
@return true if successfully initialized, false otherwise.
*/
bool CompiledMdlParser::Init(CompiledInterface* pInterface,BaseManager* pBm)
{
	m_pCompiled = pInterface;
	m_pRootMdl = m_pCompiled->LoadModel(pBm);
	return true;
}

bool CompiledMdlParser::CompileExpression(const OMEChar* expr, OMEObject* pCaller)
{
	//not used
	return false;
}

bool CompiledMdlParser::TestExpression(const OMEChar* expr)
{
	//not used
	return false;
}

bool CompiledMdlParser::DefineVar(const OMEChar* varName, Evaluable *pEval)
{
	return m_pCompiled->Register(pEval);
}

bool CompiledMdlParser::DefineVar(const OMEChar* varName, OME_SCALAR *pSclr)
{
	//not used
	return false;
}

bool CompiledMdlParser::RedefineVar(const OMEChar* varName, Evaluable *pEval)
{
	return DefineVar(varName,pEval);
}

bool CompiledMdlParser::Evaluate(OME_SCALAR & value, const bool isTest,const SclrArgs & args)
{
	//not used
	return false;
}

bool CompiledMdlParser::Evaluate(OME_SCALAR* & values, int & count, const bool isTest,const SclrArgs & args)
{
	//not used
	return false;
}

bool CompiledMdlParser::Evaluate(SclrResults & values, int & count, const bool isTest,const SclrArgs & args)
{
	//not used
	return false;
}

bool CompiledMdlParser::BatchEvaluate(const SclrArgs & args, const bool isTest)
{
	BaseManager* pBm=m_pRootMdl->GetManager();
	return m_pCompiled->Execute(m_modeStack.top(),pBm,(pBm ? pBm->GetSpatialDataProvider() : NULL));
}

bool CompiledMdlParser::BatchEvaluate(SclrResults & values, int & count, const SclrArgs & args, const bool isTest)
{
	return BatchEvaluate(args,isTest);
}

bool CompiledMdlParser::BatchEvaluate(OME_SCALAR* & values, int & count,const SclrArgs & args, const bool isTest)
{
	return BatchEvaluate(args,isTest);
}

OMEParser* CompiledMdlParser::Copy()
{
	//not used?
	return NULL;
}

void CompiledMdlParser::DefineEnumVals(const ModelEnum & enumVals)
{
	//m_pCompiled->Register(pEval);
}

void CompiledMdlParser::RedefineEnumVals(const ModelEnum & enumVals)
{
	//m_pCompiled->Register(pEval);
}

bool CompiledMdlParser::DefineArray(const OMEChar* varName, SclrVArray* pArr)
{
	//not used
	return false;
}

bool CompiledMdlParser::DefineParameter(Param* pParam)
{
	//not used
	return false;
}

bool CompiledMdlParser::RedefineParameter(Param* pParam)
{
	//not used
	return false;
}

bool CompiledMdlParser::DefineSubmodel(Model* pMdl)
{
	//not used
	return false;
}

bool CompiledMdlParser::DefineAssoc(SubmodelAssoc* pSa)
{
	//not used
	return false;
}

bool CompiledMdlParser::UpdateSubmodelEntry(Model* pMdl, const int ind)
{
	//not used
	return false;
}

bool CompiledMdlParser::UpdateSubmodelEntries(Model* pMdl, const int start,const int count)
{
	//not used
	return false;
}

void CompiledMdlParser::ConvertExpressionForEval(Evaluable* pEval)
{
	//not used
}
void CompiledMdlParser::PushBatch(const OMEChar* batch)
{
	//not used

}
void CompiledMdlParser::ClearBatch()	
{
	//not used
}

void CompiledMdlParser::PopBatch()
{
	//not used
}

bool CompiledMdlParser::InBatchMode()	
{
	//always the equivalent batch
	return true;
}

void CompiledMdlParser::SetBreaks(const BreakSet & breaks)
{
	//not used
}

void CompiledMdlParser::Reset(Model* pMdl)
{
	/// \todo add reset code
}

STLString CompiledMdlParser::GetModeID() const
{
	STLString ret=STLString();

	switch(m_modeStack.top())
	{
	case CompiledInterface::INIT:
		ret="Init";
		break;
	case CompiledInterface::EVAL:
		ret="Update";
		break;
	/*case CompiledInterface::SPAWN:
		ret = "Spawn";
		break;*/
	case CompiledInterface::NONE:
		ret="None";
		break;
	}

	return ret;
}

void CompiledMdlParser::PushModeID(const STLString & eh) 
{
	if(eh=="Init")
		m_modeStack.push(CompiledInterface::INIT);
	else if(eh=="Update")
		m_modeStack.push(CompiledInterface::EVAL);
	/*else if (eh == "Spawn")
		m_modeStack.push(CompiledInterface::SPAWN);*/
	else
		m_modeStack.push(CompiledInterface::NONE);
}
STLString CompiledMdlParser::PopModeID()
{
	STLString ret=GetModeID();
	m_modeStack.pop();
	return ret;
}

void CompiledMdlParser::ClearModeIDs()
{
	while(!m_modeStack.empty())
		m_modeStack.pop();
}

BaseEvalSerializer* CompiledMdlParser::GenerateSerializer()
{
	return new CPPSerializer();
}

bool CompiledMdlParser::AssociateWithSerializer(BaseEvalSerializer* pS)
{
	bool ret = true;
	if (pS)
	{
		m_pSerializer = dynamic_cast<CPPSerializer*>(pS);
		ret = m_pSerializer;
	}
	return ret;
}
