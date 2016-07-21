#include "MDLHandlers/MDLParser.h"
#include "OMEDefines.h"
#include "omeobjects/SimObjs.h"
#include "DrawDetails.h"
#include "CPPLoopCluster.h"
#include "ModifierProcessor.h"

MDLParser::MDLParser()
{
}


MDLParser::~MDLParser()
{
}

Model* MDLParser::ParseModelFromFile(const STLString & path)
{
	//parse characters into string buffer, one at a time, removing comments

	std::ifstream inFile(path);

	if (!inFile.good())
		return NULL; //err
		
	unsigned short cLvl = 0;
	OMEChar currChar;
	oSTLSStream outBuff;

	DBG_PRINT("Purging inline comments");
	while (inFile.good())
	{
		inFile >> std::noskipws >> currChar;
		
		
		if(currChar=='{')
		{
			//increase comment level
			++cLvl;
		}

		//remove /r when ever encountered
		if (!cLvl && currChar!='\r')
		{
			//add char to string buffer
			//but only if char is not within comment.
			outBuff << currChar;
		}

		if (currChar == '}')
		{
			//decrease comment level
			--cLvl;
		}
	}

	STLString src = outBuff.str();

	//extract Equations section;
	const STLString divider = "\\\\\\---///";
	size_t cutoff = src.find(divider);
	STLString eqns = src.substr(0, cutoff);
	//further shrink if .control is present
	size_t subCut = eqns.find(".Control");
	if (subCut != STLString::npos)
		eqns = eqns.substr(0,subCut);

	ParseEquations(eqns);
	
	Model* pRootMdl=AssembleModel();

	//process equations.
	UpdateEquations(pRootMdl);
	AssembleInfluences(pRootMdl);
	//process modifiers
	//modifiers are presently untested with Vensim stuff. May break when used.
	DBG_PRINT("Applying modifiers.");
	OMEArray omittot;
	for (auto itr = m_modifiers.begin(); itr != m_modifiers.end(); ++itr)
	{
		if ((*itr).first != NULL)
		{
			OMEObject* keyPtr = (*itr).first;
			ModifierProcessor::ProcessObject(keyPtr, (*itr).second, omittot, NULL, ConvertExpression);
			if (keyPtr != (*itr).first)
			{
				DrawDetails::ReplaceObjectForDD((*itr).first, keyPtr);
			}
			//if omissions occurred, future pointers may have been invalidated. set those pointers to null.
		}
	}

	return pRootMdl;
}

/** Parse equations out of a block of text.
	This is intended to parse the Equations section of an MDL file.
	@param block The block of text to parse.
*/
void MDLParser::ParseEquations(STLString block)
{
	DBG_PRINT("Parsing Equations");
	DBG_PUSHLEVEL();
	size_t startPos = 0;
	

	//scrub groups
	oSTLSStream outBuff;
	STLsmatch eqnMtch;
	STLsmatch commMtch;

	//size_t pos = block.find("****");
	//while (pos!=STLString::npos)
	//{
	//	outBuff << block.substr(0, pos);
	//	block = block.substr(block.find('|') + 1);
	//	pos = block.find("****");
	//}

	//outBuff << block;

	//block = outBuff.str();

	//remove all new lines
	DBG_PRINT("Purging extra newlines.");

	oSTLSStream nlBuff;
	for (OMEChar & c : block)
	{
		switch (c)
		{
		case '\\':
		case '\r':
		case '\n':
			break;
		default:
			nlBuff << c;
		}
	}
	block = nlBuff.str();

	STLRegex eqnReg("^[\\s]*(.*?)[\\s]*(?:[:]?[=][=]?|<->|:[Ii][Ss]:)[\\s]*(.*?)~.*$");
	STLRegex commReg("~[\\s]*(.*?)[\\s]*~[\\s]*(.*?)[\\s]*[|]?$");
	
	//fix below for table / comment stuff
	enum { NAME = 1, EQN };						//eqn fields
	enum { UN = 1, COMM };						//comment fields

	size_t endPos = block.find('|');

	while (startPos!= STLString::npos &&  endPos != STLString::npos)
	{
		size_t len = endPos != STLString::npos ? endPos - startPos : STLString::npos;
		STLString searchStr = block.substr(startPos, len);

		std::regex_match(searchStr, commMtch, commReg);

		STLString id;
		
		if (std::regex_match(searchStr, eqnMtch, eqnReg))
		{
			//new equation record entry.
			id = eqnMtch[NAME];
			MDLEqnRecord newRec(id, eqnMtch[EQN], commMtch[UN], commMtch[COMM]);
			m_eqnMap[newRec.GetVariable()] = newRec;
			DBG_PRINT("Recorded Equation for '" + newRec.GetVariable() + "'");
		}
		else
		{
			EvalTable::DataArray xVals;
			EvalTable::DataArray yVals;
			AssembleInterpData(searchStr.substr(0,searchStr.find('~')), xVals, yVals, id);

			if (!id.empty())
			{
				MDLEqnRecord newRec(id, xVals, yVals, commMtch[UN], commMtch[COMM]);
				m_eqnMap[newRec.GetVariable()] = newRec;
				DBG_PRINT("Recorded new table entry '" + newRec.GetVariable() + "'");
			}

		}
		startPos = endPos + 1;

		if (startPos!=STLString::npos)
			endPos = block.find('|', startPos);
	}
	DBG_POPLEVEL();
}

