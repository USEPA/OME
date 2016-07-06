#include "CPPSerializer.h"
#include "OMEDefines.h"
#include "DebugOutput.h"
#include "omeobjects/SimObjs.h"
#include "CompiledMdlFuncs.h"
#include "EvalXSetInterpTable.h"

#include <fstream>
#include<iomanip>
#include <stack>

const char* OME_CLASS_TEMPLATE =
"class __MDL_EXPORT__ OME_MDL_CLASS : public CompiledInterface\n"
"{\n"
"public:\n"
"	virtual OMEChar* GetCompiledName() { return OME_MDL_NAME; }\n"
"	virtual Model* LoadModel(BaseManager*); //script generated\n"
"\n"
"protected:\n"
"\n"
"	virtual void DoInit(const OME_SCALAR &, BaseManager*, ISpatialDataProvider*); //script generated\n"
"	virtual void DoUpdate(const OME_SCALAR &, BaseManager*, ISpatialDataProvider*); //script generated\n"
"\n"
"\n};"
;

#ifndef OME_MAIN_TEMPLATE_NAME
#define OME_MAIN_TEMPLATE_NAME "CompiledMainTemplate.txt"
#endif

#define BTXT(x)		((x) ? "true" : "false")

/** String representations of various OMETYPE flags. */
const OMEChar* CPPSerializer::m_sOmeTypeStrs[] = { "OME_NULL", "OME_BASE", "OME_EVAL", "OME_STATEVAR",
											"OME_VARIABLE", "OME_VARARRAY", "OME_TSVAR", "OME_FLOW", 
											"OME_INFLUENCE", "OME_MODEL", "OME_SUBPORT", "OME_ASSOCPORT",
											"OME_SPAWNER", "OME_LABEL", "OME_ITERCOND", "OME_SUBASSOC","OME_ALIAS",
											"OME_CONVEYOR"};

using namespace OMECFuncs;

/** Default constructor.
	@param pMdl Optional Model Pointer to associate with Serializer.
*/
CPPSerializer::CPPSerializer(Model* pMdl)
	:m_pRootModel(pMdl)
	, m_rootCluster(this,"root",1)
	, m_constArrayCount(0)
	,m_templateDir("./")
{
	m_cache.precision(OME_WRITE_PRECISION);
}

/** Destructor. */
CPPSerializer::~CPPSerializer()
{
}

bool CPPSerializer::SerializeEval(Model* pRootModel)
{
	m_pRootModel=pRootModel;
		
	//Serialize based on priorities

	m_cache<<"void ";
	if(!m_className.empty())
		m_cache<<m_className<<"::";
	m_cache<<"DoInit(const OME_SCALAR & "<<m_className<<"Time, BaseManager* "<<m_className<<"Bm,ISpatialDataProvider* "<<m_className<<"SDP)"<<endl<<'{'<<endl;
	m_cache<<"using namespace "<<m_className<<"Defs;"<<endl;
	m_cache<<"using namespace OMECFuncs;"<<endl;
	WriteVAAllocs(pRootModel, m_cache);
	//init first
	SerializeModel(pRootModel,true);
	//m_cache << "InitSpawners(" << m_className << "Time, " << m_className << "Bm," << m_className << "SDP);" << endl;
	m_cache<<"AdjustTempPool();"<<endl;
	m_cache<<'}'<<endl<<endl;

	//resort for eval
	
	m_cache<<"void ";
	if(!m_className.empty())
		m_cache<<m_className<<"::";
	m_cache<<"DoUpdate(const OME_SCALAR & "<<m_className<<"Time, BaseManager* "<<m_className<<"Bm,ISpatialDataProvider* "<<m_className<<"SDP)"<<endl<<'{'<<endl;
	m_cache<<"using namespace "<<m_className<<"Defs;"<<endl;
	m_cache<<"using namespace OMECFuncs;"<<endl;
	//then eval
	SerializeModel(pRootModel,false);
	//m_cache << "UpdateSpawners(" << m_className << "Time, " << m_className << "Bm," << m_className << "SDP);" << endl;
	m_cache<<"AdjustTempPool();"<<endl;
	m_cache<<'}'<<endl<<endl;
	
	return true;
}

bool CPPSerializer::SerializeEval(const EvalArray & entries,const bool forInit)
{
	/// \todo implement
	return false;
}

bool CPPSerializer::SerializeEval(Evaluable* entry,const bool forInit)
{
	/// \todo implement
	return false;
}

STLString CPPSerializer::GetInitStatements(const bool forPrint)
{
	//unused
	return STLString();
}

STLString CPPSerializer::GetEvalStatements(const bool forPrint)
{
	//unused
	return STLString();
}

void CPPSerializer::ClearSerializer()
{
	m_cache.clear();
	m_cache.str(STLString());

}

bool CPPSerializer::WriteSerialExpressions(const STLString & initPath, const STLString & evalPath)
{
	//unused
	return false;
}

size_t CPPSerializer::GetLineCount(const bool forInit)
{
	//unused
	return 0;
}

