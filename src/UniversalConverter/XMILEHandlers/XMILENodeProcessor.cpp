#include "XMILEHandlers/XMILENodeProcessor.h"
#include "XMILEHandlers/XMILEOptionSet.h"
#include "DebugOutput.h"
#include "DrawDetails.h"
#include <map>
#include "CharType.h"
#include "OMEDefines.h"

XMILEGHandlerMap XMILENodeProcessor::s_generalHandlers = XMILEGHandlerMap();
XMILEVHandlerMap XMILENodeProcessor::s_viewHandlers = XMILEVHandlerMap();

/** Initialize static jump tables. This is called on the first instantiation of a XMILEProcessor object.*/
void XMILENodeProcessor::InitNodeProcessors()
{
	static bool isInitialized = false;

	if (!isInitialized)
	{
		//initialize handlers here...
		//general handlers
		s_generalHandlers[STLString("header")] = ProcessHeader;
		s_generalHandlers[STLString("sim_specs")] = ProcessSimSpecs;
		s_generalHandlers[STLString("model")] = ProcessModel;
		s_generalHandlers[STLString("stock")] = ProcessStock;
		s_generalHandlers[STLString("flow")] = ProcessFlow;
		s_generalHandlers[STLString("aux")] = ProcessAux;
		s_generalHandlers[STLString("gf")] = ProcessGraphFunction;
		//view handlers
		s_viewHandlers[STLString("stock")] = ProcessNodeView;
		s_viewHandlers[STLString("flow")] = ProcessFlowView;
		s_viewHandlers[STLString("aux")] = ProcessNodeView;
		s_viewHandlers[STLString("connector")] = ProcessConnector;

		isInitialized = true;
	}
}

/** Default constructor. */
XMILENodeProcessor::XMILENodeProcessor()
:m_idCount(0)
{
	InitNodeProcessors();
}

Model* XMILENodeProcessor::ParseModelFromFile(const STLString & path)
{
	TI2Document inDoc;
	if (!path.empty() && inDoc.LoadFile(path.c_str()) == tinyxml2::XML_SUCCESS)
		return ProcessXMILEDoc(inDoc);
	return NULL;
}

/** Construct a new model from a XMILE xml document.
	@param doc TinyXml2 object containing the XMILE xml file.
	@return A newly constructed Model.
*/
Model* XMILENodeProcessor::ProcessXMILEDoc(TI2Document & doc)
{
	TI2Element* pRoot = doc.RootElement();

	DBG_PRINT("Processing XMILE Document");
	DBG_PUSHLEVEL();

	//parse header
	HResults* pHRes = (HResults*)ProcessNode(pRoot, STLString("header"));

	if (!pHRes)
	{
		//err - should already be printed
		DBG_POPLEVEL();
		return NULL;
	}

	STLString rootName = pHRes->name;
	XMILEOptionSet opts = pHRes->options;
	delete pHRes;

	if (HasUnsupportedOptions(opts))
	{
		//err
		DBG_PRINTC("Error: unsupported options detected.", DBG_RED);
		DBG_POPLEVEL();
		return NULL;
	}

	//parse sim_specs
	//This is data that would normally end up in the control file.
	//for now, dump to screen. Potentially add option to generate simple .omec file in the future.
	SimSpecResults* pSpecs = (SimSpecResults*)ProcessNode(pRoot, STLString("sim_specs"));
	if (!pSpecs)
	{
		//err		
		DBG_POPLEVEL();
		return NULL;
	}
	
	OMEDBG_COLOR oldColor;
	DBG_GETTXCOLOR(oldColor);
	DBG_SETTXCOLOR(DBG_CYAN);
	DBG_PRINT("Specs found:");
	DBG_PUSHLEVEL();
	DBG_PRINT("Start: " + std::to_string(pSpecs->start));
	DBG_PRINT("Stop: " + std::to_string(pSpecs->stop));
	DBG_PRINT("Dt: " + std::to_string(pSpecs->dt));
	DBG_PRINT("Method: " + pSpecs->method);
	DBG_PRINT("Time Units: " + pSpecs->timeUnits);
	DBG_PRINT("Pause Interval: " + (pSpecs->pauseInt==OME_SCALAR_INF ? STLString("None") : std::to_string(pSpecs->pauseInt)));
	DBG_PRINT("Model Run Mode: " + pSpecs->runModels);
	DBG_POPLEVEL();
	DBG_SETTXCOLOR(oldColor);

	delete pSpecs;

	//parse model_units (ignored)

	//parse dimensions (ignored for now)

	//parse behavior (ignored)

	//parse data (ignored)

	//parse model(s)
	//if more than one root model, a parent model should be used to encapsulate all others.
	unsigned int count = 0;
	TI2Element* pMdlElem = pRoot->FirstChildElement("model");
	for (; pMdlElem; ++count, pMdlElem = pMdlElem->NextSiblingElement("model"));
	
	Model* pRet;
	if (count <= 1)
	{
		pRet = (Model*)ProcessNode(pRoot, STLString("model"));
	}
	else
	{
		pRet = (Model*)RegisterObject(new Model("root"));

		DrawDetails* pDD = new DrawDetails(pRet);
		TI2Element* pMdlElem = pRoot->FirstChildElement("model");

		while (pMdlElem)
		{
			Model* pCurrMdl = (Model*)ProcessNode(pMdlElem);
			if (pCurrMdl)
				pRet->AddSubmodel(pCurrMdl);
			else
			{
				DBG_POPLEVEL();
				delete pRet;
				return NULL;
			}
			pMdlElem = pMdlElem->NextSiblingElement("model");
		}
	}


	//connect flows to endpoints
	WireFlows(pRet);

	//modifiers are presently untested with XMILE stuff. May break when used.
	DBG_PRINT("Applying modifiers.");
	OMEArray omittot;
	for (auto itr = m_modifiers.begin(); itr != m_modifiers.end(); ++itr)
	{
		if ((*itr).first != NULL)
		{
			OMEObject* keyPtr = (*itr).first;
			ModifierProcessor::ProcessObject(keyPtr, (*itr).second, omittot,NULL,ConvertExpression);
			if (keyPtr != (*itr).first)
			{
				DrawDetails::ReplaceObjectForDD((*itr).first,keyPtr);
			}
			//if omissions occurred, future pointers may have been invalidated. set those pointers to null.
		}
	}


	//parse macros (ignore for now, should be first piece expanded.)

	//convert all expressions
	DBG_PRINT("Converting expressions...");
	DBG_PUSHLEVEL();
	for (auto &objEntry : m_objMap)
	{
		Evaluable* pEval = dynamic_cast<Evaluable*>(objEntry.second);
		if (pEval)
			pEval->SetExpr(ConvertExpression(pEval->GetExpr(),pEval));
		/*else
			DBG_PRINTC("Error: Failed lookup: " + objEntry.first, DBG_RED);*/

	}

	//clean up any omitted entries
	std::for_each(omittot.begin(), omittot.end(), [&](OMEObject* pObj){ m_objMap.erase(pObj->GetID()); delete pObj; });
	DBG_POPLEVEL();


	DBG_POPLEVEL();

	return pRet;
}