/** Parse through the views section of the .MDL file.
	Presently, most of this information is unused by OME.
	@param block The block of text to parse.
*/
void MDLParser::ParseRepView(const STLString & block)
{
	StrArray lines = SplitString(block);

	DBG_PRINT("Parsing Sketch view.");
	DBG_PUSHLEVEL();
	for (size_t i = 0; i < lines.size();++i)
	{
		int objType;
		//split on commas
		StrArray fields = SplitString(lines[i], ',');
		if (!fields.empty())
		{
			try
			{
				objType = std::stoi(fields[0]);
				switch (objType)
				{
				case T_ARROW:
					//new arrow record
					AddArrowEntry(fields);
					break;
				case T_VAR:
				case T_VALVE:
				case T_COMMENT:
				case T_BITMAP:
				case T_METAFILES:

					DBG_PRINT("Recorded new sketch object.");
					AddObjectEntry(fields, i + 1 < lines.size() ? lines[i + 1] : STLString());
					break;
				default:
					DBG_PRINTC("Warning: unidentified type flag: " + fields[0], DBG_YELLOW);
				}
			}
			catch (...)//(std::invalid_argument *excpt)
			{
				//entry is not a number; skip to next iteration
				continue;
			}
		}
	}

	DBG_POPLEVEL();
}

/** Add an arrow entry to the internal registry.
	@param parts The entries supplied by the comma-delimited entry from the .MDL file.
*/
void MDLParser::AddArrowEntry(StrArray & parts)
{
	enum {TYPE=0,ID,FROM,TO,SHAPE,HID,POL,THICK,HASF,DTYPE,RES,COLOR,FONT,PLIST};

	ArrowEntry newEntry;
	newEntry.id = parts[ID];
	newEntry.from = parts[FROM];
	newEntry.to = parts[TO];
	newEntry.shape = std::stoi(parts[SHAPE]);
	newEntry.hidden = std::stoi(parts[HID]);
	newEntry.thickness = std::stoi(parts[THICK]);

	STLString pList = parts[PLIST];
	StrArray ptParts = SplitString(pList, '|');
	newEntry.numPoints = stoi(ptParts[0]);
	
	STLRegex ptReg("^[(]([0-9]+),([0-9]+)[)]$");
	STLsmatch ptMtch;
	for (int i = 1; i <= newEntry.numPoints; ++i)
	{
		std::regex_match(ptParts[i], ptMtch, ptReg);
		newEntry.pts.emplace_back(std::stoi(ptMtch[1].str()), std::stoi(ptMtch[2].str()));
	}
	
	DBG_PRINT("Recorded new arrow "+newEntry.id);
	m_arrRecords.push_back(newEntry);
}

/** Add an object entry to the internal registry. 
	@param parts The entries supplied by the comma-delimited entry from the .MDL file.
	@param next The next line following the line constituting parts. Some entries require the second
	line (such as comments).
*/
void MDLParser::AddObjectEntry(StrArray & parts,const STLString & next)
{
	enum {TYPE=0,ID,NAME,X,Y,W,H,SH,BITS,HID,HASF,TPOS,BW,NAV1,NAV2,BOX,FILL,FONT};

	
	ObjEntry newEntry;

	newEntry.type = std::stoi(parts[TYPE]);
	newEntry.id = parts[ID];

	newEntry.name = MDLEqnRecord::FixIllegalChars(MDLEqnRecord::Dequote(ToLowerCase(parts[NAME])));
	int x = std::stoi(parts[X]);
	int y = std::stoi(parts[Y]);
	newEntry.dims = OMEBox(x, y, x + std::stoi(parts[W]), y + std::stoi(parts[H]));
	newEntry.shape = (char)std::stoi(parts[SH]);
	newEntry.bits = (char)std::stoi(parts[BITS]);
	newEntry.hidden = std::stoi(parts[HID]);

	//check to see if we have a comment
	if (newEntry.bits & 4)
		newEntry.comment = next;
	
	m_objRecMap[newEntry.name] = newEntry;

	DBG_PRINT("Recorded sketch details for " + newEntry.name);
}

