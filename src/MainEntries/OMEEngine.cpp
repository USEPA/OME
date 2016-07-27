#include "CharType.h"
#include "ValTypes.h"

#include "omeobjects/SimObjs.h"
#include "DebugOutput.h"
#include "SimManager.h"
#include "FileExts.h"
#include "MainUtils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>

using namespace std;

static StrList reportNames;
MainUtils::MODE mode;

static const char IM_CODE = 's';
static const char REPORT_CODE = 'r';
static const char INTERACTIVE_CODE = 'i';
static const char FULL_CSV_CODE = 'f';
static const char CSV_CODE = 'c';
static const char QUIET_CODE = 'q';
static const char LOG_CODE = 'l';
static const char HELP_CODE = 'h';
static const char OVERRIDE_CODE = 'o';


void PrintHelp();
void IterationResults(const OME_SCALAR & iteration, Model * model);
bool LoadReportList(StrList & sl, STLString & path);
void SimResults(const OME_SCALAR & iteration, BaseManager * mngr, void* extra);
bool ParseParam(ParamArray & params, const OMEChar* assignment);

int main(int argc, const char* argv[])
{
	using namespace MainUtils;
	MODE mode = DEFAULT;
	bool fullReport = false;
	OMEIntegrationBlock::INT_METHOD intMethodArg = OMEIntegrationBlock::IM_UNDEF;
	ParamArray userParams;

	if (argc <= 1)
	{
		PrintHelp();
		DoExit(mode);
	}

	//increase console scrollback buffer size
	//HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//COORD bSize;
	//CONSOLE_SCREEN_BUFFER_INFO csbi;
	//GetConsoleScreenBufferInfo(hConsole, &csbi);
	//bSize.X = csbi.dwSize.X;
	//bSize.Y = 9999;
	//SetConsoleScreenBufferSize(hConsole, bSize);


	STLString filename;
	STLString intStr;
	STLString reportNamePath;
	STLString resultsPath;
	STLString logPath;

	//parse args
	for (int i = 1; i < argc; ++i)
	{
		if (argv[i][0] == '-')
		{
			char code = argv[i][1];
			switch (code)
			{
			case IM_CODE:
				intStr = ToLowerCase(STLString(argv[i] + 2));
				
				break;
			case HELP_CODE:
				PrintHelp();
				DoExit(mode);
				break;
			case REPORT_CODE:
				reportNamePath = argv[i] + 2;
				break;
			case FULL_CSV_CODE:
				fullReport = true;
			case CSV_CODE:
				resultsPath = argv[i] + 2;
				break;
			case QUIET_CODE:
				mode = QUIET;
				break;
			case INTERACTIVE_CODE:
				mode = INTERACTIVE;
				break;
			case LOG_CODE:
				logPath = argv[i] + 2;
				break;
			case OVERRIDE_CODE:
				if(!ParseParam(userParams, argv[i] + 2))
					PrintError(mode, STLString("Parsing failed: ") + argv[i]);
				break;
			default:
				PrintError(mode, STLString("Unrecognized code: ") + argv[i]);
				break;
			}
		}
		else
		{
			if (filename.empty())
				filename = argv[i];
			else
				PrintError(mode, STLString("Unknown Parameter: ") + argv[i]);
		}
	}


	if (filename.empty())
	{
		if (mode == INTERACTIVE)
		{
			cout << "Input path to Model File (.omem) or Control File (.omec):" << endl << "> ";
			getline(cin, filename);
		}
		else
			PrintError(mode, "No input path provided. Exiting.", true);
	}

	TI2Document doc;

	if (!filename.empty())
	{
		if (intStr == "rk4")
			intMethodArg = OMEIntegrationBlock::IM_RK4;
		else if (intStr == "euler")
			intMethodArg = OMEIntegrationBlock::IM_EULER;

		if (!reportNamePath.empty())
			LoadReportList(reportNames, reportNamePath);

		const float start = 0.0;
		const float stop = 5.0;
		const float interval = 1.0;
		const float stepsize = 1.0;
		unsigned int smFlags = SimManager::IS_SINGLETON;
		if (mode == QUIET)
			smFlags |= SimManager::NO_DEBUG | SimManager::NO_PRINT;
		SimManager sMngr(filename, start, stop, interval, stepsize);
		//try
		//{
			if (mode != QUIET)
			{
				sMngr.SetModelReportFunction(IterationResults);
				sMngr.SetManagerReportFunction(SimResults);
				DBG_PRINTBIGDIV();
				if (!logPath.empty())
					DBG_LOGFILE(logPath);
			}
			DBG_TIMESTAMP();
#ifdef OME_SINGLE_PRECISION
			DBG_PRINT("Precision: Single");
#else
			DBG_PRINT("Precision: Double");
#endif

			DBG_PRINT((STLString("Running \"") + filename + "\""));
			if (intMethodArg)
				sMngr.SetIntMethod(intMethodArg);

			sMngr.EnableFileCache();
			sMngr.SetMinimalRecords(!fullReport);
			STLString sTypeStr;
			switch (sMngr.GetSolverType())
			{
			case SimManager::HIRES:
				sTypeStr = "High Resolution";
				break;
			default:
				sTypeStr = "Default";
			}
			DBG_PRINT(("Solver: " + sTypeStr));
			DBG_PRINT((STLString("Integration Method: ") + (sMngr.GetIntMethod() == OMEIntegrationBlock::IM_RK4 ? "rk4" : "euler")));
			sMngr.PreloadSubmodels();

			for (Param & p : userParams)
			{
				if (!sMngr.AddUserParameter(p))
					PrintError(mode, STLString("Parameter assignment for ") + p.GetTargetName() + " failed; model component not found");
			}
			
			clock_t startTime = clock();
			sMngr.RunSimulation();
			clock_t stopTime = clock();

			cout << "Total time (s): " << (stopTime - startTime) / CLOCKS_PER_SEC << endl;

			if (!resultsPath.empty())
			{
				bool result = fullReport ? sMngr.ExportAllValuesToCSV(resultsPath) : sMngr.ExportLoadedValuesToCSV(resultsPath);
				if (!result)
				{
					PrintError(mode, STLString("Error: Failed to save results at '") + resultsPath + "'", true);
				}
			}

		DoExit(mode);
	}

}

