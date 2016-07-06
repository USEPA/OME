#include "omeobjects/VarArray.h"
#include "BaseManager.h"
#include "omeobjects/SubmodelPort.h"
#include "omeobjects/Model.h"
#include "UpdateContainers/TempVCArray.h"
#include "UpdateContainers/TempPoolArray.h"
#include "UpdateContainers/StaticUpArray.h"

#define GETARRINDREF(x)		m_pParentModel->GetCurrentInstanceVal(m_mdlIndex+(x),m_length)
#define SETMVAL(x,y)		m_pParentModel->SetCurrentInstanceVal((y),m_mdlIndex+(x),m_length)
#define GETARCHINFOREF(i)	*m_pParentModel->GetValueArchive(); \
							i= m_pParentModel->GetArchiveIndex(m_mdlIndex)

VarArray::VarArray()
	:Listable(true)
{

}

/** Simple Constructor.
	@param n The name of the VarArray.
	@param desc The description of the VarArray.
*/
VarArray::VarArray(const STLString & n, const STLString & desc)
	: Variable(n, desc, 0.0), Listable(true)
{

}

/** XML Constructor.
	@param pCurrElem Pointer to current XML Element.
	@param tag The tag used to identify this object.
	@param filename Path to the originating file.
*/
VarArray::VarArray(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
	:Listable(true)
{
	OMEObject::PopulateFromXML(pCurrElem, tag, filename);
	CheckConstant();
}

/** Copy Constructor.
	@param va The VarArray to copy.
*/
VarArray::VarArray(const VarArray & va)
	:Listable(true)
{
	Duplicate(va);
}

/** Conversion Constructor.
	@param v Variable to use as basis for construction. The Constructed VarArray is a list of one value.
*/
VarArray::VarArray(const Variable & v)
	:Listable(true)
{
	Variable::Duplicate(v);
	//SetValue(((Variable)v).GetValue());
}

/** Conversion Constructor.
	@param v Variable to use as basis for construction. The Constructed VarArray is a list of one value.
	@param isReplace If true, new VarArray replaces v in its model heirarchy associations.
*/
VarArray::VarArray(Variable & v, const bool & isReplace)
	:Listable(true)
{
	Variable::Duplicate(v);
	//SetValue(((Variable)v).GetValue());

	if(isReplace)
	{
		//rewire linkages to this VarArray
		Link* pLnk;
		Evaluable* pEval;
		unsigned int i;
		for(i=0; i<v.UpObjectCount(); ++i)
		{
			pEval=v.GetUpObject(i);
			pLnk=dynamic_cast<Link*>(pEval);
			if(pLnk)
				pLnk->SetPTarget(this);
			else
				pEval->AddUpObject(this);
		}
		v.Detach();
	}
}

/** Set the value at a specific index.
	@param index The index to modify.
	@param value The value to assign to index.
	@param clamp If true, clamp assigned value between any predefined bounds.
	@return true if value was not clamped; false otherwise.
*/
bool VarArray::SetValue(const size_t index, const OME_SCALAR value, const bool & clamp)
{
	OME_SCALAR chkVal=value;
	bool ret=true; //Variable::CheckBounds(chkVal,clamp);

	if(index >= m_length)
		Allocate(index+1);
	

	//us m_value as temp/last set value
	SETMVAL(index,!IsAsInt() ? chkVal : (int)chkVal);
	
	return ret;
}

/** Set the value at a specific index.
@param index The index to modify.
@param value The value to assign to index.
@return true if value was not clamped; false otherwise.
*/
bool VarArray::SetValue(const size_t & index, const OME_SCALAR & value)
{
	return SetValue(index, value, false);
}

/** Set the value at a specific index to a ModelEnum.
@param index The index to modify.
@param value Pointer to the ModelEnum to assign to index.
@return true if value was not clamped; false otherwise.
*/
bool VarArray::SetValue(const size_t & index, ModelEnum * value)
{
	m_pEnumParent = value;
	SetIsEnumClass(true);
	return SetValue(index, value->GetEnumValueCount(), false);
}

/** Set the value at a specific index to an EnumEntry.
@param index The index to modify.
@param value Pointer to the EnumEntry to assign to index.
@return true if value was not clamped; false otherwise.
*/
bool VarArray::SetValue(const size_t & index, EnumEntry* value)
{
	m_pEnumParent = value->pCollection;
	SetIsEnumClass(false);
	return SetValue(index, value->ind, false);
}

/** Find a value at a given index.
	@param i Index to retrieve value from.
	@return The value at i. If i is out of bounds, 0 is returned.
*/
OME_SCALAR VarArray::GetValue(const size_t & i) const
{
	
	return i < m_length ? GETARRINDREF(i) : 0.0;
}

/** Find Value at given index.
	@param i Index to retrieve value from.
	@param out variable to hold value found if i is a valid index.
	@return true if i is a valid index; false otherwise.
*/
bool VarArray::GetValue(const size_t & i, OME_SCALAR & out)
{
	bool ret=false;

	if(i< m_length)
	{
		out = GETARRINDREF(i);
		ret=true;
	}
	return ret;
}

/** Get Represented ModelEnum class.
	@param i Index to value to check.
	@return Pointer to ModelEnum representation, or NULL if not associated with ModelEnum.
*/
ModelEnum* VarArray::GetEnumClass(const size_t & i)
{
	//ignore index
	return Evaluable::GetEnumClass();
}

/** Get Represented EnumEntry.
@param i Index to value to check.
@return Pointer to EnumEntry representation, or NULL if not associated with ModelEnum.
*/

EnumEntry* VarArray::GetEnumEntry(const size_t & i)
{
	EnumEntry* ret = NULL;

	if (m_pEnumParent)
	{
		ret = m_pEnumParent->AddrForIndex(GetValue(i));
	}
	return ret;
}

/** Determine if value is an EnumClass.
@param i Index to value to check.
@return true if value is ModelEnum; false otherwise.
*/
bool VarArray::IsEnumClass(const size_t & i) const
{
	//ignore index
	return Evaluable::IsEnumClass();
}

/** Determine if value is an EnumValue.
@param i Index to value to check.
@return true if value is EnumEntry; false otherwise.
*/
bool VarArray::IsEnumValue(const size_t & i) const
{
	//ignore index
	return Evaluable::IsEnumValue();
}

/** Allocates space based on assigned dimensions.*/
void VarArray::Allocate()
{
	size_t oldLength = m_length;
	m_step = m_dims.CalcStep();
	m_length = m_dims.CalcRepLength();
	
	
	//(*GETMARRADDR()).resize(dimSize, 0.0);
	if (m_length > oldLength && m_pParentModel->GetValueArchive())
	{
		
		m_pParentModel->GetValueArchive()->ResizeMultiValSlot(this, oldLength);
	
	}
}
/** Resize the internal storage to hold a number of values.
	@param newSize The number of entries to allocate space for. Any newly created slots are allocated to 0.
*/
void VarArray::Allocate(const size_t newSize)
{
	m_dims.ClearDims();
	m_dims.AppendDim(newSize);
	Allocate();
}

/** Resize the internal storage based on dimensional specifications.
	@param newSizes A list of indices indicating the maximum extent along each dimension of interest.
*/
void VarArray::Allocate(const IndVec & newSizes)
{
	m_dims = newSizes;
	Allocate();
}

void VarArray::SetValues(const OME_SCALAR* vals, const size_t count)
{
	SetValues(vals, count, false);
}

/** Set internal values to those found in an array.
	@param vals Pointer to an array of values.
	@param count The number of entries in vals.
	@param resize If true, internal storage is resized to count; 
			if false, insert as many values as possible into the VarArray starting from index 0.
*/
void VarArray::SetValues(const OME_SCALAR* vals, const size_t count, const bool & resize)
{

	size_t limit=count;
	if(count!=m_length)
	{
		if(resize)
			Allocate(count);
		else if(m_length < count)
			limit=m_length;
	}
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);

	if (!IsAsInt())
		for(unsigned int i=0; i<limit; ++i)
			rVArch[insertPoint+i]=vals[i];
	else
		for (unsigned int i = 0; i<limit; ++i)
			rVArch[insertPoint + i] = (int)vals[i];
}

