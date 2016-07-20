#include "OMEParser.h"
#include "omeobjects/Evaluable.h"
#include "CPPLoopCluster.h"

std::map<STLString, OMEParser::UnpackFunc> OMEParser::s_unpackFuncs;

//NOTE: most of the function expansions are taken from the
// iSee Systems documentation found here: http://www.iseesystems.com/Helpv9/Content/Builtins/Special_Functions.htm

/** Initializer for Function jump table. */
void OMEParser::BuildFuncLists()
{
	s_unpackFuncs.clear();
	s_unpackFuncs["delay1"] = UnpackDelay1;
	s_unpackFuncs["delay3"] = UnpackDelay3;
	s_unpackFuncs["delayn"] = UnpackDelayN;
	s_unpackFuncs["forcst"] = UnpackForcst;
	s_unpackFuncs["smth1"]  = UnpackSmth1;
	s_unpackFuncs["smth3"]  = UnpackSmth3;
	s_unpackFuncs["smthn"]  = UnpackSmthN;
	s_unpackFuncs["trend"]  = UnpackTrend;

}

/** Retrieve the appropriate Unpack function for an expression function name. 
	This function will initialize the jump table on its first call.
	@param call The expression function call that is a stand-in for an expanded model. 
	@return Pointer to the appropriate unpack function, or NULL if there is no unpack function
	associated with the value of call.
*/
OMEParser::UnpackFunc OMEParser::UnpackFuncForID(const STLString & call)
{
	if (s_unpackFuncs.empty())
		BuildFuncLists();

	UnpackFunc retFunc = NULL;
	auto fItr = s_unpackFuncs.find(call);

	if (fItr != s_unpackFuncs.end())
		retFunc = (*fItr).second;

	return retFunc;
}

OMEParser::OMEParser(void)
{
}


OMEParser::~OMEParser(void)
{
}

/** Removes and rewires model components that were generated and adjusted from an unpack.
	Presently this function is unused and unimplemented.
	@param pEval The target of the packing call.		
*/
void OMEParser::PerformPack(Evaluable* pEval)
{

}

/** Expansion and inject any model components needed for a specific family of functions
	when any one of them are called.
	@param pEval Pointer to the Evaluable to potentially unpack.	
*/
void OMEParser::PerformUnPack(Evaluable* pEval)
{
	oSTLSStream outBuff;

	STLString expr = pEval->GetExpr();

	STLRegex funcReg("([_a-zA-Z][_a-zA-Z0-9]+)\\s*([(])");
	STLsmatch mtch;

	bool unpackPerformed = false;
	while (std::regex_search(expr, mtch, funcReg))
	{
		outBuff << mtch.prefix();
		STLString sig = mtch[1].str();
		UnpackFunc UFunc = UnpackFuncForID(sig);
		if (UFunc)
		{
			unpackPerformed = true;
			size_t cPos = FindMatching(expr, mtch.position(2), '(', ')') + 1;
			STLString invocation = expr.substr(mtch.position(0), cPos - mtch.position(0));
			outBuff << UFunc(pEval, invocation);
			expr = expr.substr(cPos);
		}
		else
		{
			outBuff << mtch[0].str();
			expr=mtch.suffix();
		}
	}
	
	outBuff << expr;
	pEval->SetExpr(outBuff.str());

	if (unpackPerformed)
	{
		DBG_PRINTC("Unpacked " + pEval->GetExprName(), DBG_MAGENTA);
		//DBG_PRINTC("    " + pEval->GetExpr(), DBG_MAGENTA);
		//remove any obsolete influences
		EvalArray upObjs = pEval->GetUpObjects();
		for (Evaluable* pUE : upObjs)
		{
			if (pUE->IsOMEType(OME_INFLUENCE))
			{
				Influence* pInfl = ((Influence*)pUE);
				pUE = pInfl->GetPEvalSource();
				STLString searchStr;
				pUE->GetParentModel()->GetUpPath(searchStr);
				searchStr += "." + pUE->GetExprName();
				if (pEval->GetExpr().find(searchStr) == STLString::npos)
				{
					//remove influence
					pInfl->GetParentModel()->RemoveInfluence(pInfl);
					delete pInfl;
				}
			}
		}
	}
}