/** Serialize the entire contents of a model.
	@param pRootModel Pointer to the model to act as the root during this conversion process.
	@param useInit If true, use initialization statements; otherwise, use update statements.

*/
bool CPPSerializer::SerializeModel(Model *pRootModel,const bool useInit)
{
	EvalArray evals;

	//initially sort before constructing clusters tree
	pRootModel->GetOrderedEvalsForUpdates(evals,useInit);
	m_constArrayCount = 0;
	
	size_t count = 0;
	if (!pRootModel->IsDynamic() && !pRootModel->ExpectsSpatial())
		count = pRootModel->GetInitNumInstances();
	CPPLoopCluster* pCluster = new(&m_rootCluster)CPPLoopCluster(this,pRootModel->GetID(),count,false,pRootModel);

	std::map<Model*,size_t> spwnsAdded;
	//pair<model,activestate of model>
	std::stack< std::pair<Model*,bool> > mdlStack;
	mdlStack.emplace(pRootModel,false);
	Model* currMdl;
	unsigned int execGrp = 0;

	for (auto itr = evals.begin(); itr != evals.end(); ++itr)
	{
		//subports need to be evaluated outside of model loop
		currMdl = (*itr)->GetOMEType() != OME_SUBPORT ? (*itr)->GetParentModel() : ((SubmodelPort*)(*itr))->GetOuterModel();

		if (spwnsAdded.find(currMdl) == spwnsAdded.end())
				spwnsAdded[currMdl] = 0;
		//ensure that any aggregation evals are flagged as such

		if (NeedsHeader(currMdl) && CPPLoopCluster::ExprHasAggFuncs(useInit ? (*itr)->GetInitExpr() : (*itr)->GetExpr()) && mdlStack.top().first == currMdl)
		{
			//force break, as any objects in same model in this point likely require aggregation.
			pCluster = EndCluster(mdlStack, pCluster);
		}

		//check for execution groups, break cluster if we need to
		if ((*itr)->GetExecGroup() != execGrp)
		{
			if (NeedsHeader(currMdl) && (*itr)->GetExecGroup() != OME_NO_EXEC_GRP && execGrp != OME_NO_EXEC_GRP)
			{
				//if there are differing non-zero execution groups, split cluster here.
				pCluster = EndCluster(mdlStack, pCluster);
			}
			execGrp = (*itr)->GetExecGroup();
			
		}

		if (mdlStack.top().first != currMdl)
		{
			ModelList path;
			while (!mdlStack.top().first->IsNested(currMdl, &path))
			{
				pCluster = EndCluster(mdlStack, pCluster);
			}

			for (auto pItr = path.begin(); pItr != path.end(); ++pItr)
			{
				bool needsSpawner = spwnsAdded[currMdl] == currMdl->GetSpawnerCount() && spwnsAdded[currMdl]>0;
				//if (NeedsHeader(*pItr))
				//	AddModelLoopHeader((*pItr), needsSpawner);
				pCluster = pCluster->NewSubLoopCluster(*pItr,needsSpawner);
				if (needsSpawner)
					pCluster->StartMortalityHere(currMdl);
				mdlStack.emplace(*pItr, needsSpawner);

			}

		}

		if (useInit || !((*itr)->IsOMEType(OME_STATEVAR) || ((*itr)->GetIsConstant() && !(*itr)->GetParentModel()->IsConditional())))
		{
			//pCluster->AppendSrcLabel((*itr)->GetID() + " (" + (*itr)->GetExprName() + ")");
			//make sure new cluster has exec group
			pCluster->SetExecGroup(execGrp);
			pCluster->AppendEval(*itr);
		}

		if ((*itr)->GetOMEType() == OME_SPAWNER)
		{
			++spwnsAdded[currMdl];

			if (spwnsAdded[currMdl] == currMdl->GetSpawnerCount())
			{
				pCluster->StartMortalityHere(currMdl);
				//at this point, model at top of stack should be equivalent to currMdl.
				mdlStack.top().second = true;
			}
		}
	}
	
	//flush
	while (!mdlStack.empty())
	{
		pCluster = EndCluster(mdlStack, pCluster);
	}
	m_cache << m_rootCluster.ProcessTree(useInit);

	return false;
}

/** Query if model needs loop in batch representation.
	@param pBm Pointer to base model to query against.
	@return true if model needs a loop in batch representation; false otherwise.
*/
bool CPPSerializer::NeedsHeader(Model* pBm)
{
	///@todo move conditional to better location.
	bool ret= (pBm->IsDynamic() || (pBm->GetInitNumInstances()!=1) || pBm->IsConditional() || pBm->ExpectsSpatial());
	return ret;
}

