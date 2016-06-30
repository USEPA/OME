#include "OMEDefines.h"
#include "omeobjects/Conveyor.h"
#include "omeobjects/Model.h"
#include "BaseManager.h"

Conveyor::Conveyor()
:StateVar(), m_transitTime(1.0), m_tailIndex(OME_NOIND), m_capacity(OME_NOLIM), m_isArrested(false) {}

/** Simple constructor.
	@param id The id to assign.
	@param name The name to assign.
*/
Conveyor::Conveyor(STLString id, STLString name)
: StateVar(id, name), m_transitTime(1.0), m_tailIndex(OME_NOIND), m_capacity(OME_NOLIM), m_isArrested(false) {}

/** Simple XML constructor.
@param pCurrElem Pointer to the current XML element being evaluated.
@param tag The tag of the node being evaluated.
@param filename The source of the model file.
*/
Conveyor::Conveyor(TI2Element* pCurrElem, const STLString & tag, const STLString & filename)
:m_transitTime(1.0), m_tailIndex(OME_NOIND), m_capacity(OME_NOLIM), m_isArrested(false)
{
	OMEObject::PopulateFromXML(pCurrElem, tag, filename);
}
	
/** Copy constructor.
	@param c The Conveyor to copy.
*/
Conveyor::Conveyor(const Conveyor & c)
{
	Duplicate(c);
}

/** Assignment operator.
	@param c The Conveyor to copy.
	@return a reference to this.
*/
Conveyor& Conveyor::operator=(const Conveyor & c) 
{ 
	Duplicate(c); 
	return *this; 
}

/** Equality operator.
	@param c The Conveyor to compare against.
	@return true if equal; false otherwise.
*/
bool Conveyor::operator==(const Conveyor & c)
{
	return this == &c ||
		(
		StateVar::operator==(c) &&
		m_transitTime==c.m_transitTime &&
		m_capacity==c.m_capacity &&
		m_isArrested==c.m_isArrested
		);
}

/** Scalar Assignment operator.
@param val The scalar to assign.
@return a reference to this.
*/
Conveyor& Conveyor::operator=(const OME_SCALAR & val)
{
	StateVar::SetValue(val);
	return *this;
}

/** EnumEntry Assignment operator.
@param pEE Pointer to the EnumEntry to assign.
@return a reference to this.
*/
Conveyor& Conveyor::operator=(EnumEntry* pEE)
{
	StateVar::SetValue(pEE);
	return *this;
}

/** ModelEnum Assignment operator.
@param pME Pointer to the ModelEnum to assign.
@return a reference to this.
*/
Conveyor& Conveyor::operator=(ModelEnum* pME)
{
	StateVar::SetValue(pME);
	return *this;
}

Conveyor::~Conveyor(void) 
{ 
	Detach(); 
}

//OME_SCALAR Conveyor::GetValue() const
//{
//	OME_SCALAR ret = 0.0;
//	if (m_pParentModel->GetCurrentInstanceRef()->IsActive())
//		ret = m_pParentModel->GetCurrentInstanceVal(m_tailIndex/*, GetRequestedSlotCount()*/);
//	return ret;
//}
//
//OME_SCALAR Conveyor::GetValue(const size_t & inst) const
//{
//	OME_SCALAR out=0.0;
//	if (m_pParentModel->GetInstance(inst)->IsActive())
//	{
//		if (!m_pParentModel->GetValForInstIndex(inst, m_tailIndex, out))
//			throw OMEObjException("Target instance is invalid.", this);
//	}
//	return out;
//}
//
//OME_SCALAR Conveyor::GetValueForRawInst(const size_t & inst) const
//{
//	OME_SCALAR out = 0.0;
//	if (m_pParentModel->GetInstance(inst)->IsActive())
//	{
//		if (!m_pParentModel->GetValForRawInstIndex(inst, m_tailIndex, out))
//			throw OMEObjException("Target instance is invalid.", this);
//	}
//	return out;
//}

void Conveyor::Reset(BaseManager* pBm)
{
	StateVar::Reset(pBm);
	m_isArrested = false;
}

STLString Conveyor::GetXMLRepresentation(const unsigned int indent, const unsigned int inc)
{
	return GetXMLForObject(indent, inc);
}

void Conveyor::GetXMLAttributes(StrArray & out) const
{
	StateVar::GetXMLAttributes(out);

	//Conveyor attrs
	AddAttributeValue<STLString>("transit_expr", m_transitExpr, out);
	if (m_capacity!=OME_NOLIM)
		AddAttributeValue<OME_SCALAR>("capacity", m_capacity, out);

}

void Conveyor::GetXMLSubNodes(StrArray & out, const unsigned int indent, const unsigned int inc)
{
	StateVar::GetXMLSubNodes(out, indent, inc);
}

void Conveyor::AdjustFlows(const size_t & inst)
{
	//since OME_NOLIM is equal to infinity, we don't have to explicitly check for it here
	if (m_isArrested || GetTotalInTransit(inst) >= m_capacity)
	{
		//stall all inflows and outflows
		for (Flow* pIn : m_inputs)
			pIn->SetValue(0.0,inst);
		for (Flow* pOut : m_outputs)
			pOut->SetValue(0.0,inst);

	}
	else
	{
		for (Flow* pOut : m_outputs)
			pOut->SetValue(GetValue(inst),inst);
	}
}