/** Take collected inflow/outflow data and use it to attach Flows properly to their associated StateVars.
	@param pMdl Pointer to the model whose Flows are to be wired up.
	@return true if Flows were successfully connected; false otherwise.
*/
bool XMILENodeProcessor::WireFlows(Model* pMdl)
{
	bool ret = true;
	
	DBG_PUSHLEVEL();
	DBG_PRINT("Joining flows for " + pMdl->GetName());

	for (size_t i = 0; i < pMdl->GetFlowCount(); ++i)
	{
		Flow* pFlow = pMdl->GetFlow(i);
		
		//find sources
		auto fItr = m_outflows.find(pFlow->GetExprName());
		if (fItr != m_outflows.end())
			pFlow->SetPSource((*fItr).second);


		//find targets
		fItr = m_inflows.find(pFlow->GetExprName());
		if (fItr != m_inflows.end())
			pFlow->SetPTarget((*fItr).second);
		

	}

	for (size_t i = 0; i < pMdl->GetSubmodelCount(); ++i)
		ret = ret && WireFlows(pMdl->GetSubmodel(i));

	DBG_POPLEVEL();

	return ret;
}

/** Register an object with the instance's object map.
	@param pObj Pointer to the object to register.
	@return pObj; this is useful for an inline allocation and registration.
*/
OMEObject* XMILENodeProcessor::RegisterObject(OMEObject * pObj)
{
	m_objMap[pObj->GetID()] = pObj;

	return pObj;
}

/** Check to see if the documents header is requesting options that OME does not presently support.
	@param opts The list of options to check.
	@return true if any unsupported options are requested; false otherwise.
*/
bool XMILENodeProcessor::HasUnsupportedOptions(const XMILEOptionSet & opts)
{
	return	opts.GetOption(XMILEOptionSet::CONV) ||
		opts.GetOption(XMILEOptionSet::QUEUE) ||
		opts.GetOption(XMILEOptionSet::ARRAY) ||
		opts.GetOption(XMILEOptionSet::MACROS);
		//all other options can safely be ignored.
}