void PrintHelp()
{
	using namespace std;

	const char* desc =
		"OMEEngine [-h -i -q -s<euler|rk4> -r<name> -f<name> -l<name> -c<name> -o<assign>] <infile> \n"
		"Description:\n"
		"    Execute OME Model, optionally printing out and/or saving results to disk.\n\n"
		"Options:\n"
		"    -h:  Print this message.\n"
		"    -i:  Interactive command prompt. When set, OMEEngine\n"
		"         will ask for arguments.\n"
		"    -q:  Quiet mode. Universal Converter will not display any text and\n"
		"         will immediately exit on completion; ignores -i flag.\n"
		"    -s:  Integration solving method. The presently supported options are\n"
		"         \"euler\" and \"rk4\".\n"
		"    -r:  Report only a specific subset of values to the command prompt.\n"
		"         The argument passed should be the path to a text file containing a\n"
		"         whitespace-delimited list of names of variables to include.\n"
		"    -f:  Export the results from all timesteps as a .csv to the provided path.\n"
		"    -l:  Write simulation output to a file located at the provided path.\n"
		"    -c:  Export the results of the final timestep as a .csv to the provided path.\n"
		"    -o:  Override an internal variable with a user supplied value; flag can appear\n"
		"         multiple times. The for <assign> is <full_model_var_path>=<number>\n"
		"\nArguments:\n"
		" <infile>    Name of the model to process; can be either a .omem or .omec file. The .omec (control) file"
		"			  is used to map DLL's  \n";

	cout << desc << endl;
}

void IterationResults(const OME_SCALAR & iteration, Model * model)
{

	oSTLSStream msgBuff;
	msgBuff << "Model " << model->GetName() << "Time: " << std::setprecision(2) << iteration;
	DBG_PRINTC(msgBuff.str(), DBG_DEFAULT);

	DBG_PUSHLEVEL();

	OMEDBG_COLOR lastColor;
	DBG_GETTXCOLOR(lastColor);

	DBG_SETTXCOLOR(DBG_CYAN);
	size_t m, i;
	for (m = 0; m < model->GetActiveInstanceCount(); m++)
	{
		model->SetCurrentInstance(m);
		if (model->GetCurrentInstanceRef()->IsActive())
		{
			DBG_PRINTC(STLString("Inst ") + std::to_string((int)m + 1) + ": ", DBG_YELLOW);
			DBG_PRINT("STATES");
			DBG_PUSHLEVEL();

			for (i = 0; i < model->GetStateVarCount(); i++)
			{
				StateVar* pSv = model->GetStateVar(i);
				DBG_PRINT(STLString("Name: ") + pSv->GetName() + "\tValue: " + std::to_string(pSv->GetValue()));
			}

			DBG_POPLEVEL();
			DBG_PRINT("VARIABLES");
			DBG_PUSHLEVEL();

			VarArray* pVa;
			for (i = 0; i < model->GetVarCount(); i++)
			{
				Variable* pVar = model->GetVariable(i);
				msgBuff.clear();
				msgBuff.str(STLString());
				msgBuff << "Name: " << pVar->GetName() << "\tValue: ";

				if (pVar->GetOMEType() == OME_VARARRAY)
				{
					pVa = (VarArray*)pVar;
					msgBuff << '{';

					for (int a = 0; a < pVa->GetSize(); a++)
						msgBuff << (*pVa)[a] << endl;
					msgBuff << '}';
				}
				else
					msgBuff << pVar->GetValue();

				DBG_PRINT(msgBuff.str());
			}

			DBG_POPLEVEL();
			DBG_PRINT("FLOWS");
			DBG_PUSHLEVEL();

			for (i = 0; i < model->GetFlowCount(); i++)
			{
				Flow* pFlow = model->GetFlow(i);
				DBG_PRINT(STLString("Name: ") + pFlow->GetName() + "\tValue: " + std::to_string(pFlow->GetValue()));
			}

			DBG_POPLEVEL();
			DBG_PRINT("ITERCONDITIONALS");
			DBG_PUSHLEVEL();

			for (i = 0; i < model->GetIterConditionalCount(); i++)
			{
				IterConditional* pIc = model->GetItrCond(i);
				DBG_PRINT(STLString("Name: ") + pIc->GetName() + "\tValue: " + std::to_string(pIc->GetValue()));
			}
			DBG_POPLEVEL();

			for (int i = 0; i < model->GetSubmodelCount(); i++)
			{
				IterationResults(iteration, model->GetSubmodel(i));
			}
		}
		else
		{
			//DBG_COLOR(DBG_PARTIAL_PRINT(msg.GetString()), DBG_YELLOW);
			DBG_PRINTC(" DEAD", DBG_RED);
		}
	}
	model->SetCurrentInstance(0);
	DBG_POPLEVEL();

	DBG_SETTXCOLOR(lastColor);
}

