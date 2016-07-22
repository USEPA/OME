// TestConsole.cpp : Defines the entry point for the console application.
//

#include "omeobjects/SimObjs.h"
#include "OMEDefines.h"
#include "DebugOutput.h"
#include "CPPSerializer.h"
#include "SimManager.h"
#include "MainUtils.h"

#include <iostream>
#include <fstream>

static const char HELP_CODE = 'h';
static const char INTERACTIVE_CODE = 'i';
static const char QUIET_CODE = 'q';
static const char OUTDIR_CODE = 'o';
static const char CLASS_CODE = 'c';

void PrintHelp();

int main(int argc, const char* argv[])
{
	using namespace std;
	using namespace MainUtils;
	MODE mode = DEFAULT;

	STLString inPath;
	STLString outDir;
	STLString className;

	if (argc <= 1)
	{
		PrintHelp();
		DoExit(mode);
	}

	for (int i = 1; i < argc; ++i)
	{
		if (argv[i][0] == '-')
		{
			char code = argv[i][1];
			switch (code)
			{
			case INTERACTIVE_CODE:
				mode = INTERACTIVE;
				break;

			case HELP_CODE:
				PrintHelp();
				DoExit(mode);

				break;
			case QUIET_CODE:
				mode = QUIET;
				break;

			case OUTDIR_CODE:
				outDir = STLString(&argv[i][2]);
				break;
			case CLASS_CODE:
				className = STLString(&argv[i][2]);
				break;
			default:
				PrintError(mode, STLString("Unrecognized code: ") + argv[i]);
				break;
			}
		}
		else
		{
			if (inPath.empty())
				inPath = STLString(argv[i]);
			else
			{
				PrintError(mode, STLString("Unknown Parameter: ") + argv[i]);
			}
		}
	}

	if (mode == QUIET)
		DBG_DISABLE();
	if (inPath.empty())
	{
		if (mode == INTERACTIVE)
		{
			cout << "Input path to Model File (.omem):" << endl << "> ";
			getline(cin, inPath);
		}
		else
			PrintError(mode, "No input path provided. Exiting.", true);
	}

	if (outDir.empty())
	{
		if (mode == INTERACTIVE)
		{
			cout << "Where would you like to save the output files (leave blank for default)?" << endl << "> ";
			getline(cin, outDir);
		}
	}

	if (className.empty())
	{
		if (mode == INTERACTIVE)
		{
			cout << "What class name would you like to use (leave blank for default)?" << endl << "> ";
			getline(cin, className);
		}
	}

	//don't ask for tmplDir in interactive mode.

	/*TI2Document doc;
	
	if (doc.LoadFile(inPath.c_str()) == tinyxml2::XML_NO_ERROR)
	{
*/
		const float start = 0.0;
		const float stop = 5.0;
		const float interval = 1.0;
		const float stepsize = 1.0;
		unsigned short flags = SimManager::NO_DEBUG | SimManager::NO_PARSING |
			SimManager::NO_SERIALIZING | SimManager::NO_COMPILED;
		if (inPath.substr(inPath.find('.')) != STLString(OME_EXT_CTL))
			flags |= SimManager::NO_SDP | SimManager::NO_COVERAGE;
		SimManager sMngr(inPath, start, stop, interval, stepsize, flags);
		//try
		//{
		DBG_PRINTBIGDIV();
		//DBG_LOGFILE("OME Test log.txt");
		DBG_TIMESTAMP();

		//grab model here
		//produce cpp file using serializer
		CPPSerializer serializer;

		if (outDir.empty())
			outDir = OMEGetWorkingDirectory();

		if (className.empty())
			className = sMngr.GetRootModel()->GetExprName();

		clock_t timeStart = clock();
		//retain original working driectory
		if (chdir(outDir.c_str()))
			PrintError(mode, STLString("The output directory ") + outDir + " could not be reached. Aborting.", true);
		serializer.WriteToCPPFile(sMngr.GetRootModel(), className,outDir, sMngr.GetSpatialDataProvider());
		clock_t timeStop = clock();
		//exit

		cout << "Total time (s): " << (timeStop - timeStart) / CLOCKS_PER_SEC << endl;
	//}
	/*else
	{
		const char* msgBuff = doc.GetErrorStr1();
		STLString outMsg = STLString();
		if (msgBuff)
			outMsg += msgBuff;
		msgBuff = doc.GetErrorStr2();
		if (msgBuff)
		{
			if (!outMsg.empty())
				outMsg += ": ";
			outMsg += msgBuff;
		}
		PrintError(mode,outMsg,true);
	}*/

	DoExit(mode);
}

void PrintHelp()
{
	using namespace std;

	const char* desc =
		"ModelClassBuilder [-h -i -q -c<name> -o<dir> -t<dir>] <infile>\n"
		"Description:\n"
		"    Takes an OME model and creates a compile CPP class representing the model logic.\n\n"
		"Options:\n"
		"    -h:  Print this message.\n"
		"    -i:  Interactive command prompt. When set, ModelClassBuilder\n"
		"         will ask for arguments.\n"
		"    -q:  Quiet mode. Universal Converter will not display any text and\n"
		"         will immediately exit on completion; ignores -i flag.\n"
		"    -c:  The name of the newly generated class. If omitted, uses the root\n"
		"         model name.\n"
		"    -o:  Output directory location. Uses the current working directory if omitted.\n"
		"\nArguments:\n"
		" <infile>    Name of the model to process; can be either a .omem or .omec file.\n";

	cout << desc << endl;
}