/** Convert an object expression from XMILE syntax to OME syntax.
	@param str The expression to convert.
	@param pObj Optional pointer to the expression's parent object.
	@return A converted version of str.
*/
STLString XMILENodeProcessor::ConvertExpression(const STLString & str,OMEObject* pObj)
{
	//{XMILE Func, OME Func}
	//pairs without OME funcs are not yet implemented in OME and will through a warning.
	static std::map<STLString, STLString> knownFunctions{
		{ "abs", "abs" },
		{ "arccos", "acos" },
		{ "arcsin", "asin" },
		{ "arctan", "atan" },
		{ "cos", "cos" },
		{ "exp", "exp" },
		{ "inf", "inf" },
		{ "int", "int" },
		{ "ln", "log" },
		{ "log10", "log10" },
		{ "max", "max" },
		{ "min", "min" },
		{ "pi", "pi" },
		{ "sin", "sin" },
		{ "sqrt", "sqrt" },
		{ "tan", "tan" },
		{ "exprnd", "" },
		{ "lognormal", "" },
		{ "normal", "" },
		{ "poisson", "poidev" },
		{ "rand", "rand_var" },
		{ "delay", "" },
		{ "delay1", "delay1" },
		{ "delay3", "delay3" },
		{ "delayn", "delayn" },
		{ "forcst", "forcst" },
		{ "smth1", "smth1" },
		{ "smth3", "smth3" },
		{ "smthn", "smthn" },
		{ "trend", "trend" },
		{ "pulse", "pulse" },
		{ "ramp", "ramp" },
		{ "step", "step" },
		{ "init", "" },
		{ "previous", "last" },
		{ "self", "self" },

		//OME pre-inserted functions
		{ "graph", "graph" }
	};

	STLString expr = ToLowerCase(str);

	if (!expr.empty())
	{
		oSTLSStream buff;

		//strip comments
		size_t lastPos = 0;
		size_t currPos = expr.find('{');
		if (currPos != STLString::npos)
		{
			do
			{
				buff << expr.substr(lastPos, currPos - lastPos);

				currPos = FindMatching(expr, currPos, '{', '}');
				lastPos = currPos + 1;
				currPos = expr.find('{', lastPos);
			} while (currPos != STLString::npos);

			buff << expr.substr(lastPos);
			expr = buff.str();
			buff.clear();
			buff.str(STLString());

		}

		//dequote any identifiers
		lastPos = 0;
		currPos = expr.find('"');
		if (currPos != STLString::npos)
		{
			bool activeParse = false;

			//if the quote is escaped, skip
			if (currPos == 0 || expr[currPos - 1] != '\\')
			{
				//if we are outside a quote, just directly append;
				//otherwise scrub identify
				//then mark that we are either entering or leaving a quote as necessary
				buff << activeParse ? ScrubName(expr.substr(lastPos, currPos - lastPos)) : expr.substr(lastPos, currPos - lastPos);
				lastPos = currPos + 1;
				activeParse = !activeParse;
			}

			//if activeParse is still true, then there are unbalanced quotes
			if (activeParse)
				DBG_PRINTC("Error: Unbalanced quotes found.", DBG_RED);

			buff << expr.substr(lastPos);
			expr = buff.str();
			buff.clear();
			buff.str(STLString());
		}

		//convert operators and known keywords
		const OMEChar* patterns[] = { "\\bnot\\b", "!",
			"\\bmod\\b", "%",
			"<>", "!=",
			"\0"
		};
		expr = BatchRegexReplace(expr, patterns);

		//place "end" keyword at the end of if-then-else
		if (expr.find("else") != STLString::npos)
		{

			STLRegex elseReg("\\belse\\b");
			STLsmatch mtch;
			while (std::regex_search(expr, mtch, elseReg))
			{
				buff << mtch.prefix() << mtch[0].str();

				currPos = mtch.position(0) + mtch.length(0);
				lastPos = currPos;

				short balance = 0;
				//find end of statement (either end of string or unbalanced ')'
				for (; currPos < expr.length() && balance >= 0; ++currPos)
				{
					switch (expr[currPos])
					{
					case '(':
						++balance;
						break;
					case ')':
						--balance;
						if (balance < 0)
							buff << "end";
						break;
					}
					buff << expr[currPos];
				}
				if (currPos < expr.length())
					expr = expr.substr(currPos);
				else
					buff << "end";

			}
			if (!expr.empty())
				buff << expr;

			buff << expr.substr(lastPos);
			expr = buff.str();
			buff.clear();
			buff.str(STLString());
		}

		//try to identify variables.
		// -if variable name is found, substitute full path.
		// - variable can be called with argument lists, but only when certain options are enabled.
		//any unknown identifiers should be checked against a list of known functions.
		// - functions can be called without argument lists if no arguments are needed.

		STLRegex idReg("\\b[A-Za-z](?:[.A-Za-z_$0-9]*[.A-Za-z$0-9])?\\b");
		STLsmatch idMtch;
		const OMEChar* reserved[] = { "^if$", "^then$", "^else$", "^mod$", "^and$", "^or$", "\0" };

		StrList pathParts;
		Evaluable* pEval = dynamic_cast<Evaluable*>(pObj);
		if (pEval)
		{
			pEval->GetParentModel()->GetUpPath(pathParts);
			STLString path = ConcatStrings(pathParts, '.') + ".";

			while (std::regex_search(expr, idMtch, idReg))
			{
				buff << idMtch.prefix();
				if (!BatchRegexMatchAny(idMtch[0].str(), reserved, 0))
				{
					//check for object.
					OMEObject* pObj = pEval->GetParentModel()->ObjectForInsensitiveName(idMtch[0].str());
					if (pObj)
						buff << path << pObj->GetExprName();
					else
					{
						//do function lookup
						auto fItr = knownFunctions.find(idMtch[0].str());
						if (fItr != knownFunctions.end())
						{
							//function found; insert appropriate code
							if (!(*fItr).second.empty())
							{
								//add appropriate function call
								buff << (*fItr).second;
								//add parens if needed.
								if (idMtch.suffix().str().empty() || idMtch.suffix().str()[0] != '(')
									buff << "()";
							}
							else
							{
								//function not yet implemented by OME; let user know.
								DBG_PRINTC("Warning: Function " + idMtch[0].str() + " is not yet implemented in OME.", DBG_YELLOW);
								buff << idMtch[0].str();
							}
						}
						else
						{
							//no function found; insert as-is
							DBG_PRINTC("Warning: Identifier not found: " + idMtch[0].str(), DBG_YELLOW);
							buff << idMtch[0].str();
						}
					}
				}
				expr = idMtch.suffix();
			}
			buff << expr;
		}

		return buff.str();
	}

	return str;
}

