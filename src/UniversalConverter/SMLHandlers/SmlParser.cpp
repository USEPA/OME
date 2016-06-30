//#include "StdAfx.h"
#include "SMLHandlers/SmlParser.h"
#include "DebugOutput.h"
#include <sstream>

Model* SmlParser::ParseModelFromFile(const STLString & path)
{
	const char* MODEL_KEY="Simile model";


	DBG_PRINT(STLString("Loading file: ") + path);

	ParseFile(path);

#ifdef OME_CONV_DUMP_FILES
	KeyList keys=GetKeys();
	STLString buff;
	for(auto itr=keys.begin(); itr!=keys.end(); ++itr)
	{
		ofstream outFile("DUMP-"+(*itr));
		buff=GetDataForKey(*itr);
		outFile<<buff<<endl;
		outFile.close();
	}
#endif
	DBG_PRINT("File Loaded");
	
	//build stream of decoded data from pl file in MIME file.
	STLString plStr=GetDataForKey(MODEL_KEY);
	iSTLSStream inStrm(plStr);

	Model* newModel;
	newModel=ParseStream(inStrm);

	DBG_PRINT("Acquiring Runtime info (if present)...");
	//load runtime settings here
	PopulateRuntimeForModel(newModel);

	DBG_PRINT("Done.");
	return newModel;
}

void SmlParser::PopulateRuntimeForModel(Model* mdl)
{
	const char* RUNTIME_KEY="Run Status";
	enum {RT_EXECTIME=1, RT_TIMEUNIT, RT_DISPINT, RT_INTMETH, RT_PHASELIST };

	STLString rtStr = GetDataForKey(RUNTIME_KEY);
	
	if(!rtStr.empty())
	{
		static const STLRegex rtReg(	"^"
										"(?:execTime (.*?) ?)?"
										"(?:timeUnit (.*?) ?)?"
										"(?:displayInt (.*?) ?)?"
										"(?:intMethod (.*?) ?)?"
										"(?:phaseList (.*?) ?)?"
										"$"
								  );

		STLsmatch match;
		std::regex_match(rtStr,match,rtReg);

		if(!match.empty())
		{
			STLString intMethod(match[RT_INTMETH]);
			if(intMethod == "Euler")
				mdl->SetPreferredIntMethod(OMEIntegrationBlock::IM_EULER);
			else if(intMethod == "Runge-Kutta")
				mdl->SetPreferredIntMethod(OMEIntegrationBlock::IM_RK4);

			mdl->SetReportInterval(std::stof(match[RT_DISPINT]));
			//add additional fields here as necessary

		}
	}
}