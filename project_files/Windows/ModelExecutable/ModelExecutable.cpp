#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>

#include "CharType.h"
#include "ValTypes.h"
#include "SimObjs.h"
#include "DebugOutput.h"
#include "SimManager.h"
#include "FileExts.h"
#include "LuaOMEParser.h"
#include "CompiledMdlParser.h"
#include "CompiledInterface.h"
#include "CSVSpatialDataProvider.h"

#include "TestModel.h"

#define COMP_TYPE TestModel
#define RUN_INTMETHOD OMEIntegrationBlock::IM_EULER
#define RUN_START 0
#define RUN_STOP 2556
#define RUN_INTERVAL 0.3
#define RUN_STEPSIZE 1
#define SDP_FILE "hexAllMod.csv"
#define SDP_DELIM ','
#define SDP_XCOORD "X_CENTER"
#define SDP_YCOORD "Y_CENTER"
#define SDP_SCRIPT "\
  local record,col=...\
\
--for record=i, rCount do\
local found={}\
local i=1\
local nList=SDPGetStringData(record,col,1024)\
\
for n in string.gmatch(nList,\"[^|]+\") do\
	found[i]=tonumber(n)\
	i=i+1\
end\
--end\
\
return unpack(found)\
  "
#define SDP_LANG "lua"
#define SDP_MODE CSVSpatialDataProvider::NM_USE_COLUMN
#define SDP_EXTRA "neighbors"
// ModelExecutable.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "CharType.h"
#include "ValTypes.h"

#include "SimObjs.h"
//includes will be inserted separately
#include "DebugOutput.h"
#include "SimManager.h"
#include "FileExts.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include "LuaOMEParser.h"
#include "CompiledMdlParser.h"
#include "CompiledInterface.h"
#include "CSVSpatialDataProvider.h"

//model class

using namespace std;

static StrList reportNames;

void IterationResults(const OME_SCALAR & iteration, Model * model)
{
	CString msg;
	msg.Format("Model: %s, Time: %.2f", model->GetName().c_str(), iteration);
	DBG_PRINT(msg.GetString());

	DBG_PUSHLEVEL();

	OMEDBG_COLOR lastColor;
	DBG_GETTXCOLOR(lastColor);

	DBG_SETTXCOLOR(DBG_CYAN);
	size_t m, i;
	for (m = 0; m<model->GetActiveInstanceCount(); m++)
	{
		model->SetCurrentInstance(m);
		msg.Format("Inst %i:", m + 1);
		DBG_PRINTC(msg.GetString(), DBG_YELLOW);

		DBG_PRINT("STATES");
		DBG_PUSHLEVEL();

		for (i = 0; i<model->GetStateVarCount(); i++)
		{
			StateVar* pSv = model->GetStateVar(i);
			msg.Format("Name: %s\tValue: %f", pSv->GetName().c_str(), pSv->GetValue());
			DBG_PRINT(msg.GetString());
		}

		DBG_POPLEVEL();
		DBG_PRINT("VARIABLES");
		DBG_PUSHLEVEL();

		VarArray* pVa;
		for (i = 0; i<model->GetVarCount(); i++)
		{
			Variable* pVar = model->GetVariable(i);
			msg.Format("Name: %s\tValue: ", pVar->GetName().c_str());
			if (pVar->GetOMEType() == OME_VARARRAY)
			{
				pVa = (VarArray*)pVar;
				msg.Append("{");

				for (int a = 0; a<pVa->GetSize(); a++)
					msg.AppendFormat(" %f ", (*pVa)[a]);
				msg.Append("}");
			}
			else
				msg.AppendFormat("%f", pVar->GetValue());

			DBG_PRINT(msg.GetString());
		}

		DBG_POPLEVEL();
		DBG_PRINT("FLOWS");
		DBG_PUSHLEVEL();

		for (i = 0; i<model->GetFlowCount(); i++)
		{
			Flow* pFlow = model->GetFlow(i);
			msg.Format("Name: %s\tValue: %f", pFlow->GetName().c_str(), pFlow->GetValue());
			DBG_PRINT(msg.GetString());
		}

		DBG_POPLEVEL();
		DBG_PRINT("ITERCONDITIONALS");
		DBG_PUSHLEVEL();

		for (i = 0; i<model->GetIterConditionalCount(); i++)
		{
			IterConditional* pIc = model->GetItrCond(i);
			msg.Format("Name: %s\tValue: %f", pIc->GetName().c_str(), pIc->GetValue());
			DBG_PRINT(msg.GetString());
		}
		DBG_POPLEVEL();

		for (int i = 0; i<model->GetSubmodelCount(); i++)
		{
			IterationResults(iteration, model->GetSubmodel(i));
		}
	}
	model->SetCurrentInstance(0);
	DBG_POPLEVEL();

	DBG_SETTXCOLOR(lastColor);
}

bool LoadReportList(StrList & sl, STLString & path)
{
	STLString file = "\\reportNames.txt";
	STLString output = path + file;

	std::ifstream inFile(output);
	STLString temp;
	bool ret = inFile.good();
	while (inFile >> temp)
		sl.push_back(temp);

	return ret;
}
void SimResults(const OME_SCALAR & iteration, BaseManager * mngr, void* extra)
{

	if (!reportNames.empty())
	{
		oSTLSStream outstrm;
		Model* pRoot = mngr->GetRootModel();
		Evaluable* pEval;
		CString msg;
		msg.Format("Time %.3f,(iteration %.3f):", iteration, iteration / mngr->GetInterval());
		DBG_PRINT(msg.GetString());
		DBG_PUSHLEVEL();
		for (auto itr = reportNames.begin(); itr != reportNames.end(); ++itr)
		{
			pEval = dynamic_cast<Evaluable*>(pRoot->ObjectForName(*itr, true));
			DBG_COLOR(DBG_PARTIAL_PRINT((*itr) + ":\t"), DBG_CYAN);

			MIQuery getInfo(pEval, pEval->GetModelIndex());
			if (pEval)
			{
				MIValue res = pEval->GetParentModel()->GetInstance(0)->ValForQuery(getInfo);

				if (!res.IsSingle())
				{
					msg = "{";
					for (int i = 0; i<res.GetValCount(); i++)
						msg.AppendFormat(" %f", res.ValAtInd(i));
					msg.Append("}");
				}
				else
					msg.Format("%f", res.SingleVal());
				MIValue::ClearValCache();
			}
			else
				msg.Format("Value not found");
			MIValue::ClearValCache();
			DBG_PRINTC(msg.GetString(), DBG_WHITE);
		}
		DBG_POPLEVEL();
	}
	else if (mngr->GetModelReportFunction())
		mngr->GetModelReportFunction()(iteration, mngr->GetRootModel());

}


int _tmain(int argc, _TCHAR* argv[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);


	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{

			//the following variables will be filled in with macroes which will be populated by ModelClassBuilder
			const unsigned short mgrFlags = SimManager::IS_SINGLETON | SimManager::NO_DEBUG | SimManager::NO_SERIALIZING;

			OMEIntegrationBlock::INT_METHOD intMethodArg = RUN_INTMETHOD;	//sub
			LoadReportList(reportNames, STLString("."));
			//TestModel will be replaced by macro name
			COMP_TYPE compiledRep;

			SimManager sMngr(&compiledRep, "./", RUN_START, RUN_STOP, RUN_INTERVAL, RUN_STEPSIZE, mgrFlags);
			sMngr.SetIntMethod(intMethodArg);

			//sdp info?
#ifdef SDP_FILE
			CSVSpatialDataProvider* pSdp = new CSVSpatialDataProvider(SDP_FILE, SDP_DELIM, SDP_XCOORD, SDP_YCOORD);	//sub
			sMngr.SetSpatialDataProvider(pSdp,true);
			LuaOMEParser csvPrsr(sMngr.GetRootModel());
			csvPrsr.LoadSDPFuncs(pSdp);

			pSdp->SetNeighborExpr(SDP_SCRIPT, SDP_LANG, &csvPrsr, SDP_MODE, (void*)SDP_EXTRA);
#endif
			try
			{
				sMngr.SetModelReportFunction(IterationResults);
				sMngr.SetManagerReportFunction(SimResults);
				DBG_PRINTBIGDIV();
				DBG_LOGFILE("TestModel Run log.txt");
				DBG_TIMESTAMP();

				DBG_PRINT((STLString("Running \"") + STLString(compiledRep.GetCompiledName()) + "\""));

				sMngr.EnableFileCache("./");
				DBG_PRINT(("Integration Method: " + CString(sMngr.GetIntMethod() == OMEIntegrationBlock::IM_RK4 ? "rk4" : "euler")).GetString());
				sMngr.PreloadSubmodels();

				clock_t start = clock();
				sMngr.RunSimulation();
				clock_t stop = clock();

				cout << "Total time (s): " << (stop - start) / CLOCKS_PER_SEC << endl;
			}
			catch (std::exception & e)
			{
				cout << "Exception! " << e.what() << endl;
			}
			catch (CMemoryException* ce)
			{
				sMngr.ClaimReserveMem();
				const size_t MSG_SIZE = 2048;
				TCHAR msg[MSG_SIZE];
				ce->GetErrorMessage(msg, MSG_SIZE);
				DBG_COLOR(cout << msg << endl, DBG_RED);
				ce->Delete();
#ifndef WIN64
				DBG_COLOR(cout << "If this problem persists, try running the model in a 64-bit OME build." << endl, DBG_RED);
#endif
			}

		}
		DBG_PRINT("Done");
		system("pause");
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return 0;
}

#undef COMP_TYPE
#undef RUN_INTMETHOD
#undef RUN_START
#undef RUN_STOP
#undef RUN_INTERVAL
#undef RUN_STEPSIZE
#undef SDP_FILE
#undef SDP_DELIM
#undef SDP_XCOORD
#undef SDP_YCOORD
#undef SDP_SCRIPT
#undef SDP_LANG
#undef SDP_MODE
#undef SDP_EXTRA