/** Assemble a model object from all parsed information.
	@return A Pointer to a newly allocated Model component.
*/
Model* MDLParser::AssembleModel()
{
	StrArray flowIDs;

	Model* pRoot = new Model("root", "root");

	STLRegex integReg("^[\\s]*integ[\\s]*[(](.*)[)][\\s]*$");
	STLRegex dfReg("^[\\s]*delay_fixed[\\s]*[(](.*,.*,.*)[)][\\s]*$");
	STLRegex ifReg("(?:[+]|^)[\\s]*([_a-zA-Z0-9'$]+)(?=[\\s+-]|$)");
	STLRegex ofReg("[-][\\s]*([_a-zA-Z0-9'$]+)(?=[\\s+-]|$)");
	STLsmatch mtch;
	


	DrawDetails* pDD=new DrawDetails(pRoot);

	//find State vars with integ
	
	DBG_PRINT("Parsing out StateVars");
	DBG_PUSHLEVEL();
	for (auto & rec : m_eqnMap)
	{
		STLString searchEqn = rec.second.GetEquation();

		if (std::regex_match(searchEqn, mtch, integReg))
		{
			STLString args = mtch[1];

			//find argument split point
			size_t pos = 0;
			size_t depth = 0;
			for (; pos < args.length() && (depth || args[pos] != ','); ++pos)
			{
				switch (args[pos])
				{
				case '(':
					++depth;
					break;
				case ')':
					--depth;
					break;
				}
			}

			if (pos < args.length())
			{

				STLString flows = args.substr(0, pos);
				STLString expr = args.substr(pos + 1);

				//grab SV record
				//ObjEntry & svEntry = m_objRecMap[rec.first];
				StateVar* pSV = new StateVar(GenUniqueVarID("sv"), rec.second.GetName());
				DBG_PRINT(pSV->GetExprName()+ " (State Variable)");
				pSV->SetExpr(expr);
				pRoot->AddStateVar(pSV);
				pDD = new DrawDetails(pSV);
				STLString desc;
				if (!rec.second.GetComment().empty())
					desc = rec.second.GetComment() + "\n";
				pDD->SetDescription(desc);
				m_modifiers[pSV] = ModifierProcessor::ExtractExpression(desc);

				//register inflows
				STLString currFlow = flows;
				STLString key;
				while (std::regex_search(currFlow, mtch, ifReg))
				{
					flowMap[pSV->GetID()].inflows.push_back(mtch[1].str());
					currFlow = mtch.suffix();
				}

				//register outflows
				currFlow = flows;
				while (std::regex_search(currFlow, mtch, ofReg))
				{
					flowMap[pSV->GetID()].outflows.push_back(mtch[1].str());
					currFlow = mtch.suffix();
				}

				//mark entry as processed by setting type to 0
				//svEntry.type = 0;

				rec.second.SetHasBeenApplied(true);
			}
			else
				DBG_PRINTC("Error: Malformed INTEG call for " + rec.first, DBG_RED);

		}
		else if (std::regex_match(searchEqn, mtch, dfReg))
		{
			/*
			DELAY FIXED (input, delay time,initial value)
			where
				input = inflow
				delay time= transit time
				initial value = initial expression (expr)
			*/

			StrList argList;
			CPPLoopCluster::FindFuncArgs(mtch[1].str(), argList);
			if (argList.size() == 3)
			{
				auto argItr = argList.begin();

				//create new conveyor + input flow
				Conveyor* pC = new Conveyor(GenUniqueVarID("cv"), rec.second.GetName());
				DBG_PRINT(pC->GetExprName() + " (Conveyor)");
				//add input flow
				Flow* pCFlow = new Flow(GenUniqueVarID("cif"), "", pC->GetID(), NULL, pC);
				pCFlow->SetExpr(*argItr);
				pRoot->AddFlow(pCFlow);
				pDD = new DrawDetails(pCFlow);
				pC->SetTransitExpr((*++argItr));
				pC->SetExpr((*++argItr));
				pRoot->AddStateVar(pC);
				pDD = new DrawDetails(pC);
				STLString desc;
				if (!rec.second.GetComment().empty())
					desc = rec.second.GetComment() + "\n";
				pDD->SetDescription(desc);
				m_modifiers[pC] = ModifierProcessor::ExtractExpression(desc);

				rec.second.SetHasBeenApplied(true);
			}
			else
				DBG_PRINTC("Error: Malformed DELAY_FIXED call for " + rec.first, DBG_RED);
		}
	}
	DBG_POPLEVEL();

	//use flow records to build flows
	DBG_PRINT("Constructing Flows");

	/*
		New flow strategy (due to old method producing redundant entry).
		-each variable referenced in stateVar is saved as normal variable
		-each inclusion represents an inflow or out flow for each statevar
		-all net inflows and all net outflows are used to generate an inflow and outflow per state var
		-While there will be no direct traversal between state variables, the net effect will be the same.
	*/
	DBG_PUSHLEVEL();

	for (auto & fE : flowMap)
	{
		if (!fE.second.inflows.empty())
		{
			Flow* pInFlow = new Flow(GenUniqueVarID("f"), STLString(),fE.first);
			
			pInFlow->SetName(fE.first + "_inflow");
			pInFlow->SetExpr(ConcatStrings(fE.second.inflows, '+'));
			pDD = new DrawDetails(pInFlow);
			pRoot->AddFlow(pInFlow);

			DBG_PRINT("New In Flow '" + pInFlow->GetExprName() + "'");
		}

		if (!fE.second.outflows.empty())
		{
			Flow* pOutFlow = new Flow(GenUniqueVarID("f"), fE.first, STLString());

			pOutFlow->SetName(fE.first + "_outflow");
			pOutFlow->SetExpr(ConcatStrings(fE.second.outflows, '+'));
			pDD = new DrawDetails(pOutFlow);
			pRoot->AddFlow(pOutFlow);

			DBG_PRINT("New Out Flow '" + pOutFlow->GetExprName() + "'");
		}
	}
	DBG_POPLEVEL();

	for (auto & rec : m_eqnMap)
	{
		if (!rec.second.GetHasBeenApplied())
		{

			if (!pRoot->ObjectForExprName(rec.first))
			{
				//variable or table
				EvalTable* pTbl = rec.second.GenTable();
				if (!pTbl)
				{
					Variable* pNewVar = new Variable(GenUniqueVarID("v"), rec.second.GetName(), 0.0);
					pNewVar->SetExpr(rec.second.GetEquation());
                    
					STLString desc;
					if (!rec.second.GetComment().empty())
						desc = rec.second.GetComment() + "\n";

					pDD = new DrawDetails(pNewVar);
					pDD->SetDescription(desc);
					m_modifiers[pNewVar] = ModifierProcessor::ExtractExpression(desc);
					pRoot->AddVariable(pNewVar);
					DBG_PRINT("New Variable '" + pNewVar->GetExprName() + "'");
				}
				else
				{
					//more here?
					pRoot->AddEvalTable(pTbl);
					DBG_PRINT("Table " + pTbl->GetID() + " added.");
				}
			}
		}
	}

	//Variable* pNewVar;
	//STLString desc;
	//EvalXSetInterpTable* pTbl;
	//DBG_PRINT("Processing Variables and tables");
	//DBG_PUSHLEVEL();

	////assume all other entries are variables or tables
	//for (auto & rec:m_objRecMap)
	//{
	//	switch (rec.second.type)
	//	{
	//	case T_ARROW:
	//	case T_UNDEF:
	//		//already handled
	//		break;
	//	case T_VAR:
	//		
	//		pTbl = m_eqnMap[rec.second.name].GenTable();
	//		if (!pTbl)
	//		{
	//			pNewVar = new Variable("n_" + rec.second.id, rec.first, 0.0);
	//			pNewVar->SetExpr(m_eqnMap[rec.second.name].GetEquation());

	//			if (!m_eqnMap[rec.second.name].GetComment().empty())
	//				desc = m_eqnMap[rec.second.name].GetComment() + "\n";
	//			if (!rec.second.comment.empty())
	//				desc += rec.second.comment;

	//			pDD = new DrawDetails(pNewVar);
	//			pDD->SetDescription(desc);
	//			pRoot->AddVariable(pNewVar);
	//			DBG_PRINT("New Variable '" + pNewVar->GetExprName() + "'");
	//		}
	//		else
	//		{
	//			//more here?
	//			pRoot->AddEvalTable(pTbl);
	//			DBG_PRINT("Table "+pTbl->GetID()+" added.");
	//		}

	//		break;
	//	case T_COMMENT:	
	//		//skip
	//		break;
	//	case T_VALVE:	
	//	case T_BITMAP:
	//	case T_METAFILES:
	//		//valid but presently unimplemented.
	//		DBG_PRINTC("Request for unimplemented object of type " + std::to_string(rec.second.type), DBG_YELLOW);
	//		break;
	//	default:
	//		DBG_PRINTC("Error unknown type " + std::to_string(rec.second.type), DBG_RED);
	//	}
	//}

	DBG_POPLEVEL();

	//use arrows to build influences
	//sketch records will be incomplete; instead build from identifiers while parsing vars
	//DBG_PRINT("Wiring Influences");
	//for (ArrowEntry & ae : m_arrRecords)
	//{
	//	STLString theId = "infl_" + ae.id;
	//	Influence* pInfl = new Influence(theId, theId, pRoot->ObjectForExprName(ae.from), pRoot->ObjectForExprName(ae.to));
	//	pDD = new DrawDetails(pInfl);
	//	//for now just pass in first point
	//	pDD->SetCtrlPt1(ae.pts.front());

	//	pRoot->AddInfluence(pInfl);
	//}

	pRoot->RefreshObjectMaps();
	
	return pRoot;
}