/** Grab the actual address of value stored in an Evaluable object
	@param pEval a pointer to the evaluable object.
	@return a pointer to the OME_SCALAR value stored in pEval.
*/
OME_SCALAR* OMEParser::GetValueAddress(Evaluable* pEval)
{
	return (OME_SCALAR*)pEval->GetValueAddr(); 
}

/** Unpack function for delay1 call.
	@param pCaller Pointer to the Evaluable to be expanded.
	@param call The delay1() call extracted from the expression function.
	@return The modified statement that is to be applied to pCaller that incorporates the new Model components.
*/
STLString OMEParser::UnpackDelay1(Evaluable* pCaller, const STLString & call)
{
	//               ___________
	//				|           |
	//  input value |           |  delay value
	// ============>|   stock   |==============>
	//				|           |
	//				|___________|
	//
	//

	STLString inStr, delDur, init;
	UnpackArgs1(call, inStr, delDur, init);

	//call helper
	return UnpackDelay(pCaller, inStr, delDur, 1, init);
}

/** Unpack function for delay3 call.
	@param pCaller Pointer to the Evaluable to be expanded.
	@param call The delay3() call extracted from the expression function.
	@return The modified statement that is to be applied to pCaller that incorporates the new Model components.
*/
STLString OMEParser::UnpackDelay3(Evaluable* pCaller, const STLString & call)
{
	//               ___________		  ___________		   ___________
	//				|           |		 |           |		  |           |
	//  input value |           |  		 |           |  	  |           | output value 
	// ============>|  stock 1  |=======>|  stock 2  |=======>|  stock 3  |===============>
	//				|           |		 |           |		  |           |
	//				|___________|		 |___________|		  |___________|
	//
	//

	STLString inStr, delDur, init;
	UnpackArgs1(call, inStr, delDur, init);
	//call helper
	return UnpackDelay(pCaller, inStr, delDur, 3, init);
}

/** Unpack function for delayn call.
	@param pCaller Pointer to the Evaluable to be expanded.
	@param call The delayn() call extracted from the expression function.
	@return The modified statement that is to be applied to pCaller that incorporates the new Model components.
*/
STLString OMEParser::UnpackDelayN(Evaluable* pCaller, const STLString & call)
{
	//               ___________		            		   ___________
	//				|           |		                 	  |           |
	//  input value |           |  		                   	  |           | output value 
	// ============>|  stock 1  |=======>    ....     =======>|  stock N  |===============>
	//				|           |		            		  |           |
	//				|___________|		             		  |___________|
	//
	//


	STLString inStr, delDur, orderStr, init;
	UnpackArgs2(call,inStr, delDur, orderStr, init);
	int order;
	try
	{
		order= stoi(orderStr);
	}
	catch (...)
	{
		Evaluable* pOrderVal=dynamic_cast<Evaluable*>(pCaller->GetParentModel()->GetRootModel()->NestedObjectForName(orderStr));
		try
		{
			order = (int)stod(pOrderVal->GetExpr());
		}
		catch (...)
		{
			throw OMEObjException("Object must be integer when used in order for DelayN", pOrderVal);

		}

	}

	//call helper
	return UnpackDelay(pCaller, inStr, delDur, order, init);
}