//handlers
/** Process a general model component node.
	@param pParentElem Pointer to the parent xml element.
	@param tag Identifier of node to process.
	@param isReq If true, an error is printed if a node with the name in tag is not found.
	@return Pointer to the newly allocated object derived from the data in the node identified by tag; NULL is returned on construction failure.
*/
void* XMILENodeProcessor::ProcessNode(TI2Element* pParentElem, const STLString & tag, const bool & isReq)
{
	TI2Element* pCurrElem = pParentElem->FirstChildElement(tag.c_str());
	if (!pCurrElem)
	{
		//err
		DBG_PRINTC("Error: '"+tag+"' element not found.", DBG_RED);
		return NULL;
	}

	void* pRes = NULL;
	
	if (!(pRes = s_generalHandlers[tag](pCurrElem,this)))
	{
		//err
		if (isReq)
			DBG_PRINTC("Error: missing required '" + tag + "' element fields.", DBG_RED);
		return NULL;
	}

	return pRes;
}

/** Process a general model component node.
@param pCurrElem Pointer to the xml node to process.
@param isReq If true, an error is printed if a node with the name in tag is not found.
@return Pointer to the newly allocated object derived from the data in the node identified by tag; NULL is returned on construction failure.
*/
void* XMILENodeProcessor::ProcessNode(TI2Element* pCurrElem, const bool & isReq)
{
	STLString tag = pCurrElem->Value();
	void* pRes = NULL;

	if (!(pRes = s_generalHandlers[tag](pCurrElem,this)))
	{
		//err
		if (isReq)
			DBG_PRINTC("Error: missing required '" + tag + "' element fields.", DBG_RED);
		return NULL;
	}

	return pRes;
}

/** Process a XMILE header element.
	@param pElem Pointer to the xml node to process.
	@param pNProc Pointer to the calling XMILENodeProcessor element.
	@return A pointer to a newly allocated HResults struct on success; NULL on failure.
*/
void* XMILENodeProcessor::ProcessHeader(TI2Element* pElem, XMILENodeProcessor* pNProc)
{
	TI2Element* pCurrE;
	
	DBG_PRINT("Parsing header");
	DBG_PUSHLEVEL();

	//just grab relevant OME options for now

	//parse vendor (ignored)

	//parse product (ignored)

	//parse options
	XMILEOptionSet opts;
	if ((pCurrE = pElem->FirstChildElement("options")))
	{
		DBG_PRINT("Requested optional supports:");
		DBG_PUSHLEVEL();
		//--attributes
		//	namespace (ignored)

		//--subnodes
		//for now, we are only interested in the existances of a stated feature.
		//in the future, attributes of each may need to be parsed as well.


		if (pCurrE->FirstChildElement("uses_conveyor"))
		{
			opts.SetOption(XMILEOptionSet::CONV, true);
			DBG_PRINT("Uses conveyors");
		}
		if (pCurrE->FirstChildElement("uses_queue"))
		{
			opts.SetOption(XMILEOptionSet::QUEUE, true);
			DBG_PRINT("Uses queues");
		}
		if (pCurrE->FirstChildElement("uses_arrays"))
		{
			opts.SetOption(XMILEOptionSet::ARRAY, true);
			DBG_PRINT("Uses arrays");
		}
		if (pCurrE->FirstChildElement("uses_submodels"))
		{
			opts.SetOption(XMILEOptionSet::SUBMODELS, true);
			DBG_PRINT("Uses submodels");
		}
		if (pCurrE->FirstChildElement("uses_macros"))
		{
			opts.SetOption(XMILEOptionSet::MACROS, true);
			DBG_PRINT("Uses macros");
		}
		if (pCurrE->FirstChildElement("uses_event_posters"))
		{
			opts.SetOption(XMILEOptionSet::EVENTS, true);
			DBG_PRINT("Uses event posters");
		}
		if (pCurrE->FirstChildElement("has_model_view"))
		{
			opts.SetOption(XMILEOptionSet::VIEW, true);
			DBG_PRINT("Has model view");
		}
		if (pCurrE->FirstChildElement("uses_outputs"))
		{
			opts.SetOption(XMILEOptionSet::OUTPUTS, true);
			DBG_PRINT("Uses outputs");
		}
		if (pCurrE->FirstChildElement("uses_inputs"))
		{
			opts.SetOption(XMILEOptionSet::INPUTS, true);
			DBG_PRINT("Uses inputs");
		}
		if (pCurrE->FirstChildElement("uses_annotation"))
		{
			opts.SetOption(XMILEOptionSet::ANNOTS, true);
			DBG_PRINT("Uses annotations");
		}
		DBG_POPLEVEL();
	}
	//parse name
	STLString rootName;
	if (pCurrE=pElem->FirstChildElement("name"))
		rootName= pCurrE->Value();

	//parse version (ignored)

	//parse caption (ignored)

	//parse author (ignored)

	//parse affiliation (ignored)

	//parse client (ignored)

	//parse copyright (ignored)

	//parse created (ignored)

	//parse modified (ignored)

	//parse uuid (ignored)

	//parse includes (ignored, but may need to be supported soon)

	HResults* pRet = new HResults();
	pRet->name = rootName;
	pRet->options = opts;

	DBG_POPLEVEL();
	return pRet;
}

