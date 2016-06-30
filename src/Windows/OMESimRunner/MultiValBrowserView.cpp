#include "stdafx.h"
#include "MultiValBrowserView.h"
#include "ArrNode.h"
#include "UpdateContainers/StaticUpArray.h"

using namespace OMESimRunner;

/// <summary>Populate tree of nodes used to create a collapsable tree list.</summary>
/// <param name="pMdl"> Pointer to the parent model.</param>
/// <returns>Handle to a new TreeRepNode representing pMdl.</returns>
TreeRepNode^ MultiValBrowserView::PopulateTree(Model* pMdl)
{
	TreeRepNode^ ret = PopulateTree(pMdl, 0, m_pListControl);
	m_pListControl->Root = ret;
	RepopulateBS(ret);
	return ret;
}

/// <summary>Populate tree of nodes used to create a collapsable tree list.</summary>
/// <param name="pRoot"> Pointer to the root model.</param>
/// <param name="level"> Number indicating the tree depth for this node.</param>
/// <param name="ctrl">Handle to object that controls sorting.</param>
/// <returns>Handle to a new TreeRepNode representing pMdl.</returns>
TreeRepNode^ MultiValBrowserView::PopulateTree(Model* pRoot, int level, RNControl^ ctrl)
{
	TreeRepNode^ parent;
	parent = gcnew TreeRepNode((Evaluable*)pRoot, level, ctrl);

	parent->IsParent = true;
	//m_treeSource->Add(root);
	EvalArray subs;
	//std::sort(subs.begin(), subs.end(), [](Evaluable* lhs, Evaluable* rhs){return lhs->GetName() < rhs->GetName(); });
	pRoot->GetAllEvals(subs, DT_FULL | DT_NO_AUTOGEN | DT_ALIAS);

	level++;
	size_t i;
	for (i = 0; i<subs.size(); i++)
	{
		TreeRepNode^ leaf = gcnew TreeRepNode(subs[i], level, ctrl);
		parent->AddNode(leaf);
		//	m_treeSource->Add(leaf);
	}

	//TreeRepNode^ sub;
	//for (i = 0; i < pRoot->GetSubmodelCount(); i++)
	//	parent->AddNode(PopulateTree(pRoot->GetSubmodel(i), level, ctrl));

	parent->SortSubs();
	return parent;
}

/// <summary>Recursively resort nodes.</summary>
/// <param name="parent"> Handle to parent node.</param>
System::Void MultiValBrowserView::ResortNodes(TreeRepNode^ parent)
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
System::Void MultiValBrowserView::RepopulateBS(TreeRepNode^ root)
{
	m_isPopulating = true;
	m_treeSource->Clear();
	for (int i = 0; i < root->SubNodeCount; ++i)
		AddBSNode(root->GetNode(i));
	m_isPopulating = false;
}

/// <summary>Add node to tree source object (which is a flat list).</summary>
/// <param name="node"> Handle to node to add.</param>
System::Void MultiValBrowserView::AddBSNode(TreeRepNode^ node)
{

	m_treeSource->Add(node);
	for (int i = 0; i < node->SubNodeCount; i++)
		AddBSNode(node->GetNode(i));
}

/// <summary>Refresh active/visible nodes.</summary>
System::Void MultiValBrowserView::RefreshActive()
{
	TreeResultsDetails* pDetails = (TreeResultsDetails*)m_sim->ViewMgr->GetActiveDetails();
	EvalRepArray reps = pDetails->GetPartialReps();
	msclr::interop::marshal_context context;
	STLString id;
	TreeRepNode^ currNode;
	for (size_t i = 0; i<m_treeSource->Count; i++)
	{
		currNode = (TreeRepNode^)m_treeSource[i];

		if (currNode->ObjectType != OME_MODEL)
		{
			currNode->Active = false;
			id = currNode->Object->GetID();
			for (auto itr = reps.begin(); itr != reps.end() && !currNode->Active; ++itr)
			{
				if (id == (*itr).id)
				{
					currNode->Active = true;
					currNode->Color = Color::FromArgb(255, (*itr).color.r * 255, (*itr).color.g * 255, (*itr).color.b * 255);
				}
			}
		}
	}

	UpdateVisibility();
}

/// <summary>Update visibility of collapsed/expanded nodes.</summary>
System::Void MultiValBrowserView::UpdateVisibility()
{
	MultiValResultsDetails* pCurrDetails = (MultiValResultsDetails*)m_sim->ViewMgr->GetActiveDetails();
	
	bool useSelect = pCurrDetails->GetUseSelected();
	bool isVisible;
	TreeRepNode^ currNode;
	//first pass, find absolute visibility
	if (m_pListControl->Updated)
	{
		ResortNodes(m_pListControl->Root);
		RepopulateBS(m_pListControl->Root);
	}

	TreeGridView->CurrentCell = nullptr;
	for (size_t i = 0; i< TreeGridView->Rows->Count; i++)
	{
		
		//isVisible = currNode->Visible;// && currNode->Active;
		//if (isVisible && currNode->IsParent)
		//	isVisible = currNode->TreeActive;
		//TreeGridView->CurrentCell = nullptr;

		//TODO: add filter support here
		if (useSelect)
		{
			currNode = (TreeRepNode^)m_treeSource[i];
			TreeGridView->Rows[i]->Visible = currNode->Active;
		}
		else
			TreeGridView->Rows[i]->Visible = true; //isVisible;
	}
	//second pass, remove submodels that have nothing within them other than other submodels
	//...

	TreeGridView->Refresh();
}