/** Unpack function for forcst call.
@param pCaller Pointer to the Evaluable to be expanded.
@param call The forcst() call extracted from the expression function.
@return The modified statement that is to be applied to pCaller that incorporates the new Model components.
*/
STLString OMEParser::UnpackForcst(Evaluable* pCaller, const STLString & call)
{
	STLString input, time, horizon, init;
	UnpackArgs2(call,input, time, horizon, init,false);

	//cache up objects
	EvalArray inpObjs, timeObjs, horObjs, initObjs;
	
	for (Evaluable* pEval : pCaller->GetUpObjects())
	{
		if (pEval->IsOMEType(OME_INFLUENCE))
			pEval = ((Influence*)pEval)->GetPEvalSource();

		STLString searchStr = pEval->GetFullMdlPathName();

		if (input.find(searchStr) != STLString::npos)
			inpObjs.push_back(pEval);
		if (init.find(searchStr) != STLString::npos)
			initObjs.push_back(pEval);
		if (time.find(searchStr) != STLString::npos)
			timeObjs.push_back(pEval);
		if (horizon.find(searchStr) != STLString::npos)
			horObjs.push_back(pEval);


	}

	oSTLSStream eqnBuilder;
	
	Model* pMdl = pCaller->GetParentModel();

	//add state var
	StateVar* pState = new StateVar(CPPLoopCluster::GenVarName("autoSV"), "");
	pState->SetName(pState->GetID());
	pState->SetAutoGenerated(true);
	pState->SetExpr(init);
	pMdl->AddStateVar(pState);
	
	//add variables
	Variable* pAvgTime = new Variable(CPPLoopCluster::GenVarName("autoVar"), "");
	pAvgTime->SetName(pAvgTime->GetID());
	pAvgTime->SetAutoGenerated(true);
	pAvgTime->SetExpr(time);
	pMdl->AddVariable(pAvgTime);

	Variable* pInp=new Variable(CPPLoopCluster::GenVarName("autoVar"), "");
	pInp->SetName(pCaller->GetID());
	pInp->SetAutoGenerated(true);
	pInp->SetExpr(input);
	pMdl->AddVariable(pInp);

	eqnBuilder << '(' << pInp->GetFullMdlPathName() << '-' << pState->GetFullMdlPathName() << ")/(" << pState->GetFullMdlPathName() << '*' << pAvgTime->GetFullMdlPathName() << ')';
	
	Variable* pTrnd = new Variable(CPPLoopCluster::GenVarName("autoVar"), "");
	pTrnd->SetName(pTrnd->GetID());
	pTrnd->SetAutoGenerated(true);
	pTrnd->SetExpr(eqnBuilder.str());
	pMdl->AddVariable(pTrnd);

	eqnBuilder.clear();
	eqnBuilder.str(STLString());

	eqnBuilder << pInp->GetFullMdlPathName() << "*(1.0+" << pTrnd->GetFullMdlPathName() << "*(" << horizon << "))";

	Variable* pFrcst = new Variable(CPPLoopCluster::GenVarName("autoVar"), "");
	pFrcst->SetName(pFrcst->GetID());
	pFrcst->SetAutoGenerated(true);
	pFrcst->SetExpr(eqnBuilder.str());
	pMdl->AddVariable(pFrcst);

	//add flow
	eqnBuilder.clear();
	eqnBuilder.str(STLString());

	eqnBuilder << '(' << pInp->GetFullMdlPathName() << '-' << pState->GetFullMdlPathName() << ")/" << pAvgTime->GetFullMdlPathName();

	Flow* pChnge=new Flow(CPPLoopCluster::GenVarName("autoFlow"), "", "");
	pChnge->SetName(pChnge->GetID());
	pChnge->SetAutoGenerated(true);
	pChnge->SetExpr(eqnBuilder.str());
	pMdl->AddFlow(pChnge);
	pChnge->SetPTarget(pState);

	//build autoInfluence web
	InsertAutoInfluence(pState, pChnge, pMdl);
	InsertAutoInfluence(pState, pTrnd, pMdl);
	InsertAutoInfluence(pAvgTime, pChnge, pMdl);
	InsertAutoInfluence(pAvgTime, pTrnd, pMdl);
	InsertAutoInfluence(pTrnd, pFrcst, pMdl);
	InsertAutoInfluence(pInp, pTrnd, pMdl);
	InsertAutoInfluence(pInp, pFrcst, pMdl);
	InsertAutoInfluence(pFrcst, pCaller, pMdl);

	//wire in external influences
	for (Evaluable* pEval : inpObjs)
		InsertAutoInfluence(pEval, pInp, pMdl);
	for (Evaluable* pEval : initObjs)
		InsertAutoInfluence(pEval, pState, pMdl);
	for (Evaluable* pEval : timeObjs)
		InsertAutoInfluence(pEval, pAvgTime, pMdl);
	for (Evaluable* pEval : horObjs)
		InsertAutoInfluence(pEval, pFrcst, pMdl);

	//return forcast variable
	return pFrcst->GetFullMdlPathName();
}