/** Process a XMILE sim_specs element.
	@param pElem Pointer to the xml node to process.
	@param pNProc Pointer to the calling XMILENodeProcessor element.
	@return A pointer to a newly allocated SimSpecResults struct on success; NULL on failure.
*/
void* XMILENodeProcessor::ProcessSimSpecs(TI2Element* pElem, XMILENodeProcessor* pNProc)
{
	DBG_PRINT("Processing SimSpecs...");
	DBG_PUSHLEVEL();

	OME_SCALAR start, stop;

	//parse start and stop nodes
	TI2Element* pStartElem = pElem->FirstChildElement("start");
	TI2Element* pStopElem = pElem->FirstChildElement("stop");
	if (!(pStartElem && pStopElem))
	{
		DBG_PRINTC("Error: missing 'start' and/or 'stop' elements in sim_spec element.", DBG_RED);
		DBG_POPLEVEL();
		return NULL;
	}
	start = OME_SCLR_READSTR(pStartElem->GetText());
	stop = OME_SCLR_READSTR(pStopElem->GetText());

	SimSpecResults* pRet = new SimSpecResults(start, stop);

	//optionals

	//parse dt node
	TI2Element* pOptElem = pElem->FirstChildElement("dt");
	if (pOptElem)
	{
		pRet->dt = OME_SCLR_READSTR(pOptElem->GetText());
		bool isRecip = false;
		pOptElem->QueryBoolAttribute("reciprocal", &isRecip);
		if (isRecip)
			pRet->dt = 1 / pRet->dt;

	}

	//parse method attr
	const char* pAttr = pElem->Attribute("method");
	if (pAttr)
		pRet->method = pAttr;
	
	//parse time_units attr
	pAttr = pElem->Attribute("time_units");
	if (pAttr)
		pRet->timeUnits = pAttr;

	//parse pause attr
	XMLVALQUERY(pElem, "pause", &pRet->pauseInt);

	//parse run attr
	pAttr = pElem->Attribute("run");
	if (pAttr)
		pRet->runModels = pAttr;

	DBG_POPLEVEL();
	return pRet;
}

/** Populate a model component xml node with fields shared across all model variable types.
@param pElem Pointer to the xml node to process.
@param pEval Pointer to the Evaluable object to populate.
@param pNProc Pointer to the calling XMILENodeProcessor element.
@return true if all required field assignments were successful; false otherwise.
*/
bool XMILENodeProcessor::PopulateWithCommonFields(TI2Element* pElem, Evaluable* pEval, XMILENodeProcessor* pNProc)
{
	TI2Element* pCurrElem;

	//id should be auto generated
	pEval->SetID(pNProc->GenerateID());

	//create drawdetail entry for object (the pointer is tracked elsewhere and does not need to be managed here).
	DrawDetails* pDD = new DrawDetails(pEval);

	//parse name
	const OMEChar* nm = pElem->Attribute("name");
	if (!nm)
	{
		DBG_PRINTC("Error: element does not have a name, which is required.", DBG_RED);

		return false;
	}
	pEval->SetName(ProcessID(nm));

	//parse dimensions (ignored until arrays are in place)

	//parse elements (ignored)

	//parse access attr (ignored)

	//parse autoexport attr (ignored)

	//parse eqn
	pCurrElem = pElem->FirstChildElement("eqn");
	if (pCurrElem)
		pEval->SetExpr(pCurrElem->GetText());

	//parse mathml (ignored)

	//parse units
	pCurrElem = pElem->FirstChildElement("units");
	if (pCurrElem)
		pDD->SetUnits(STLString(pCurrElem->GetText()));

	//parse documentation
	pCurrElem = pElem->FirstChildElement("doc");
	if (pCurrElem)
		pDD->SetDescription(ProcessID(pCurrElem->GetText()));

	pNProc->m_modifiers[pEval] = ModifierProcessor::ExtractExpression(pDD->GetDescription());
	//parse event poster (ignored)

	//parse range (ignored)

	//parse scale (ignored)

	//parse format (ignored)

	//parse gf; convert to either interp table or eval table
	TI2Element* pGFElem = pElem->FirstChildElement("gf");
	if (pGFElem)
	{
		EvalTable* pET = (EvalTable*)ProcessGraphFunction(pGFElem, pNProc);
		if (pET)
		{
			//make sure gf is used
			pEval->SetExpr("graph(" + pEval->GetExpr() + ")");
			//register table
			pEval->SetPEvalTable(pET,0);
			pNProc->m_tables[pET->GetID()] = pET;
		}
	}

	return true;
}