/** Write out Model definition to a .h and .cpp file who together create a class which represents the model and its runtime logic.
	@param pMdl The root model object to serialize.
	@param className The name to use for the generated class.
	@param path String containing path to directory which will hold the generated .h and .cpp files.
	@param pSDP Optional pointer to Spatial Data Provider to query for details about the represented spatial structure.
	@return true if files were written successfully; false otherwise.
*/
bool CPPSerializer::WriteToCPPFile(Model* pMdl, const STLString & className, STLString path,ISpatialDataProvider* pSDP)
{
	//for header:
	//load buffer
	//add headers
	//Add class name as #define OME_MDL_CLASS
	//Add body from CompiledHeaderTemplate.txt
	//save header file
	m_className=className;
	
    if (path.back()!='/')
        path=path+"/";

    //write out docking header
    ofstream docFile(path+"MdlClassInclude.h");
    docFile<<"#include "<<'"'<<m_className<<".h\""<<std::endl;
    docFile.close();

	ofstream hFile(path+m_className+".h");
	bool ret=false;
	if(hFile.good())
	{
		hFile<<"#pragma once"<<endl;
		hFile<<"#include \"OMEDefines.h\""<<endl;
		hFile<<"#include \"CompiledInterface.h\""<<endl;
		hFile<<"#include \"CompiledMdlFuncs.h\""<<endl;
		hFile<<"#define OME_MDL_CLASS "<<m_className<<endl;
		hFile<<"#define OME_MDL_NAME \""<<m_className<<'"'<<endl<<endl;

		hFile << OME_CLASS_TEMPLATE<<endl;
		ofstream cppFile(path+m_className+".cpp");
	
		if(cppFile.good())
		{
			oSTLSStream iniCache,declCache;
			iniCache.precision(OME_WRITE_PRECISION);
			declCache.precision(OME_WRITE_PRECISION);

			cppFile<<ConstructHeader(className,pSDP);
			cppFile<<"namespace "<<m_className<<"Defs"<<endl<<'{'<<endl;
			//write out any static arrays
				
			WriteModelKeys(pMdl);
			m_cache <<pMdl->GetID() << ".JoinAliases();" << endl;
			m_cache << "return &" << pMdl->GetID() << ';' << endl;
			m_cache << '}' << endl << endl;


			WriteModelInitializations(pMdl, iniCache, declCache);
			WriteEnumInitializations(iniCache, declCache);
			SerializeEval(pMdl);
			iniCache << m_arrayCache.str() << endl<<endl;
			iniCache<<"};"<<endl;
	
			iniCache << "Model* " << m_className <<"::LoadModel(BaseManager* "<<m_className<<"Bm)"<<endl<<'{'<<endl;
			iniCache<<"using namespace "<<m_className<<"Defs;"<<endl;
			iniCache<<"using namespace OMECFuncs;"<<endl;
			iniCache<<declCache.str();
			iniCache<<pMdl->GetID()<<".JoinLinks(true,true);"<<endl;
			//iniCache<<pMdl->GetID()<<"->PurgeInfluences(true);"<<endl;
			iniCache<<"//Begin Lookup table"<<endl;

				
			cppFile <<iniCache.str()<< endl << endl;
			cppFile<<m_cache.str();
			//cppFile<<endl<<"#undef and"<<endl<<"#undef or"<<endl;
			ret=true;
		}
		else
			DBG_PRINTC("Could not open file "+path+m_className+".cpp",DBG_RED);

	}
	else
		DBG_PRINTC("Could not open file "+path+m_className+".h",DBG_RED);
	return ret;
}

/** Write out a .cpp file which will contain a main entry point for loading, running, and cleaning up after the generated model class.
	@param className The name to use for the generated class.
	@param path String containing path to directory which will hold the generated .cpp file.
	@param pBm BaseManager to use for generating default manager arguments in the main function body.
*/
bool CPPSerializer::WriteMainFile(const STLString & className, STLString path,BaseManager* pBm)
{
	/// @todo update
    if(path.back()!='/')
        path=path+"/";
    
	ofstream mainFile(path + className + "Main.cpp");
	bool ret = false;
	if (mainFile.good())
	{
		mainFile << "#include <iostream>" << endl;
		mainFile << "#include <fstream>" << endl;
		mainFile << "#include <sstream>" << endl;
		mainFile << "#include <string>" << endl;
		mainFile << "#include <ctime>" << endl;
		mainFile << endl;
		mainFile << "#include \"CharType.h\"" << endl;
		mainFile << "#include \"ValTypes.h\"" << endl;
		mainFile << "#include \"SimObjs.h\"" << endl;
		mainFile << "#include \"DebugOutput.h\"" << endl;
		mainFile << "#include \"SimManager.h\"" << endl;
		mainFile << "#include \"FileExts.h\"" << endl;
		mainFile << "#include \"LuaOMEParser.h\"" << endl;
		mainFile << "#include \"CompiledMdlParser.h\"" << endl;
		mainFile << "#include \"CompiledInterface.h\"" << endl;
		mainFile << endl;
		//add custom class header
		mainFile << "#include \"" << className << ".h\"" << endl;

		//fill in custom #defines
		mainFile << endl;
		mainFile << "//-- Begin Customizations --" << endl;
		mainFile << "#define COMP_TYPE " << className << endl;
		mainFile << "#define RUN_INTMETHOD " << (pBm->GetIntMethod() == OMEIntegrationBlock::IM_EULER ? "OMEIntegrationBlock::IM_EULER" : "OMEIntegrationBlock::IM_RK4") << endl;
		mainFile << "#define RUN_START " << pBm->GetStartTime() << endl;
		mainFile << "#define RUN_STOP " << pBm->GetStopTime() << endl;
		mainFile << "#define RUN_INTERVAL " << pBm->GetInterval() << endl;
		mainFile << "#define RUN_STEPSIZE " << pBm->GetStepSize() << endl;

		ifstream templateFile(m_templateDir+OME_MAIN_TEMPLATE_NAME);
		if (templateFile.good())
		{
			mainFile << templateFile.rdbuf();
			mainFile.close();
			ret = true;
		}
		else
			DBG_PRINTC("Could not open file " OME_MAIN_TEMPLATE_NAME, DBG_RED);
	}
	else
		DBG_PRINTC("Could not open file " + path + className + "Main.cpp", DBG_RED);
	return ret;
}