/** Unpack function for smth1 call.
@param pCaller Pointer to the Evaluable to be expanded.
@param call The smth1() call extracted from the expression function.
@return The modified statement that is to be applied to pCaller that incorporates the new Model components.
*/
STLString OMEParser::UnpackSmth1(Evaluable* pCaller, const STLString & call)
{
	STLString inStr, avgTime, init;
	UnpackArgs1(call, inStr, avgTime, init);

	//call helper
	return UnpackDelay(pCaller, inStr, avgTime, 1, init);
}

/** Unpack function for smth3 call.
@param pCaller Pointer to the Evaluable to be expanded.
@param call The smth3() call extracted from the expression function.
@return The modified statement that is to be applied to pCaller that incorporates the new Model components.
*/
STLString OMEParser::UnpackSmth3(Evaluable* pCaller, const STLString & call)
{
	STLString inStr, avgTime, init;
	UnpackArgs1(call, inStr, avgTime, init);
	//call helper
	return UnpackDelay(pCaller, inStr, avgTime, 3, init);
}

/** Unpack function for smthn call.
@param pCaller Pointer to the Evaluable to be expanded.
@param call The smthn() call extracted from the expression function.
@return The modified statement that is to be applied to pCaller that incorporates the new Model components.
*/
STLString OMEParser::UnpackSmthN(Evaluable* pCaller, const STLString & call)
{
	STLString input, avgTime, orderStr, init;
	UnpackArgs2(call,input, avgTime, orderStr, init);
	
	int order;
	try
	{
		order = stoi(orderStr);
	}
	catch (...)
	{
		Evaluable* pOrderVal = dynamic_cast<Evaluable*>(pCaller->GetParentModel()->GetRootModel()->NestedObjectForName(orderStr));
		try
		{
			order = (int)stod(pOrderVal->GetExpr());
		}
		catch (...)
		{
			throw OMEObjException("Object must be integer when used in order for SmthN", pOrderVal);

		}

	}
	
	//call helper
	return UnpackSmooth(pCaller, input, avgTime, order, init);
}

