#include "omeobjects/ModelInstance.h"
#include "omeobjects/Model.h"
#include "omeobjects/Conveyor.h"

//ModelInstance
/** Detailed Constructor.
	@param pM Pointer to the parent model.
	@param ind The index of the ModelInstance.
	@param act If true, ModelInstance is marked as active.
*/
ModelInstance::ModelInstance(Model* pM,const size_t & ind,const bool & act)
	:m_pParentModel(pM), m_pParentSpawner(NULL),m_index(ind)//,m_pSubInstances(NULL)
{
	m_flags = CLEAR_FLAGS;
	if (act)
	{
		m_flags |= ACTIVE_FLAG | TOACTIVE_FLAG;
	}
}

/** Copy Constructor.
@param inst ModelInstance to copy.
@param parentInst index of ModelInstance that spawned this one.
@param pSpwn Pointer to Spawner that spawned this ModelInstance.
@param ind The index of the ModelInstance.
*/
ModelInstance::ModelInstance(const ModelInstance & inst,const size_t & ind, const size_t & parentInst, Spawner* pSpwn)
{
	m_flags = ACTIVE_FLAG | TOACTIVE_FLAG;
	
	m_flags |= SUB_FLAG;
	m_pParentModel = inst.m_pParentModel;
	m_parentInstance = parentInst;
	m_pParentSpawner = pSpwn;
	m_index = ind;
}

ModelInstance::~ModelInstance()
{
}

/** Calc the Derivative for a given state variable for this instance. Some SV types
	(such as conveyors) take up multiple derivative slots; in this case, update each one.
	@param pSv Pointer to the StateVar to calc derivative for.
	@param inst The index to use for instance lookup.
	@param deriveArray The array containing the calculated derivates
	@param ind The index in deriveArray to start applying derivatives in this step.
	@return The next available index.
*/
size_t ModelInstance::CalcDerivative(StateVar* pSv,const size_t & inst,OME_SCALAR* deriveArray,size_t ind)
{
	if (pSv->GetOMEType() == OME_CONVEYOR)
		return CalcDerivative((Conveyor*)pSv, inst, deriveArray, ind);

	FlowArray* currArray;
	OME_SCALAR sum = 0.0;
	if (IsActive())
	{
		currArray = &pSv->GetInputs();
		unsigned int i;
		for (i = 0; i < currArray->size(); i++)
			sum += (*currArray)[i]->GetValueForRawInst(inst);

		currArray = &pSv->GetOutputs();
		for (i = 0; i < currArray->size(); i++)
			sum -= (*currArray)[i]->GetValueForRawInst(inst);
	}
	
	deriveArray[ind] = OMEABS(sum) > numeric_limits<OME_SCALAR>::epsilon() ? sum : 0.0;
	
	return ind+1;
}

/** Calc the Derivative for a given Conveyor for this instance. Conveyors
take up multiple derivative slots; in this case, update each one.
@param pC Pointer to the Conveyor to calc derivative for.
@param inst The index to use for instance lookup.
@param deriveArray The array containing the calculated derivates
@param ind The index in deriveArray to start applying derivatives in this step.
@return The next available index.
*/
size_t ModelInstance::CalcDerivative(Conveyor* pC, const size_t & inst, OME_SCALAR* deriveArray, size_t ind)
{
	FlowArray* currArray;
	OME_SCALAR sum = 0.0;
	if (IsActive())
	{
		bool dbg = false;

		size_t convInd = ind + pC->GetRequestedSlotCount()-1;
		
		//grab inputs at front of conveyor
		currArray = &pC->GetInputs();
		unsigned int i;
		for (i = 0; i < currArray->size(); i++)
			sum += (*currArray)[i]->GetValueForRawInst(inst);

		//determine adjustment to make for size of timestep
		OME_SCALAR adj = (1+pC->GetTailIndex() - pC->GetModelIndex()) / pC->GetTransitTime();
		deriveArray[convInd] = sum*adj;
		
		
		//convey values along conveyor
		for (i = pC->GetTailIndex(); i > pC->GetModelIndex(); --i)
		{
			m_pParentModel->GetValForRawInstIndex(inst, i, sum);
			
			deriveArray[convInd--] -= sum*adj;
			deriveArray[convInd] = sum*adj;
			
		}

		//clear out last slot
		m_pParentModel->GetValForRawInstIndex(inst, pC->GetModelIndex(), sum);
		deriveArray[convInd] -= sum*adj;

	}
		
	return ind+pC->GetRequestedSlotCount();
}

/** @return a new pointer to a copy of this ModelInstance. */
ModelInstance* ModelInstance::Clone()
{
	return new ModelInstance(*this);
}


/**Apply any status changes set to be applied to a given ModelInstance.*/
void ModelInstance::ApplyMortality()
{
	if (GetFlag(TOACTIVE_FLAG) != GetFlag(ACTIVE_FLAG))
		m_pParentModel->GetValueArchive()->SetModelInstanceActive(m_pParentModel, m_index, GetFlag(TOACTIVE_FLAG));
	SetFlag(m_flags & TOACTIVE_FLAG, ACTIVE_FLAG);
	SetFlag(m_flags & TOKILL_FLAG, DEAD_FLAG);
}
