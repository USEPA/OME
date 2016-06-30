#include "ModifierProcessor.h"
#include "omeobjects/SimObjs.h"
//#include "PLToken.h"
#include "OMEDefines.h"
#include "DrawDetails.h"

/** Evaluate any identified modification statements.
	@param pObj Pointer to object to analyze.
	@param expr Statement that may contain modifier expressions.
	@param omitted On return, an array that is populated with objects that should be removed from the model.
	@param pConvFunc Pointer to function to be used to convert the expression's syntax.
	@param pScpFunc Pointer to function to be used to determine scoping in added expression.
	@return A RESULT flag indicating if any errors occurred.
*/
ModifierProcessor::RESULT ModifierProcessor::ProcessObject(OMEObject*& pObj, const STLString & expr, OMEArray & omitted, ScopeFunc pScpFunc, ExprConvFunc pConvFunc)
{

	static const OMEChar delim=':';
	enum {REG_ALL=0, REG_OMIT,REG_INITONLY,REG_INFLTO,REG_INFLFROM,REG_EXPR,REG_MOVE,REG_TS,REG_SPATIAL,REG_EXECGRP};
	static const OMEChar* patterns[]={	"@omit",
										"@initOnly",
										"@influenceTo[:][^@]*",
										"@influenceFrom[:][^@]*",
										"@expression[:][^@]*",
										"@move[:][^@]*",
										"@toTimeSeries",
										"@expectsSpatial",
										"@execGroup[:][^@]*",
										"\0"};
	RESULT err=NO_ERR;

	if(!expr.empty())
	{
		StrArray results=BatchRegex(expr,patterns,0,true);
		StrArray subs;
		if (!results[REG_MOVE].empty())
		{
			subs = SplitString(results[REG_INFLTO], delim);
			if (subs.size() > 1 && !subs[1].empty())
				err = ProcessMove(pObj, subs[1], omitted);
			else
				err = MISSING_ARG;
		}
		if(!err && !results[REG_OMIT].empty())
		{
			err=ProcessOmit(pObj,pObj->GetParentModel(),omitted);
		}
		if (!err && !results[REG_TS].empty())
		{
			err = ProcessToTS(pObj);
		}
		if(!err && !results[REG_INFLTO].empty())
		{
			subs=SplitString(results[REG_INFLTO],delim);
			if(subs.size()>1 && !subs[1].empty())
				err=ProcessInfluenceTo(pObj,subs[1],pObj->GetParentModel());
			else
				err=MISSING_ARG;
		}
		if(!err && !results[REG_INFLFROM].empty())
		{
			subs=SplitString(results[REG_INFLFROM],delim);
			if(subs.size()>1 && !subs[1].empty())
				err=ProcessInfluenceFrom(pObj,subs[1],pObj->GetParentModel());
			else
				err=MISSING_ARG;
		}
		if(!err && !results[REG_EXPR].empty())
		{
			subs=SplitString(results[REG_EXPR],delim);
			if(subs.size()>1 && !subs[1].empty())
				err=ProcessExpression(pObj,subs[1],pConvFunc,pScpFunc);
			else
				err=MISSING_ARG;
		}
		if(!err && !results[REG_INITONLY].empty())
			err=ProcessInitOnly(pObj);

		if (!err && !results[REG_SPATIAL].empty())
		{
			if (pObj->IsOMEType(OME_MODEL))
				((Model*)pObj)->SetExpectsSpatial(true);
			else
				err = BAD_TYPE;
		}

		if (!err && !results[REG_EXECGRP].empty())
		{
			if (pObj->IsOMEType(OME_EVAL))
			{
				//extract value
				STLString grpStr = results[REG_EXECGRP].substr(results[REG_EXECGRP].find(':')+1);
				try
				{
					unsigned int grp = std::stoi(grpStr);
					((Evaluable*)pObj)->SetExecGroup(grp);
				}
				catch (...)
				{
					err = MISSING_ARG;
				}
			}
			else
				err = BAD_TYPE;
		}
	}
	return err;
}

/** Find expression buried within a larger string using special start and end identifiers.
	@param inStr The string to inspect.
	@return A string containing modifier expressions, if any.
*/
STLString ModifierProcessor::ExtractExpression(const STLString & inStr)
{
	STLString outStr=STLString();
	static const STLRegex boundReg(".*--!!OME:(.*?)!!--.*");
	STLsmatch mtch;

	if(!inStr.empty() && std::regex_match(inStr,mtch,boundReg))
		outStr=STLString(mtch[1].str());

	return outStr;
}

/** Add an influence between objects.
	@param pObj Influence from this object.
	@param trgName Name of influence target.
	@param pBm Pointer to Model that will hold the new influence.
	@return A result code indicating the success or failure of the process.
*/
ModifierProcessor::RESULT ModifierProcessor::ProcessInfluenceTo(OMEObject* pObj, const STLString & trgName, Model* pBm)
{
	RESULT out=NO_ERR;

	if(!trgName.empty())
	{
		OMEObject* pTarget=pBm->ObjectForName(trgName);
		if(pTarget)
		{
			STLString nameid=pObj->GetExprName()+"->"+pTarget->GetExprName();
			Influence* pInf=new Influence(nameid,nameid,pObj,pTarget);
			DrawDetails* pDD=new DrawDetails(pInf);
			//do anything with draw details?

			pBm->AddInfluence(pInf);
		}
		else
			out=OBJECT_NF;
	}
	else
		out=MISSING_ARG;

	return out;
}