/** Update all equations to OME-friendly expressions.
	@param pMdl Pointer to Model which contains all objects to be updated.
*/
void MDLParser::UpdateEquations(Model* pMdl)
{
	
	EvalArray evals;
	pMdl->GetAllEvals(evals, DT_FULL);


	for (Evaluable* pE : evals)
	{
		pE->SetExpr(ProcessExpression(pE, pMdl, pE->GetExpr()));
	
		if (pE->IsOMEType(OME_CONVEYOR))
			((Conveyor*)pE)->SetTransitExpr(ProcessExpression(pE, pMdl, ((Conveyor*)pE)->GetTransitExpr()));
	}
}

/** Convert an expression from Vensim to OME. 
	@param pEval Pointer to the Evaluable object containing the equation.
	@param pRootMdl Pointer to the top-level Model Component.
	@param expr The expression being evaluated.
	@return The OME version of the expression.
*/
STLString MDLParser::ProcessExpression(Evaluable* pEval, Model* pRootMdl, STLString expr)
{

	expr = ProcessQuotedVariables(pRootMdl, expr);
	const OMEChar* replacePats[] = {
		":not:", " ! ",
		"<>", "!=",
		"([^<>!=]|^)=(?=[^=]|$)", "$1==",
		":and:", " and ",
		":or:", " or ",
		"\t", " ",
		"\0"
	};

	static std::map<STLString, STLString> specialVars;
	if (specialVars.empty())
	{
		specialVars["time"] = "time()";
		specialVars["initial_time"] = "init_time()";
		specialVars["final_time"] = "end_time()";
		specialVars["time_step"] = "dt(0)";
		specialVars["saveper"] = "dt(0)";
		//...
	}

	const OMEChar* reserved[] = { "^if$", "^then$", "^else$", "^mod$", "^and$", "^or$", "\0" };

	const STLRegex funcValReg("\\s*\\b([a-zA-Z_][a-zA-Z0-9_]*)\\s*([(])?");
	STLsmatch mtch;

	//perform simple substitutions
	expr = BatchRegexReplace(expr, replacePats);
	oSTLSStream buff;
	//parse identifiers
	while (std::regex_search(expr, mtch, funcValReg))
	{
		buff << mtch.prefix();
	

		STLString sfx = mtch.suffix();
		//determine if we have a function or identifier
		if (mtch[2].matched)
		{
			//get arguments
			size_t startPos = mtch.position(0) + mtch.length(0);
			size_t endPos = FindMatching(expr, startPos - 1, '(', ')');
			size_t len = (endPos - startPos);

			//function

			buff << ProcessFunction(pEval, mtch[1], ProcessExpression(pEval, pRootMdl, expr.substr(startPos, len)));
			expr = expr.substr(startPos + len + 1);
		}
		else
		{
			//skip any matched reserved words
			if (!BatchRegexMatchAny(mtch[1], reserved, 0))
			{
				//check if it is a special identifier.
				auto fItr = specialVars.find(mtch[1].str());
				if (fItr == specialVars.end())
				{
					//identifier
					OMEObject* pObj = pRootMdl->ObjectForInsensitiveName(mtch[1]);

					if (pObj)
					{

						StrList pathParts;
						pObj->GetParentModel()->GetUpPath(pathParts);
						pathParts.push_back(pObj->GetExprName());
						buff << ConcatStrings(pathParts, '.');
					}
					else
					{
						//err
					}
				}
				else
				{
					buff << (*fItr).second;
				}
			}
			else
				buff << mtch[0].str();
			expr = mtch.suffix();
		}

	}

	buff << expr;

	return buff.str();
}