/** Set internal values to those found in an array.
	@param vals valarray containing the values to be used.
	@param resize If true, internal storage is resized to count; 
			if false, insert as many values as possible into the VarArray starting from index 0.
*/
void VarArray::SetValues(const VALVECTOR vals, const bool & resize)
{
	size_t limit=vals.size();

	if(limit!=m_length)
	{
		if(resize)
			Allocate(limit);
		else if(m_length < limit)
			limit=m_length;
	}
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	if (!IsAsInt())
		for (unsigned int i = 0; i<limit; ++i)
			rVArch[insertPoint + i] = vals[i];
	else
		for (unsigned int i = 0; i<limit; ++i)
			rVArch[insertPoint + i] = (int)vals[i];
}

void VarArray::SetValues(const OME_SCALAR & val)
{
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	OME_SCALAR inVal = !IsAsInt() ? val : (int)val;
	
	for (unsigned int i = 0; i < m_length; ++i)
		rVArch[insertPoint + i] = inVal;
}

/** Set Enumerated Class value to all VarArray values.
	@param pMe Pointer to ModelEnum value to associate with VarArray.
*/
void VarArray::SetValues(ModelEnum* pMe)
{
	m_pEnumParent = pMe;
	SetIsEnumClass(true);
	SetValues(pMe->GetEnumValueCount());
}