/** Process a XMILE stock element.
	@param pElem Pointer to the xml node to process.
	@param pNProc Pointer to the calling XMILENodeProcessor element.
	@return A pointer to a newly allocated StateVar object on success; NULL on failure.
*/
void* XMILENodeProcessor::ProcessStock(TI2Element* pElem, XMILENodeProcessor* pNProc)
{
	StateVar* pState = new StateVar();

	if (!PopulateWithCommonFields(pElem, pState,pNProc))
	{
		return NULL;
	}

	DBG_PRINT("Processing Stock " + pState->GetName());

	//inflow, outflow are handled when wiring flows, but recorded here
	TI2Element* pFlowElem = pElem->FirstChildElement("inflow");
	while (pFlowElem)
	{
		pNProc->m_inflows[STLString(pFlowElem->GetText())] = pState;
		pFlowElem = pFlowElem->NextSiblingElement("inflow");
	}

	pFlowElem = pElem->FirstChildElement("outflow");
	while (pFlowElem)
	{
		pNProc->m_outflows[STLString(pFlowElem->GetText())] = pState;
		pFlowElem = pFlowElem->NextSiblingElement("outflow");
	}

	//conveyor/queue stuff unsported for now

	//non-negative ignored for now (should be added to OME at some point)

	pNProc->RegisterObject(pState);
	return pState;
}

/** Process a XMILE flow element.
	@param pElem Pointer to the xml node to process.
	@param pNProc Pointer to the calling XMILENodeProcessor element.
	@return A pointer to a newly allocated Flow object on success; NULL on failure.
*/
void* XMILENodeProcessor::ProcessFlow(TI2Element* pElem, XMILENodeProcessor* pNProc)
{
	Flow* pFlow = new Flow();

	if (!PopulateWithCommonFields(pElem, pFlow,pNProc))
	{
		return NULL;
	}

	DBG_PRINT("Processing Flow " + pFlow->GetName());

	//conveyor/queue stuff unsported for now

	//non-negative ignored for now (should be added to OME at some point)

	pNProc->RegisterObject(pFlow);
	return pFlow;
}

/** Process a XMILE aux element.
@param pElem Pointer to the xml node to process.
@param pNProc Pointer to the calling XMILENodeProcessor element.
@return A pointer to a newly allocated Variable object on success; NULL on failure.
*/
void* XMILENodeProcessor::ProcessAux(TI2Element* pElem, XMILENodeProcessor* pNProc)
{
	Variable* pVar = new Variable();

	if (!PopulateWithCommonFields(pElem, pVar,pNProc))
	{
		return NULL;
	}

	DBG_PRINT("Processing Variable " + pVar->GetName());

	//conveyor/queue stuff unsported for now

	//non-negative ignored for now (should be added to OME at some point)

	pNProc->RegisterObject(pVar);
	return pVar;
}

/** Process a XMILE model element.
@param pElem Pointer to the xml node to process.
@param pNProc Pointer to the calling XMILENodeProcessor element.
@return A pointer to a newly allocated Model object on success; NULL on failure.
*/
void* XMILENodeProcessor::ProcessModel(TI2Element* pElem, XMILENodeProcessor* pNProc)
{
	DBG_PRINT("Processing Model...");
	DBG_PUSHLEVEL();

	//if encrypted, fail
	if (pElem->Attribute("encryption-scheme"))
	{
		DBG_PRINTC("Error: Model is encrypted,which is unsupported", DBG_RED);
		DBG_POPLEVEL();
		return NULL;
	}

	//check for variables
	TI2Element* pVarElem = pElem->FirstChildElement("variables");
	if (!pVarElem)
	{
		DBG_PRINTC("Error: Model missing required 'variables' tag", DBG_RED);
		DBG_POPLEVEL();
		return NULL;
	}

	Model* pMdl = (Model*)pNProc->RegisterObject(new Model(pNProc->GenerateID()));
	
	//parse name attribute
	const char* name = pElem->Attribute("name");
	if (name)
		pMdl->SetName(ProcessID(name));
	else
		pMdl->SetName(pMdl->GetName());

	//parse run attribute (ignored)


	//parse namespace attribute (ignored)

	//parse resource attribute (ignored)

	//parse sim_specs (ignored)

	//parse behavior (ignored)

	//parse variables
	TI2Element* pCurrElem = pVarElem->FirstChildElement();
	while (pCurrElem)
	{
		OMEObject* pOut = (OMEObject*)pNProc->ProcessNode(pCurrElem, false);
		if (pOut)
			pMdl->AddObject(pOut);
		pCurrElem = pCurrElem->NextSiblingElement();
	}

	//parse views (parsed for draw info, influences)
	if (pCurrElem = pElem->FirstChildElement("views"))
	{
		//for now, just pull information from the first encountered stock-flow view

		TI2Element* pViewElem = pCurrElem->FirstChildElement("view");
		while (pViewElem)
		{
			if (!pViewElem->Attribute("type") || STLString(pViewElem->Attribute("type")) == "stock_flow")
			{
				//found diagram, collect draw info.
				pCurrElem = pViewElem->FirstChildElement();
				while (pCurrElem)
				{
					auto fItr = s_viewHandlers.find(pCurrElem->Value());
					if (fItr != s_viewHandlers.end())
						(*fItr).second(pCurrElem, pMdl, pNProc);
					pCurrElem = pCurrElem->NextSiblingElement();
				}

				break;
			}
			pViewElem = pViewElem->NextSiblingElement("view");
		}
	}

	//add tables to model
	for (auto tEntry : pNProc->m_tables)
		pMdl->AddEvalTable(tEntry.second);

	pNProc->m_tables.clear();
	DrawDetails* pDD = new DrawDetails(pMdl);

	DBG_POPLEVEL();
	return pMdl;
}