/** Add header to generated class .cpp file.
	@param hName The name with the paired header file, minus the .h extension.
	@param pSDP Optional pointer to Spatial Data Provider. If present, information is extracted to put limits on
	coverage-related data collections.
	@return A string containing the header for the .cpp file.
*/
STLString CPPSerializer::ConstructHeader(const STLString & hName,ISpatialDataProvider* pSDP)
{
	oSTLSStream out;
	out<<"#include \""<<hName<<".h\""<<endl;
	out<<"#include <map>"<<endl;
	out << "#include \"OMEDefines.h\"" << endl;
	//out << "#include \"DynamicTempArray.h\"" << endl << endl;
	out << "#include \"SubIndexVal.h\"" << endl;
	out << "#include \"ValueArchive.h\"" << endl;
	
	if (pSDP)
	{
		out << "#define OME_MAX_NEIGHBORS " << pSDP->GetMaxNeighborCount() << "//derived from coverage at code generation time. Update if coverage changes"<<endl;
	}
	/*out << "#ifndef LSIZE_BEST_GUESS" << endl;
	out << "#define LSIZE_BEST_GUESS 100" << endl;
	out << "#endif" << endl << endl;*/
	
	//out<<"#define and &&"<<endl<<"#define or ||"<<endl<<endl;
	return out.str();
}

///* Write out functions for updating model Spawner objects.
//	@param pMdl Pointer to the root model to begin with.
//*/
//void CPPSerializer::WriteSpawnerUpdates(Model* pMdl)
//{
//	m_cache << "void " << m_className << "::InitSpawners(const OME_SCALAR & " << m_className << "Time, BaseManager* " << m_className << "Bm,ISpatialDataProvider* " << m_className << "SDP)" << endl << '{' << endl;
//	m_cache << "using namespace " << m_className << "Defs;" << endl;
//	m_cache << "using namespace OMECFuncs;" << endl;
//	ProcessSpawnerUpdateLevel(pMdl,true);
//	m_cache << '}'<<endl<<endl;
//
//	m_cache << "void "<<m_className<<"::UpdateSpawners(const OME_SCALAR & " << m_className << "Time, BaseManager* " << m_className << "Bm,ISpatialDataProvider* " << m_className << "SDP)" << endl << '{' << endl;
//	m_cache << "using namespace " << m_className << "Defs;" << endl;
//	m_cache << "using namespace OMECFuncs;" << endl;
//	ProcessSpawnerUpdateLevel(pMdl, false);
//	m_cache << '}' << endl;
//}
//
///* Evaluate Spawner objects for this model and any submodels.
//	@param pMdl Pointer to the Model containing the spawners to translate.
//	@param init Whether or not the generated statements are for an initialization state.
//*/
//void CPPSerializer::ProcessSpawnerUpdateLevel(Model* pMdl,const bool & init)
//{
//	if (pMdl->IsDynamic() || pMdl->GetSubmodelCount())
//	{
//		if (pMdl->IsDynamic() || pMdl->GetInitNumInstances()>1)
//			AddModelLoopHeader(pMdl);
//		Spawner* pSpwn;
//		size_t i;
//		STLString expr;
//		for (i = 0; i < pMdl->GetSpawnerCount(); i++)
//		{
//			pSpwn = pMdl->GetSpawner(i);
//			expr = CPPizeExpression(pSpwn->GetExpr(), pSpwn,NULL);
//			if (init)
//				m_cache << pSpwn->GetID() << "->BatchInitialize(" << m_className << "Time," << m_className << "Bm);" << endl;
//			if (init || !pSpwn->RunsOnce())
//				m_cache << pSpwn->GetID() << "->EvalWithVal(" << expr << "); ResetLine();" << endl;
//		}
//
//		for (i = 0; i < pMdl->GetSubmodelCount(); i++)
//			ProcessSpawnerUpdateLevel(pMdl->GetSubmodel(i), init);
//		if (pMdl->IsDynamic() || pMdl->GetInitNumInstances()>1)
//			AddModelLoopFooter(pMdl);
//	}
//}

/** Generate code that registers default model objects with map of ids and objects.
	@param pMdl Pointer to root model object.
*/
void CPPSerializer::WriteModelKeys(Model* pMdl)
{
	
	m_cache<<"Register(&"<<pMdl->GetID()<<");"<<endl;

	size_t i;
	
	for(i=0; i<pMdl->GetVarCount(); i++)
		m_cache<<"Register(&"<<pMdl->GetVariable(i)->GetID()<<");"<<endl;
	
	for(i=0; i<pMdl->GetStateVarCount(); i++)
		m_cache<<"Register(&"<<pMdl->GetStateVar(i)->GetID()<<");"<<endl;

	for(i=0; i<pMdl->GetFlowCount(); i++)
		m_cache<<"Register(&"<<pMdl->GetFlow(i)->GetID()<<");"<<endl;

	//for(i=0; i<pMdl->GetInfluenceCount(); i++)
		//m_cache<<"Register(&"<<pMdl->GetInfluence(i)->GetID()<<");"<<endl;

	for (i = 0; i < pMdl->GetSubPortCount(); i++)
	{
		if (pMdl->GetSubPort(i)->LinkedToFlow())
			m_cache << "Register(&" << pMdl->GetSubPort(i)->GetID() << ");" << endl;
	}


	for(i=0; i<pMdl->GetIterConditionalCount(); i++)
		m_cache<<"Register(&"<<pMdl->GetItrCond(i)->GetID()<<");"<<endl;
	
	for(i=0; i<pMdl->GetAssocCount(); i++)
		m_cache<<"Register(&"<<pMdl->GetAssoc(i)->GetID()<<");"<<endl;

	for(i=0; i<pMdl->GetSpawnerCount(); i++)
		m_cache << "Register(&"<< pMdl->GetSpawner(i)->GetID() << ");" << endl;

	for(i=0;i<pMdl->GetSubmodelCount();i++)
		WriteModelKeys(pMdl->GetSubmodel(i));

}

