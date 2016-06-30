#pragma once
#include "SimWrapper.h"
#include "TreeResultsDetails.h"
#include "TreeRepNode.h"
#include "ArrayBrowser.h"
#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace OMESimRunner {

	/// <summary>
	/// View for showing results in a collapsable tree-list summary.
	/// </summary>
	public ref class SimpleTreeView : public System::Windows::Forms::UserControl
	{
	public:
		/// <summary>Detailed constructor.</summary>
		SimpleTreeView(SimWrapper^ sim)
			:m_sim(sim), m_pTreeControl(gcnew RNControl())
		{
			InitializeComponent();
			MDL_FONT = gcnew System::Drawing::Font(this->Font, FontStyle::Bold);
			ALS_FONT = gcnew System::Drawing::Font(this->Font, FontStyle::Italic);
			m_treeSource = gcnew BindingSource();
			modelTree->DataSource = m_treeSource;
			modelTree->AutoGenerateColumns = false;

		}

		/// <summary>Refresh data from source data set.</summary>
		System::Void ReloadData()
		{
			TreeResultsDetails* pDetails = (TreeResultsDetails*)m_sim->ViewMgr->GetActiveDetails();

			m_pRoot = m_sim->RootModel;


			//populate model tree; normally we would only use selected fields.
			//if(m_sim->HasRun)
			//{
			m_treeSource->Clear();
			if (m_pRoot)
				PopulateTree(m_pRoot);

			RefreshActive();
			modelTree->Columns["Name"]->HeaderCell->SortGlyphDirection = m_pTreeControl->IsAscending ? SortOrder::Ascending : SortOrder::Descending;
			//}
		}

		TreeRepNode^ PopulateTree(Model* pMdl);
		TreeRepNode^ PopulateTree(Model* pRoot, int level, RNControl^ root);
		System::Void RefreshActive();
		System::Void UpdateVisibility(const bool & resort, const bool & visCheck);
	protected:

		/// <summary> Bold font for use with labelling models.</summary>
		System::Drawing::Font^ MDL_FONT;
		/// <summary> Italicized font for use with labelling aliases.</summary>
		System::Drawing::Font^ ALS_FONT;
		/// <summary> Handle to associated SimWrapper.</summary>
		SimWrapper^ m_sim;
		/// <summary> Binding source for tree DGV.</summary>
		BindingSource^ m_treeSource;
		/// <summary> Handle to Sorting controller.</summary>
		RNControl^ m_pTreeControl;
		/// <summary> Flag indicating whether or not to show only selected fields.</summary>
		bool m_selectedOnly;
		/// <summary> Pointer to the root Model.</summary>
		Model *m_pRoot;

		System::Void RepopulateBS(TreeRepNode^ root);
		System::Void AddBSNode(TreeRepNode^ node);
		System::Void ResortNodes(TreeRepNode^ parent);

		/// <summary>DGV Column containing buttons for expanding/collapsing model nodes.</summary>
	private: System::Windows::Forms::DataGridViewButtonColumn^  expander;
			 /// <summary>DGV Column containing field name.</summary>
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  name;
			 /// <summary>DGV Column containing model instance selector.</summary>
	private: System::Windows::Forms::DataGridViewLinkColumn^  instance;
			 /// <summary>DGV Column containing current value.</summary>
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  currVal;
			 /// <summary>DGV That holds the model tree.</summary>
	private: System::Windows::Forms::DataGridView^  modelTree;

			 /// <summary>
			 /// Clean up any resources being used.
			 /// </summary>
			 ~SimpleTreeView()
			 {
				 if (components)
				 {
					 delete components;
				 }
			 }

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle1 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle2 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			this->modelTree = (gcnew System::Windows::Forms::DataGridView());
			this->expander = (gcnew System::Windows::Forms::DataGridViewButtonColumn());
			this->name = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->instance = (gcnew System::Windows::Forms::DataGridViewLinkColumn());
			this->currVal = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->modelTree))->BeginInit();
			this->SuspendLayout();
			// 
			// modelTree
			// 
			this->modelTree->AllowUserToAddRows = false;
			this->modelTree->AllowUserToDeleteRows = false;
			this->modelTree->AllowUserToResizeRows = false;
			this->modelTree->BackgroundColor = System::Drawing::SystemColors::Window;
			this->modelTree->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->modelTree->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->modelTree->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(4) {
				this->expander, this->name,
					this->instance, this->currVal
			});
			this->modelTree->Dock = System::Windows::Forms::DockStyle::Fill;
			this->modelTree->Location = System::Drawing::Point(0, 0);
			this->modelTree->MultiSelect = false;
			this->modelTree->Name = L"modelTree";
			this->modelTree->RowHeadersVisible = false;
			this->modelTree->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
			this->modelTree->Size = System::Drawing::Size(736, 441);
			this->modelTree->TabIndex = 1;
			this->modelTree->CellClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &SimpleTreeView::modelTree_CellClick);
			this->modelTree->CellFormatting += gcnew System::Windows::Forms::DataGridViewCellFormattingEventHandler(this, &SimpleTreeView::modelTree_CellFormatting);
			this->modelTree->CellLeave += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &SimpleTreeView::modelTree_CellLeave);
			this->modelTree->CellPainting += gcnew System::Windows::Forms::DataGridViewCellPaintingEventHandler(this, &SimpleTreeView::modelTree_CellPainting);
			this->modelTree->ColumnHeaderMouseClick += gcnew System::Windows::Forms::DataGridViewCellMouseEventHandler(this, &SimpleTreeView::modelTree_ColumnHeaderMouseClick);
			// 
			// expander
			// 
			dataGridViewCellStyle1->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleCenter;
			dataGridViewCellStyle1->Format = L"N6";
			dataGridViewCellStyle1->NullValue = nullptr;
			this->expander->DefaultCellStyle = dataGridViewCellStyle1;
			this->expander->Frozen = true;
			this->expander->HeaderText = L"+/-";
			this->expander->MinimumWidth = 35;
			this->expander->Name = L"expander";
			this->expander->Resizable = System::Windows::Forms::DataGridViewTriState::False;
			this->expander->Width = 35;
			// 
			// name
			// 
			this->name->DataPropertyName = L"Name";
			this->name->HeaderText = L"Name";
			this->name->Name = L"name";
			this->name->Width = 300;
			// 
			// instance
			// 
			this->instance->DataPropertyName = L"Instance";
			this->instance->HeaderText = L"Active Instance";
			this->instance->Name = L"instance";
			this->instance->Width = 110;
			// 
			// currVal
			// 
			this->currVal->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			this->currVal->DataPropertyName = L"Value";
			dataGridViewCellStyle2->NullValue = nullptr;
			this->currVal->DefaultCellStyle = dataGridViewCellStyle2;
			this->currVal->HeaderText = L"Value";
			this->currVal->MinimumWidth = 20;
			this->currVal->Name = L"currVal";
			// 
			// SimpleTreeView
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoSize = true;
			this->Controls->Add(this->modelTree);
			this->Name = L"SimpleTreeView";
			this->Size = System::Drawing::Size(736, 441);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->modelTree))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

		/// <summary>Event handler for cell click in TreeView.</summary>
		/// <param name="sender">Object that invoked the event handler.</param>
		/// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void modelTree_CellClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
				 if (e->RowIndex >= 0 && e->ColumnIndex == modelTree->Columns["expander"]->Index)
				 {
					 modelTree->ClearSelection();
					 TreeRepNode^ currNode = (TreeRepNode^)m_treeSource[e->RowIndex];
					 currNode->ToggleExpanded();

					 UpdateVisibility(false, true);
				 }
				 else if (e->RowIndex >= 0 && e->ColumnIndex == modelTree->Columns["instance"]->Index)
				 {
					 DataGridViewTextBoxCell^ tb;
					 modelTree->ClearSelection();
					 TreeRepNode^ currNode = (TreeRepNode^)m_treeSource[e->RowIndex];
					 switch (currNode->ObjectType)
					 {
					 case OME_MODEL:
						 //just edit text in line, if possible
						 if (currNode->Expanded)
						 {
							 tb = gcnew DataGridViewTextBoxCell();
							 modelTree[e->ColumnIndex, e->RowIndex] = tb;
						 }
						 break;
					 case OME_VARARRAY:
						 //bring up array content dialog.
						 ArrayBrowser^ selectForm = gcnew ArrayBrowser(currNode);

						 selectForm->Text = currNode->Name;
						 selectForm->ShowDialog();
						 break;
					 }

					 UpdateVisibility(false, false);
				 }
	}

			 /// <summary>Event handler for cell painting in TreeView.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void modelTree_CellPainting(System::Object^  sender, System::Windows::Forms::DataGridViewCellPaintingEventArgs^  e) {

				 if (e->RowIndex >= 0)
				 {
					 TreeRepNode^ currNode = (TreeRepNode^)m_treeSource[e->RowIndex];

					 if (currNode->Visible)
					 {
						 if (e->ColumnIndex == modelTree->Columns["expander"]->Index)
						 {
							 if (currNode->IsParent)
								 modelTree[e->ColumnIndex, e->RowIndex]->Value = currNode->Expanded ? "-" : "+";
							 else
							 {
								 e->PaintBackground(e->ClipBounds, true);
								 e->Handled = true;
							 }
							 //if(!currNode->IsParent)
							 //	modelTree[e->ColumnIndex,e->RowIndex]= gcnew DataGridViewTextBoxCell();

						 }
						 else if (e->ColumnIndex == modelTree->Columns["name"]->Index)
						 {
							 //the name stuff may need to be moved to an initialization spot
							 System::Windows::Forms::Padding pad;
							 pad.Left = currNode->Level * 10;
							 modelTree[e->ColumnIndex, e->RowIndex]->Style->Padding = pad;
							 if (currNode->IsParent)
								 modelTree[e->ColumnIndex, e->RowIndex]->Style->Font = MDL_FONT;
							 else if (currNode->ObjectType == OME_ALIAS)
								 modelTree[e->ColumnIndex, e->RowIndex]->Style->Font = ALS_FONT;
							 else
								 modelTree[e->ColumnIndex, e->RowIndex]->Style->Font = this->Font;

							 modelTree[e->ColumnIndex, e->RowIndex]->Style->ForeColor = currNode->Color;
						 }
						 else if (e->ColumnIndex == modelTree->Columns["currVal"]->Index)
						 {
							 if (currNode->IsParent || !m_sim->HasRun)
							 {
								 e->Handled = true;
								 e->PaintBackground(e->ClipBounds, true);
							 }
						 }
						 else if (e->ColumnIndex == modelTree->Columns["instance"]->Index)
						 {
							 if ((!(currNode->IsParent && currNode->Expanded) && currNode->ObjectType != OME_VARARRAY) || !m_sim->HasRun)
							 {
								 e->Handled = true;
								 e->PaintBackground(e->ClipBounds, true);
							 }
						 }
					 }
				 }
	}
			 /// <summary>Event handler for cell formatting in TreeView.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>

	private: System::Void modelTree_CellFormatting(System::Object^  sender, System::Windows::Forms::DataGridViewCellFormattingEventArgs^  e) {
				 if (e->ColumnIndex == modelTree->Columns["instance"]->Index && !modelTree[e->ColumnIndex, e->RowIndex]->IsInEditMode)
				 {
					 e->FormattingApplied = true;
					 switch (((TreeRepNode^)m_treeSource[e->RowIndex])->ObjectType)
					 {
					 case OME_MODEL:
						 if (!((TreeRepNode^)m_treeSource[e->RowIndex])->Expanded)
						 {
							 e->FormattingApplied = false;
							 break;
						 }
					 case OME_VARARRAY:
						 e->Value = e->Value->ToString() + " of " + ((TreeRepNode^)m_treeSource[e->RowIndex])->Count.ToString();
						 break;
					 default:
						 e->FormattingApplied = false;
					 }
				 }
	}

			 /// <summary>Event handler for mouse pointer leaving a cell in TreeView.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void modelTree_CellLeave(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
				 if (e->ColumnIndex == modelTree->Columns["instance"]->Index && ((TreeRepNode^)m_treeSource[e->RowIndex])->ObjectType == OME_MODEL && ((TreeRepNode^)m_treeSource[e->RowIndex])->Expanded)
				 {
					 DataGridViewLinkCell^ lc = gcnew DataGridViewLinkCell();
					 modelTree[e->ColumnIndex, e->RowIndex] = lc;
					 //UpdateVisibility(false,true);
					 modelTree->EndEdit();
					 modelTree->Refresh();
					 //this->Refresh();
				 }
	}

			 /// <summary>Event handler for header cell click in TreeView.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void modelTree_ColumnHeaderMouseClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellMouseEventArgs^  e) {
				 if (modelTree->Columns[e->ColumnIndex]->DataPropertyName == "Name")
				 {
					 m_pTreeControl->ToggleSortOrder();
					 ((TreeResultsDetails*)m_sim->ViewMgr->GetActiveDetails())->SetSortAsc(m_pTreeControl->IsAscending);
					 UpdateVisibility(true, false);
				 }
	}
	};
}
