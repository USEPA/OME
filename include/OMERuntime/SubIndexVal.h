#pragma once

#include "OMEDefines.h"

#include "ValueArchive.h"
#include "omeobjects/Evaluable.h"
#include "omeobjects/VarArray.h"
#include "omeobjects/Model.h"

/** Workaround for retrieving values similar to Simile's "element" call. */
template<class TSRC>
class SubIndexVal
{
public:
	inline SubIndexVal(Evaluable& eval, const size_t & depth,const bool & useRawInd=false);
	inline SubIndexVal(OME_SCALAR* tmpList, size_t* depthDims, const size_t & depth);

	inline SubIndexVal(const SubIndexVal & siv);

	inline OME_SCALAR operator[](const size_t & ind);
	inline SubIndexVal Sub(const size_t & ind) const;
	inline operator OME_SCALAR();
	inline operator Evaluable&();

	inline SubIndexVal& operator=(const OME_SCALAR & rhs);

	inline OME_SCALAR GetPrev() const;
	inline OME_SCALAR GetPrev(const OME_SCALAR & offset) const;

protected:

	TSRC m_pValSrc;		///< Pointer to the data source.

	union
	{
		Evaluable* m_pEval;		///< Pointer to the represented Evaluable object.
		size_t* m_depthInds;	///< Pointer to an array containing dimensional limits.
	};

	size_t m_start;				///< The start index.
	size_t m_step;				///< The stride for increment along the index.
	size_t m_dim;				///< The limit of the current dimension.

private:
	inline void RefreshSubStep();
};


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--==-=-=-=-=-
/** Evaluable-targetting constructor.
	@param eval The Evaluable object to target.
	@param depth Indicator of the number of dimensions to cover.
	@param useRawInd If true, start indexing from eval's first archive index.
*/
template<class TSRC>
SubIndexVal<TSRC>::SubIndexVal(Evaluable& eval, const size_t & depth,const bool & useRawInd)
:m_pEval(&eval), m_dim(depth), m_pValSrc(*eval.GetParentModel()->GetValueArchive())
{
	if (!useRawInd)
	{
		size_t step = 1;
		if (m_pEval->IsOMEType(OME_VARARRAY))
			step = ((VarArray*)m_pEval)->GetSize();
		m_start = m_pEval->GetParentModel()->GetArchiveIndex(m_pEval->GetModelIndex(),step);
	}
	else
		m_start = m_pEval->GetModelIndex();
	RefreshSubStep();
}

/** Simple array-targetting constructor.
	@param tmpList The target array.
	@param depthDims Array containing the limits for each dimension.
	@param depth Indicator of the number of dimensions to cover.
*/
template<class TSRC>
SubIndexVal<TSRC>::SubIndexVal(OME_SCALAR* tmpList, size_t* depthDims, const size_t & depth)
:m_depthInds(depthDims), m_dim(depth), m_start(0), m_pValSrc(tmpList)
{
	RefreshSubStep();
}

/** Copy Constructor.
	@param siv The SubIndexVal to copy.
*/
template<class TSRC>
SubIndexVal<TSRC>::SubIndexVal(const SubIndexVal & siv)
:m_pEval(siv.m_pEval), m_dim(siv.m_dim), m_pValSrc(siv.m_pValSrc), m_start(siv.m_start), m_step(siv.m_step)
{

}

/** Simple Index Operator.
	@param ind The index/offset to query.
	@return The value found at the specified index.
*/
template<class TSRC>
OME_SCALAR SubIndexVal<TSRC>::operator[](const size_t & ind)
{
	return m_pValSrc[m_start + ind];
}

/** Simple Index Operator (ValueArchive specialization).
	@param ind The index/offset to query.
	@return The value found at the specified index.
*/
template<>
inline OME_SCALAR SubIndexVal<ValueArchive&>::operator[](const size_t & ind)
{
	size_t instNum = (m_start + ind) - m_pEval->GetModelIndex();
	if (m_pEval->IsOMEType(OME_VARARRAY))
		instNum /= ((VarArray*)m_pEval)->GetSize();
	bool isActive = m_pEval->GetParentModel()->GetRawInstance(instNum)->IsActive();
	return isActive ? m_pValSrc[m_start + ind] : 0.0;
}