/** Unpack function for trend call.
@param pCaller Pointer to the Evaluable to be expanded.
@param call The trend() call extracted from the expression function.
@return The modified statement that is to be applied to pCaller that incorporates the new Model components.
*/
STLString OMEParser::UnpackTrend(Evaluable* pCaller, const STLString & call)
{
	STLString input, time, init;
	UnpackArgs1(call, input, time, init,true);

	//cache up objects
	EvalArray inpObjs, timeObjs, initObjs;

	for (Evaluable* pEval : pCaller->GetUpObjects())
	{
		if (pEval->IsOMEType(OME_INFLUENCE))
			pEval = ((Influence*)pEval)->GetPEvalSource();

		STLString searchStr = pEval->GetFullMdlPathName();

		if (input.find(searchStr) != STLString::npos)
			inpObjs.push_back(pEval);
		if (init.find(searchStr) != STLString::npos)
			initObjs.push_back(pEval);
		if (time.find(searchStr) != STLString::npos)
			timeObjs.push_back(pEval);
	}

	oSTLSStream eqnBuilder;

	Model* pMdl = pCaller->GetParentModel();

	//add state var
	StateVar* pState = new StateVar(CPPLoopCluster::GenVarName("autoSV"), "");
	pState->SetName(pState->GetID());
	pState->SetAutoGenerated(true);
	pState->SetExpr("0.0");
	pMdl->AddStateVar(pState);

	//add variables
	Variable* pAvgTime = new Variable(CPPLoopCluster::GenVarName("autoVar"), "");
	pAvgTime->SetName(pAvgTime->GetID());
	pAvgTime->SetAutoGenerated(true);
	pAvgTime->SetExpr(time);
	pMdl->AddVariable(pAvgTime);

	Variable* pInp = new Variable(CPPLoopCluster::GenVarName("autoVar"), "");
	pInp->SetName(pCaller->GetID());
	pInp->SetAutoGenerated(true);
	pInp->SetExpr(input);
	pMdl->AddVariable(pInp);

	eqnBuilder << "(" << pInp->GetFullMdlPathName() << ")/(1.0+((" << init << ")*" << pAvgTime->GetFullMdlPathName() << "))";
	pState->SetExpr(eqnBuilder.str());
	eqnBuilder.clear();
	eqnBuilder.str(STLString());

	eqnBuilder << '(' << pInp->GetFullMdlPathName() << '-' << pState->GetFullMdlPathName() << ")/(" << pState->GetFullMdlPathName() << '*' << pAvgTime->GetFullMdlPathName() << ")";

	Variable* pTrnd = new Variable(CPPLoopCluster::GenVarName("autoVar"), "");
	pTrnd->SetName(pTrnd->GetID());
	pTrnd->SetAutoGenerated(true);
	pTrnd->SetInitExpr(init);
	pTrnd->SetExpr(eqnBuilder.str());
	pMdl->AddVariable(pTrnd);

	//add flow
	eqnBuilder.clear();
	eqnBuilder.str(STLString());

	eqnBuilder << '(' << pInp->GetFullMdlPathName() << '-' << pState->GetFullMdlPathName() << ")/" << pAvgTime->GetFullMdlPathName();

	Flow* pChnge = new Flow(CPPLoopCluster::GenVarName("autoFlow"), "", "");
	pChnge->SetName(pChnge->GetID());
	pChnge->SetAutoGenerated(true);
	pChnge->SetExpr(eqnBuilder.str());
	pMdl->AddFlow(pChnge);
	pChnge->SetPTarget(pState);

	//build autoInfluence web
	InsertAutoInfluence(pState, pChnge, pMdl);
	InsertAutoInfluence(pState, pTrnd, pMdl);
	InsertAutoInfluence(pAvgTime, pChnge, pMdl);
	InsertAutoInfluence(pAvgTime, pTrnd, pMdl);
	InsertAutoInfluence(pInp, pTrnd, pMdl);
	InsertAutoInfluence(pInp, pChnge, pMdl);
	InsertAutoInfluence(pTrnd, pCaller, pMdl);

	//wire in external influences
	for (Evaluable* pEval : inpObjs)
		InsertAutoInfluence(pEval, pInp, pMdl);
	for (Evaluable* pEval : initObjs)
		InsertAutoInfluence(pEval, pState, pMdl);
	for (Evaluable* pEval : timeObjs)
		InsertAutoInfluence(pEval, pAvgTime, pMdl);

	//return forcast variable
	return pTrnd->GetFullMdlPathName();
}