/** Write out construction code for a specific model object.
	@param pEval Pointer to Evaluable object who will have their construction code generated.
	@param decCache ostringstream which contains object declarations.
	@param iniCache ostringstream which holds object's initialization statements.
*/
void CPPSerializer::WriteEvalInitialization(Evaluable* pEval,oSTLSStream & decCache, oSTLSStream & iniCache)
{
	size_t i;
	//create empty object.
	STLString id=pEval->GetID();
	decCache << pEval->StrForType() << "& " << id << "=*((" << pEval->StrForType() << "*)BaseManager::GenModelObject(" << m_sOmeTypeStrs[pEval->GetOMEType()] << "));" << endl;

	//fill in ome object level
	iniCache << id << ".SetID(\"" << id << "\");" << endl;

	iniCache << id << ".SetName(\"" << TagEscapes(pEval->GetName()) << "\");" << endl;
	iniCache << id << ".SetInternal(" << (pEval->GetInternal() ? "true" : "false") << ");" << endl;

	if (!pEval->IsOMEType(OME_ALIAS))
	{
		//fill in eval level
		iniCache << id << ".SetExpr(\"" << pEval->GetExpr() << "\");" << endl;
		if (pEval->GetInitExpr() != pEval->GetExpr())
			iniCache << id << ".SetInitExpr(\"" << pEval->GetInitExpr() << "\");" << endl;
		for (size_t i = 0; i < pEval->GetTableCount();++i)
			iniCache << id << ".AppendTableID(\"" << pEval->GetTableID(i) << "\");" << endl;

		if (pEval->IsAsInt())
			iniCache << id << ".SetAsInt(true);" << endl;
		if (pEval->GetAskVal())
			iniCache << id << ".SetAskVal(true);" << endl;
		if (pEval->GetAutoGenerated())
			iniCache << id << ".SetAutoGenerated(true);" << endl;
	}
	else
		iniCache << id << ".SetSrcID(\"" << ((EvalAlias*)pEval)->GetSrcID() << "\");" << endl;
	if (pEval->IsOMEType(OME_CONVEYOR))
	{
		iniCache << id << ".SetTransitExpr(\"" << ((Conveyor*)pEval)->GetTransitExpr() << "\");" << endl;
		if (((Conveyor*)pEval)->HasCapacity())
			iniCache << id << ".SetCapacity(" << ((Conveyor*)pEval)->GetCapacity() << ");" << endl;
	}


	if(pEval->IsOMEType(OME_VARIABLE))
	{
		Variable* pVar=(Variable*)pEval;
		if (pVar->GetMinMaxLimit() == Variable::HAS_MIN || pVar->GetMinMaxLimit() == Variable::HAS_MINMAX)
			iniCache << id << ".SetMinValue(" << pVar->GetMin() << ");" << endl;
		if (pVar->GetMinMaxLimit() == Variable::HAS_MAX || pVar->GetMinMaxLimit() == Variable::HAS_MINMAX)
			iniCache << id << ".SetMaxValue(" << pVar->GetMax() << ");" << endl;
		iniCache << id << ".SetExpectsFile(" << BTXT(pVar->GetExpectsFile()) << ");" << endl;
		
		iniCache << id << ".SetInitOnly(" << BTXT(pVar->GetInitOnly() && !pEval->GetParentModel()->IsDynamic()) << ");" << endl;
	}

	if(pEval->IsOMEType(OME_VARARRAY))
	{
		VarArray* pVa=(VarArray*)pEval;

		//grab dims;
		iniCache << "IndVec " << id << "_dims=IndVec(" << pVa->GetDimCount() << ");" << endl;
		for (i = 0; i<pVa->GetDimCount(); i++)
			iniCache<<id<<"_dims["<<i<<"]="<<pVa->GetDim(i)<<';'<<endl;
		iniCache << id << ".OverrideDims(" << id << "_dims);" << endl;
		//grab actual values
		/*
		iniCache<<"OME_SCALAR "<<id<<"_vals[]={";
		for(i=0; i<pVa->GetSize(); i++)
		{
			iniCache<<(*pVa)[i];
			if(i<pVa->GetSize()-1)
				iniCache<<',';
		}
		iniCache<<"};"<<endl;
		iniCache<<id<<"->SetValues("<<id<<"_vals,"<<pVa->GetSize()<<",true);"<<endl;
*/
	}

	if(pEval->IsOMEType(OME_TSVAR))
	{
		TimeSeriesVar* pTs=(TimeSeriesVar*)pEval;

		iniCache << id << ".SetInterval(" << pTs->GetInterval() << ");" << endl;
		iniCache << id << ".SetInterpolation(TimeSeriesVar::";
		switch(pTs->GetInterpolation())
		{
		case TimeSeriesVar::LAST:
			iniCache<<"LAST";
			break;
		case TimeSeriesVar::NEAREST:
			iniCache<<"NEAREST";
			break;
		case TimeSeriesVar::INTERPOLATE:
			iniCache<<"INTERPOLATE";
			break;
		case TimeSeriesVar::FIXED:
			iniCache << "FIXED";
			break;
		}
		iniCache<<");"<<endl;

		OMESclrArray times=pTs->GetTimes();
		if(!times.empty())
		{
			for(auto itr=times.begin(); itr!=times.end();++itr)
				iniCache << id << ".AddTimeVal(" << *itr << ',' << pTs->GetValueForTime(*itr) << ");" << endl;
		}
	}

	
	/// \todo Omit Influence from code once order is determined
	if(pEval->IsOMEType(OME_FLOW) || pEval->IsOMEType(OME_INFLUENCE))
	{
		Link* pLink=dynamic_cast<Link*>(pEval);
		if(!pLink->GetSourceID().empty())
			iniCache << id << ".SetSourceID(\"" << pLink->GetSourceID() << "\");" << endl;
		if(!pLink->GetTargetID().empty())
			iniCache << id << ".SetTargetID(\"" << pLink->GetTargetID() << "\");" << endl;
	}

	/// \todo Omit non-evaluated ports from code once order is determined
	if(pEval->IsOMEType(OME_SUBPORT))
	{
		SubmodelPort* pPort=(SubmodelPort*)pEval;
		StrArray portIds;
		
		portIds=pPort->GetSubSourceIDs();
		for(i=0; i<portIds.size();i++)
			iniCache << id << ".AddSubSourceID(\"" << portIds[i] << "\");" << endl;
		portIds=pPort->GetSubTargetIDs();
		for(i=0; i<portIds.size();i++)
			iniCache << id << ".AddSubTargetID(\"" << portIds[i] << "\");" << endl;
		portIds=pPort->GetOutSourceIDs();
		for(i=0; i<portIds.size();i++)
			iniCache << id << ".AddOutSourceID(\"" << portIds[i] << "\");" << endl;
		portIds=pPort->GetOutTargetIDs();
		for(i=0; i<portIds.size();i++)
			iniCache << id << ".AddOutTargetID(\"" << portIds[i] << "\");" << endl;

		iniCache << id << ".SetOuterModel(" << (pPort->GetOuterModel() ? pPort->GetOuterModel()->GetID() : STLString("NULL")) << ");" << endl;
	}

	if(pEval->IsOMEType(OME_SPAWNER))
	{
		Spawner* pSpwn = (Spawner*)pEval;
		if (pSpwn->RunsOnce())
			iniCache << id << ".SetRunsOnce(true);" << endl;
		if (pSpwn->IsStochastic())
			iniCache << id << ".SetIsStochastic(true);" << endl;
		if (pSpwn->GetPerInstance())
			iniCache << id << ".SetPerInstance(true);" << endl;
		if (pSpwn->IsLoss())
			iniCache << id << ".SetIsLoss(true);" << endl;
		if (pSpwn->IsConditional())
			iniCache << id << ".SetConditional(true);" << endl;

	}

	if(pEval->IsOMEType(OME_ITERCOND))
	{
		/// \todo update when dynamic is re-enabled
	}

	if(pEval->IsOMEType(OME_SUBASSOC))
	{
		/// \todo update when dynamic is re-enabled
	}
}

