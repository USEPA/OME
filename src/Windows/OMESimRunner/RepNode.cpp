#include "RepNode.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace OMESimRunner;

/// <summary>Detailed Constructor. </summary>
/// <param name="pObj">Pointer to Object to represent.</param>
RepNode::RepNode(OMEObject* pObj)
	:m_pObj(pObj), m_index(0)
{
	if (m_pObj)
	{
		msclr::interop::marshal_context context;
		m_name = context.marshal_as<String^>(m_pObj->GetName())->Replace("\n", " ");
	}
	m_txtColor = Drawing::Color::Black;
}

/// <summary>Compare function used for sorting. </summary>
/// <param name="obj">Handle of object to compare against.</param>
/// <returns> 1 if Model is compared to non-model, -1 if non-model is compared to model, and -2 otherwise.</returns>
Int32 RepNode::CompareTo(System::Object^ obj)
{
	//for now, just sort like this.
	Int32 result = -2;
	RepNode^ repCast = (RepNode^)obj;

	if (m_pObj->GetOMEType() == OME_MODEL && repCast->ObjectType != OME_MODEL)
		result = 1;
	else if (m_pObj->GetOMEType() != OME_MODEL && repCast->ObjectType == OME_MODEL)
		result = -1;
	return result;
}
