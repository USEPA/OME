#include "SMLHandlers/pltokens/PLTSource.h"

PLTSource::PLTSource()
{
}

OMEObject* PLTSource::buildOMEObject(STLString args, PrologParser & parser, STLString & outLabel)
{
	/*! Attrs:
		program
		version
		edition
		date
	*/
	std::smatch match;
	static const STLRegex reg(	"^"
								"(?:program='?(.*?)'?,?)?"
								"(?:version=\\w*(.*?),?)?"
								"(?:edition=(.*?),?)?"
								"(?:date='(.*?)'.?)?"
								"$"
							);

	std::regex_match(args,match,reg);

	outLabel=STLString("source");
	STLString progStr(match[PLTS_PROGRAM]);
	STLString versVal(match[PLTS_VERSION]);
	STLString editStr(match[PLTS_EDITION]);
	STLString dateStr(match[PLTS_DATE]);

	return new NullSource(progStr.c_str(), versVal.c_str(), editStr.c_str(), dateStr.c_str());


}
