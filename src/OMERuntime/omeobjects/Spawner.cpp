#include "omeobjects/Spawner.h"
#include "omeobjects/Model.h"
#include "events/CentralEventDispatcher.h"
#include "BaseManager.h"

std::default_random_engine Spawner::s_rEngine;
std::normal_distribution<OME_SCALAR> Spawner::s_nDist(0.0,1.0);

/** Default Constructor.
*/
Spawner::Spawner(void)
	:Evaluable()
{
}

/** Detailed constructor.
	@param name The name of the Spawner.
	@param desc Description of the Spawner.
	@param runOnce Should the model run once (initialization).
	@param perInst Should the model run on a per-instance basis.
	@param isLoss Should Spawner calculate loss instead of gain.
	@param stoch Should Spawner use Stochastic calculations.
	@param cond If true, the Spawner is conditional.
*/
Spawner::Spawner(STLString name, STLString desc, bool runOnce,bool perInst, bool isLoss, bool stoch, bool cond)
	:Evaluable(name,desc,0.0)
{
	m_actionFlags[SB_INIT_ONLY]=runOnce;
	m_actionFlags[SB_PER_INSTANCE]=perInst;
	m_actionFlags[SB_IS_LOSS]=isLoss;
	m_actionFlags[SB_IS_STOCHASTIC]=stoch;
	m_actionFlags[SB_CONDITIONAL]=cond;
}

/** Destructor.*/
Spawner::~Spawner()
{
}

STLString Spawner::GetXMLRepresentation(const unsigned int indent, const unsigned int inc)
{
	return GetXMLForObject(indent,inc);
}

void Spawner::GetXMLAttributes(StrArray & out) const
{
	//function attrs
	Evaluable::GetXMLAttributes(out);
	
	//stateVar attrs

	AddAttributeValue<int>("init_only",m_actionFlags[SB_INIT_ONLY],out);
	AddAttributeValue<int>("stochastic",m_actionFlags[SB_IS_STOCHASTIC],out);
   AddAttributeValue<int>("per_instance",m_actionFlags[SB_PER_INSTANCE],out);
	AddAttributeValue<int>("is_loss",m_actionFlags[SB_IS_LOSS],out);
	AddAttributeValue<int>("is_conditional",m_actionFlags[SB_CONDITIONAL],out);
}

void Spawner::GetXMLSubNodes(StrArray & out,const unsigned int indent,const unsigned int inc)
{
	Evaluable::GetXMLSubNodes(out,indent,inc);
}

void Spawner::GetXMLAttributeMap(XMLAttrVector & out)
{
	Evaluable::GetXMLAttributeMap(out);
}

int Spawner::PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
{
	const OMEChar* attribs[]={  "init_only",
								"stochastic",
								"per_instance",
								"is_loss",
								"is_conditional"
								};
	
	for(unsigned short i=0; i<SB_BITCOUNT; i++)
		m_actionFlags[i]=pCurrElem->BoolAttribute(attribs[i]);
	
	return 1;
}

OME_SCALAR Spawner::Evaluate(const OME_SCALAR time,BaseManager* pBm)
{
	if(m_pParentModel && !m_pParentModel->IsInIntegrate() && (!m_actionFlags[SB_INIT_ONLY] || !IsInitialized() ))
	{
		if(m_actionFlags[SB_INIT_ONLY])
			SetInitialized(true); //set here to avoid loop

		unsigned int numCopies=0;

		//if(numCopies >= 1.0)
		//{
			if(!m_actionFlags[SB_PER_INSTANCE])
			{
				if(!m_actionFlags[SB_CONDITIONAL])
				{
					//spawn instances here
					OME_SCALAR result=Evaluable::Evaluate(time,pBm);
					numCopies=GetNumCopies(result);
					SpawnCopies(numCopies);
				}
				else
					EvaluateConditional(time,pBm);
				
			}
			else
			{
				/// \todo update for new instance stuff
				//BaseModel* currInst=NULL;
				//
				//unsigned int count=pMC->GetActiveInstanceCount();
				//for(unsigned int i=0; i < count; i++)
				//{
				//	currInst=pMC->GetInstance(i);
				//	if(currInst)
				//	{	
				//		//evaluate spawning for individual instance.
				//		//evaluate in context of instance
				//		m_value=Evaluable::Evaluate(time,pBm);
				//		SpawnCopies(numCopies,currInst);
				//	}
				//}
			}

			//m_pParentModel->InitSupports(time);
			if(m_actionFlags[SB_INIT_ONLY])
				m_pParentModel->UpdateSupports(time);

		//}
		
	}

	return Evaluable::GetValue();

}