/// <summary>Update text filter for source list.</summary>
/// <param name="filterText">The text to filter against.</param>
System::Void MultiValBrowserView::UpdateFilter(String^ filterText)
{
	bool isVisible;
	if (filterText->Length)
	{
		int nameInd = 0;//TreeGridView->Columns["Name"]->Index;
		int mdlInd = 1;// TreeGridView->Columns["Parent Model"]->Index;

		String^ adjFilt = filterText->ToLower();
		String^ chkStr;
		for (int i = 0; i < TreeGridView->Rows->Count; ++i)
		{
			chkStr = (String^)TreeGridView->Rows[i]->Cells[nameInd]->Value;
			isVisible = chkStr->ToLower()->Contains(adjFilt);
			if (!isVisible)
			{
				chkStr = (String^)TreeGridView->Rows[i]->Cells[mdlInd]->Value;
				isVisible = chkStr->ToLower()->Contains(adjFilt);
			}
			if (!isVisible && TreeGridView->SelectedRows->Count && TreeGridView->SelectedRows[0]->Index == i)
			{
				TreeGridView->ClearSelection();
				TreeGridView->CurrentCell = nullptr;
			}
			TreeGridView->Rows[i]->Visible = isVisible;
			
		}
		TreeGridView->Refresh();
	}
	else
		ClearFilter();
}

/// <summary>Clear filtered text.</summary>
System::Void MultiValBrowserView::ClearFilter()
{
	for (int i = 0; i < TreeGridView->Rows->Count; ++i)
		TreeGridView->Rows[i]->Visible = true;
	TreeGridView->Refresh();
}

/// <summary>Build a list of values for a specific model component.</summary>
/// <param name="rp">Handle to the TreeRepNode that represents the component in question.</param>
System::Void MultiValBrowserView::BuildList(TreeRepNode^ rp)
{
	if (rp != nullptr)
	{
		m_instSource->Clear();
		size_t lvl = 0;

		oSTLSStream coordBuff;
		coordBuff << "{   ";

		Model* pMdl;
		bool noCoord;
		if (pMdl=rp->Object->GetParentModel())
		{
			while (pMdl->GetParentModel() != NULL)
			{
				noCoord = pMdl->GetActiveInstanceCount() <= 1 && !pMdl->IsDynamic() && !pMdl->IsConditional();
				if (noCoord)
					coordBuff << "(";
				coordBuff << pMdl->GetName();
				if (noCoord)
					coordBuff << ")";
				coordBuff<< "   ";

				pMdl = pMdl->GetParentModel();
			}
		}

		coordBuff << "}" << endl << "{";

		OMEObject* pObj = rp->Object;
		if (pObj->IsOMEType(OME_ALIAS))
			pObj = ((EvalAlias*)pObj)->GetSrc();

		switch (pObj->GetOMEType())
		{
		case OME_MODEL:
			break;
		
		case OME_SUBPORT:
			pMdl = ((SubmodelPort*)pObj)->GetOuterModel();
			if (pMdl)
			{
				while (pMdl->GetParentModel() != NULL)
				{
					coordBuff << pMdl->GetActiveInstanceCount() << "   ";
					pMdl = pMdl->GetParentModel();
					++lvl;
				}
			}
			break;
		case OME_VARARRAY:
			for (unsigned short i = 0; i < ((VarArray*)pObj)->GetDimCount(); ++i)
			{
				coordBuff << ((VarArray*)pObj)->GetDim(i) << " ";
				++lvl;
			}
			//drop through
		default:
			pMdl = pObj->GetParentModel();
			
			while (pMdl != NULL)
			{
				if (pMdl->GetActiveInstanceCount()>1 || pMdl->IsDynamic() || pMdl->IsConditional())
				{
					coordBuff << pMdl->GetActiveInstanceCount() << "   ";
					
					++lvl;
				}
				pMdl = pMdl->GetParentModel();
			}
			if (lvl == 0 && pObj->GetParentModel()->GetNumInstances())
				lvl = 1;
			break;
		}

		
		coordBuff << '}';

		msclr::interop::marshal_context context;
		InfoLabel->Text = context.marshal_as<String^>(rp->Object->GetName() + STLString("\n") +
			STLString(rp->Object->LabelForType()) + STLString("\n") + coordBuff.str());


		//construct and assign list values
		if (pObj->IsOMEType(OME_EVAL))
		{
			if (!pObj->IsOMEType(OME_VARARRAY) && lvl == 1 && ((Evaluable*)pObj)->GetParentModel()->GetActiveInstanceCount() == 1)
				m_instSource->Add(gcnew ArrNode(((Evaluable*)pObj)->GetValue(), "{ 1 }"));
			else
			{
				StaticUpArray upList = StaticUpArray::NewSUAFromStack((Evaluable*)pObj, lvl);
				PersistListDims currCoord(upList.GetDimCount());

				for (size_t i = 0; i < upList.GetSize(); ++i)
				{
					upList.DeriveDims(i, currCoord);
					m_instSource->Add(gcnew ArrNode(upList[i], StringForDims(currCoord)));
				}
			}
		}
	}
}

/// <summary>Retrieve a string for a list of dimensions.</summary>
/// <param name="dims">The list of dimensions to process.</param>
/// <returns>A string representation of dimensions.</returns>
String^ MultiValBrowserView::StringForDims(ListDims & dims)
{
	StringBuilder^ sb = gcnew StringBuilder();
	sb->Append("{ ");

	for (unsigned int i = 0; i < dims.GetSize(); ++i)
	{
		sb->Append(dims[i] + 1);
		sb->Append(" ");
	}
	sb->Append("}");

	return sb->ToString();
}