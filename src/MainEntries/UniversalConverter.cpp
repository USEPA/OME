// UniversalConverter.cpp : Defines the entry point for the console application.
//

#include "omeobjects/SimObjs.h"
#include "FileExts.h"
#include "DrawDetails.h"
#include "DebugDefines.h"
#include "XMILEHandlers/XMILENodeProcessor.h"
#include "MDLHandlers/MDLParser.h"
#include "SMLHandlers/PrologParser.h"
#include "SMLHandlers/SmlParser.h"

#include "MainUtils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

static const char HELP_CODE = 'h';
static const char INTERACTIVE_CODE = 'i';
static const char PARAM_CODE = 'p';
static const char QUIET_CODE = 'q';

//protos
void PrintHelp();

void ProcessParamFile(const STLString & entry);
void ProcessParamFile(const STLString & modelName,const STLString & paramPath);


//globals
MainUtils::MODE mode;
std::map<STLString, STLString> paramPaths;

//main
int main(int argc, OMEChar* argv[])
{
	using namespace std;
	using namespace MainUtils;

	STLString inPath;
	STLString outPath;

	mode = DEFAULT;

	DBG_PRINTC("Starting up Conversion tool...", DBG_CYAN);

	if (argc <= 1)
		PrintHelp();

	for (int i = 1; i < argc; ++i)
	{
		if (argv[i][0] == '-')
		{
			char code = argv[i][1];
			switch (code)
			{
			case PARAM_CODE:
				ProcessParamFile(&argv[i][2]);
				break;
			case INTERACTIVE_CODE:
				mode = INTERACTIVE;
				break;

			case HELP_CODE:
				PrintHelp();
				DoExit(mode);
				
				break;
			case QUIET_CODE:
				mode=QUIET;
				break;
			default:
				PrintError(mode,STLString("Unrecognized code: ") + argv[i]);
				break;
			}
		}
		else
		{
			if (inPath.empty())
				inPath = STLString(argv[i]);
			else if (outPath.empty())
				outPath = STLString(argv[i]);
			else
			{
				PrintError(mode,STLString("Unknown Parameter: ")+argv[i]);
			}
		}
	}

	if (mode == QUIET)
		DBG_DISABLE();
	if (inPath.empty())
	{
		if (mode == INTERACTIVE)
		{
			cout << "Input path to source File:" << endl << "> ";
			getline(cin,inPath);
		}
		else
			PrintError(mode,"No input path provided. Exiting.",true);
	}

	if (outPath.empty())
	{
		if (mode == INTERACTIVE)
		{
			cout << "Where would you like to save the output file?" << endl << "> ";
			getline(cin, outPath);
		}
		else
			PrintError(mode,"No output path provided.Exiting.",true);
	}

	if (mode == INTERACTIVE)
	{
		bool getParams = false;
		STLString buff;
		cout << "Would you like to enter any parameter files?" << endl << "y/N> ";
		getline(cin, buff);
		buff = LTrimString(buff);
		if (!buff.empty() && buff[0] == 'y' || buff[0] == 'Y')
			getParams = true;

		while (getParams)
		{
			STLString mdlName;
			STLString paramPath;
			cout << "What is the name of the submodel, if any?" << endl << "> ";
			getline(cin, mdlName);
			cout << "Where is the parameter file?" << endl << "> ";
			getline(cin, paramPath);
			ProcessParamFile(mdlName, paramPath);
			cout << "Would you like to enter another parameter file?" << endl << "y/N> ";
			getline(cin, buff);
			if (buff.empty() || (buff[0] != 'y' && buff[0] != 'Y'))
				getParams = false;
		}
	}
	//select the Parser based on extension
	ModelFileParser* pMdlPrsr = NULL;
	STLString ext = ToLowerCase(GetExtension(inPath));
	STLString sourceName;
	if (ext == "sml")
	{
		pMdlPrsr = new SmlParser();
		sourceName = "Simile";
	}
	else if (ext == "pl")
	{
		pMdlPrsr = new PrologParser();
		sourceName = "Simile";
	}
	else if (ext == "mdl")
	{
		pMdlPrsr = new MDLParser();
		sourceName = "Vensim";
	}
	else if (ext == "xml")
	{
		pMdlPrsr = new XMILENodeProcessor();
		sourceName = "XMILE";
	}
	else
		PrintError(mode,STLString("Unrecognized file extension: ") + ext, true);

	DBG_PRINTC("Converting " + sourceName + "-type model...", DBG_GREEN);
	Model* pMdl = pMdlPrsr->ParseModelFromFile(inPath);
	delete pMdlPrsr;

	if (pMdl == NULL)
		PrintError(mode,"There was a problem loading the model.", true);

	DBG_PRINT("Writing to " + outPath);
	ofstream outFile(outPath);
	outFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl << "<ome>" << std::endl;
	//add parameter files
	for (auto & entry : paramPaths)
	{
		outFile << "<param_file filepath='" << entry.second << "' ";
		if (!entry.first.empty())
			outFile << "target_model_path='" << entry.first << "' ";
		outFile << "/>" << endl;
	}

	DrawDetails* pDD = DrawDetails::DDForObject(pMdl);
	STLString body = pDD->GetXMLRepresentation();
	outFile << body << std::endl << "</ome>" << std::endl;
	outFile.close();

	
	DoExit(mode);
}

void ProcessParamFile(const STLString & entry)
{
	if (entry.find("::") != STLString::npos)
	{
		StrArray parts = SplitString(entry, "::");
		ProcessParamFile(parts[0], parts[1]);
	}
	else
		ProcessParamFile("", entry);
}

void ProcessParamFile(const STLString & modelName, const STLString & paramPath)
{
	paramPaths[modelName] = paramPath;
}

void PrintHelp()
{
	using namespace std;

	const char* desc =
		"UniversalConverter [-h -i -p[<submodel>:]<filepath> -q] <infile> <outfile> \n"
		"Description:\n"
		"    Converts models from other other tools to OME .omem files.\n\n"
		"Options:\n"
		"    -h:  Print this message.\n"
		"    -i:  Interactive command prompt. When set, UniversalConverter\n"
		"         will ask for arguments.\n"
		"    -p:  Parameter file entry. Used to bind a parameter file to a\n"
		"         a sub model; arguments are passed in as a submodel::filepath\n"
		"         pair; if the submodel is omitted, the parameter file will be\n"
		"         paired with the root model.\n"
		"         Supported file types:\n"
		"               spf (Simile)\n"
		"               csv\n"
		"    -q:  Quiet mode. Universal Converter will not display any text and\n"
		"         will immediately exit on completion; ignores -i flag.\n\n"
		"Arguments:\n"
		" <infile>  Name of the model to convert; the appropriate conversion will be\n"
		"           applied base on the file extension.\n"
		"           Supported file types:\n"
		"              sml (Simile)\n"
		"              pl  (Simile)\n"
		"              mdl (Vensim)\n"
		"              xml (XMILE)\n"
		" <outfile> Name to apply to output file.\n";
	cout << desc << endl;
}