/** Set Enumerated value to all VarArray values.
@param pEe Pointer to EnumEntry value to associate with VarArray.
*/
void VarArray::SetValues(EnumEntry* pEe)
{
	m_pEnumParent = pEe->pCollection;
	SetIsEnumClass(false);
	SetValues(pEe->ind);
}


/** Get a copy of the internal values.
	@param outCount a variable to store the number of entries to be returned.
	@return A pointer to a newly allocated array of length outCount containing a copy of the values.
*/
OME_SCALAR* VarArray::GetValues(size_t & outCount) const
{
	outCount = m_length;
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);

	OME_SCALAR* ret=NULL;
	if(outCount)
	{
		ret=new OME_SCALAR[outCount];

		for(unsigned int i=0; i<outCount; ++i)
			ret[i] = rVArch[insertPoint + i];
	}

	return ret;
}

/** Get a copy of the internal values.
@return A Container of the stored internal values.
*/
VALVECTOR VarArray::GetValues() const
{
	VALVECTOR ret(m_length);
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	for (unsigned int i = 0; i<m_length; ++i)
		ret[i] = rVArch[insertPoint + i];
	return ret;
}

/** Get pointer to the internal values.
	@return A valarray containing a copy of the values.
*/
SclrPtrArray VarArray::GetValueAddrs() const
{
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	SclrPtrArray out;
	out.resize(m_length);
	for(size_t i=0; i<m_length; ++i)
		out[i] = &(rVArch[insertPoint + i]);

	return out;
}

void VarArray::Reset(BaseManager* pBm)
{
	Variable::Reset(pBm);
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	for (size_t i = 0; i<m_length; ++i)
		rVArch[insertPoint + i] = 0.0;
}

STLString VarArray::GetXMLRepresentation(const unsigned int indent,const unsigned int inc)
{
	return GetXMLForObject(indent,inc);
}

void VarArray::GetXMLAttributes(StrArray & out) const
{
	Variable::GetXMLAttributes(out);

	//remove value tag
	unsigned int i;
	for(i=0; i<out.size(); ++i)
	{
		if(out[i].substr(0,5)=="value")
			break;
	}

	if(i<out.size())
		out.erase(out.begin()+i);
	else
		i=0;

	oSTLSStream outStrm;
	//reinsert value tag if values are present
	if(m_fileVals.size())
	{
		outStrm<<"value=\"";
		for(unsigned int j=0; j<m_fileVals.size(); j++)
		{
			if(j)
				outStrm<<' ';
			outStrm<<m_fileVals[j];
		}
		outStrm<<'"';
		out.insert(out.begin()+i,outStrm.str());
	}

	if(m_dims.GetSize())
	{
		outStrm.clear();
		outStrm.str(STLString());

		for(i=0; i<m_dims.GetSize();++i)
			outStrm<<' '<<m_dims[i];
		AddAttributeValue("dimensions",outStrm.str().substr(1),out);
	}
}

void VarArray::GetXMLSubNodes(StrArray & out,const unsigned int indent,const unsigned int inc)
{
	Variable::GetXMLSubNodes(out,indent,inc);
}

OME_SCALAR VarArray::Evaluate(const OME_SCALAR time,BaseManager* pBm)
{
	if(m_expr.length() && !IsInitialized())
	{
		m_lastTime=time;
		OMEParser* pPrsr=pBm->GetPEvalParser();
		OME_SCALAR* newVals;
		int count=0;
		pPrsr->CompileExpression(m_expr.c_str(),this);
		pPrsr->Evaluate(newVals,count);

		SetValues(newVals,count,true);
		delete[] newVals;
	}
	//return count of values
	return m_length;
}

OME_SCALAR VarArray::Initialize(const OME_SCALAR time,BaseManager* pBm)
{
	if(!IsInitialized())
	{
		m_mdlIndex=GetModelIndex();
		if (m_fileVals.size())
		{
			if (m_length < m_fileVals.size())
			{
				m_dims.ClearDims();
				m_dims.AppendDim(m_fileVals.size());
				m_length = m_fileVals.size();
				Allocate();
			}
			size_t insertPoint;
			ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
			for (size_t i = 0;i < m_length;++i)
				rVArch[insertPoint+i]=m_fileVals[i];
		}
		else
			Evaluate(time,pBm);
			
		SetInitialized(true);
	}

	//return count of values
	return m_length;
}

size_t VarArray::GetInstValCount() const
{
	size_t currCount = 0;
	Model* pMdl;
	if (pMdl = m_pParentModel)
	{
		currCount = GetSize();
		for (; pMdl && currCount; pMdl = pMdl->GetParentModel())
			currCount *= pMdl->GetNumInstances();
	}
	return currCount;
}