/** Unpacking process for all delay*() expression functions.
	@param pCaller Pointer to the Evaluable to be expanded.
	@param input The input expression.
	@param delayDuration The length of the delay.
	@param order The power of the duration (ie how many StateVars are unpacked).
	@param init The initialization expression.
	@return The modified statement that is to be applied to pCaller that incorporates the new Model components.
*/
STLString OMEParser::UnpackDelay(Evaluable* pCaller, const STLString & input, const STLString & delayDuration, const unsigned int & order, const STLString & init)
{
	//collect list of potential influence objects
	EvalArray upObjs = pCaller->GetUpObjects();
	EvalArray inputObjs, initObjs, delayObjs;

	//cache init objects and delay objects for influence building
	for (Evaluable* pEval : upObjs)
	{
		if (pEval->IsOMEType(OME_INFLUENCE))
			pEval = ((Influence*)pEval)->GetPEvalSource();

		STLString searchStr = pEval->GetFullMdlPathName();

		if (input.find(searchStr)!=STLString::npos)
			inputObjs.push_back(pEval);
		if (init.find(searchStr) != STLString::npos)
			initObjs.push_back(pEval);
		if (delayDuration.find(searchStr) != STLString::npos)
			delayObjs.push_back(pEval);


	}
	//cache expr pieces
	STLString initExpr = "(" + init + "*" + delayDuration + ")/" + std::to_string(order)+".0";
	STLString delayDenom = "/(" + delayDuration + "/"+std::to_string(order)+".0)";


	//add new components to model
	Model* pMdl = pCaller->GetParentModel();

	Flow* pIOFlow = new Flow(CPPLoopCluster::GenVarName("autoFlow"), "", "");
	pIOFlow->SetName(pIOFlow->GetExpr());
	pIOFlow->SetAutoGenerated(true);
	pIOFlow->SetExpr(input);
	pMdl->AddFlow(pIOFlow);

	for (Evaluable* pE : inputObjs)
		InsertAutoInfluence(pE, pIOFlow, pMdl);

	Flow* pInFlow = pIOFlow;
	for (unsigned int i = 0; i < order; ++i)
	{
		//generate new statevar
		StateVar* pState = new StateVar(CPPLoopCluster::GenVarName("autoSV"), "");

		pState->SetName(pState->GetID());
		pState->SetAutoGenerated(true);
		pState->SetExpr(initExpr);
		pMdl->AddStateVar(pState);

		//wire up inflow
		pIOFlow->SetPTarget(pState);

		//Generate outflow
		pIOFlow = new Flow(CPPLoopCluster::GenVarName("autoFlow"), "", "");
		pIOFlow->SetName(pIOFlow->GetID());
		pIOFlow->SetPSource(pState);
		pIOFlow->SetAutoGenerated(true);
		pIOFlow->SetExpr(pState->GetExprName()+delayDenom);
		pMdl->AddFlow(pIOFlow);

		//add influence from stock to Flow
		InsertAutoInfluence(pState, pIOFlow, pMdl);

		//wire init influences to stock
		for (Evaluable* pE : initObjs)
			InsertAutoInfluence(pE, pState, pMdl);

		//wire delayDurations to stock and flow
		for (Evaluable* pE : delayObjs)
		{
			InsertAutoInfluence(pE, pState, pMdl);
			InsertAutoInfluence(pE, pIOFlow, pMdl);
		}
	}

	//wire final outflow to caller
	Flow* pOutFlow = pIOFlow;
	pOutFlow->SetInitExpr(init);
	InsertAutoInfluence(pOutFlow, pCaller, pMdl);

	//return qualified name of final outflow
	return pOutFlow->GetFullMdlPathName();
}

/** Unpacking process for all smth*() expression functions.
@param pCaller Pointer to the Evaluable to be expanded.
@param input The input expression.
@param avgTime The amount of time to average over.
@param order The power of the duration (ie how many StateVars are unpacked).
@param init The initialization expression.
@return The modified statement that is to be applied to pCaller that incorporates the new Model components.
*/
STLString OMEParser::UnpackSmooth(Evaluable* pCaller, const STLString & input, const STLString & avgTime, const unsigned int & order, const STLString & init)
{
	EvalArray inputObjs, initObjs, timeObjs;

	//cache init objects and delay objects for influence building
	for (Evaluable* pEval : pCaller->GetUpObjects())
	{
		if (pEval->IsOMEType(OME_INFLUENCE))
			pEval = ((Influence*)pEval)->GetPEvalSource();

		STLString searchStr = pEval->GetFullMdlPathName();

		if (input.find(searchStr) != STLString::npos)
			inputObjs.push_back(pEval);
		if (init.find(searchStr) != STLString::npos)
			initObjs.push_back(pEval);
		if (avgTime.find(searchStr) != STLString::npos)
			timeObjs.push_back(pEval);
	}

	FlowArray genFlows;
	SVArray genStateVars;

	STLString lastInp = input;
	STLString denom = "/(" + avgTime + ")/"+std::to_string(order)+".0";
	StateVar* pLastSV = NULL;

	Model* pMdl = pCaller->GetParentModel();
	oSTLSStream eqnBuilder;
	for (unsigned int i = 0; i < order; ++i)
	{
		StateVar* pState = new StateVar(CPPLoopCluster::GenVarName("autoSV"), "");
		pState->SetName(pState->GetID());
		pState->SetAutoGenerated(true);
		pState->SetExpr(init);
		pMdl->AddStateVar(pState);
		genStateVars.push_back(pState);

		eqnBuilder.clear();
		eqnBuilder.str(STLString());
		eqnBuilder << "((" << lastInp << ")-" << pState->GetFullMdlPathName() << ")" << denom;
		Flow* pFlow = new Flow(CPPLoopCluster::GenVarName("autoFlow"), "");
		pFlow->SetName(pFlow->GetID());
		pFlow->SetAutoGenerated(true);
		pFlow->SetExpr(eqnBuilder.str());
		pFlow->SetPTarget(pState);
		genFlows.push_back(pFlow);

		//build influences for this level
		InsertAutoInfluence(pState, pFlow, pMdl);
		if (pLastSV)
			InsertAutoInfluence(pLastSV, pFlow, pMdl);

		pLastSV = pState;
		lastInp = pState->GetFullMdlPathName();
	}

	//rewire external influences.
	for (Evaluable* pEval : inputObjs)
		InsertAutoInfluence(pEval, genFlows.front(), pMdl);

	for (Evaluable* pEval : initObjs)
		for (StateVar* pSV : genStateVars)
			InsertAutoInfluence(pEval, pSV, pMdl);

	for (Evaluable* pEval : timeObjs)
		for (Flow* pF : genFlows)
			InsertAutoInfluence(pEval, pF, pMdl);

	//wire output to caller
	InsertAutoInfluence(pLastSV, pCaller, pMdl);

	return pLastSV->GetFullMdlPathName();
}

