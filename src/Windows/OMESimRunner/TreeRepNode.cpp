#include "TreeRepNode.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace OMESimRunner;

/// <summary>Detailed constructor.</summary>
/// <param name="pObj">Pointer to the represented object.</param>
/// <param name="level">The tree level that this node applies to.</param>
/// <param name="ctrl">The control object used for sorting.</param>
TreeRepNode::TreeRepNode(OMEObject* pObj, const int & level, RNControl^ ctrl)
	:RepNode(pObj), m_visible(true), m_isParent(false), m_level(level), m_expanded(true), m_active(true), m_pTreeControl(ctrl)
{

}

/// <summary>Detailed constructor.</summary>
/// <param name="pObj">Pointer to the represented object.</param>
/// <param name="ctrl">The control object used for sorting.</param>
TreeRepNode::TreeRepNode(OMEObject* pObj, RNControl^ ctrl)
	:RepNode(pObj), m_visible(true), m_isParent(false), m_level(0), m_expanded(true), m_active(true), m_pTreeControl(ctrl)
{

}

/// <summary>Add a subnode to the tree.</summary>
/// <param name="node">Handle to the node to add.</param>
System::Void TreeRepNode::AddNode(TreeRepNode^ node)
{
	m_subNodes.Add(node);
}

/// <summary>Remove a subnode from the tree.</summary>
/// <param name="node">Handle to the node to remove.</param>
/// <returns>True if the removal was successful; false otherwise.</returns>
bool TreeRepNode::RemoveNode(TreeRepNode^ node)
{
	return m_subNodes.Remove(node);
}

/// <summary>Retrieve a specific subnode.</summary>
/// <param name="i">Index of the node to retrieve.</param>
/// <returns>A handle to the subnode at i.</returns>
TreeRepNode^ TreeRepNode::GetNode(int i)
{
	return m_subNodes[i];
}

/// <summary>Toggle expansion flag.</summary>
System::Void TreeRepNode::ToggleExpanded()
{
	this->Expanded = !this->Expanded;
}

Int32 TreeRepNode::CompareTo(System::Object^ obj)
{
	//for now, just sort like this.
	Int32 result = -2;
	TreeRepNode^ repCast = (TreeRepNode^)obj;

	if (m_pTreeControl->SortMode != MIXED)
	{
		if (m_pObj->GetOMEType() == OME_MODEL && repCast->ObjectType != OME_MODEL)
			result = 1;
		else if (m_pObj->GetOMEType() != OME_MODEL && repCast->ObjectType == OME_MODEL)
			result = -1;

		if (m_pTreeControl->SortMode == MFIRST)
			result *= -1;
	}
	if (result<-1 || result>1)
	{

		switch (m_pTreeControl->SortField)
		{
		case RNControl::SORT_FIELD::NAME:
			result = m_name->CompareTo(((TreeRepNode^)obj)->m_name);
			break;
		case RNControl::SORT_FIELD::PARENT:
			result = this->ParentModelName->CompareTo(((TreeRepNode^)obj)->ParentModelName);
			break;
		}
		if (!m_pTreeControl->IsAscending)
			result *= -1;
	}
	return result;
}

/// <summary>Sort all subnodes.</summary>
System::Void TreeRepNode::SortSubs()
{
	m_subNodes.Sort();
	for (int i = 0; i < m_subNodes.Count; i++)
		m_subNodes[i]->SortSubs();
}

/// <summary>Check to see if subtree is active.</summary>
/// <returns>True if subtree is active; false otherwise.</returns>
bool TreeRepNode::IsTreeActive()
{
	if (!m_isParent)
		return m_active;

	bool ret = false;
	for (int i = 0; i < m_subNodes.Count && !ret; i++)
		ret = m_subNodes[i]->IsTreeActive();

	return ret;
}

////////////////////////////////////////
RNControl::RNControl()
	:m_ascending(true), m_sortMode(CFIRST), m_pRoot(nullptr), m_updateRequired(true), m_sortField(SORT_FIELD::NAME)
{
}

/// <summary>Toggle the direction of node sorting.</summary>
System::Void RNControl::ToggleSortOrder()
{
	m_ascending = !m_ascending;
	m_updateRequired = true;
}

/// <summary>Clear flag used for requesting updates.</summary>
System::Void RNControl::ClearUpdateFlag()
{
	m_updateRequired = false;
}