/** Create a copy of another VarArray.
	@param va The VarArray to copy.
*/
void VarArray::Duplicate(const VarArray & va)
{
	Variable::Duplicate(va);
	m_dims = va.m_dims;
	RecalcStep();
	m_length = m_dims.CalcRepLength();
	Allocate();

	size_t lIPoint,rIPoint;
	ValueArchive& lrVArch = GETARCHINFOREF(lIPoint);
	ValueArchive& rrVArch = *va.GetParentModel()->GetValueArchive();
	
	lIPoint = GetModelIndex();
	rIPoint = va.GetModelIndex();

	for (size_t i = 0; i < m_length; ++i)
	{
		lrVArch[lIPoint + i] = rrVArch[rIPoint + i];
	}
}

void VarArray::GetXMLAttributeMap(XMLAttrVector & out)
{
	Variable::GetXMLAttributeMap(out);
}

int VarArray::PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
{
	//input for value should be a whit=space delineated series of values in a quotes string
	const OMEChar* valStr=pCurrElem->Attribute("value");
	if(pCurrElem->Attribute("dimensions"))
	{
		STLString inString=pCurrElem->Attribute("dimensions");
		//parse dimensions
		size_t totEnt=1;
		unsigned int currDim;
		iSTLSStream inStrm(inString);
		
		while(!inStrm.eof())
		{
			inStrm>>currDim;
			totEnt*=currDim;
			m_dims.AppendDim(currDim);
		}
		
		//m_fileVals.resize(totEnt,0.0);

	}
	if(valStr)
	{
		STLString in(valStr);
		iSTLSStream parser(in);
		std::list<OME_SCALAR> vals;
		OME_SCALAR curr;

		while(parser.good())
		{
			parser>>curr;
			vals.push_back(curr);
		}
		if(m_fileVals.size()<vals.size())
			m_fileVals.resize(vals.size());
		auto itr=vals.begin();
		for(unsigned int i=0; itr!=vals.end(); ++itr, ++i)
			m_fileVals[i]=*itr;
	}
	return Variable::PopulateFromComplexElements(pCurrElem,tag,filename);
}

/** Copy Values from the relationships of a SubmodelPort.
	@param rhs The SubmodelPort containing the values to copy.
	@return A reference to the VarArray.
*/
VarArray& VarArray::operator=(const SubmodelPort& rhs)
{
	bool useInner=true;
	size_t srcSize=rhs.GetSubSourceCount();
	if(!srcSize)
	{
		srcSize=rhs.GetOutSourceCount();
		useInner=false;
	}
	
	Allocate(srcSize);
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	

	for(unsigned int i=0; i<srcSize; ++i)
		rVArch[insertPoint+i]=useInner ? rhs.GetSubmodelSource(i)->GetValue() : rhs.GetOutmodelSource(i)->GetValue();
	

	return *this;
}

/** Retrieve the value at a given index.
	@param pos The index of the value in question.
	@return A reference to the value at pos.
*/
OME_SCALAR& VarArray::operator[](size_t pos)
{
	if (pos > m_length)
		throw OMEOOBException("VarArray::operator[]");
	
	return m_pParentModel->GetCurrentInstanceVal(m_mdlIndex+pos,m_length);
}

/** Retrieve the value at a given index.
	@param pos The index of the value in question.
	@return A copy of the value at pos.
*/
const OME_SCALAR VarArray::operator[](size_t pos) const
{
	if (pos > m_length)
		throw OMEOOBException("VarArray::operator[]");

	return m_pParentModel->GetCurrentInstanceVal(m_mdlIndex + pos, m_length);
}

/** Test to see if any internal value equals the testCase
	@param testCase The value to test against.
	@return true if At least one internal value is equal to testCase; false otherwise.
*/
bool VarArray::TestAny(const OME_SCALAR & testCase) const
{
	bool ret=false;

	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);

	for (unsigned int i = 0; i < m_length && ret == false; ++i)
		ret= rVArch[insertPoint+i]==testCase;

	return ret;
}

/** Test to see if all internal values equal the testCase
	@param testCase The value to test against.
	@return true if all internal values are equal to testCase; false otherwise.
*/
bool VarArray::TestAll(const OME_SCALAR & testCase) const
{
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	bool ret=m_length>0;
	for(unsigned int i=0; i<m_length && ret==true; ++i)
		ret= rVArch[insertPoint+i]==testCase;

	return ret;
}

/** @return index of max value, or OME_NOVAL if the VarArray is empty.
*/
size_t VarArray::GetMaxInd() const
{
	size_t m=OME_NOVAL;
	
	if(m_length)
	{
		size_t insertPoint;
		ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
		OME_SCALAR currVal,maxVal = rVArch[insertPoint];
		m = 0;
		for (size_t i = 1; i < m_length; ++i)
		{
			currVal = rVArch[insertPoint + i];
			if (currVal > maxVal)
			{
				maxVal = currVal;
				m = i;
			}
		}
	}
	return m;
}