/** Add an influence between objects.
	@param pObj Influence to this object.
	@param srcName Name of influence source.
	@param pBm Pointer to BaseModel that will hold the new influence.
	@return A result code indicating the success or failure of the process.
*/
ModifierProcessor::RESULT ModifierProcessor::ProcessInfluenceFrom(OMEObject* pObj, const STLString & srcName, Model* pBm)
{
	RESULT out=NO_ERR;

	if(!srcName.empty())
	{
		OMEObject* pSource=pBm->ObjectForName(srcName);
		if(pSource)
		{
			STLString nameid=pSource->GetExprName()+"To"+pObj->GetExprName();
			Influence* pInf=new Influence(nameid,nameid,pSource,pObj);
			DrawDetails* pDD=new DrawDetails(pInf);
			pBm->AddInfluence(pInf);
		}
		else
			out=OBJECT_NF;
	}
	else
		out=MISSING_ARG;

	return out;
}

/** Move an object from one model to another.
	@param pObj Pointer to object to process.
	@param mdlName The name of the model to move pObj to.
	@param omitted An OMEArray that will contain a list of objects to be removed on return.
*/
ModifierProcessor::RESULT ModifierProcessor::ProcessMove(OMEObject* pObj, const STLString & mdlName, OMEArray & omitted)
{
	RESULT out = NO_ERR;

	if (!mdlName.empty())
	{
		//find root model
		Model* pMdl = pObj->GetParentModel();
		for (; pMdl->GetParentModel(); pMdl = pMdl->GetParentModel());

		Model* pFound = pMdl->SubmodelForName(mdlName, true);
		if (pFound)
		{
			if (pObj->IsOMEType(OME_EVAL))
			{
				size_t i;
				Evaluable* pEval = dynamic_cast<Evaluable*>(pObj);
				for (i = 0; i < pEval->UpObjectCount(); i++)
					RemoveInflChain(pEval->GetUpObject(i), pEval, omitted);
				for (i = 0; i < pEval->DownObjectCount(); i++)
					RemoveInflChain(pEval->GetDownObject(i), pEval, omitted);
				if (pObj->GetParentModel())
					pObj->GetParentModel()->RemoveObject(pObj);
				pFound->AddObject(pObj);
			}

		}
		else
			out = OBJECT_NF;
	}
	else
		out = MISSING_ARG;

	return out;
}

/** Omit object from model. Any Influence and SubmodelPort chains will be removed if they only point to the object being omitted.
	@param pObj Pointer to object to omit.
	@param pBm Model to omit object from.
	@param omitted List that will contain any omitted objects.
	@return A result code indicating the success or failure of the process.
*/
ModifierProcessor::RESULT ModifierProcessor::ProcessOmit(OMEObject* pObj, Model* pBm,OMEArray & omitted)
{
	unsigned int i,j;
	if(pObj->IsOMEType(OME_EVAL))
	{
		Evaluable* pEval=dynamic_cast<Evaluable*>(pObj);
		for(i=0; i<pEval->UpObjectCount();i++)
			RemoveInflChain(pEval->GetUpObject(i),pEval,omitted);
		for(i=0; i<pEval->DownObjectCount();i++)
			RemoveInflChain(pEval->GetDownObject(i),pEval,omitted);
	}
	else if(pObj->IsOMEType(OME_MODEL))
	{

		Model* pMdl=(Model*)pObj;
		
		if(!pMdl->isRoot())
		{
			Model* parent=dynamic_cast<Model*>(pMdl->GetParentModel());

			//remove all associated SubmodelAssocs
			SubmodelAssoc* pAssoc;
			i=0;
			while(i<parent->GetAssocCount())
			{
				pAssoc=parent->GetAssoc(i);
				if(pAssoc->GetBaseId()==pMdl->GetID() || pAssoc->GetSubId()==pMdl->GetID())
					RemoveInflChain(pAssoc, parent, omitted);
				else
					i++;
			}

			//eradicate submodelports and associated influences
			SubmodelPort* pPort;
			Model* outer;
			EvalArray outs;
			for(i=0; i<pMdl->GetSubPortCount(); i++)
			{
				pPort=pMdl->GetSubPort(i);
				outer=pPort->GetOuterModel();
				if(outer)
				{
					outs.clear();
					pPort->GetOutObjects(outs);
					for(j=0; j<outs.size(); j++)
					{
						if(outs[j]->GetOMEType()==OME_INFLUENCE)
							RemoveInflChain(outs[j],pPort,omitted);
					}
					outer->RemoveSubPort(pPort);
				}
				pPort->Detach();
			}

			StrArray keys=pMdl->GetObjectKeys(DT_FULL);
			std::for_each(keys.begin(),keys.end(),[pMdl,&omitted](const STLString & key) { omitted.push_back(pMdl->ObjectForKey(key));});
			//omitted.insert(omitted.end(),keys.begin(),keys.end());
		}
	}

	// add more cases as necessary
	pBm->RemoveObject(pObj);
	omitted.push_back(pObj);

	return NO_ERR;
}

