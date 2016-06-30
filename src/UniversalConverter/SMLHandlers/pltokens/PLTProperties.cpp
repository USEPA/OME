#include "SMLHandlers/pltokens/PLTProperties.h"
#include "SMLHandlers/PrologParser.h"

PLTProperties::PLTProperties()
{

}

PLTProperties::~PLTProperties()
{

}

OMEObject* PLTProperties::buildOMEObject(STLString args, PrologParser & parser, STLString & outLabel)
{
	/*! Maps to NullProperties
		Attribs:
			complete
			file_name
			fill_colour
			fix_math_args
			image_posn
			multiplication_spec
			name
			separate
			step
	*/
	const OMEChar* patterns[]= {
						"^.*complete-(.*?),.*$",
						"^.*file_name-(.*?),.*$",
						"^.*fill_colour-(.*?),.*$",
						"^.*fix_math_args-(.*?),.*$",
						"^.*image_posn-(.*?),.*$",
						"^.*multiplication_spec-(.*?),.*$",
						"^.*name-'?(.*?)'?,.*$",
						"^.*separate-(.*?),.*$",
						"^.*step-(.*?),.*$",
						"\0"
						   };

	StrArray match=BatchRegex(args,patterns);


	//currently uncertain which attributes carry over from Simile. For now, just grab name.
	STLString name=match[PLTP_NAME];
	outLabel=STLString("properties");

	return new NullProperties(name.c_str());
}