/** @return index of min value, or -1 if the VarArray is empty.
*/
size_t VarArray::GetMinInd() const
{
	size_t m = OME_NOVAL;

	if (m_length)
	{
		size_t insertPoint;
		ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
		OME_SCALAR currVal, minVal = rVArch[insertPoint];
		m = 0;
		for (size_t i = 1; i < m_length; ++i)
		{
			currVal = rVArch[insertPoint + i];
			if (currVal < minVal)
			{
				minVal = currVal;
				m = i;
			}
		}
	}
	return m;
}

/** @return The maximum contained value. */
OME_SCALAR VarArray::GetMax() const
{
	OME_SCALAR maxVal = -OME_NOVAL;

	if (m_length)
	{
		size_t insertPoint;
		ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
		OME_SCALAR currVal;
		
		for (size_t i = 0; i < m_length; ++i)
		{
			currVal = rVArch[insertPoint + i];
			if (currVal > maxVal)
				maxVal=currVal;
		}
	}
	return maxVal;
}

/** @return The minimum contained value. */
OME_SCALAR VarArray::GetMin() const
{
	OME_SCALAR minVal = OME_NOVAL;

	if (m_length)
	{
		size_t insertPoint;
		ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
		OME_SCALAR currVal;

		for (size_t i = 1; i < m_length; ++i)
		{
			currVal = rVArch[insertPoint + i];
			if (currVal < minVal)
				minVal = currVal;
		}
	}
	return minVal;
}

/** @return the combined product of all stored values.
*/
OME_SCALAR VarArray::GetProduct() const
{
	unsigned int i;
	OME_SCALAR prod=0.0;
	if(m_length)
	{
		size_t insertPoint;
		ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
		prod=1.0;
		for(i=0; i<m_length; ++i)
			prod *= rVArch[insertPoint+i];
	}
	return prod;
}

/** @return the combined sum of all stored values.
*/
OME_SCALAR VarArray::GetSum() const
{
	unsigned int i;
	OME_SCALAR tot = 0.0;
	if (m_length)
	{
		size_t insertPoint;
		ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
		
		for (i = 0; i<m_length; ++i)
			tot += rVArch[insertPoint + i];
	}
	return tot;
}

void VarArray::OverrideDims(const std::initializer_list<unsigned int> & dims)
{
	m_dims.ClearDims();

	for (const unsigned int & d : dims)
		m_dims.AppendDim(d);

}

const OMEChar* VarArray::ToString(const OMEChar tabChar, const unsigned int indent, const unsigned int inc) const
{
	oSTLSStream buff;

	STLString head(indent,tabChar);
	buff << head << "Array Values:" << std::endl;
	size_t i,insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	if(!m_length)
		buff<<"None"<<endl;
	for(i=0;i<m_length;++i)
		buff << i + 1 << ": " << rVArch[insertPoint+i] << endl;
	buff<<endl;

	STLString out=STLString(OMEObject::ToString(tabChar,indent,inc))+buff.str();

	char* outStr=new char[out.length()+1];
	for(i=0; i < out.length(); ++i)
		outStr[i]=out[i];
	outStr[i]='\0';
	return outStr;
}

/** Retrieve dimensional indices for 1D (raw) index.
	@param ind The raw index to query for.
	@param outInds IndVec to hold resulting indices; outInds will be resized if too small.
*/
void VarArray::GetDimsForRawIndex(size_t ind, IndVec & outInds) const
{
	
	if (outInds.size() < m_dims.GetSize())
		outInds.resize(m_dims.GetSize());

	size_t denom=1;
	for (unsigned short i = 0; i < m_dims.GetSize(); ++i)
		denom *= m_dims[i];
	
	for (size_t i = m_dims.GetSize() - 1; i > 0; --i)
	{
		denom /= m_dims[i];
		outInds[i] = ind / denom;
		ind -= denom*outInds[i];
	}
	outInds[0] = ind;
}

/** Retrieve dimensional indices for 1D (raw) index.
@param ind The raw index to query for.
@param outInds ListDims to hold resulting indices; outInds will be resized if too small.
*/
void VarArray::GetDimsForRawIndex(size_t ind, ListDims & outInds) const
{
    
    outInds.ClearDims();
    
    size_t denom=1;
    for (unsigned short i = 0; i < m_dims.GetSize(); ++i)
        denom *= m_dims[i];
    
    for (size_t i = m_dims.GetSize() - 1; i > 0; --i)
    {
        denom /= m_dims[i];
        outInds.AppendDimFront(ind / denom);
        ind -= denom*outInds[i];
    }
    outInds.AppendDimFront(ind);
}


size_t VarArray::GetStepSize() const
{
	//for now;
	return m_step;
}
size_t VarArray::GetLevel() const
{
	return m_dims.GetSize();
}

