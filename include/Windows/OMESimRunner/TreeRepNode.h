#pragma once

#include "TreeResultsDetails.h"
#include "RepNode.h"
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace OMESimRunner
{

	ref class RNControl;

	/// <summary>RepNode used for tree-style lists.</summary>
	public ref class TreeRepNode : RepNode
	{
	public:

		TreeRepNode(OMEObject* pObj, const int & level, RNControl^ root);
		TreeRepNode(OMEObject* pObj, RNControl^ root);

		/// <summary>Flag indicating whether the node is visible.</summary>
		property bool Visible
		{
			bool get()
			{
				return m_visible;
			}

			System::Void set(bool v)
			{
				m_visible = v;
				for (size_t i = 0; i<m_subNodes.Count; i++)
					m_subNodes[i]->Visible = m_visible;
			}
		}

		/// <summary>Depth in tree where the node resides.</summary>
		property int Level
		{
			int get() { return m_level; }
			void set(int lvl) { m_level = lvl; }
		}

		/// <summary>Flag indicating whether the subnodes are visible or not.</summary>
		property bool Expanded
		{
			bool get()
			{
				return m_expanded;
			}

			System::Void set(bool exp)
			{
				m_expanded = exp;
				for (size_t i = 0; i<m_subNodes.Count; i++)
					m_subNodes[i]->Visible = m_expanded;
			}
		}

		/// <summary>True if node has one or more direct subnodes.</summary>
		property bool IsParent
		{
			bool get() { return m_isParent; }
			void set(bool p) { m_isParent = p; }
		}

		/// <summary>Flag indicating whether or not the node is active.</summary>
		property bool Active
		{
			bool get() { return m_active; }
			void set(bool a) { if (m_pObj && m_pObj->GetOMEType() != OME_MODEL) m_active = a; }
		}

		/// <summary>Flag indicating whether or not the entire tree is active.</summary>
		property bool TreeActive
		{
			bool get() { return IsTreeActive(); }
		}

		/// <summary>The total number of immediate subnodes.</summary>
		property Int32 SubNodeCount
		{
			Int32 get() { return m_subNodes.Count; }
		}


		System::Void AddNode(TreeRepNode^ node);
		bool RemoveNode(TreeRepNode^ node);
		TreeRepNode^ GetNode(int i);
		System::Void ToggleExpanded();
		virtual Int32 CompareTo(System::Object^ obj) override;
		System::Void SortSubs();

	protected:
		/// <summary>List of immediate subnodes.</summary>
		Generic::List<TreeRepNode^> m_subNodes;
		/// <summary>Flag indicating whether the subnodes are visible or not.</summary>
		bool m_expanded;
		/// <summary>Flag indicating whether the node is visible.</summary>
		bool m_visible;
		/// <summary>Flag indicating whether or not the node is active.</summary>
		bool m_active;
		/// <summary>True if node has one or more direct subnodes.</summary>
		bool m_isParent;
		/// <summary>Depth in tree where the node resides.</summary>
		int m_level;
		/// <summary>Handle to the sorting control object.</summary>
		RNControl^ m_pTreeControl;

		bool IsTreeActive();
	};


	////////////////////////////////////////
	/// <summary> Sorting control for TreeView.</summary> 
	public ref class RNControl
	{

	public:
		RNControl();
		/// <summary>Sort criteria for specific field types.</summary>
		enum class SORT_FIELD 
		{ 
			/// <summary>Leaf node type.</summary>
			NAME, 
			/// <summary>Branch node type.</summary>
			PARENT 
		};

		/// <summary>Tree node type identifier.</summary>
		property SORT_FIELD SortField
		{
			SORT_FIELD get()
			{
				return m_sortField;
			}
			void set(SORT_FIELD sf)
			{
				if (m_sortField != sf)
				{
					m_sortField = sf;
					m_updateRequired = true;
				}
			}
		}

		/// <summary>True if contents are to sort in ascending order;
		/// false if contents are to sort in descending order.</summary>
		property bool IsAscending
		{
			bool get()
			{
				return m_ascending;
			}
			System::Void set(bool asc)
			{
				if (asc != m_ascending)
					m_updateRequired = true;
				m_ascending = asc;
			}
		}

		/// <summary>The sort mode presently being used.</summary>
		property RN_SORTMODE SortMode
		{
			RN_SORTMODE get()
			{
				return m_sortMode;
			}
			void set(RN_SORTMODE sm)
			{
				if (m_sortMode != sm)
					m_updateRequired = true;
				m_sortMode = sm;
			}
		}

		/// <summary>Handle to the root tree node.</summary>
		property TreeRepNode^ Root
		{
			TreeRepNode^ get()
			{
				return m_pRoot;
			}
			System::Void set(TreeRepNode^ pNode)
			{
				m_pRoot = pNode;
			}
		}

		/// <summary>If true, the sorting criteria has been updated.</summary>
		property bool Updated
		{
			bool get()
			{
				return m_updateRequired;
			}
		}

		System::Void ToggleSortOrder();
		System::Void ClearUpdateFlag();

	protected:
		/// <summary>True if contents are to sort in ascending order;
		/// false if contents are to sort in descending order.</summary>
		bool m_ascending;
		/// <summary>The sort mode presently being used.</summary>
		RN_SORTMODE m_sortMode;
		/// <summary>Handle to the root tree node.</summary>
		TreeRepNode^ m_pRoot;
		/// <summary>If true, the sorting criteria has been updated.</summary>
		bool m_updateRequired;
		/// <summary>Tree node type identifier.</summary>
		SORT_FIELD m_sortField;
	};

};