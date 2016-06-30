#include "StdAfx.h"
#include "FieldSelector.h"

using namespace OMESimRunner;

/// <summary>Simple constructor.</summary>
/// <param name="pRD">Pointer to the source details view.</param>
/// <param name="sim">Handle to the SimWrapper object.</param>
/// <param name="upper">Text to display above the upper box.</param>
/// <param name="lower">Text to display above the lower box.</param>
FieldSelector::FieldSelector(BaseResultsDetails* pRD, SimWrapper^ sim, String^ upper, String^ lower)
	:FieldSelector(pRD, sim, upper, lower, false, nullptr, nullptr)
{

}

/// <summary>Detailed constructor.</summary>
/// <param name="pRD">Pointer to the source details view.</param>
/// <param name="sim">Handle to the SimWrapper object.</param>
/// <param name="upper">Text to display above the upper box.</param>
/// <param name="lower">Text to display above the lower box.</param>
/// <param name="redundantSelect">Flag indicating whether or not a field can be selected more than once.</param>
/// <param name="addText">Text to apply to addButton.</param>
/// <param name="remText">Text to apply to removeButton.</param>
FieldSelector::FieldSelector(BaseResultsDetails* pRD, SimWrapper^ sim, String^ upper, String^ lower, bool redundantSelect, String^ addText, String^ remText)
	:m_pRD(pRD), m_sim(sim), m_redundantSelect(redundantSelect)
{
	InitializeComponent();

	selectableObjs->AutoGenerateColumns = false;
	selectedFields->AutoGenerateColumns = false;

	m_available = gcnew BindingSource();
	m_selected = gcnew BindingSource();

	selectableObjs->DataSource = m_available;
	selectedFields->DataSource = m_selected;

	label1->Text = upper;
	label2->Text = lower;

	m_sim->LoadFieldData(m_available, m_selected,!m_redundantSelect);
	selectableObjs->CurrentCell = nullptr;
	selectedFields->CurrentCell = nullptr;
	m_upperFilter = "";
	m_lowerFilter = "";

	if (addText)
	{
		addButton->Text = addText;
		addButton->Font = addButton->DefaultFont;
	}

	if (remText)
	{
		removeButton->Text = remText;
		removeButton->Font = removeButton->DefaultFont;
	}

	ALS_FONT = gcnew System::Drawing::Font(this->Font, FontStyle::Italic);

}

/// <summary>Enable a specific source field column.</summary>
/// <param name="columnID">The id of the column to enable.</param>
/// <returns>true if there is a column matching columnID; false otherwise.</returns>
bool FieldSelector::EnableSFColumn(String^ columnID)
{
	if (selectedFields->Columns[columnID])
	{
		selectedFields->Columns[columnID]->Visible = true;
		return true;
	}
	return false;
}

/// <summary>Disable a specific source field column.</summary>
/// <param name="columnID">The id of the column to disable.</param>
/// <returns>true if there is a column matching columnID; false otherwise.</returns>
bool FieldSelector::DisableSFColumn(String^ columnID)
{
	if (selectedFields->Columns[columnID])
	{
		selectedFields->Columns[columnID]->Visible = false;
		return true;
	}
	return false;
}

/// <summary>Determine what coordinates can be selected for a specific field.</summary>
System::Void FieldSelector::CalcSelectableRepCoords()
{
	msclr::interop::marshal_context context;

	Model* pRootMdl = m_sim->RootModel;
	Model* pMdl;
	//add specific coord here when ready
	for (unsigned int i = 0; i < m_available->Count; ++i)
	{
		FieldSelectRep^ node = (FieldSelectRep^)m_available[i];
		Evaluable* pEval = (Evaluable*)pRootMdl->ObjectForKey(context.marshal_as<STLString>(node->ID)); //deep search

		if (pEval)
		{
			switch (pEval->GetOMEType())
			{

			case OME_SUBPORT:
				pMdl = ((SubmodelPort*)pEval)->GetOuterModel();
				if (pMdl)
				{
					while (pMdl->GetParentModel() != NULL)
					{
						node->AddCoord(1);
					}
				}
				break;
			case OME_VARARRAY:
				for (unsigned short i = 0; i < ((VarArray*)pEval)->GetDimCount(); ++i)
				{
					node->AddCoord(1);
				}
				//drop through
			default:
				pMdl = pEval->GetParentModel();

				while (pMdl != NULL && pMdl->GetParentModel()!=NULL)
				{
					//if ((m_sim->HasRun ? pMdl->GetActiveInstanceCount() : pMdl->GetInitNumInstances())>1 || pMdl->IsDynamic() || pMdl->IsConditional())
					//{
						node->AddCoord(1);
					//}
					pMdl = pMdl->GetParentModel();
				}

				break;
			}
			
		}
	}
}

/// <summary>Swap row between DGVs.</summary>
/// <param name="src">The first DGV to involve in the swap.</param>
/// <param name="dst">The second DGV to involve in the swap.</param>
System::Void FieldSelector::SwapRows(DataGridView^ src, DataGridView^ dst)
{
	SwapRows(src, dst, true, true, false);
}

