#include "stdafx.h"
#include "SimpleTableView.h"
#include "TableResultsDetails.h"

using namespace OMESimRunner;

/// <summary>Refresh data from source data set.</summary>
void SimpleTableView::ReloadData()
{
	TableResultsDetails* pDetails = (TableResultsDetails*)m_sim->ViewMgr->GetActiveDetails();
	Model* rootMdl = m_sim->RootModel;
	m_tableSource->Clear();
	//cycle through entries and build list for viewing
	EvalRepArray reps = pDetails->GetPartialReps();
	for (auto itr = reps.begin(); itr != reps.end(); ++itr)
	{
		OMEObject* pObj = rootMdl->ObjectForKey((*itr).id);
		Model* pMdl;
		if (pObj)
		{
			TblRepNode^ node = gcnew TblRepNode(pObj);
			node->Color = Color::FromArgb(255, (*itr).color.r * 255, (*itr).color.g * 255, (*itr).color.b * 255);
			if (!(*itr).selected.empty())
			{
				for (auto liItr = (*itr).selected.begin(); liItr != (*itr).selected.end(); ++liItr)
					node->AppendCoordinate(*liItr);
			}
			m_tableSource->Add(node);

		}
		else
		{
			msclr::interop::marshal_context context;
			MessageBox::Show(context.marshal_as<String^>("Missing object" + (*itr).id));
		}
	}
}

/// <summary>Populate a ScopeSelector object.</summary>
/// <param name="rep">Handle of rep to use to populate ScopeSelector.</param>
/// <param name="scpSel">Handle to the ScopeSelector to populate.</param>
System::Void SimpleTableView::AddLevelsToScopeSelector(TblRepNode^ rep, ScopeSelector^ scpSel)
{
	msclr::interop::marshal_context context;
	Evaluable* pEval = dynamic_cast<Evaluable*>(rep->Object);

	if (pEval)
	{
		Model* pMdl = pEval->GetParentModel();
		unsigned int currInd = 0;

		switch (pEval->GetOMEType())
		{
			//todo: add Submodelport?
		case OME_VARARRAY:
			for (unsigned int i = 0; i < ((VarArray*)pEval)->GetDimCount(); ++i)
				scpSel->AddEntry(String::Format("{0}_{1}", context.marshal_as<String^>(pEval->GetName()), i + 1), rep->Coord[currInd++]);

		default:

			while (pMdl != NULL && pMdl->GetParentModel() != NULL)
			{
				scpSel->AddEntry(context.marshal_as<String^>(pMdl->GetName()), rep->Coord[currInd++]);
				pMdl = pMdl->GetParentModel();
			}
		}
	}
}

/// <summary>Retrieve coords from ScopeSelector.</summary>
/// <param name="rep">Handle of rep to receive values from the ScopeSelector.</param>
/// <param name="scpSel">Handle to the ScopeSelector to retrieve values from.</param>
/// <param name="index">The repNode index (used to map to details backend).</param>
System::Void SimpleTableView::GetIndicesFromScopeSelector(TblRepNode^ rep, ScopeSelector^ scpSel, int index)
{
	TableResultsDetails* pDetails = (TableResultsDetails*)m_sim->ViewMgr->GetActiveDetails();
	for (unsigned int i = 0; i < scpSel->LevelCount; ++i)
	{
		rep->Coord[i] = scpSel->Index[i];
		pDetails->SetCoordForRep(index, i, rep->Coord[i]);
	}

}