OME_SCALAR Spawner::EvalWithVal(const OME_SCALAR & value)
{
	if (m_pParentModel && /*!m_pParentModel->IsInIntegrate() &&*/ (!m_actionFlags[SB_INIT_ONLY] || !IsInitialized()))
	{
		SetInitialized(true); //set here to avoid loop

		unsigned int numCopies = 0;
		SetValue(value);

		if (!m_actionFlags[SB_PER_INSTANCE])
		{
			if (!m_actionFlags[SB_CONDITIONAL])
			{
				//spawn instances here
				numCopies = GetNumCopies(value);
				SpawnCopies(numCopies);
			}
			else
			{
				ModelInstance* pInstance = m_pParentModel->GetCurrentInstanceRef();
				//conditional
				if (!value)
					pInstance->SetActive(false);//MarkForDeath(i,this);
				else if (!pInstance->IsActive() && value)
				{
					pInstance->SetActive(true);
				}
			}

		}
		else
		{
			ModelInstance* currInst = NULL;

			unsigned int count = m_pParentModel->GetActiveInstanceCount();
			for (unsigned int i = 0; i < count; i++)
			{
				currInst = m_pParentModel->GetRawInstance(i);
				if (currInst)
				{
					//evaluate spawning for individual instance.
					//evaluate in context of instance
					SpawnCopies(numCopies, i);
				}
			}
		}

	}
		return value;
		//m_pParentModel->InitSupports(time);
		//if (m_actionFlags[SB_INIT_ONLY])
			//m_pParentModel->UpdateSupports(time);
}


/** Copy Another Spawner object.
	@param spwn The Spawner object to copy
*/
void Spawner::Duplicate(const Spawner & spwn)
{
	m_actionFlags=spwn.m_actionFlags;

	Evaluable::Duplicate(spwn);
}

void Spawner::Reset(BaseManager* pBm)
{
	m_accumulator=0.0;
	Evaluable::Reset(pBm);
}

void Spawner::EventAction(const CentralEvent & ev, void* extra)
{
	STLString type=ev.GetEventType();
	if(type==CED_INCREMENT)
	{
		//if(!m_initOnly)
		//	m_hasRun=false;
	}
	
	Evaluable::EventAction(ev,extra);
}

unsigned int Spawner::Prioritize(unsigned int currPriority, const bool isInitial)
{

	unsigned int maxPriority=0, tempPriority;
	unsigned int* pCompPriority=(isInitial ? &m_initPriority : &m_evalPriority);
	if(currPriority>*pCompPriority)
	{
		if(!m_actionFlags[SB_INIT_ONLY])
			*pCompPriority=currPriority; 
		else
			*pCompPriority=0;

		Evaluable* curr;
		int i;
		for(i=0; i< m_upObjects.size(); i++)
		{
			curr=m_upObjects[i];
			if (curr->GetOMEType() == OME_INFLUENCE && (!(((Influence*)curr)->GetUseCurrentVal()) || isInitial))
			{
				tempPriority=curr->Prioritize(*pCompPriority+1, isInitial);
				maxPriority= tempPriority > maxPriority ? tempPriority : maxPriority;
			}
		}

		if(!isInitial)
		{
			for(i=0; i< m_downObjects.size(); i++)
			{
				curr=m_downObjects[i];
				if (curr->GetOMEType() == OME_INFLUENCE && ((Influence*)curr)->GetUseCurrentVal())
				{
					tempPriority=curr->Prioritize(*pCompPriority-1,isInitial);
					maxPriority= tempPriority > maxPriority ? tempPriority : maxPriority;
				}
			}
		}
	}
	return maxPriority > *pCompPriority ? maxPriority : *pCompPriority;
}

