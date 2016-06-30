#include "SMLHandlers/pltokens/PLTLinks.h"
#include "SMLHandlers/PrologParser.h"

PLTLinks::PLTLinks()
{

}

PLTLinks::~PLTLinks()
{

}

OMEObject* PLTLinks::buildOMEObject(STLString args, PrologParser & parser, STLString & outLabel)
{
	OMEObject* ret=NULL;
	
	enum {PLTL_ALL=0,PLTL_SUBMODEL, PLTL_LINKS};

	static const STLRegex argReg("^([a-zA-Z0-9]*),\\[(.*)\\]$");
	std::smatch match;
	std::regex_match(args,match,argReg);

	NullArcLinkMap* newALM = new NullArcLinkMap();

	STLString submodelStr=match[PLTL_SUBMODEL].str();
	STLString linksStr=match[PLTL_LINKS].str();

	newALM->SetSubmodelID(submodelStr.c_str());

	static const STLRegex linkReg("(.*?)-(.*?)(?:,|$)");
	std::smatch linkMatch;

	while(std::regex_search(linksStr,linkMatch,linkReg))
	{
		STLString lftStr=linkMatch[1].str();
		STLString rhtStr=linkMatch[2].str();

		newALM->AddArcLink(arcLink(lftStr.c_str(),rhtStr.c_str()));
		linksStr=linkMatch.suffix();
	}

	ret=newALM;
	outLabel=newALM->GetSubmodelID()+"-links";

	parser.m_interModelLinkMap[newALM->GetSubmodelID()]=newALM;
	return ret;
}