/** Convert a quoted name into a name that conforms to identifier restrictions. 
	@param pRootMdl Pointer to the root Model component.
	@param expr The expression to be evaluated.
	@return The dequoted expression.
*/
STLString MDLParser::ProcessQuotedVariables(Model* pRootMdl, const STLString & expr)
{
	bool inQuote = false;
	oSTLSStream outBuff;
	size_t lastPos = 0;
	for (size_t i = 0; i < expr.size(); ++i)
	{
		if (expr[i] == '"')
		{
			if (inQuote)
			{
				OMEObject* pObj = pRootMdl->ObjectForInsensitiveName(expr.substr(lastPos, i - lastPos + 1));
				if (pObj)
					outBuff << pObj->GetExprName();
				else
				{
					STLString msg = "Undefined object label: " + expr.substr(lastPos, i - lastPos + 1);
					DBG_PRINTC(msg, DBG_RED);
					throw OMEException(msg.c_str());
				}
				lastPos = i + 1;
			}
			else
			{
				outBuff << expr.substr(lastPos, i - lastPos);
				lastPos = i;
			}
			inQuote = !inQuote;
		}
	}
	outBuff << expr.substr(lastPos);
	return outBuff.str();
}

/** Process a Vensim function for use with OME.
	__NOTE:__ The function conversion list is presently incomplete.
	@param pCaller The Evaluable object that is calling the function. 
	@param funcName The identifier of the function being called.
	@param args The arguments passed into the function.
	@return The OME-friendly equivalent of the Vensim function call.
*/
STLString MDLParser::ProcessFunction(Evaluable* pCaller, const STLString & funcName, const STLString & args)
{
	STLString errRet = funcName + "(" + args + ")";

	//table for mapping functions
	static const std::map<STLString, STLString> funcTable{
        {"a_function_of",               "@ignore"/*can be ignored*/         },
        {"abs",                         "abs"                               },
        {"active_initial",              "@activeInit"  /*untested*/         },
        {"allocate_available",          "@unimplemented"                    },
        {"allocate_by_priority",		"@unimplemented"                    },
        {"alloc_p",						"@unimplemented"                    },
        {"arccos",						"acos"								},
        {"arcsin",						"asin"								},
        {"arctan",						"atan"								},
        {"cos",							"cos"								},
        {"cosh",						"cosh"								},
        {"cumulate",					"subtotals"							},
        {"cumulatef",					"sum"								},
        {"delay_batch",					"" /* implement with conveyor*/     },
        {"delay_conveyor",				"" /* implement with conveyor*/     },
        {"delay_fixed",					"@ignore"      						}, 
        {"delay_information",			"" /* implement with conveyor*/     },
        {"delay_material",				"" /* implement with conveyor*/     },
        {"delay_n",						"delayn($1,$2,$4,$3)"				}, 
        {"delay_profile",				"" /* ??? */                        },
        {"delay1",						"delay1"							}, 
        {"delay1i",						"delay1"							}, 
        {"delay3",						"delay3"							}, 
        {"delay3i",						"delay3"							}, 
        {"delayp",						""	/* break in to delay 3 and SV*/ },
        {"demand_at_price",				"@unimplemented"                    },
        {"depreciate_by_schedule",		"@unimplemented"                    },
        {"depreciate_straightline",		"@unimplemented"                    },
        {"elmcount",					"count"								},
        {"exp",							"exp"								},
		{"find_market_price",           "@unimplemented"                    },
        {"find_zero",					"@unimplemented"                    },
        {"forecast",					"forcst"							},
        {"game",						"$1"								},
        {"gamma_ln",					"log(factorial($1-1))"				},
        {"get_123_constants",			"@unimplemented"                    },
        {"get_123_data",				"@unimplemented"                    },
		{"get_123_lookups",				"@unimplemented"                    },
        {"get_data_at_time",			""	/* implement*/					},
        {"get_data_between_times",		""	/* implement*/					},
        {"get_data_first_time",			""	/* implement*/					},
        {"get_data_last_time",			""	/* implement*/					},
        {"get_data_total_points",		""	/* ??? */						},
        {"get_xls_constants",			"@unimplemented"                    },
        {"get_xls_data",				"@unimplemented"                    },
		{"get_xls_lookups",				"@unimplemented"                    },
		{"get_xls_subscript",			"@unimplemented"                    },
        {"if_then_else",				" if $1 then $2 else $3 end "		},
        {"initial",						"initOnly"							},	//!!
        {"integ",						"@ignore"							},
        {"integer",						"int"								},
        {"internal_rate_of_return",		"@unimplemented"                    },
        {"invert_matrix",				"@unimplemented"					},
        {"ln",							"log"								},
        {"log",							"log"								},
        {"lookup_area",					""	/* implement ?? */				},
        {"lookup_backward",				""	/* implement ?? */				},
        {"lookup_extrapolate",			""	/* implement ?? */				},
        {"lookup_forward",				""	/* implement ?? */				},
        {"lookup_invert",				""	/* implement ?? */				},
        {"lookup_slope",				""	/* implement ?? */				},
        {"marketp",						"@unimplemented"                    },
        {"max",							"max"								},
        {"message",						""	/* implement*/					},
        {"min",							"min"								},
        {"modulo",						"fmod"								},
        {"npv",							"@unimplemented"                    },
        {"npve",						"@unimplemented"                    },
        {"net_present_value",			"@unimplemented"                    },
        {"power",						"pow"								},
        {"prod",						"product"							},
        {"pulse",						"pulse($2,$1)"						}, //arguments need to be flipped
        {"pulse_train",					""	/* implement ?? */				},
        {"quantum",						""	/* implement */					},
        {"queue_age_average",			"@unimplemented"					},
        {"queue_age_in_range",			"@unimplemented"					},
        {"queue_age_oldest",			"@unimplemented"					},
        {"queue_attrib_average",		"@unimplemented"					},
        {"queue_attrib_in_range",		"@unimplemented"					},
        {"queue_attrib_max",			"@unimplemented"					},
        {"queue_attrib_min",			"@unimplemented"					},
        {"queue_attrib_quantity",		"@unimplemented"					},
        {"queue_fifo",					"@unimplemented"					},
        {"queue_fifo_attrib",			"@unimplemented"					},
        {"ramp",						"" /* implement similar to step*/	},
		{"random_0_1",                  "rand_var(0,1)"                     },
        {"random_beta",					"beta_ran($3,$4,$1,$2,$5,$6)"		},
        {"random_binomial",				"binome($3,$4,$1,$2,$5,$6)"			},
        {"random_exponential",			"exponent_ran($1,$2,$3,$4)"			},
		{"random_gamma",				"gamma_ran($3,$5,$1,$2,$4)"		    },
        {"random_lookup",				"" /* implement */					},
        {"random_negative_binomal",		"negBinome_ran($3,$4,$1,$2,$5,$6)"  },
        {"random_normal",				"gaussian_var($3,$4,$1,$2)"			},
        {"random_poisson",				"poidev($3,$1,$2,$4,$5)"         	},
		{"random_triangular",			"triangle_ran($3,$5,$4,$1,$2)"		},
        {"random_uniform",				"rand_var($1,$2)"					},
        {"random_weibull",				"weib_ran($3,$5,$1,$2,$4)"			},
        {"rc_compare",					"@unimplemented"					},
        {"rc_compare_check",			"@unimplemented"					},
        {"rc_decay",					"@unimplemented"					},
        {"rc_decay_check",				"@unimplemented"					},
        {"rc_grow",						"@unimplemented"					},
        {"rc_grow_check",				"@unimplemented"					},
        {"rc_ramp",						"@unimplemented"					},
        {"rc_ramp_check",				"@unimplemented"					},
        {"rc_step",						"@unimplemented"					},
        {"rc_step_check",				"@unimplemented"					},
        {"reinitial",					""									},
        {"sample_if_true",				""									},
        {"shift_if_true",				""									},
        {"sin",							"sin"								},
        {"sinh",						"sinh"								},
        {"sinteg",						""									},
        {"smooth",						"smth1"								},
        {"smoothi",						"smth1"								},
        {"smooth_n",					"smthn"								},
        {"smooth3",						"smth3"								},
        {"smooth3i",					"smth3"								},
        {"sqrt",						"sqrt"								},
        {"step",						"step"								},//!!
        {"sum",							"sum"								},
        {"supply_at_price",				"@unimplemented"                    },
        {"tabbed_array",				""									},
        {"tan",							"tan"								},
        {"tanh",						"tanh"								},
        {"time_base",					""									},
        {"time_shift",					""									},
        {"trend",						"trend"								}, //!!
        {"vector_elm_map",				""									},
        {"vector_rank",					""									},
        {"vector_reorder",				""									},
        {"vector_select",				""									},
        {"vector_sort_order",			""									},
        {"vmax",						""									},
        {"vmin",						""									},
        {"with_lookup",					"@interpTable"						},//!!
        {"x_if_missing",				""									},
        {"xidz",						"if abs($2)<1e-6 then $3 else ($1)/($2) end"	},
        {"zidz",						"if abs($2)<1e-6 then 0.0 else ($1)/($2) end"	}
	};

	auto fItr = funcTable.find(funcName.c_str());

	//args pattern for now assume we won't handle any more than 3

	if (fItr != funcTable.end())
	{
		STLString funcPattern = (*fItr).second;
		if (funcPattern.empty())
		{
			//err
			return errRet;
		}

		//check for simple substitution
		if (funcPattern[0] != '@' && funcPattern.find('$') == STLString::npos)
		{
			//if predefined argument list, just return
			if (funcPattern.find('(') != STLString::npos)
				return funcPattern;

			//simple substitution
			return funcPattern + "(" + args + ")";
		}
		else if (funcPattern[0] == '@')
		{
			//special commands
			if (funcPattern == "@interpTable")
			{
				STLString argIn = args.substr(0, args.find(','));
				//assemble equation for use with interp table scraper
				STLString dummyEqn = pCaller->GetID() + "_table" + args.substr(args.find(',') + 1);
				EvalTable::DataArray xVals;
				EvalTable::DataArray yVals;
				STLString tblID;
				AssembleInterpData(dummyEqn, xVals, yVals, tblID);
				EvalXSetInterpTable* pTbl = new EvalXSetInterpTable(tblID, xVals, yVals);
				pCaller->GetParentModel()->AddEvalTable(pTbl);
				pCaller->AppendTableID(tblID);

				return "graph(" + argIn + ")";
			}
			else if (funcPattern == "@activeInit")
			{
				StrList subArgs;
				CPPLoopCluster::FindFuncArgs(args, subArgs);
				auto itr = subArgs.begin();
				//I think if we reach this point, both arguments should always be processed,
				//so we don't need to call process expression again.
				pCaller->SetExpr(*itr);
				++itr;
				pCaller->SetInitExpr(*itr);

				return pCaller->GetExpr();
			}
			else if (funcPattern == "@unimplemented")
			{
				//err
				DBG_PRINTC("Vensim function '" + funcName + "' is unimplemented in OME.",DBG_RED);
				return errRet;
			}
			else if (funcPattern == "@ignore")
			{
				return STLString();
			}
			//more to come
			else
			{
				//err
			}

		}
		else
		{

			StrArray repPatterns;
			int i = 1;
			StrList argList;
			CPPLoopCluster::FindFuncArgs(args, argList);
			for (STLString & a : argList)
			{
				repPatterns.push_back("[$]" + std::to_string(i));
				repPatterns.push_back(a);
				++i;
			}


			//substitution pattern
			return BatchRegexReplace(funcPattern, repPatterns);
		}
	}
	else
	{
		//check for table
		EvalTable* pTbl = pCaller->GetParentModel()->GetEvalTable(funcName);
		if (pTbl)
		{
				size_t tblInd=pCaller->AppendTableID(pTbl->GetID());
				return "graph(" + args + ","+std::to_string(tblInd)+")";
		}
		else
		{
			STLString msg = "Undefined function label: " + funcName;
			throw OMEFuncOpException("MDLParser::ProcessFunction", msg.c_str());
		}
	}
		
	//err
	return errRet;
	
}