/** Process any commands that request an object to be evaluated only once.
	@param pObj Pointer to object to process.
*/
ModifierProcessor::RESULT ModifierProcessor::ProcessInitOnly(OMEObject* pObj)
{
	RESULT err=BAD_TYPE;

	if(pObj->IsOMEType(OME_VARIABLE))
	{
		((Variable*)pObj)->SetInitOnly(true);
		err=NO_ERR;
	}

	return err;
}

/** Override an object's expression with a new one. 
	@param pObj Pointer to the object to modify.
	@param expr The expression to apply to pObj.
	@param pConvFunc Pointer to function to be used to convert the expression's syntax.
	@param pScpFunc Pointer to function to be used to determine scoping in added expression.
	@return A result code indicating the success or failure of the process.
*/
ModifierProcessor::RESULT ModifierProcessor::ProcessExpression(OMEObject* pObj, const STLString & expr, ExprConvFunc pConvFunc, ScopeFunc pScpFunc)
{
	RESULT out=NO_ERR;
	Evaluable* pEval=dynamic_cast<Evaluable*>(pObj);

	if(pEval)
	{
		pEval->SetExpr(pConvFunc(expr,pObj));
		//apply scope if requested to
		if(pScpFunc)
			pScpFunc(pEval);
	}
	else
		out=BAD_TYPE;
	return out;
}

/** Recursive function used to remove objects that have become obsolete due to omission commands.
	@param pObj Pointer to the object to evaluate for removal.
	@param pLast Pointer to the last object that was evaluated.
	@param omitted List that will contain any omitted objects.
	@return A result code indicating the success or failure of the process.
*/
void ModifierProcessor::RemoveInflChain(OMEObject* pObj, OMEObject* pLast,OMEArray & omitted)
{
	Link* pLnk;
	union
	{
		SubmodelPort* pPort;
		AssocPort* pAp;
	};
	unsigned int i=0;
	bool remove=false;
	Model* outer;
	OMEObject* src,*trg;
	switch(pObj->GetOMEType())
	{
	case OME_INFLUENCE:
	case OME_SUBASSOC:
		pLnk = dynamic_cast<Link*>(pObj);
		
		src = pLnk->GetPOMESource();
		trg = pLnk->GetPOMETarget();
	

		if(trg && src==pLast)
			RemoveInflChain(trg,pObj,omitted);
		else if(src && trg==pLast)
			RemoveInflChain(src,pObj,omitted);

		if (pObj->GetOMEType() == OME_INFLUENCE)
			((Influence*)pObj)->Detach();
		
		pObj->GetParentModel()->RemoveObject(pObj);
		
		omitted.push_back(pObj);
		break;
	case OME_SUBPORT:
		pPort=((SubmodelPort*)pObj);

		//if the port is only associated with one object on a side facing the omited object, then remove
		if(pPort->DownObjectCount()==1 && pPort->GetDownObject(0)==pLast)
		{
			while(pPort->UpObjectCount())
				RemoveInflChain(pPort->GetUpObject(0),pObj,omitted);
			remove=true;
		}
		else if(pPort->UpObjectCount()==1 && pPort->GetUpObject(0)==pLast)
		{
			while(pPort->DownObjectCount())
				RemoveInflChain(pPort->GetDownObject(0),pObj,omitted);
			remove=true;
		}

		if(remove)
		{
			pPort->GetParentModel()->RemoveSubPort(pPort);
			outer=pPort->GetOuterModel();
			if(outer)
				outer->RemoveSubPort(pPort);

			omitted.push_back(pObj);
		}
		break;
	case OME_ASSOCPORT:
		pAp = (AssocPort*)pObj;
		if (pAp->GetOutAssoc(0) == pLast)
			RemoveInflChain(pAp->GetInAssoc(0), pObj, omitted);
		else if (pAp->GetInAssoc(0) == pLast )
			RemoveInflChain(pAp->GetOutAssoc(0), pObj, omitted);

		pAp->GetParentModel()->RemoveAssocPort(pPort);
		outer = pAp->GetOuterModel();
		if (outer)
			outer->RemoveAssocPort(pPort);

		omitted.push_back(pObj);

		break;
	}
}

/** Convert a variable to a time series.
	@param pObj Pointer to the Variable to convert to a time series.
	@return A result code indicating the success or failure of the process.
*/
ModifierProcessor::RESULT ModifierProcessor::ProcessToTS(OMEObject*& pObj)
{
	RESULT err = pObj->GetOMEType() == OME_VARIABLE ? NO_ERR : BAD_TYPE;

	if (!err)
	{
		pObj = TimeSeriesVar::ReplaceVariable((Variable*)pObj);
	}

	return err;
}