bool LoadReportList(StrList & sl, STLString & path)
{
	std::ifstream inFile(path);
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
		//use string stream so precision can be set
		oSTLSStream outBuff;
		outBuff.precision(OME_WRITE_PRECISION);

		oSTLSStream outstrm;
		Model* pRoot = mngr->GetRootModel();
		Evaluable* pEval;
		outBuff << "Time " << std::setprecision(3) << iteration << ",(iteration "
			<< std::setprecision(3) << iteration / mngr->GetInterval() << "):";
		DBG_PRINT(outBuff.str());
		DBG_PUSHLEVEL();
		for (auto itr = reportNames.begin(); itr != reportNames.end(); ++itr)
		{
			pEval = dynamic_cast<Evaluable*>(pRoot->ObjectForExprName(*itr, true));
			DBG_COLOR(DBG_PARTIAL_PRINT((*itr) + ":\t"), DBG_CYAN);

			if (pEval)
			{
				Model* pMdl = pEval->GetParentModel();
				OME_SCALAR temp;
				/// @todo continue rewrite for valueArchive approach
				switch (pEval->GetOMEType())
				{
				case OME_VARARRAY:
					for (size_t i = 0; i < pMdl->GetNumRawInstances(); ++i)
					{
						outBuff.clear();
						outBuff.str(STLString());
						if (i)
							DBG_COLOR(DBG_PARTIAL_PRINT(to_string(i + 1) + ":\t"), DBG_DK_CYAN);
						if (pMdl->GetRawInstance(i)->IsActive())
						{
							size_t len = ((VarArray*)pEval)->GetSize();
							outBuff << '{'<<std::setprecision(4);
							for (size_t j = 0; j < len; ++j)
							{
								pMdl->GetValForRawInstIndex(i, pEval->GetModelIndex() + j, len, temp);
								outBuff << ' ' << temp;
							}
							outBuff << " }";

						}
						else
							outBuff << "Inactive";

						DBG_PRINTC(outBuff.str(), DBG_WHITE);
					}
					break;
				default:
					for (size_t i = 0; i < pMdl->GetNumRawInstances(); ++i)
					{
						outBuff.clear();
						outBuff.str(STLString());
						outBuff << std::setprecision(4);
						if (i)
							DBG_COLOR(DBG_PARTIAL_PRINT(to_string(i + 1) + ":\t"), DBG_DK_CYAN);
						temp = pEval->GetValue(i);
						if (pMdl->GetRawInstance(i)->IsActive())
						{
							outBuff << ' ' << temp;
						}
						else
							outBuff << "Inactive";
						DBG_PRINTC(outBuff.str(), DBG_WHITE);
					}
				}
			}
			else
			{
				DBG_PRINTC("Value not found", DBG_WHITE);
			}
		}
		DBG_POPLEVEL();
	}
	else if (mngr->GetModelReportFunction())
		mngr->GetModelReportFunction()(iteration, mngr->GetRootModel());

}

bool ParseParam(ParamArray & params, const OMEChar* assignment)
{
	try
	{
		StrArray parts=SplitString(STLString(assignment), '=');
		//ensure we are assigning a single value
		if (parts.size() != 2)
			return false;

		params.emplace_back(parts[0], OME_SCLR_READSTR(parts[1]));

	}
	catch (...)
	{
		//something went wrong (probably a bad scalar reading)
		return false;
	}
	return true;
}