/** Insert an autogen flagged Influence into the model.
	@param pSrc Pointer to the source object.
	@param pTrg Pointer to the target object.
	@param pMdl Pointer to the enclosing Model object.
	@return Pointer to the newly created Influence; which already has been added to pMdl and does not need to be
	retained.
*/
Influence* OMEParser::InsertAutoInfluence(Evaluable* pSrc, Evaluable* pTrg, Model* pMdl)
{
	Influence* pInfl = new Influence(CPPLoopCluster::GenVarName("autoInFluence"), "", pSrc, pTrg);
	pInfl->SetAutoGenerated(true);
	pMdl->AddInfluence(pInfl);
	return pInfl;
}

/** Unpack function arguments from expression function call.
	@param call The original call to the expression in question.
	@param input On return, contains the input expression.
	@param arg On return, contains the function-specific argument expression.
	@param init On return, contains the initialization expression.
	@param fillInitWith0 If true, set init value to 0; otherwise use the input value.
*/
void OMEParser::UnpackArgs1(const STLString & call, STLString & input, STLString & arg, STLString & init,const bool & fillInitWith0)
{
	size_t argPos = call.find('(');
	StrList argList;
	CPPLoopCluster::FindFuncArgs(call.substr(argPos + 1, FindMatching(call, argPos, '(', ')') - argPos - 1), argList);


	auto aItr = argList.begin();
	input = (*aItr++);
	arg = (*aItr++);
	STLString toInit = fillInitWith0 ? STLString("0.0") : input;
	init = aItr == argList.end() ? toInit : (*aItr);
}

/** Unpack function arguments from expression function call.
@param call The original call to the expression in question.
@param input On return, contains the input expression.
@param arg1 On return, contains the first function-specific argument expression.
@param arg2 On return, contains the second function-specific argument expression.
@param init On return, contains the initialization expression.
@param fillInitWith0 If true, set init value to 0; otherwise use the input value.
*/
void OMEParser::UnpackArgs2(const STLString & call, STLString & input, STLString & arg1, STLString & arg2, STLString & init,const bool & fillInitWith0)
{
	size_t argPos = call.find('(');
	StrList argList;
	CPPLoopCluster::FindFuncArgs(call.substr(argPos + 1, FindMatching(call, argPos, '(', ')') - argPos - 1), argList);


	auto aItr = argList.begin();
	input = (*aItr++);
	arg1 = (*aItr++);
	arg2 = (*aItr++);
	STLString toInit = fillInitWith0 ? STLString("0.0") : input;
	init = aItr == argList.end() ? toInit : (*aItr);
}