/** Assemble and register the data for an interpolated table.
	@param inExpr The expression defining the table.
	@param outXVals populated with the defined x-values on return.
	@param outYVals populated with the defined y-values on return.
	@param outID contains the ID of the newly created table object on return.
*/
void MDLParser::AssembleInterpData(const STLString & inExpr, EvalTable::DataArray & outXVals, EvalTable::DataArray & outYVals, STLString & outID)
{
	

	//scrub groups
	STLRegex starReg("[*]{4,}");
	oSTLSStream outBuff;
	STLsmatch eqnMtch;
	STLsmatch tblMtch;
	STLsmatch commMtch;
	STLsmatch ptMtch;


	STLRegex tblReg("^[\\s]*([^=]*?)[\\s]*[(]\\s*(?:\\[\\(([-+]?[0-9.]+(?:[eE][+-][0-9]+)?),([-+]?[0-9.]+(?:[eE][+-][0-9]+)?)\\)-\\(([-+]?[0-9.]+(?:[eE][+-][0-9]+)?),([-+]?[0-9.]+(?:[eE][+-][0-9]+)?).*\\)\\],)?((?:\\(\\s*[-+]?[0-9.]+(?:[eE][+-][0-9]+)?\\s*,\\s*[-+]?[0-9.]+(?:[eE][+-][0-9]+)?\\s*\\)\\s*,?\\s*)+)[)].*$");
	STLRegex tblAltReg("^[\\s]*([^=]*?)[\\s]*[(](.*)[)].*$");
	STLRegex ptReg("\\(\\s*([-+]?[0-9.]+(?:[eE][+-][0-9]+)?)\\s*,\\s*([-+]?[0-9.]+(?:[eE][+-][0-9]+)?)\\s*\\)");
	
	//fix below for table / comment stuff
	enum { TBL = 1, XMIN, YMIN, XMAX, YMAX, PTS };	//table fields
	enum { XPT = 1, YPT };						//point fields

	if (std::regex_match(inExpr, tblMtch, tblReg))
	{
		
		//this will be an xset interp table
		//scrape out max and min
		//NOTE: max/min in .mdl is known to be unrealiable;
		//start with min/max here, but replace with other data points.
		//if lower/higher values are found.
//		OME_SCALAR xMin = OME_SCLR_READSTR(tblMtch[XMIN].str());
//		OME_SCALAR xMax = OME_SCLR_READSTR(tblMtch[XMAX].str());
//		OME_SCALAR yMin = OME_SCLR_READSTR(tblMtch[YMIN].str());
//		OME_SCALAR yMax = OME_SCLR_READSTR(tblMtch[YMAX].str());
		STLString pointStr = tblMtch[PTS].str();
		
		while (std::regex_search(pointStr, ptMtch, ptReg))
		{
			outXVals.push_back(OME_SCLR_READSTR(ptMtch[XPT].str()));
			outYVals.push_back(OME_SCLR_READSTR(ptMtch[YPT].str()));
			pointStr = ptMtch.suffix();
		}

		//if (outXVals[0] > xMin)
		//{
		//	outXVals.push_front(xMin);
		//	outYVals.push_front(yMin);
		//}
		//
		//if (outXVals.back() < xMax)
		//{
		//	outXVals.push_back(xMax);
		//	outYVals.push_back(yMax);
		//}

		outID = tblMtch[TBL];
	}
	else if (std::regex_match(inExpr, tblMtch, tblAltReg))
	{
		STLString valStr = tblMtch[2];
		EvalTable::DataArray rawVals;
		STLRegex valReg("^[\\s]*([-+]?[0-9.]+)(?:,|$)");
		while (std::regex_search(valStr, ptMtch, valReg))
		{
			rawVals.push_back(OME_SCLR_READSTR(ptMtch[1].str()));
			valStr = ptMtch.suffix();
		}

		size_t i = 0;
		size_t mid = rawVals.size() / 2;
		for (; i < mid; ++i)
		{
			//assumes rawVals is evenly divisible
			outXVals.push_back(rawVals[i]);
			outYVals.push_back(rawVals[i + mid]);
		}
		outID = tblMtch[TBL];
	}
	
}

