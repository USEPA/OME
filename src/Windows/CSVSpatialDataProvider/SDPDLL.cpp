// SDPDLL.cpp : Defines the exported functions for the DLL application.
//

#include "SDPDLL.h"
#include "CSVSpatialDataProvider.h"
#include "LuaOMEParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern "C" ISpatialDataProvider* PASCAL EXPORT GenSDP(SimManager* pSim,Model* pRoot, TI2Element* pSDPElem, const char* modelPath)
{
	//return a new instance of the target model 
	//pick and choose arguments for specified 
	STLString spPath = pSDPElem->Attribute("filepath");
	STLString xLbl = pSDPElem->Attribute("x_label");
	STLString yLbl = pSDPElem->Attribute("y_label");
	char delim = ',';

	STLString covStr = pSDPElem->Attribute("coverage_format");
	int covType = 0;
	if (covStr.find("grid") != STLString::npos)
		covType |= GRID_SUPPORT;

	if (covStr.find("hex") != STLString::npos)
		covType |= GRID_SUPPORT | HEX_SUPPORT;

	if (covStr.find("poly") != STLString::npos)
		covType |= POLY_SUPPORT;


	const char* delimCheck = pSDPElem->Attribute("delim");

	if (delimCheck)
		delim = delimCheck[0];

	CSVSpatialDataProvider* pProvider = NULL;
	try
	{
		pProvider = new CSVSpatialDataProvider(PrepPath(spPath, modelPath).c_str(),delim, xLbl, yLbl);
	}
	catch (exception* pE)
	{
		pSim->ReportError(pE->what());
			return NULL;
	}

	pProvider->SetCoverageType(covType);

	LuaOMEParser csvParser(pRoot);
	csvParser.LoadSDPFuncs(pProvider);

	TI2Element *pXmlExpr = pSDPElem->FirstChildElement("poly_expr");
	if (pXmlExpr)
	{
		STLString polyCol = pXmlExpr->Attribute("poly_label");
		STLString partCol = pXmlExpr->Attribute("part_label");
		pProvider->SetPolyCols(polyCol, partCol);

		/// \todo add support for script override

		pProvider->RefreshPolys();

	}

	//set expression after sdp is registered with manager, so as to
	pXmlExpr = pSDPElem->FirstChildElement("neighbor_expr");
	if (pXmlExpr)
	{
		CSVSpatialDataProvider::NEIGHMODE nMode = CSVSpatialDataProvider::NM_NONE;
		CSVSpatialDataProvider::GRIDMODE gMode = CSVSpatialDataProvider::GM_NONE;

		const char* scriptPath = pXmlExpr->Attribute("script");
		STLString ne;

		if (scriptPath)
		{
			std::ifstream inFile(PrepPath(scriptPath, modelPath));
			if (inFile.good())
			{
				oSTLSStream strmTemp;
				strmTemp << inFile.rdbuf();
				ne = strmTemp.str();
			}
			else
			{
				pSim->ReportError(STLString("File ") + scriptPath + " failed to open.");
				return NULL;
			}
		}
		else
			ne = STLString(pXmlExpr->GetText());

		const char* mode = pXmlExpr->Attribute("mode");
		const char* modeArg = pXmlExpr->Attribute("extra");
		const char* lang = pXmlExpr->Attribute("language");
		if (mode)
		{
			if (!strcmp(mode, "compare_all"))
				nMode = CSVSpatialDataProvider::NM_COMPARE_ALL;
			else if (!strcmp(mode, "compare_xy"))
				nMode = CSVSpatialDataProvider::NM_COMPARE_XY;
			else if (!strcmp(mode, "use_column"))
				nMode = CSVSpatialDataProvider::NM_USE_COLUMN;
		}
	


		const char* grid = pXmlExpr->Attribute("grid_type");
		if (grid)
		{
			if (!strcmp(grid, "grid4"))
				gMode = CSVSpatialDataProvider::GM_4GRID;
			else if (!strcmp(grid, "grid8"))
				gMode = CSVSpatialDataProvider::GM_8GRID;
			else if (!strcmp(grid, "hex"))
				gMode = CSVSpatialDataProvider::GM_HEX;
		}
		pProvider->SetGridMode(gMode);

		if (!ne.empty())
		{
			DBG_PRINT(STLString("Determining Neighbor relationships for '") + mode + '\'');
			pProvider->SetNeighborExpr(ne, lang, &csvParser, nMode, (void*)modeArg);
		}
	}

	return pProvider;
}

extern "C" void PASCAL EXPORT DisposeSDP(ISpatialDataProvider* pSDP)
{
	//cleanup sdp passed out from the dll.
	//default handler
	delete pSDP;
}

extern "C" const char* PASCAL EXPORT GetSDPLabel()
{
	//label identifier for SDP module
	return "CSV";
}
//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CModelProjectApp

BEGIN_MESSAGE_MAP(CModelProjectApp, CWinApp)
END_MESSAGE_MAP()


// CModelProjectApp construction

CModelProjectApp::CModelProjectApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CModelProjectApp object

CModelProjectApp theApp;


// CModelProjectApp initialization

BOOL CModelProjectApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
