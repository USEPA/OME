#include "SMLHandlers/pltokens/PLTArc.h"
#include "DebugOutput.h"
#include "SMLHandlers/PrologParser.h"
#include "ModifierProcessor.h"
#include "DrawDetails.h"
#include <sstream>

PLTArc::PLTArc()
{

}

PLTArc::~PLTArc()
{

}

OMEObject* PLTArc::buildOMEObject(STLString args, PrologParser & parser, STLString & outLabel)
{
	//arc=influence, flow, or relation
	
	enum {SUB_ALL=0, SUB_ATTACHED,SUB_COMMENT,SUB_COMPLETE, SUB_NAME, SUB_ROLE, SUB_SOFAR, SUB_CAPOFFS, SUB_CURVE, SUB_COUNT};

	OMEObject* ret=NULL;
	Link* outLink=NULL;
	static const STLRegex argReg("^([a-zA-Z0-9]*),([a-zA-Z0-9]*),([a-zA-Z0-9]*),(.*?),(\\[.*\\]),(\\[.*\\])$");
	std::smatch match;
	std::regex_match(args,match,argReg);

	STLString type=match[PLTA_TYPE];
	STLString source=match[PLTA_SOURCE];
	STLString target=match[PLTA_TARGET];

	static const OMEChar* patterns[] = {			
												"^.*attached=\\[(.*?)\\](?:,|\\]).*$",
												"^.*comment='(.*?)'.*$",
												"^.*complete=(.*?)(?:,|\\]).*$",
												"^.*name='?(.*?)'?(?:,|\\]).*$",
												"^.*role=\\[(use\\(.*?,.*?,.*?,.*?\\))\\],?.*$",
												"^.*use_sofar=(.*?),?.*$",
												"^.*\\[.*caption_offset=\\[(.*?,.*?)\\].*\\].*$",
												"^.*\\[.*curve=\\[(.*?,.*?)\\].*\\].*$",
												"\0"};
	STLString attrs=match[PLTA_ATTR];


	StrArray subMatches=BatchRegex(attrs,patterns);
	
	//static const STLRegex wsReg("[ \\t]");
	STLString name=ConvertEscapes(subMatches[SUB_NAME]);

	if(type=="influence")
	{
		/*! maps to Influence
			attr:
				attached
				complete
				name
				role
				curve
		*/

		STLString role=ConvertEscapes(subMatches[SUB_ROLE]);
		Influence* inf = new Influence(name, source, target);
		parser.AddInflRole(role, inf);
		inf->SetUseCurrentVal(false);
		if (!subMatches[SUB_SOFAR].empty())
		{
			bool useCurr;
			std::stringstream(subMatches[SUB_SOFAR]) >> useCurr;
			inf->SetUseCurrentVal(useCurr);
		}
		ret=inf;
		outLink=inf;
	}
	else if(type=="flow")
	{
		/*! maps to Flow
			attr:
				attached
				complete
				name
				curve
				caption_offset
		*/
		
		//attached is a Simile 6+ thing
		Flow* fl = new Flow(name, source, target);
		if (subMatches[SUB_ATTACHED].length()>0)
			parser.AddFlowFunc(fl, subMatches[SUB_ATTACHED]);
		ret=fl;
		outLink=fl;
	}
	else if(type=="relation")
	{
		
		SubmodelAssoc* sma= new SubmodelAssoc(name,source,target,STLString());
		ret=sma;
		outLink = sma;
	}
	else
		DBG_PRINT(STLString("Unknown Type detected: ")+type);
	if(ret)
	{
		outLabel=match[PLTA_ID];
		parser.AddModifier(ret,ModifierProcessor::ExtractExpression(subMatches[SUB_COMMENT]));
		//add to parser link table

		//extract drawDetails.
		DrawDetails* pDD=new DrawDetails(ret);
		pDD->SetDescription(ConvertEscapes(subMatches[SUB_COMMENT]));
		OMEPoint tmp;

		iSTLSStream buff;
		if(!subMatches[SUB_CAPOFFS].empty())
		{
			buff.str(subMatches[SUB_CAPOFFS]);
			if(buff>>tmp.x>>tmp.y)
				pDD->SetLabelOrigin(tmp);
			tmp=OMEPoint();
		}

		if(!subMatches[SUB_CURVE].empty())
		{
			//for now, just assign to control point1
			buff.str(subMatches[SUB_CURVE]);
			if(buff>>tmp.x>>tmp.y)
				pDD->SetCtrlPt1(tmp);
			tmp=OMEPoint();
		}
		
		if(outLink)
			parser.m_linkMap[outLabel]=outLink;
		
	}

	return ret;
}