/** Produces a representation of the present list with one dimension less than that used by the current SubIndexVal.
	@param ind The index to start the sub partition.
	@return A SubIndexVal object with the same source and -1 dimensions within its representation.
*/
template<class TSRC>
SubIndexVal<TSRC> SubIndexVal<TSRC>::Sub(const size_t & ind) const
{
	//if (!m_step)
	//	throw err

	SubIndexVal<TSRC> ret(*this);
	--ret.m_dim;
	ret.m_start += ind*m_step;
	ret.RefreshSubStep();
	return ret;
}

/** Scalar cast operator (ValueArchive specialization).
	@return Scalar value.
*/
template<>
inline SubIndexVal<ValueArchive&>::operator OME_SCALAR()
{
//	size_t instNum = m_start-m_pEval->GetModelIndex();
//	if (m_pEval->IsOMEType(OME_VARARRAY))
//		instNum /= ((VarArray*)m_pEval)->GetSize();

	return m_pValSrc[m_start];
}

/** Scalar cast operator.
@return Scalar value.
*/
template<class TSRC>
SubIndexVal<TSRC>::operator OME_SCALAR()
{
	return m_pValSrc[m_start];
}

/** Recalculate the stride for each increment of the index operation (ValueArchive specialization).
	
*/
template<>
inline void SubIndexVal<ValueArchive&>::RefreshSubStep()
{
	m_step = 0;
	if (m_dim)
	{

		Model* pMdl = m_pEval->GetParentModel();

		size_t count = 1;
		m_step = 1;

		size_t slotCount = 1;

		VarArray* pVa = dynamic_cast<VarArray*>(m_pEval);
		if (pVa)
		{

			for (size_t i = 0; count < m_dim && count <= pVa->GetDimCount(); ++i, ++count)
			{
				slotCount *= pVa->GetDim(i);
			}
		}

		if (count < m_dim)
		{
			//m_step = pMdl->GetNumRawInstances();
			//pMdl = pMdl->GetParentModel();
			while (count < m_dim && pMdl)
			{
				if (pMdl->IsDynamic() || pMdl->GetInitNumInstances() != 1)
				{
					m_step *= pMdl->GetNumInstances();
					++count;
				}

				pMdl = pMdl->GetParentModel();
			}
		}

		m_step *= slotCount;
	}
}

/** Recalculate the stride for each increment of the index operation.

*/
template<>
inline void SubIndexVal<OME_SCALAR*>::RefreshSubStep()
{
	m_step = 0;
	if (m_dim)
	{

		m_step = 1;
		size_t slotCount = 1;
		for (size_t i = m_dim-1; i >0; --i)
			slotCount *= m_depthInds[i];
		m_step *= slotCount;
	}
}

/** Retrieves associated Evaluable object. */
template<class TSRC>
SubIndexVal<TSRC>::operator Evaluable&()
{
	return *m_pEval;
}

/** Retrieves associated Evaluable object (Scalar array specialization). Presently throws an error. */
template<>
inline SubIndexVal<OME_SCALAR*>::operator Evaluable&()
{
	throw OMESupportException("SubIndexVal::Evaluable(): Cannot be called when representing simple array");
	return *m_pEval;
}

/**Assigns Scalar value to first represented entry.
	@param rhs The scalar value to assign.
	@return A reference to this.
*/
template<class TSRC>
SubIndexVal<TSRC>& SubIndexVal<TSRC>::operator=(const OME_SCALAR & rhs)
{
	m_pValSrc[m_start] = rhs;
	return *this;
}

/** @return Previous value for represented object.*/
template<>
inline OME_SCALAR SubIndexVal<ValueArchive&>::GetPrev() const
{
	return m_pValSrc.GetPreviousValue(m_start);
}

template<class TSRC>
OME_SCALAR SubIndexVal<TSRC>::GetPrev() const
{
	throw OMESupportException("SubIndexVal::GetPrev(): not supported for generalized templates");
	return 0.0;
}

/**	@param offset Offset to the previous value.
	 @return Previous value for represented object.
*/
template<>
inline OME_SCALAR SubIndexVal<ValueArchive&>::GetPrev(const OME_SCALAR & offset) const
{
	return m_pValSrc.GetValueForTime(m_start,m_pEval->GetParentModel()->GetManager()->GetPresentTime()-offset);
}

template<class TSRC>
OME_SCALAR SubIndexVal<TSRC>::GetPrev(const OME_SCALAR & offset) const
{
	throw OMESupportException("SubIndexVal::GetPrev(): not supported for generalized templates");
	return 0.0;
}