/** Process a XMILE gf element.
@param pGFElem Pointer to the xml node to process.
@param pNProc Pointer to the calling XMILENodeProcessor element.
@return A pointer to a newly allocated EvalTable object or subclass object on success; NULL on failure.
*/
void* XMILENodeProcessor::ProcessGraphFunction(TI2Element* pGFElem, XMILENodeProcessor* pNProc)
{
	//for now, ignore xpts entirely.
	//for now, assume interp table
	
	//grab y-points
	TI2Element* pYPtElem = pGFElem->FirstChildElement("ypts");
	if (!pYPtElem)
	{
		DBG_PRINTC("Error: graph function is missing y coordinate list",DBG_RED);
		return NULL;
	}
	TI2Element* pXBElem = pGFElem->FirstChildElement("xscale");
	TI2Element* pXPtElem = pGFElem->FirstChildElement("xpts");
	if (!(pXBElem || pXPtElem))
	{
		DBG_PRINTC("Error: graph function is missing 'xscale' element", DBG_RED);
		return NULL;
	}

	OMEChar delim = ',';
	if (pYPtElem->Attribute("sep"))
		delim = pYPtElem->Attribute("sep")[0];

	StrArray pieces = SplitString(pYPtElem->GetText(), delim);
	EvalTable::DataArray yPts;
	for (STLString & p : pieces)
		yPts.push_back(OME_SCLR_READSTR(p));

	EvalTable::DataArray xPts;
	if (pXPtElem)
	{
		if (pXPtElem->Attribute("sep"))
			delim = pXPtElem->Attribute("sep")[0];

		StrArray pieces = SplitString(pXPtElem->GetText(), delim);
		for (STLString & p : pieces)
			xPts.push_back(OME_SCLR_READSTR(p));
	}

	//gather iterpolation and boundary handling characteristics
	STLString mode = "continuous";

	EvalInterpTable::INTERPMODE iMode = EvalInterpTable::INTERP;
	EvalInterpTable::BOUNDSMODE bMode = EvalInterpTable::CLAMP;

	if (pYPtElem->Attribute("type"))
		mode = pYPtElem->Attribute("type");

	if (mode == "discrete")
		iMode = EvalInterpTable::FLOOR;

	else if (mode == "extrapolate")
		bMode = EvalInterpTable::EXTRAP;

	STLString name;
	if (pGFElem->Attribute("name"))
		name = ProcessID(pGFElem->Attribute("name"));
	else
		name = pNProc->GenerateID();
	EvalInterpTable* pTbl;
	if (xPts.empty())
		pTbl = new EvalInterpTable(name, iMode, bMode, yPts);
	else
		pTbl = new EvalXSetInterpTable(name, iMode, bMode, xPts, yPts);

	//grab bounds
	OME_SCALAR lbound,ubound;
	if (pXBElem)
	{
		XMLVALQUERY(pXBElem,"min", &lbound);
		XMLVALQUERY(pXBElem,"max", &ubound);
		pTbl->SetBounds(lbound, ubound);
	}

	

	return pTbl;
}

/** Process Nodes found in a XMILE document's views node.
	@param pElem The view element to process.
	@param pMdl Pointer to the encompassing Model object.
	@param pNProc Pointer to the calling XMILENodeProcessor element.
	@return true if the node was successfully processed; false otherwise. 
*/
bool XMILENodeProcessor::ProcessNodeView(TI2Element* pElem, Model* pMdl, XMILENodeProcessor* pNProc)
{
	DBG_PRINT("Retrieving draw info for " + STLString(pElem->Value()));
	if (!pElem->Attribute("name"))
	{
		DBG_PRINTC("Error: name attribute not found.", DBG_RED);
		return false;
	}
	
	OMEObject* pObj = pMdl->ObjectForName(ProcessID(pElem->Attribute("name")));
	if (!pObj)
	{
		DBG_PRINTC("Error: Object with name "+STLString(pElem->Attribute("name"))+" not found.", DBG_RED);
		return false;
	}

	DrawDetails* pDD = DrawDetails::DDForObject(pObj);

	OMEBox dBox;
	pElem->QueryIntAttribute("x", &dBox.tl.x);
	pElem->QueryIntAttribute("y", &dBox.tl.y);
	pElem->QueryIntAttribute("width", &dBox.br.x);
	pElem->QueryIntAttribute("height", &dBox.br.y);

	//correct bottom right corner
	dBox.br += dBox.tl;
	pDD->SetBoundingBox(dBox);
	//other draw info may be included, but this is all we have from demo files.

	return true;
}