Listable& VarArray::operator+(const OME_SCALAR & rhs)
{
	
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = rVArch[insertPoint+i] + rhs;
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator+(Listable & rhs)
{

	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		size_t insertPoint;
		ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
		ret->GetDims() = m_dims;
		for (size_t i = 0; i < m_length; ++i)
			(*ret)[i] = rVArch[insertPoint+i] + rhs[i];
	}
	else
	{
		Listable* pLrg, *pSml;
		if (m_dims.GetSize() > rhs.GetLevel())
		{
			pLrg = this;
			pSml = &rhs;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
		}
		size_t inStep = pLrg->GetSize() / pSml->GetSize();
		ret->GetDims() = pLrg->GetDims();
		
		for (size_t i = 0, s = 0, flip = 0; i<pLrg->GetSize(); ++i, ++flip)
		{
			if (flip >= inStep)
			{
				++s;
				flip = 0;
			}
			(*ret)[i] = (*pLrg)[i] + (*pSml)[s];
		}
	}
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator-(const OME_SCALAR & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = rVArch[insertPoint+i] - rhs;
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator-(Listable & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		size_t insertPoint;
		ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
		ret->GetDims() = m_dims;
		for (size_t i = 0; i < m_length; ++i)
			(*ret)[i] = rVArch[insertPoint + i] - rhs[i];
	}
	else
	{
		Listable* pLrg, *pSml;
		OME_SCALAR resSgn;
		if (m_dims.GetSize() > rhs.GetLevel())
		{
			pLrg = this;
			pSml = &rhs;
			resSgn = 1.0;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
			resSgn = -1.0;
		}
		size_t inStep = pLrg->GetSize() / pSml->GetSize();
		ret->GetDims() = pLrg->GetDims();
		
		for (size_t i = 0, s = 0, flip = 0; i<pLrg->GetSize(); ++i, ++flip)
		{
			if (flip >= inStep)
			{
				++s;
				flip = 0;
			}
			(*ret)[i] = ((*pLrg)[i] - (*pSml)[s])*resSgn;
		}
	}
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator*(const OME_SCALAR & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = rVArch[insertPoint+i] * rhs;
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator*(Listable & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		size_t insertPoint;
		ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
		ret->GetDims() = m_dims;
		for (size_t i = 0; i < m_length; ++i)
			(*ret)[i] = rVArch[insertPoint + i] * rhs[i];
	}
	else
	{
		Listable* pLrg, *pSml;
		if (m_dims.GetSize() > rhs.GetLevel())
		{
			pLrg = this;
			pSml = &rhs;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
		}
		
		size_t inStep = pLrg->GetSize() / pSml->GetSize();
		ret->GetDims() = pLrg->GetDims();
		for (size_t i = 0,s=0, flip = 0; i<pLrg->GetSize(); ++i, ++flip)
		{
			if (flip >= inStep)
			{
				++s;
				flip = 0;
			}
			(*ret)[i] = (*pLrg)[i] * (*pSml)[s];
		}
	}
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator/(const OME_SCALAR & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = rVArch[insertPoint+i] / rhs;
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator/(Listable & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		size_t insertPoint;
		ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
		ret->GetDims() = m_dims;
		for (size_t i = 0; i < m_length; ++i)
			(*ret)[i] = rVArch[insertPoint + i] / rhs[i];
	}
	else
	{
		Listable* pLrg, *pSml;
		bool noRecip = m_dims.GetSize() > rhs.GetLevel();
		if (noRecip)
		{
			pLrg = this;
			pSml = &rhs;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
		}
		size_t inStep = pLrg->GetSize() / pSml->GetSize();
		ret->GetDims() = pLrg->GetDims();
		
		if (noRecip)
		{
			for (size_t i = 0, s = 0, flip = 0; i<pLrg->GetSize(); ++i, ++flip)
			{
				if (flip >= inStep)
				{
					++s;
					flip = 0;
				}
				(*ret)[i] = (*pLrg)[i] / (*pSml)[s];
			}
		}
		else
		{
			for (size_t i = 0, s = 0, flip = 0; i<pLrg->GetSize(); ++i, ++flip)
			{
				if (flip >= inStep)
				{
					++s;
					flip = 0;
				}
				(*ret)[i] = 1.0/((*pLrg)[i] / (*pSml)[s]);
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator==(const OME_SCALAR & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = rVArch[insertPoint + i] == rhs;
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator==(Listable & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		size_t insertPoint;
		ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
		ret->GetDims() = m_dims;
		for (size_t i = 0; i < m_length; ++i)
			(*ret)[i] = rVArch[insertPoint + i] == rhs[i];
	}
	else
	{
		Listable* pLrg, *pSml;
		if (m_dims.GetSize() > rhs.GetLevel())
		{
			pLrg = this;
			pSml = &rhs;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
		}
		size_t inStep = pLrg->GetSize() / pSml->GetSize();
		ret->GetDims() = pLrg->GetDims();
		
		for (size_t i = 0, s = 0, flip = 0; i<pLrg->GetSize(); ++i, ++flip)
		{
			if (flip >= inStep)
			{
				++s;
				flip = 0;
			}
			(*ret)[i] = (*pLrg)[i] == (*pSml)[s];
		}
	}
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator!=(const OME_SCALAR & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = rVArch[insertPoint + i] != rhs;
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator!=(Listable & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		size_t insertPoint;
		ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
		ret->GetDims() = m_dims;
		for (size_t i = 0; i < m_length; ++i)
			(*ret)[i] = rVArch[insertPoint + i] != rhs[i];
	}
	else
	{
		Listable* pLrg, *pSml;
		if (m_dims.GetSize() > rhs.GetLevel())
		{
			pLrg = this;
			pSml = &rhs;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
		}
		size_t inStep = pLrg->GetSize() / pSml->GetSize();
		ret->GetDims() = pLrg->GetDims();
		
		for (size_t i = 0, s = 0, flip = 0; i<pLrg->GetSize(); ++i, ++flip)
		{
			if (flip >= inStep)
			{
				++s;
				flip = 0;
			}
			(*ret)[i] = (*pLrg)[i] != (*pSml)[s];
		}
	}
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator<=(const OME_SCALAR & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = rVArch[insertPoint + i] <= rhs;
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator<=(Listable & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		size_t insertPoint;
		ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
		ret->GetDims() = m_dims;
		for (size_t i = 0; i < m_length; ++i)
			(*ret)[i] = rVArch[insertPoint + i] <= rhs[i];
	}
	else
	{
		Listable* pLrg, *pSml;
		bool noInvert = m_dims.GetSize() > rhs.GetLevel();
		if (noInvert)
		{
			pLrg = this;
			pSml = &rhs;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
		}
		size_t inStep = pLrg->GetSize() / pSml->GetSize();
		ret->GetDims() = pLrg->GetDims();
		
		if (noInvert)
		{
			for (size_t i = 0, s = 0, flip = 0; i<pLrg->GetSize(); ++i, ++flip)
			{
				if (flip >= inStep)
				{
					++s;
					flip = 0;
				}
				(*ret)[i] = (*pLrg)[i] <= (*pSml)[s];
			}
		}
		else
		{
			for (size_t i = 0, s = 0, flip = 0; i<pLrg->GetSize(); ++i, ++flip)
			{
				if (flip >= inStep)
				{
					++s;
					flip = 0;
				}
				(*ret)[i] = !((*pLrg)[i] > (*pSml)[s]);
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator< (const OME_SCALAR & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = rVArch[insertPoint + i] < rhs;
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator< (Listable & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		size_t insertPoint;
		ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
		ret->GetDims() = m_dims;
		for (size_t i = 0; i < m_length; ++i)
			(*ret)[i] = rVArch[insertPoint + i]  < rhs[i];
	}
	else
	{
		Listable* pLrg, *pSml;
		bool noInvert = m_dims.GetSize() > rhs.GetLevel();
		if (noInvert)
		{
			pLrg = this;
			pSml = &rhs;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
		}
		size_t inStep = pLrg->GetSize() / pSml->GetSize();
		ret->GetDims() = pLrg->GetDims();

		if (noInvert)
		{
			for (size_t i = 0, s = 0, flip = 0; i<pLrg->GetSize(); ++i, ++flip)
			{
				if (flip >= inStep)
				{
					++s;
					flip = 0;
				}
				(*ret)[i] = (*pLrg)[i] < (*pSml)[s];
			}
		}
		else
		{
			for (size_t i = 0, s = 0, flip = 0; i<pLrg->GetSize(); ++i, ++flip)
			{
				if (flip >= inStep)
				{
					++s;
					flip = 0;
				}
				(*ret)[i] = !((*pLrg)[i] >= (*pSml)[s]);
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator>=(const OME_SCALAR & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = rVArch[insertPoint + i] >= rhs;
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator>=(Listable & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		size_t insertPoint;
		ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
		ret->GetDims() = m_dims;
		for (size_t i = 0; i < m_length; ++i)
			(*ret)[i] = rVArch[insertPoint + i] >= rhs[i];
	}
	else
	{
		Listable* pLrg, *pSml;
		bool noInvert = m_dims.GetSize() > rhs.GetLevel();
		if (noInvert)
		{
			pLrg = this;
			pSml = &rhs;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
		}
		size_t inStep = pLrg->GetSize() / pSml->GetSize();
		ret->GetDims() = pLrg->GetDims();

		if (noInvert)
		{
			for (size_t i = 0, s = 0, flip = 0; i<pLrg->GetSize(); ++i, ++flip)
			{
				if (flip >= inStep)
				{
					++s;
					flip = 0;
				}
				(*ret)[i] = (*pLrg)[i] >= (*pSml)[s];
			}
		}
		else
		{
			for (size_t i = 0, s = 0, flip = 0; i<pLrg->GetSize(); ++i, ++flip)
			{
				if (flip >= inStep)
				{
					++s;
					flip = 0;
				}
				(*ret)[i] = !((*pLrg)[i] < (*pSml)[s]);
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator>(const OME_SCALAR & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = rVArch[insertPoint + i] > rhs;
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator> (Listable & rhs)
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(std::max(m_length, rhs.GetSize()));
	if (m_dims.GetSize() == rhs.GetLevel())
	{
		size_t insertPoint;
		ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
		ret->GetDims() = m_dims;
		for (size_t i = 0; i < m_length; ++i)
			(*ret)[i] = rVArch[insertPoint + i]  > rhs[i];
	}
	else
	{
		Listable* pLrg, *pSml;
		bool noInvert = m_dims.GetSize() > rhs.GetLevel();
		if (noInvert)
		{
			pLrg = this;
			pSml = &rhs;
		}
		else
		{
			pLrg = &rhs;
			pSml = this;
		}
		size_t inStep = pLrg->GetSize() / pSml->GetSize();
		ret->GetDims() = pLrg->GetDims();

		if (noInvert)
		{
			for (size_t i = 0, s = 0, flip = 0; i<pLrg->GetSize(); ++i, ++flip)
			{
				if (flip >= inStep)
				{
					++s;
					flip = 0;
				}
				(*ret)[i] = (*pLrg)[i] > (*pSml)[s];
			}
		}
		else
		{
			for (size_t i = 0, s = 0, flip = 0; i<pLrg->GetSize(); ++i, ++flip)
			{
				if (flip >= inStep)
				{
					++s;
					flip = 0;
				}
				(*ret)[i] = !((*pLrg)[i] <= (*pSml)[s]);
			}
		}
	}
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::operator!()
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = !rVArch[insertPoint + i];
	ret->RecalcStep();
	return *ret;
}
Listable& VarArray::operator-()
{
	TEMP_LIST_TYPE* ret = TEMP_LIST_TYPE::NewTemp(m_length);
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	for (size_t i = 0; i < m_length; ++i)
		(*ret)[i] = -rVArch[insertPoint + i];
	ret->RecalcStep();
	return *ret;
}

Listable& VarArray::Negate()
{
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	for (size_t i = 0; i < m_length; ++i)
		rVArch[insertPoint + i] *= -1.0;
	return *this;
}

Listable& VarArray::Invert()
{
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	for (size_t i = 0; i < m_length; ++i)
		rVArch[insertPoint + i] = !rVArch[insertPoint + i];
	return *this;
}

Listable& VarArray::Reciprocate()
{
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	for (size_t i = 0; i < m_length; ++i)
		rVArch[insertPoint + i] = 1.0/rVArch[insertPoint + i];
	return *this;
}

Listable& VarArray::Subset(const size_t & start, const size_t & length)
{
	return *StaticUpArray::NewSUA(this,start,length);
}

Listable& VarArray::SubsetFromStep(const size_t & ind)
{
	StaticUpArray* ret = StaticUpArray::NewSUA(this, GetLevel()-1);
	ret->OffsetByAbsolute(ind*GetStepSize());
	return *ret;
}

Listable& VarArray::RefSubset(const size_t & start, const size_t & length)
{
	return Subset(start, length);
}

Listable& VarArray::RefSubsetFromStep(const size_t & ind)
{
	return SubsetFromStep(ind);
}

void VarArray::SubCollect(Listable & out, const size_t & offset, const size_t & outStart)
{
	size_t insertPoint;
	ValueArchive& rVArch = GETARCHINFOREF(insertPoint);
	size_t stepSize = GetStepSize();
	insertPoint += offset*stepSize;
	for (size_t i = 0; i < stepSize; ++i)
	{
		out[i + outStart] = rVArch[insertPoint+(i*stepSize)];
	}
}

//void VarArray::SubAggregate(Listable & out, AggFunc aggFunc)
//{
//
//	StaticUpArray tmp(this,GetLevel()-1);
//
//	for (size_t i = 0; i < m_dims.Innermost(); ++i, tmp.OffsetByStep(i))
//	{
//		out[i] = aggFunc(tmp);
//	}
//}

ListableSclrItr VarArray::SclrBegin()
{
	
	size_t insertPoint = m_pParentModel->GetArchiveIndex(m_mdlIndex,m_length);
	return ListableSclrItr((Listable*)this, (size_t)insertPoint, (size_t)GetSize()+insertPoint);
	//Listable* pSrc,void* initRef,const size_t & initPos,const size_t & len,IncFunc func
}

OME_SCALAR& VarArray::ValForRef(void*& ref, const size_t & pos,const bool & posChanged)
{
	if (posChanged)
	{
		ValueArchive& rVArch = *m_pParentModel->GetValueArchive();
		ref = &rVArch[pos];
	}
	return *((OME_SCALAR*)ref);
}