/** Write out construction code for a model or submodel.
@param pMdl Pointer to Model object who will have their construction code generated.
@param decCache ostringstream which contains object declarations.
@param iniCache ostringstream which holds object's initialization statements.
*/
void CPPSerializer::WriteModelInitializations(Model* pMdl,oSTLSStream & decCache, oSTLSStream & iniCache)
{
	STLString mdlID=pMdl->GetID();
	
	DBG_PRINTC(pMdl->GetName(),DBG_YELLOW);
	decCache << pMdl->StrForType() << "& " << mdlID << "=*((" << pMdl->StrForType() << "*)BaseManager::GenModelObject(" << m_sOmeTypeStrs[pMdl->GetOMEType()] << "));" << endl;

	//fill in ome object level
	iniCache << mdlID << ".SetID(\"" << mdlID << "\");" << endl;
	iniCache << mdlID << ".SetName(\"" << TagEscapes(pMdl->GetName()) << "\");" << endl;
	iniCache << mdlID << ".SetInternal(" << (pMdl->GetInternal() ? "true" : "false") << ");" << endl;
	//collect model info
	//iniCache<<mdlID<<"->SetStepSize("<<pMdl->GetStepSize()<<");"<<endl;
	iniCache << mdlID << ".SetInitNumInstances(" << pMdl->GetInitNumInstances() << ");" << endl;
	iniCache << mdlID << ".SetPreferredIntMethod(OMEIntegrationBlock::IM_";
	switch(pMdl->GetPreferredIntMethod())
	{
	case OMEIntegrationBlock::IM_EULER:
		iniCache<<"EULER";
	break;
	case OMEIntegrationBlock::IM_RK4:
		iniCache<<"RK4";
		break;
	default:
		iniCache<<"UNDEF";
	}
	iniCache<<");"<<endl;

	///\todo update data source piece when it is fixed in model.

	//tables
	StrArray tableIDs=pMdl->GetIDsForTables();
	if(!tableIDs.empty())
	{
		bool hasXpts = false;
		iniCache<<"EvalTable::DataArray "<<mdlID<<"_tableData;"<<endl;
		iniCache<<"IndVec "<<mdlID<<"_tableDims;"<<endl;
		EvalTable* pEt;
		EvalInterpTable* pIt;
		EvalXSetInterpTable* pXIt;
		IndVec dims;
		EvalTable::DataArray tData;
		size_t i;
		for(auto itr=tableIDs.begin(); itr!=tableIDs.end(); ++itr)
		{
			pEt=pMdl->GetEvalTable(*itr);
			tData=pEt->GetAllVals();
			iniCache<<mdlID<<"_tableData.clear();"<<endl;
			for(i=0; i<tData.size();i++)
				iniCache << mdlID << "_tableData.push_back(" << tData[i] <<");" << endl;
			dims=pEt->GetAllDims();
			iniCache<<mdlID<<"_tableDims.clear();"<<endl;
			for(i=0; i<dims.size(); i++)
				iniCache<<mdlID<<"_tableDims.push_back("<<dims[i]<<");"<<endl;
			
			if ((pXIt = dynamic_cast<EvalXSetInterpTable*>(pMdl->GetEvalTable(*itr))))
			{
				tData = pXIt->GetXPts();
				if (!hasXpts)
				{
					iniCache << "EvalTable::DataArray " << mdlID << "_tableXPts;" << endl;
					hasXpts = true;
				}
				iniCache << mdlID << "_tableXPts.clear();" << endl;
				for (i = 0; i<tData.size(); i++)
					iniCache << mdlID << "_tableXPts.push_back(" << tData[i] << ");" << endl;
				
				iniCache << "EvalXSetInterpTable* " + *itr << " = new EvalXSetInterpTable(\"" << *itr << "\"," << EvalInterpTable::GetInterpModeLabel(pXIt->GetInterpMode(), true) << ',' << EvalInterpTable::GetBoundsModeLabel(pXIt->GetBoundMode(), true) << ',' << mdlID << "_tableXPts," << mdlID << "_tableData);" << endl;
				iniCache << mdlID << ".AddEvalTable(" << *itr << ");" << endl;
			}
			else if ((pIt = dynamic_cast<EvalInterpTable*>(pMdl->GetEvalTable(*itr))))
			{
				iniCache << "EvalInterpTable* " + *itr << " = new EvalInterpTable(\"" << *itr << "\"," << EvalInterpTable::GetInterpModeLabel(pIt->GetInterpMode(), true) << ',' << EvalInterpTable::GetBoundsModeLabel(pIt->GetBoundMode(), true) << ',' << mdlID << "_tableData);" << endl;
				iniCache << *itr << "->SetBounds(" << pIt->GetLowerBound() << ',' << pIt->GetUpperBound() << ");" << endl;
				iniCache << mdlID << ".AddEvalTable(" << *itr << ");" << endl;
			}
			else
			{
				pEt = pMdl->GetEvalTable(*itr);
				iniCache << "EvalTable* " + *itr << "=new EvalTable(\"" << *itr << "\"," << mdlID << "_tableDims," << mdlID << "_tableData);" << endl;
				iniCache << mdlID << ".AddEvalTable(" << *itr << ");" << endl;
			}

			//if (!pEt->GetFilePath().empty() && !pEt->GetColumnName().empty())
			//	iniCache << *itr << "->PopulateFromCSV(\"" << pEt->GetFilePath() << "\", \"" << pEt->GetColumnName() << "\");" << endl;
		}
	}
	
	Evaluable* pEval;
	size_t i;

	//enums
	for (i = 0; i < pMdl->GetModelEnumCount(); ++i)
	{
		iniCache << mdlID << ".AddModelEnum(\"" << pMdl->GetModelEnum(i).ToString() << "\");" << endl;
	}
	
	for(i=0; i<pMdl->GetVarCount(); i++)
	{
		pEval=(Evaluable*)pMdl->GetVariable(i);
		WriteEvalInitialization(pEval,decCache,iniCache);
		iniCache << mdlID << ".AddVariable(&" << pEval->GetID() << ");" << endl;
		iniCache<<endl;
	}
	
	for(i=0; i<pMdl->GetStateVarCount(); i++)
	{
		pEval=(Evaluable*)pMdl->GetStateVar(i);
		WriteEvalInitialization(pEval,decCache,iniCache);
		iniCache << mdlID << ".AddStateVar(&" << pEval->GetID() << ");" << endl;
		iniCache<<endl;
	}

	for(i=0; i<pMdl->GetFlowCount(); i++)
	{
		pEval=(Evaluable*)pMdl->GetFlow(i);
		WriteEvalInitialization(pEval,decCache,iniCache);
		iniCache << mdlID << ".AddFlow(&" << pEval->GetID() << ");" << endl;
		iniCache<<endl;
	}

	//for(i=0; i<pMdl->GetInfluenceCount(); i++)
	//{
	//	pEval=(Evaluable*)pMdl->GetInfluence(i);
	//	WriteEvalInitialization(pEval,decCache,iniCache);
	//	iniCache<<mdlID<<"->AddInfluence("<<pEval->GetID()<<");"<<endl;
	//	iniCache<<endl;
	//}

	//for(i=0; i<pMdl->GetSubPortCount(); i++)
	//{
	//	pEval=(Evaluable*)pMdl->GetSubPort(i);
	//	if(pEval->GetParentModel()==pMdl && ((SubmodelPort*)pEval)->GetIsEvaluated())
	//	{
	//		WriteEvalInitialization(pEval,decCache,iniCache);
	//		iniCache<<globalID<<"->AddSubPort("<<pEval->GetID()<<",true);"<<endl;
	//		iniCache<<((SubmodelPort*)pEval)->GetOuterModel()->GetID()<<"->AddSubPort("<<pEval->GetID()<<",false);"<<endl;
	//		iniCache<<endl;
	//	}
	//}

	for(i=0; i<pMdl->GetIterConditionalCount(); i++)
	{
		pEval=(Evaluable*)pMdl->GetItrCond(i);
		WriteEvalInitialization(pEval,decCache,iniCache);
		iniCache << mdlID << ".AddIterConditional(&" << pEval->GetID() << ");" << endl;
		iniCache<<endl;
	}
	
	for(i=0; i<pMdl->GetAssocCount(); i++)
	{
		pEval=(Evaluable*)pMdl->GetAssoc(i);
		WriteEvalInitialization(pEval,decCache,iniCache);
		iniCache << mdlID << ".AddSubMCAssoc(&" << pEval->GetID() << ");" << endl;
		iniCache<<endl;
	}

	for(i=0; i<pMdl->GetSpawnerCount(); i++)
	{
		pEval=(Evaluable*)pMdl->GetSpawner(i);
		WriteEvalInitialization(pEval,decCache,iniCache);
		iniCache << mdlID << ".AddSpawner(&" << pEval->GetID() << ");" << endl;
		iniCache<<endl;
	}

	for (i = 0; i<pMdl->GetAliasCount(); i++)
	{
		pEval = (Evaluable*)pMdl->GetAlias(i);
		WriteEvalInitialization(pEval, decCache, iniCache);
		iniCache << mdlID << ".AddAlias(&" << pEval->GetID() << ");" << endl;
		iniCache << endl;
	}

	for(i=0;i<pMdl->GetSubmodelCount();i++)
	{
		pEval=(Evaluable*)pMdl->GetSubmodel(i);
		WriteModelInitializations(pMdl->GetSubmodel(i),decCache,iniCache);
		iniCache << mdlID << ".AddSubmodel(&" << pEval->GetID() << ");" << endl;
		iniCache<<endl;
	}

}