/// <summary>Swap row between DGVs.</summary>
/// <param name="src">The source DGV that contains the row to swap.</param>
/// <param name="dst">The destination DGV that will contain the row after swap.</param>
/// <param name="chgSrc">If true, remove the selected field after it is added to dst.</param>
/// <param name="chgDst">If true, add the selected field to dst.</param>
/// <param name="newCopy">If true a new copy of the entry is created on copy. </param>
System::Void FieldSelector::SwapRows(DataGridView^ src, DataGridView^ dst, const bool % chgSrc, const bool % chgDst, const bool % newCopy)
{
	DataGridViewSelectedRowCollection^ rows = src->SelectedRows;

	if (rows->Count)
	{
		for (int i = 0; i<rows->Count; i++)
		{
			FieldSelectRep^ currRep = (FieldSelectRep^)rows[i]->DataBoundItem;

			if (chgSrc)
				((BindingSource^)src->DataSource)->Remove(currRep);
			if (chgDst)
			{
				if (!newCopy)
					((BindingSource^)dst->DataSource)->Add(currRep);
				else
					((BindingSource^)dst->DataSource)->Add(gcnew FieldSelectRep(currRep));
			}
		}
		if (chgSrc)
			src->ClearSelection();
		if (chgDst)
			dst->ClearSelection();
	}
}

/// <summary> insert the selected rows in src into dest at index row.</summary>
/// <param name="src">The source DGV containing the block of selected rows.</param>
/// <param name="dst">The destination DGV where the rows will be inserted.</param>
/// <param name="row">The point of insertion in dst.</param>
System::Void FieldSelector::InsertRows(DataGridView^ src, DataGridView^ dst, int row)
{
	DataGridViewSelectedRowCollection^ rows = src->SelectedRows;

	if (rows->Count)
	{
		for (int i = 0; i<rows->Count; i++)
		{
			FieldSelectRep^ currRep = (FieldSelectRep^)rows[i]->DataBoundItem;
			((BindingSource^)src->DataSource)->Remove(currRep);
			if (row != -1)
				((BindingSource^)dst->DataSource)->Insert(i + row, currRep);
			else
				((BindingSource^)dst->DataSource)->Add(currRep);
		}
		src->ClearSelection();
		dst->ClearSelection();
	}
}

/// <summary>Apply filtered text to viewable rows.</summary>
/// <param name="dgv">The DGV to be filtered.</param>
/// <param name="prefix">The string used to find the field to act upon.</param>
/// <param name="filterText">The text to filter against.</param>
System::Void FieldSelector::UpdateFilter(DataGridView^ dgv, String^ prefix,String^ filterText)
{
	bool isVisible;
	if (filterText->Length)
	{
		int nameInd = dgv->Columns[prefix + "Name"]->Index;
		int idInd = dgv->Columns[prefix + "ID"]->Index;

		dgv->CurrentCell = nullptr;
		String^ adjFilt = filterText->ToLower();
		String^ chkStr;
		for (int i = 0; i < dgv->Rows->Count; ++i)
		{
			chkStr = (String^)dgv->Rows[i]->Cells[nameInd]->Value;
			isVisible = chkStr->ToLower()->Contains(adjFilt);
			if (!isVisible)
			{
				chkStr = (String^)dgv->Rows[i]->Cells[idInd]->Value;
				isVisible = chkStr->ToLower()->Contains(adjFilt);
			}
			dgv->Rows[i]->Visible = isVisible;
		}
		dgv->Refresh();
	}
	else
		ClearFilter(dgv);
}

/// <summary>Clear out any filters presently in use.</summary>
/// <param name="dgv">Handle of DGV to clear.</param>
System::Void FieldSelector::ClearFilter(DataGridView^ dgv)
{
	for (int i = 0; i < dgv->Rows->Count; ++i)
		dgv->Rows[i]->Visible = true;
	dgv->Refresh();
}

/// <summary>Refresh the filters in both DGVs.</summary>
System::Void FieldSelector::RefreshAllFilters()
{
	UpdateFilter(selectableObjs, "SO", m_upperFilter);
	UpdateFilter(selectedFields, "SF", m_lowerFilter);
}

/// <summary>Add selectable levels to ScopeSelector Window.</summary>
/// <param name="rep">Handle to the FieldSelectorRep to use for source information.</param>
/// <param name="scpSel">Handle to the ScopeSelector to modify.</param>
System::Void FieldSelector::AddLevelsToScopeSelector(FieldSelectRep^ rep, ScopeSelector^ scpSel)
{
	msclr::interop::marshal_context context;
	Evaluable* pEval = (Evaluable*)m_sim->RootModel->ObjectForKey(context.marshal_as<STLString>(rep->ID));
	
	if (pEval)
	{
		Model* pMdl=pEval->GetParentModel();
		unsigned int currInd=0;

		switch (pEval->GetOMEType())
		{
			//todo: add Submodelport?
		case OME_VARARRAY:
			for (unsigned int i = 0; i < ((VarArray*)pEval)->GetDimCount(); ++i)
				scpSel->AddEntry(String::Format("{0}_{1}", context.marshal_as<String^>(pEval->GetName()), i + 1),rep->Coord[currInd++]);

		default:
			
			while (pMdl != NULL && pMdl->GetParentModel() != NULL)
			{
				scpSel->AddEntry(context.marshal_as<String^>(pMdl->GetName()), rep->Coord[currInd++]);
				pMdl = pMdl->GetParentModel();
			}
		}
	}
}

/// <summary>Retrieve selected scopes and add to FieldSelectRep.</summary>
/// <param name="rep">Handle to the FieldSelectorRep to use for source information.</param>
/// <param name="scpSel">Handle to the ScopeSelector to modify.</param>
System::Void FieldSelector::GetIndicesFromScopeSelector(FieldSelectRep^ rep, ScopeSelector^ scpSel)
{
	for (unsigned int i = 0; i < scpSel->LevelCount; ++i)
	{
		rep->Coord[i] = scpSel->Index[i];
	}

}