size_t Conveyor::GetRequestedSlotCount() const
{
	return m_tailIndex!= OME_NOIND ? 1+ m_tailIndex - m_mdlIndex : 1;
}

void Conveyor::BatchInitialize(const OME_SCALAR time, BaseManager* pBm)
{
	StateVar::BatchInitialize(time, pBm);
	
	//update transit time to reflect closest timestep
	
	OME_SCALAR lower = 0.0f, upper = pBm->GetStepSize();
	for (; upper < m_transitTime; lower = upper, upper += pBm->GetStepSize());
	m_transitTime = (m_transitTime - lower < upper - m_transitTime) ? lower : upper;

	//grap initialized value
	OME_SCALAR initVal = Evaluable::GetValue();

	//resize based on travel time
	size_t oldLength = m_tailIndex == OME_NOIND ? 1 : GetRequestedSlotCount();

	m_tailIndex = m_mdlIndex;
	if (m_transitTime > pBm->GetStepSize())
		m_tailIndex += (m_transitTime / pBm->GetStepSize())-1.0;

	m_pParentModel->GetValueArchive()->ResizeMultiValSlot(this, oldLength);

	//assign initial value to all new slots
	for (size_t i = m_mdlIndex; i <= m_tailIndex; ++i)
	{
		m_pParentModel->SetCurrentInstanceVal(initVal, i);
	}
}

/** @return The total value presently in transit through the Conveyor for the current instance.*/
OME_SCALAR Conveyor::GetTotalInTransit() const
{
	OME_SCALAR ret = 0.0;
	if (m_pParentModel->GetCurrentInstanceRef()->IsActive())
	{
		size_t count = GetRequestedSlotCount();
		for (size_t i = m_mdlIndex; i<m_mdlIndex+count; ++i)
			ret += m_pParentModel->GetCurrentInstanceVal(i,count);
	}
	return ret;
}

/**  @param inst The instance to check.
@return The total value presently in transit through the Conveyor for a specific instance.	
*/
OME_SCALAR Conveyor::GetTotalInTransit(const size_t & inst) const
{
	OME_SCALAR ret = 0.0;
	if (m_pParentModel->GetInstance(inst)->IsActive())
	{
		for (size_t i = m_mdlIndex; i <= m_tailIndex; ++i)
		{
			OME_SCALAR tmp = 0.0;
			m_pParentModel->GetValForInstIndex(inst,i,tmp);
			ret += tmp;
		}
	}
	return ret;
}

void Conveyor::GetXMLAttributeMap(XMLAttrVector & out)
{
	XmlAttr attrs[] = {
		// attr				type		   address                          isReq
		{ "transit_expr",	XML_STR,	    &(this->m_transitExpr),			true },
		{ "capacity",		XML_SCALAR,		&(this->m_capacity),			false },
		{ NULL,				XML_NULL,		NULL,							false } };

	//drop null
	int count;
	for (count = 0; attrs[count].name != NULL; count++)
		out.push_back(attrs[count]);

	StateVar::GetXMLAttributeMap(out);
}

int Conveyor::PopulateFromComplexElements(TI2Element* pCurrElem, const STLString & tag, const STLString & filename)
{
	int ret = StateVar::PopulateFromComplexElements(pCurrElem, tag, filename);

	return ret;
}

/** Duplicate the values from another Conveyor.
	@param c The Conveyor to duplicate.
*/
void Conveyor::Duplicate(const Conveyor & c)
{
	m_transitTime = c.m_transitTime;
	m_tailIndex = c.m_tailIndex;
	m_capacity = c.m_capacity;
	m_isArrested = c.m_isArrested;

	StateVar::Duplicate(c);
}

void Conveyor::SetModelIndex(const size_t & index)
{
	if (m_tailIndex != OME_NOIND)
	{
		size_t offs = m_tailIndex - m_mdlIndex;
		m_tailIndex = index + offs;
	}

	StateVar::SetModelIndex(index);

}

const OMEChar* Conveyor::ToString(const OMEChar tabChar, const unsigned int indent, const unsigned int inc) const
{
	OMEChar values[MAXPATHLEN];
	const OMEChar* ptr=Evaluable::ToString(tabChar, indent, inc);

	oSTLSStream strBuff;
	strBuff << ptr;
	delete[] ptr;
	strBuff << STLString(indent, tabChar) << "In Transit:" << std::endl;

	STLString currIndent(indent + inc, tabChar);
	size_t count = GetRequestedSlotCount();
	//skip tail (already reported)
	for (size_t i = m_mdlIndex+1; i <= m_tailIndex; ++i)
	{
		strBuff << currIndent << m_pParentModel->GetCurrentInstanceVal(i, count) << std::endl;
	}

	size_t outSize = strBuff.str().size();
	char* outBuff = new char[outSize + 1];
	outBuff[outSize] = '\0';
	SAFE_STRCPY(strBuff.str().c_str(), outBuff, outSize + 1);
	return outBuff;
}