/** Process flow nodes found in a XMILE document's views node.
@param pElem The view element to process.
@param pMdl Pointer to the encompassing Model object.
@param pNProc Pointer to the calling XMILENodeProcessor element.
@return true if the node was successfully processed; false otherwise.
*/
bool XMILENodeProcessor::ProcessFlowView(TI2Element* pElem, Model* pMdl, XMILENodeProcessor* pNProc)
{
	//DBG_PRINT("Retrieving draw info for " + STLString(pElem->GetText()));
	if (!pElem->Attribute("name"))
	{
		DBG_PRINTC("Error: name attribute not found.", DBG_RED);
		return false;
	}

	OMEObject* pObj = pMdl->ObjectForName(ToLowerCase(ProcessID(pElem->Attribute("name"))));
	if (!pObj)
	{
		DBG_PRINTC("Error: Object with name " + STLString(pElem->Attribute("name")) + " not found.", DBG_RED);
		return false;
	}

	DrawDetails* pDD = DrawDetails::DDForObject(pObj);

	OMEPoint origin(0, 0);
	pElem->QueryIntAttribute("x", &origin.x);
	pElem->QueryIntAttribute("y", &origin.y);
	pDD->SetOrigin(origin);
	
	TI2Element* pPtsElem = pElem->FirstChildElement("pts");
	if (pPtsElem)
	{
		//for now just grab first and last points
		OMEPoint currPt;
		TI2Element* pCurrElem = pPtsElem->FirstChildElement("pt");
		pCurrElem->QueryIntAttribute("x", &currPt.x);
		pCurrElem->QueryIntAttribute("y", &currPt.y);
		pDD->SetCtrlPt1(currPt);

		currPt = OMEPoint();
		pCurrElem = pPtsElem->LastChildElement("pt");
		pCurrElem->QueryIntAttribute("x", &currPt.x);
		pCurrElem->QueryIntAttribute("y", &currPt.y);
		pDD->SetCtrlPt2(currPt);
	}


	return true;
}

/** Process connector nodes found in a XMILE document's views node, creating OME Influence objects.
@param pElem The view element to process.
@param pMdl Pointer to the encompassing Model object.
@param pNProc Pointer to the calling XMILENodeProcessor element.
@return true if the node was successfully processed; false otherwise.
*/
bool XMILENodeProcessor::ProcessConnector(TI2Element* pElem, Model* pMdl, XMILENodeProcessor* pNProc)
{
	DBG_PRINT("Adding influence");
	STLString id;
	if (pElem->Attribute("uid"))
		id = pElem->Attribute("uid");
	else
		id = pNProc->GenerateID();

	TI2Element* pToElem = pElem->FirstChildElement("to");
	TI2Element* pFromElem = pElem->FirstChildElement("from");

	if (!pToElem || !pFromElem)
	{
		DBG_PRINTC("Error: connector is missing to and/or from identifier", DBG_RED);
		return false;
	}

	OMEObject* pSrc = pMdl->ObjectForInsensitiveName(pFromElem->GetText());
	OMEObject* pTrg = pMdl->ObjectForInsensitiveName(pToElem->GetText());

	if (!pSrc || !pTrg)
	{
		DBG_PRINTC("Error: connector source and/or object not found in model " + pMdl->GetName(),DBG_RED);
		return false;
	}
	Influence* pInfl = (Influence*)pNProc->RegisterObject(new Influence(id, id, pSrc, pTrg));

	//don't delete pDD; it is managed elsewhere.
	DrawDetails* pDD = new DrawDetails(pInfl);

	//fill in basic draw info.
	OMEPoint origin(0,0);
	pElem->QueryIntAttribute("x", &origin.x);
	pElem->QueryIntAttribute("y", &origin.y);
	pDD->SetOrigin(origin);

	//other draw info may be included, but this is all we have from demo files.

	pMdl->AddInfluence(pInfl);
	return true;
}

/** Fix escape character tagging.
@param inExpr String containing the escape characters to correct.
@return a copy of inExpr with escape characters properly fixed.
*/
STLString XMILENodeProcessor::ProcessID(STLString inExpr)
{
	//move everything to lower case
	inExpr = ToLowerCase(inExpr);

	//fix escapes
	const char* escapes[] = { "\\\\n", "\n",
		"\\\\t", "\t",
		"\\\\r", "\r",
		"\\n", "\n",
		"\\t", "\t",
		"\\r", "\r",
		"\0"
	};

	return BatchRegexReplace(inExpr, escapes);
}