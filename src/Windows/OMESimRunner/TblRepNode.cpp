#include"TblRepNode.h"

using namespace OMESimRunner;

/// <summary>Detailed constructor.</summary>
/// <param name="pObj">Pointer to represented object.</param>
TblRepNode::TblRepNode(OMEObject* pObj)
	:RepNode(pObj)
{
	m_coords = gcnew Generic::List < size_t > ;
}

Int32 TblRepNode::CompareTo(System::Object^ obj)
{
	Int32 ret= RepNode::CompareTo(obj);

	if (!ret)
	{
		TblRepNode^ rhs = (TblRepNode^)obj;
		size_t lim = min(m_coords->Count, rhs->m_coords->Count);

		unsigned int i;
		for (unsigned int i = 0; i < lim && m_coords[i] == rhs->m_coords[i]; ++i);

		if (i < lim)
		{
			ret = m_coords[i] < rhs->m_coords[i] ? -1 : 1;
		}
		else
		{
			if (m_coords->Count == rhs->m_coords->Count)
				ret = 0;
			else
				ret = m_coords->Count < rhs->m_coords->Count ? -1 : 1;
		}

		return ret;
	}
}

/// <summary>Add coordinate to TblRepNode.</summary>
/// <param name="coord">The coordinate to add.</param>
void TblRepNode::AppendCoordinate(const size_t & coord)
{
	m_coords->Add(coord);
}

/// <summary>Remove all associated coordinates.</summary>
void TblRepNode::ClearCoordinates()
{
	m_coords->Clear();
}

/// <summary>Look up string representation of value.</summary>
/// <returns>The string representation of the node's value.</returns>
String^ TblRepNode::FindValStr()
{
	String^ ret = "Invalid index range";

	if (m_coords && m_coords->Count > 0)
	{
		//this is going to be super clunky, but it should work
		unsigned int mdlOffset = 0;
		if (m_pObj->IsOMEType(OME_VARARRAY))
		{
			mdlOffset = ((VarArray*)m_pObj)->GetDimCount();
			PersistListDims tmpDims(mdlOffset);
			for (unsigned int i = 0; i < mdlOffset; ++i)
				tmpDims[i] = m_coords[i];
			m_index = Listable::DeriveIndex(tmpDims, ((VarArray*)m_pObj)->GetDims());
		}
		//set the current model instances
		Model* pMdl = m_pObj->GetParentModel();

		for (unsigned int i = mdlOffset; pMdl && i < m_coords->Count; ++i, pMdl = pMdl->GetParentModel())
		{
			//check for out of range index
			if (m_coords[i] - 1 >= pMdl->GetNumInstances())
				return ret;
			pMdl->SetCurrentInstance(m_coords[i] - 1);
		}

		
		ret = String::Format("{0}", Value);
	}
	else
		ret =String::Format("{0}", Value);

	return ret;
}