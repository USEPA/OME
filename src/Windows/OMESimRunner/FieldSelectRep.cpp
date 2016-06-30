#include "FieldSelectRep.h"
#include <msclr\marshal_cppstd.h>

using namespace OMESimRunner;
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

/// <summary>Evaluable constructor.</summary>
/// <param name="pEval">Pointer to the Evaluable object to use in the construction of the rep.</param>
FieldSelectRep::FieldSelectRep(Evaluable* pEval)
	:m_pColor(new OMEColor), m_enabled(false), m_coords(nullptr)
{
	LoadFromEval(pEval);
}

/// <summary>Evaluable/Rep constructor.</summary>
/// <param name="pEval">Pointer to the Evaluable object to use in the construction of the rep.</param>
/// <param name="pRep">Pointer to the EvalRep object to use in the construction of the rep.</param>
FieldSelectRep::FieldSelectRep(Evaluable* pEval,EvalRep* pRep)
	: m_enabled(true), m_coords(nullptr)
{
	LoadFromEval(pEval);
	m_pColor= new OMEColor(pRep->color);
}

/// <summary>EvalRep constructor.</summary>
/// <param name="pRep">Pointer to the EvalRep object to use in the construction of the rep.</param>
/// <param name="isEnabled">Mark if node is enabled.</param>
FieldSelectRep::FieldSelectRep(EvalRep* pRep,bool isEnabled)
	:m_enabled(isEnabled), m_coords(nullptr)
{
	msclr::interop::marshal_context context;

	m_name=context.marshal_as<String^>(pRep->name);
	m_id=context.marshal_as<String^>(pRep->id);
	m_type=context.marshal_as<String^>(OMEObject::LabelForType(pRep->type));

	m_parentModel=context.marshal_as<String^>(pRep->parentModel);
	m_modelPath=context.marshal_as<String^>(pRep->modelPath);
	m_pColor=new OMEColor(pRep->color);

	if (!pRep->selected.empty())
	{
		m_coords = gcnew Generic::List<size_t>();
		for (auto itr = pRep->selected.begin(); itr != pRep->selected.end(); ++itr)
		{
			m_coords->Add(*itr);
		}
	}
}

/// <summary>Copy constructor.</summary>
/// <param name="toCopy">Handle to FieldSelectRep to handle copy construction.</param>
FieldSelectRep::FieldSelectRep(const FieldSelectRep^ toCopy)
	: m_name(toCopy->m_name)
	, m_id(toCopy->m_id)
	, m_type(toCopy->m_type)
	, m_parentModel(toCopy->m_parentModel)
	, m_modelPath(toCopy->m_modelPath)
	, m_coords(nullptr)
	
{
	
	if (toCopy->m_coords)
	{
		m_coords = gcnew Generic::List<size_t>();
		for (unsigned short i = 0; i < toCopy->m_coords->Count; ++i)
			m_coords->Add(toCopy->m_coords[i]);
	}
	m_pColor = new OMEColor(*toCopy->m_pColor);
}

FieldSelectRep::~FieldSelectRep()
{
	delete m_pColor;
}

/// <summary>Populate rep from contents of Evaluable.</summary>
/// <param name="pEval">Pointer to the Evaluable object to use to populate the rep.</param>
void FieldSelectRep::LoadFromEval(Evaluable* pEval)
{
	msclr::interop::marshal_context context;
	m_name=context.marshal_as<String^>(pEval->GetName());
	m_id=context.marshal_as<String^>(pEval->GetID());
	m_type=context.marshal_as<String^>(pEval->LabelForType());

	Model* pMdl=pEval->GetParentModel();
	
	if(pMdl)
	{
		m_parentModel= context.marshal_as<String^>(pEval->GetParentModel()->GetName());
		StrList parts;
		pMdl->GetUpPath(parts);
		m_modelPath=context.marshal_as<String^>(ConcatStrings(parts,'.'));
	}
}

/// <summary>Populate a EvalRep using the contents of the FieldSelectRep.</summary>
/// <param name="pRep">Pointer to the EvalRep to populate.</param>
System::Void FieldSelectRep::PopulatePartialRep(EvalRep* pRep)
{
	msclr::interop::marshal_context context;
	pRep->id=context.marshal_as<STLString>(m_id);
	pRep->color=*m_pColor;
	
	pRep->selected.clear();
	if (m_coords)
	{
		for (unsigned i = 0; i < m_coords->Count; ++i)
			pRep->selected.push_back(m_coords[i]);
	}
}

/// <summary>Add a coordinate to the rep.</summary>
/// <param name="coord">The coordinate component to add.</param>
System::Void FieldSelectRep::AddCoord(unsigned int coord)
{
	if (!m_coords)
		m_coords = gcnew Generic::List<size_t>(1);
	m_coords->Add(coord);
	
}

/// <summary>Clear all coordinates from the rep.</summary>
System::Void FieldSelectRep::ClearCoords()
{
	if (m_coords)
		m_coords->Clear();
}