/** Spawn instances of model.
	@param numCopies The number of copies to spawn.
	@param instIndex Index of instance that led to creation of new copies.
*/
void Spawner::SpawnCopies(const unsigned int & numCopies,const int & instIndex)
{
	
	//int instIndex=-1;
	//if(inst)
	//	instIndex=inst->m_instance;
	//
	if(!m_actionFlags[SB_IS_LOSS])
		m_pParentModel->SpawnInstances(numCopies,instIndex,this);
	else if(numCopies)
		m_pParentModel->MarkForDeath(instIndex);
}

OME_SCALAR Spawner::Initialize(const OME_SCALAR time,BaseManager* pBm)
{
	if(!IsInitialized())
	{
		if(m_actionFlags[SB_INIT_ONLY])
			Evaluable::Evaluate(time,pBm);
		SetInitialized(true);
	}
	return Evaluable::GetValue();
}

void Spawner::BatchInitialize(const OME_SCALAR time,BaseManager* pBm)
{
	if(!IsInitialized())
	{
		if (m_actionFlags[SB_INIT_ONLY] && !m_expr.empty())
		{
			m_initExpr = m_expr;
			m_expr = STLString();
		}
		//evaluate should be called by batchInitExpr
		m_mdlIndex=GetModelIndex();
		//m_isInitialized=true;
	}
}

/** Get number of copies to produce.
	Numbers may be different depending on whether or not stochastic calculations are being used.
	@param inVal The value to base evaluations on.
	@return The number of copies to produce.
*/
unsigned int Spawner::GetNumCopies(const OME_SCALAR & inVal)
{
	int numCopies=0;

	if(!m_actionFlags[SB_IS_STOCHASTIC])
	{
		//add to copy accumulator
		m_accumulator+=inVal;

		//cast to see how many copies we can create
		numCopies=m_accumulator;
		//subtract spawned copies from the accumulator, carrying over any fraction
		m_accumulator-=numCopies;
	}
	else
	{
		//determine if a new instance will be created, based on baseVal.
		numCopies= s_nDist(s_rEngine) <= inVal ? 1 : 0;
	}

	return numCopies;
}

/** Evaluate conditional statement for each (potential) model instance.
	If the instance exists and the condition evaluates to False, kill the instance.
	If the instance does not exist and the condition evaluates to True, spawn the instance.
	@param time The current timestep.
	@param pBm Pointer to the BaseManager which will carry out the evaluation.
*/
void Spawner::EvaluateConditional(const OME_SCALAR time,BaseManager* pBm)
{
	/// \todo update for new instance stuff
	//ModelCollection* pModel=dynamic_cast<ModelCollection*>(m_pParentModel);
	//BaseModel* inst;
	//if(pModel)
	//{
	//	for(int i=0; i<pModel->NumInstances(); i++)
	//	{
	//		inst=pModel->GetInstance(i);
	//		if(!inst)
	//		{
	//			pModel->OverrideInstanceNum(i);
	//		}

	//		m_value=Evaluable::Evaluate(time,pBm);

	//		if(inst && !m_value)
	//			pModel->KillInstance(i);//MarkForDeath(i,this);
	//		else if(!inst && m_value)
	//			pModel->SpawnInstance(i,-1,this);
	//	}
	//	pModel->ResetInstanceNum();
	//}
}

/** Comparison operator.
	@param rhs The Spawner to compare values against.
	@return true If Spawner values are equal; false otherwise.
*/
bool Spawner::operator==(const Spawner & rhs)
{
	return this==&rhs ||
		(
		Evaluable::operator==(rhs) &&
		m_actionFlags==rhs.m_actionFlags
		);
}

/** Determine if a model instance was spawned by this model.
	@param ind Index of instance to evaluate.
	@return true if Spawner is responsible for the creation of the given instance; false otherwise.
*/
bool Spawner::DidSpawnInst(const size_t & ind) const
{
	bool ret = false;
	for (auto itr = m_spawnedInstances.begin(); itr != m_spawnedInstances.end() && !ret; ++itr)
		ret = (*itr == ind);

	return ret;
}