/** Derive Influence arrows based on references in equations.
	@param pMdl Pointer to the Model to evaluate for Influences.
*/
void MDLParser::AssembleInfluences(Model* pMdl)
{
	//step 1: build a map of influences
	typedef std::pair<Evaluable*, Evaluable*> EvalPair;

	//using a set will prevent redundant entries
	std::set<EvalPair,bool(*)(const EvalPair&,const EvalPair &)> links([](const EvalPair& lhs, const EvalPair& rhs)
	{ return lhs.first->GetID() < rhs.first->GetID() || (lhs.first->GetID() == rhs.first->GetID() && lhs.second->GetID() < rhs.second->GetID()); });

	EvalArray evals;
	pMdl->GetAllEvals(evals, DT_FULL);

	const OMEChar* reserved[] = { "^if$", "^then$", "^else$", "^mod$", "^and$", "^or$", "^end$","\0" };
	STLRegex varReg("\\broot[.]([_a-zA-Z][_.a-zA-Z0-9]*(?![(]))\\b");
	STLsmatch mtch;
	STLString expr;
	for (Evaluable* pE : evals)
	{
		switch (pE->GetOMEType())
		{
		//case OME_STATEVAR:
		case OME_INFLUENCE:
			//skip
			break;
		default:
			//scrape equation for identifiers
			//use identifiers to lookup variables
			//build entry out of variables.
			expr = pE->GetExpr();
			while (std::regex_search(expr, mtch, varReg))
			{
				if (!BatchRegexMatchAny(mtch[0], reserved, 0))
				{
					Evaluable* pSrc=dynamic_cast<Evaluable*>(pMdl->ObjectForInsensitiveName(mtch[1]));
					if (pSrc)
						links.emplace(pSrc, pE);
					else
						DBG_PRINTC("Error: unmatched model object: " + mtch[0].str(), DBG_RED);
				}
				expr = mtch.suffix();
			}
			
		}
	}

	
	//step 2: use said map to create influences
	for (auto ep : links)
	{
		STLString id = GenUniqueVarID("i");
		Influence* pInfl = new Influence(id, id, ep.first, ep.second);
		DrawDetails* pDD = new DrawDetails(pInfl);
		pMdl->AddInfluence(pInfl);
	}
}

/** Generate a unique ID for a variable.
	@param prefix The prefix to assign to the ID.
	@return A new variable ID.
*/
STLString MDLParser::GenUniqueVarID(const STLString & prefix)
{
	static int x = 0;
	return prefix + "_" + std::to_string(x++);
}

/** Converts an expression to an OME-friendly expression. 
	This is an adapter function for use with ModifierProcessor calls.
	@param str The string to convert.
	@param pObj Pointer to the associated object.
	@return The converted expression.
*/
STLString MDLParser::ConvertExpression(const STLString & str, OMEObject* pObj)
{
	return ProcessExpression((Evaluable*)pObj, pObj->GetParentModel(), str);
}