/** Write out initialization statements for Model enumerated value types.
	@param decCache The declaration statement buffer.
	@param iniCache The initialization statement buffer.
*/
void CPPSerializer::WriteEnumInitializations(oSTLSStream & decCache, oSTLSStream & iniCache)
{

	oSTLSStream idStrm;
	for (auto itr = m_enumsToWrap.begin(); itr != m_enumsToWrap.end(); ++itr)
	{
		idStrm.clear();
		idStrm.str(STLString());
		if ((*itr).pEEntry)
		{
			idStrm << "en_" << (*itr).pMdl->GetID() << '_' << (*itr).pEEnum->GetEnumClassName() << '_' << (*itr).pEEntry->ind;
			decCache << "OMECFuncs::EnumWrap " << idStrm.str() << "(NULL);" << endl;
			iniCache << "new(&" << idStrm.str() << ")OMECFuncs::EnumWrap(" << (*itr).pMdl->GetID() << "->ModelEnumForName(\"" << (*itr).pEEnum->GetEnumClassName() << "\")->AddrForIndex(" << (*itr).pEEntry->ind << "));" << endl;
		}
		else
		{
			idStrm << "en_" << (*itr).pMdl->GetID() << '_' << (*itr).pEEnum->GetEnumClassName();
			decCache << "OMECFuncs::ECollWrap " << idStrm.str() << "(NULL);" << endl;
			iniCache << "new(&" << idStrm.str() << ")OMECFuncs::ECollWrap(" << (*itr).pMdl->GetID() << "->ModelEnumForName(\"" << (*itr).pEEnum->GetEnumClassName() << "\"));" << endl;
		}
	}
}

/** Write out allocation statements for VarArray for each model in heirarchy.
	@param pRoot Pointer to the root model.
	@param iniCache The cache of initialization statements.
*/
void CPPSerializer::WriteVAAllocs(Model* pRoot, oSTLSStream & iniCache)
{
	VarArray* pVa;
	for (size_t i = 0; i < pRoot->GetVarCount(); ++i)
	{
		if ((pVa = dynamic_cast<VarArray*>(pRoot->GetVariable(i))))
			iniCache << pVa->GetID() << ".Allocate();" << endl;
	}

	for (size_t i = 0; i < pRoot->GetSubmodelCount(); ++i)
		WriteVAAllocs(pRoot->GetSubmodel(i), iniCache);
}

/** Add an enumerated value entry for future processing.
	@param eee ModelEnumEntry to add.
*/
void CPPSerializer::AddEnumForWrap(const ModelEnumEntry & eee)
{
	auto itr = m_enumsToWrap.begin();
	for (; itr != m_enumsToWrap.end() && (*itr) != eee; ++itr);
	
	if (itr == m_enumsToWrap.end())
		m_enumsToWrap.push_back(eee);
}
