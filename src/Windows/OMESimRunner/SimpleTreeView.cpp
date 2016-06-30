#include "StdAfx.h"
#include "SimpleTreeView.h"

using namespace OMESimRunner;

/// <summary>Populate Rep Tree using pMdl as root.</summary>
/// <param name="pMdl">Pointer to Root Model.</param>
/// <returns> Handle to root TreeRepNode.</returns>
TreeRepNode^ SimpleTreeView::PopulateTree(Model* pMdl)
{
	TreeRepNode^ ret = PopulateTree(pMdl, 0, m_pTreeControl);
	m_pTreeControl->Root = ret;
	RepopulateBS(ret);
	return ret;
}

/// <summary>Populate Rep Tree using pMdl as root.</summary>
/// <param name="pRoot">Pointer to Root Model.</param>
/// <param name="level">The level to apply to the new TreeRepNode.</param>
/// <param name="ctrl">Handle to the sort controlling object.</param>
/// <returns> Handle to root TreeRepNode.</returns>
TreeRepNode^ SimpleTreeView::PopulateTree(Model* pRoot, int level, RNControl^ ctrl)
{
	TreeRepNode^ parent;
	parent= gcnew TreeRepNode((Evaluable*)pRoot, level,ctrl);
	
	parent->IsParent=true;
	EvalArray subs;
	pRoot->GetAllEvals(subs,DT_NO_AUTOGEN | DT_ALIAS);

	level++;
	size_t i;
	for(i=0; i<subs.size(); i++)
	{
		if (subs[i]->GetOMEType() == OME_ALIAS)
		{
			EvalAlias* pAlias = (EvalAlias*)subs[i];
			//if alias is referencing the src object using the same name and parent model, omit
			if (pAlias->GetName() == pAlias->GetSrc()->GetName() && pAlias->GetParentModel() == pAlias->GetSrc()->GetParentModel())
				continue;
			//if another alias with the same name and parent exist, omit
			bool match = false;
			for (size_t i = 0; !match && i < parent->SubNodeCount; ++i)
			{
				EvalAlias* pTest = dynamic_cast<EvalAlias*>(parent->GetNode(i)->Object);
				match = pTest && pTest->GetName() == pAlias->GetName() && pTest->GetSrc() == pAlias->GetSrc();
			}
			if (match)
				continue;
		}
		TreeRepNode^ leaf = gcnew TreeRepNode(subs[i], level, ctrl);
		parent->AddNode(leaf);
	}

	TreeRepNode^ sub;
	for (i = 0; i < pRoot->GetSubmodelCount(); i++)
		parent->AddNode(PopulateTree(pRoot->GetSubmodel(i), level, ctrl));
	
	parent->SortSubs();
	return parent;
}

/// <summary>Recursively resort nodes.</summary>
/// <param name="parent"> Handle to parent node.</param>
System::Void SimpleTreeView::ResortNodes(TreeRepNode^ parent)
{
	parent->SortSubs();
	for (int i = 0; i < parent->SubNodeCount; i++)
	{
		if (parent->GetNode(i)->IsParent)
			ResortNodes(parent->GetNode(i));
	}
}

/// <summary>Rebuild source tree.</summary>
/// <param name="root"> Handle to root node of tree.</param>
System::Void SimpleTreeView::RepopulateBS(TreeRepNode^ root)
{
	m_treeSource->Clear();
	AddBSNode(root);
}

/// <summary>Add node to tree source object (which is a flat list).</summary>
/// <param name="node"> Handle to node to add.</param>
System::Void SimpleTreeView::AddBSNode(TreeRepNode^ node)
{
	m_treeSource->Add(node);
	for (int i = 0; i < node->SubNodeCount; i++)
		AddBSNode(node->GetNode(i));
}

/// <summary>Refresh active/visible nodes.</summary>
System::Void SimpleTreeView::RefreshActive()
{
	TreeResultsDetails* pDetails=(TreeResultsDetails*)m_sim->ViewMgr->GetActiveDetails();
	EvalRepArray reps=pDetails->GetPartialReps();
	msclr::interop::marshal_context context;
	STLString id;
	TreeRepNode^ currNode;
	for(size_t i=0; i<m_treeSource->Count; i++)
	{
		currNode=(TreeRepNode^)m_treeSource[i];
		if(currNode->ObjectType!=OME_MODEL)
		{
			currNode->Active=false;
			id=currNode->Object->GetID();
			for(auto itr=reps.begin(); itr!=reps.end() && !currNode->Active; ++itr)
			{
				if (id == (*itr).id)
				{
					currNode->Active = true;
					currNode->Color = Color::FromArgb(255, (*itr).color.r * 255, (*itr).color.g * 255, (*itr).color.b * 255);
				}
			}
		}
	}

	UpdateVisibility(false,true);
}

/// <summary>Update visibility of collapsed/expanded nodes.</summary>
System::Void SimpleTreeView::UpdateVisibility(const bool & resort,const bool & visCheck)
{
	TreeResultsDetails* pCurrDetails = (TreeResultsDetails*)m_sim->ViewMgr->GetActiveDetails();
	bool useSelect = pCurrDetails->GetUseSelected();
	
	bool isVisible;
	TreeRepNode^ currNode;
	//first pass, find absolute visibility
	if (resort && m_pTreeControl->Updated && m_pTreeControl->Root)
	{
		m_pTreeControl->IsAscending = pCurrDetails->GetSortAsc();
		m_pTreeControl->SortMode = pCurrDetails->GetSortMode();
		ResortNodes(m_pTreeControl->Root);
		RepopulateBS(m_pTreeControl->Root);
		modelTree->Columns["Name"]->HeaderCell->SortGlyphDirection = m_pTreeControl->IsAscending ? SortOrder::Ascending : SortOrder::Descending;
	}

	if (visCheck)
	{
		//deselect first
		modelTree->CurrentCell = nullptr;
		for (size_t i = 0; i < modelTree->Rows->Count; i++)
		{
			currNode = (TreeRepNode^)m_treeSource[i];
			isVisible = currNode->Visible && (!useSelect || currNode->Active) && currNode->ObjectType != OME_SUBPORT;
			if (isVisible && useSelect && currNode->IsParent)
				isVisible = currNode->TreeActive;
			modelTree->Rows[i]->Visible = isVisible;

			int tint = 255 - (5 * currNode->Level);
			modelTree->Rows[i]->DefaultCellStyle->BackColor = System::Drawing::Color::FromArgb(255, tint, tint, tint);

		}
	}
	//second pass, remove submodels that have nothing within them other than other submodels
	//...

	modelTree->Refresh();
}
