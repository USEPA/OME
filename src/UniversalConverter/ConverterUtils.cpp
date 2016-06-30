#include "ConverterUtils.h"
#include "omeobjects/NullObjs.h"

/** Verify if the link object is still needed.
	During the model construction process, intermediate steps are sometimes used. Links are often used to combine temporary and permanent objects; VerifyRelationship() combines when necessary.
	@return false if the link can be discarded; true otherwise.
*/
bool VerifyRelationship(Link* pLn)
{

	bool keep=true;

	OMEObject* pSrc=pLn->GetPOMESource();
	OMEObject* pTrg=pLn->GetPOMETarget();
	//only run check if both source and target are present
	if(pLn->GetPOMESource() && pLn->GetPOMETarget())
	{
		OMETYPE srcType= pSrc->GetOMEType();
		OMETYPE trgType= pTrg->GetOMEType();

		if(pSrc== pTrg)
		{
			//redundant link created, probably due to reassignments; remove
			keep=false;
		}
		else if (srcType || trgType)
		{
			//at least one component has to be non-null type to work
			if (!srcType)
				keep = ProcessOMENull(pLn, (OMENull*)pSrc, pTrg);
			else if (!trgType)
				keep = ProcessOMENull(pLn, (OMENull*)pTrg, pSrc);
			else
			{
				//both are valid; check for alias
				if (trgType == OME_ALIAS)
				{
					//assign alias; dispose of influence 
					if (!pSrc->IsOMEType(OME_MODEL))
					{
						((EvalAlias*)pTrg)->SetSrc((Evaluable*)pSrc);
						((EvalAlias*)pTrg)->SetName(pSrc->GetName());
					}
					keep = false;
				}
			}
		}
		else
		{
			//both objects are of a null type; process if one is a ghost.
			if (dynamic_cast<NullFunc*>(pSrc) && dynamic_cast<NullFunc*>(pTrg))
			{
				((NullFunc*)pSrc)->AddOutInfluence((Influence*)pLn);
				((NullFunc*)pTrg)->AddInfluence((Influence*)pLn);
			}
			else
			{
				/*keep = false;

				NullGhost* srcGhost = dynamic_cast<NullGhost*>(pSrc);
				NullGhost* trgGhost = dynamic_cast<NullGhost*>(pTrg);

				if (srcGhost)
					keep = ProcessOMENull(pLn, srcGhost, pTrg);
				else if (trgGhost)
					keep = ProcessOMENull(pLn, trgGhost, pSrc);*/
			}
		}

		if(!keep)
		{
			
			Evaluable* pLnkEval=dynamic_cast<Evaluable*>(pLn);
			if(pLnkEval)
			{
				Evaluable* pEval=dynamic_cast<Evaluable*>(pSrc);
				if(pEval)
					pEval->RemoveDownObject(pLnkEval);

				pEval=dynamic_cast<Evaluable*>(pTrg);
				if(pEval)
					pEval->RemoveUpObject(pLnkEval);
			}
		}
	}
	return keep;
}

/** Process null object and its target
	Null objects are placeholders used during the construction process. This Evaluable applies them to their target object.
	@param pNullObj the null object to process.
	@param pObj the target of the null object.
	@return false if the link can be discarded; true otherwise.
*/
bool ProcessOMENull(Link* pLn,OMENull* pNullObj, OMEObject* pObj)
{
	//only mark for discarding if no errors
	bool keep=true;
	NullFunc* pNullF;
	Evaluable* pFObj;

	static unsigned int tableVal=0;

	switch(pNullObj->GetNullType())
	{
	case NULL_FUNC:
		pNullF=dynamic_cast<NullFunc*>(pNullObj);
		pFObj=dynamic_cast<Evaluable*>(pObj);

		/*if(!pNullF->GetComplete())
			keep=false;
		else*/ if(pFObj || dynamic_cast<Model*>(pObj) )
		{
			if(pObj==pLn->GetPOMESource())
			{
				pNullF->AddInfluence((Influence*)pLn);
			}
			else if(pFObj && pLn->GetLinkType()==LN_INFLUENCE)
			{
				pNullF->SetOutObject(pFObj);
				if (!pFObj->IsOMEType(OME_ALIAS) && pFObj->HasTable())
						pFObj->SetPEvalTable(pNullF->GenerateEvalTable(),0);

				//else alias table will be assigned elsewhere
				keep=false;
			}
			
		}
		else
		{
			//if we get here then pObj is not a valid NullFunc target 
			//assert error
		}
		break;
	case NULL_CLOUD:
		if(pLn->GetLinkType()==LN_FLOW)
		{
			if(pLn->GetPOMESource()==pNullObj)
				pLn->SetPSource(NULL);
			else
				pLn->SetPTarget(NULL);
		}
		else
			keep=false;
		break;
	//case NULL_GHOST:
	//	pGhost=dynamic_cast<NullGhost*>(pNullObj);
	//	pNullF=dynamic_cast<NullFunc*>(pObj);

	//	if(pLn->GetPOMETarget()==pNullObj) 
	//	{
	//		if(pNullF)
	//		{
	//			//link ghost to func, so any inbound influences to func are transferred to ghost;
	//			pNullF->SetGhost(pGhost);
	//		}
	//		else
	//		{
	//			//actual link b/w base object and ghost. set up references
	//			//ports handled later
	//			if(pObj->GetOMEType()!=OME_SUBPORT)
	//				pGhost->SetOriginal(pObj);
	//		}
	//		//either way, this link can be disposed of.

	//		keep=false;
	//	}
	//	//else if(pNullF)
	//	//{
	//	//	pGhost->AddOutLink(this);
	//	//	keep=false;//ProcessOMENull(pNullF,pGhost);
	//	//}
	//	else //ghost is source
	//	{
	//		pGhost->AddOutLink(pLn);
	//		if(pNullF)
	//			ProcessOMENull(pLn,pNullF,pGhost);
	//	}
	//	break;
	default:
		//assume object is null
		keep=false;
		break;
	};

	return keep;
}