#pragma once
#include "SimWrapper.h"
#include "MultiValResultsDetails.h"
#include "TreeRepNode.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace OMESimRunner {

	/// <summary>
	/// View used to browse multiple values for a single model component
	/// across instances.
	/// </summary>
	public ref class MultiValBrowserView : public System::Windows::Forms::UserControl
	{
	public:
		/// <summary>Detailed constructor.</summary>
		/// <param name="sim">Handle to the active SimWrapper.</param>
		MultiValBrowserView(SimWrapper^ sim)
			:m_sim(sim), m_pListControl(gcnew RNControl())
		{
			InitializeComponent();
			MDL_FONT = gcnew System::Drawing::Font(this->Font, FontStyle::Bold);
			m_treeSource = gcnew BindingSource();
			m_instSource = gcnew BindingSource();

			TreeGridView->DataSource = m_treeSource;
			TreeGridView->AutoGenerateColumns = false;

			InstValView->DataSource = m_instSource;
			InstValView->Visible = false;
			
			m_isPopulating = false;
			m_lastTime = 0.0;

			ALS_FONT = gcnew System::Drawing::Font(this->Font, FontStyle::Italic);
		}

		/// <summary>Reload active view data.</summary>
		System::Void ReloadData()
		{

			TreeResultsDetails* pDetails = (TreeResultsDetails*)m_sim->ViewMgr->GetActiveDetails();
			msclr::interop::marshal_context context;

			m_pRoot = m_sim->RootModel;


			//populate model tree; normally we would only use selected fields.
			//if(m_sim->HasRun)
			//{
			m_treeSource->Clear();
			if (m_pRoot)
				PopulateTree(m_pRoot);

			RefreshActive();
			//}
		}

		TreeRepNode^ PopulateTree(Model* pMdl);
		TreeRepNode^ PopulateTree(Model* pRoot, int level, RNControl^ root);
		System::Void RefreshActive();
		System::Void UpdateVisibility();

		/// <summary>True if tree object is in the process of repopulating itself; false otherwise.</summary>
		property System::Boolean IsPopulating
		{
			System::Boolean get()
			{
				return m_isPopulating;
			}
		}

	protected:
		//	System::Void WalkVisibility(TreeRepNode^ currNode, const bool & useSelect, const bool & noSMP);
		/// <summary>Bold font for model entries.</summary>
		System::Drawing::Font^ MDL_FONT;
		/// <summary>Italic font for alias entries.</summary>
		System::Drawing::Font^ ALS_FONT;
		/// <summary>Handle for active SimWrapper.</summary>
		SimWrapper^ m_sim;
		/// <summary>Source for list of model components.</summary>
		BindingSource^ m_treeSource;
		/// <summary>Source of list for instance values.</summary>
		BindingSource^ m_instSource;
		/// <summary>Handle to sorting control object.</summary>
		RNControl^ m_pListControl;
		/// <summary>Flag for indicating if tree is in the middle of the population process.</summary>
		bool m_isPopulating;
		/// <summary>Pointer to the root model object.</summary>
		Model* m_pRoot;
		/// <summary>Last selected time period.</summary>
		OME_SCALAR m_lastTime;

		/// <summary>Label containing information about the model component.</summary>
	private: System::Windows::Forms::Label^  InfoLabel;
			 /// <summary>DGV containing all the values for the selected model component.</summary>
	private: System::Windows::Forms::DataGridView^  InstValView;
			 /// <summary>Layout for the right side of the split pane.</summary>
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
			 /// <summary>Layout for the left side of the split pane.</summary>
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel2;
			 /// <summary>Label for filter field.</summary>
	private: System::Windows::Forms::Label^  label1;
			 /// <summary>Text box for filter text to apply to the list of model components.</summary>
	private: System::Windows::Forms::TextBox^  browserFilter;
			 /// <summary>DGV column for displaying value coordinates.</summary>
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Coords;
			 /// <summary>DGV column for displaying component name.</summary>
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  name;
			 /// <summary>DGV column for displaying component parent model.</summary>
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  parentModel;
			 /// <summary>DGV column for displaying component values.</summary>
    private: System::Windows::Forms::DataGridViewTextBoxColumn^  Values;
			 /// <summary>Root split view.</summary>
	private: System::Windows::Forms::SplitContainer^  topSplit;
			 /// <summary>DGV containing a tree list of model components.</summary>
	private: System::Windows::Forms::DataGridView^  TreeGridView;

	protected:

		System::Void RepopulateBS(TreeRepNode^ root);
		System::Void AddBSNode(TreeRepNode^ node);
		System::Void ResortNodes(TreeRepNode^ parent);

		System::Void BuildList(TreeRepNode^ rp);
		String^ StringForDims(ListDims & dims);
		System::Void UpdateFilter(String^ filterText);
		System::Void ClearFilter();

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MultiValBrowserView()
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
			this->topSplit = (gcnew System::Windows::Forms::SplitContainer());
			this->tableLayoutPanel2 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->TreeGridView = (gcnew System::Windows::Forms::DataGridView());
			this->name = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->parentModel = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->browserFilter = (gcnew System::Windows::Forms::TextBox());
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->InstValView = (gcnew System::Windows::Forms::DataGridView());
			this->Coords = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Values = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->InfoLabel = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->topSplit))->BeginInit();
			this->topSplit->Panel1->SuspendLayout();
			this->topSplit->Panel2->SuspendLayout();
			this->topSplit->SuspendLayout();
			this->tableLayoutPanel2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->TreeGridView))->BeginInit();
			this->tableLayoutPanel1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->InstValView))->BeginInit();
			this->SuspendLayout();
			// 
			// topSplit
			// 
			this->topSplit->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->topSplit->Dock = System::Windows::Forms::DockStyle::Fill;
			this->topSplit->Location = System::Drawing::Point(0, 0);
			this->topSplit->Name = L"topSplit";
			// 
			// topSplit.Panel1
			// 
			this->topSplit->Panel1->Controls->Add(this->tableLayoutPanel2);
			// 
			// topSplit.Panel2
			// 
			this->topSplit->Panel2->Controls->Add(this->tableLayoutPanel1);
			this->topSplit->Size = System::Drawing::Size(905, 445);
			this->topSplit->SplitterDistance = 227;
			this->topSplit->TabIndex = 0;
			// 
			// tableLayoutPanel2
			// 
			this->tableLayoutPanel2->ColumnCount = 2;
			this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				42)));
			this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				100)));
			this->tableLayoutPanel2->Controls->Add(this->TreeGridView, 0, 1);
			this->tableLayoutPanel2->Controls->Add(this->label1, 0, 0);
			this->tableLayoutPanel2->Controls->Add(this->browserFilter, 1, 0);
			this->tableLayoutPanel2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel2->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel2->Name = L"tableLayoutPanel2";
			this->tableLayoutPanel2->RowCount = 2;
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 25)));
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel2->Size = System::Drawing::Size(225, 443);
			this->tableLayoutPanel2->TabIndex = 1;
			// 
			// TreeGridView
			// 
			this->TreeGridView->AllowUserToAddRows = false;
			this->TreeGridView->AllowUserToDeleteRows = false;
			this->TreeGridView->BackgroundColor = System::Drawing::SystemColors::Window;
			this->TreeGridView->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->TreeGridView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(2) { this->name, this->parentModel });
			this->tableLayoutPanel2->SetColumnSpan(this->TreeGridView, 2);
			this->TreeGridView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->TreeGridView->Location = System::Drawing::Point(3, 28);
			this->TreeGridView->Name = L"TreeGridView";
			this->TreeGridView->RowHeadersVisible = false;
			this->TreeGridView->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
			this->TreeGridView->ShowRowErrors = false;
			this->TreeGridView->Size = System::Drawing::Size(219, 412);
			this->TreeGridView->TabIndex = 0;
			this->TreeGridView->DataSourceChanged += gcnew System::EventHandler(this, &MultiValBrowserView::TreeGridView_DataSourceChanged);
			this->TreeGridView->CellPainting += gcnew System::Windows::Forms::DataGridViewCellPaintingEventHandler(this, &MultiValBrowserView::TreeGridView_CellPainting);
			this->TreeGridView->ColumnHeaderMouseClick += gcnew System::Windows::Forms::DataGridViewCellMouseEventHandler(this, &MultiValBrowserView::TreeGridView_ColumnHeaderMouseClick);
			this->TreeGridView->DataBindingComplete += gcnew System::Windows::Forms::DataGridViewBindingCompleteEventHandler(this, &MultiValBrowserView::TreeGridView_DataBindingComplete);
			this->TreeGridView->SelectionChanged += gcnew System::EventHandler(this, &MultiValBrowserView::TreeGridView_SelectionChanged);
			// 
			// name
			// 
			this->name->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			this->name->DataPropertyName = L"Name";
			this->name->HeaderText = L"Name";
			this->name->Name = L"name";
			this->name->ReadOnly = true;
			// 
			// parentModel
			// 
			this->parentModel->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			this->parentModel->DataPropertyName = L"ParentModelName";
			this->parentModel->HeaderText = L"Parent Model";
			this->parentModel->Name = L"parentModel";
			this->parentModel->ReadOnly = true;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->label1->Location = System::Drawing::Point(3, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(36, 25);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Filter:";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// browserFilter
			// 
			this->browserFilter->Dock = System::Windows::Forms::DockStyle::Fill;
			this->browserFilter->Location = System::Drawing::Point(45, 3);
			this->browserFilter->Name = L"browserFilter";
			this->browserFilter->Size = System::Drawing::Size(177, 20);
			this->browserFilter->TabIndex = 2;
			this->browserFilter->TextChanged += gcnew System::EventHandler(this, &MultiValBrowserView::browserFilter_TextChanged);
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->ColumnCount = 1;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				100)));
			this->tableLayoutPanel1->Controls->Add(this->InstValView, 0, 1);
			this->tableLayoutPanel1->Controls->Add(this->InfoLabel, 0, 0);
			this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel1->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel1->Margin = System::Windows::Forms::Padding(0);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 2;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 60)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(672, 443);
			this->tableLayoutPanel1->TabIndex = 0;
			// 
			// InstValView
			// 
			this->InstValView->AllowUserToAddRows = false;
			this->InstValView->AllowUserToDeleteRows = false;
			this->InstValView->BackgroundColor = System::Drawing::SystemColors::Window;
			this->InstValView->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->InstValView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(2) { this->Coords, this->Values });
			this->InstValView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->InstValView->Location = System::Drawing::Point(3, 63);
			this->InstValView->Name = L"InstValView";
			this->InstValView->ReadOnly = true;
			this->InstValView->RowHeadersVisible = false;
			this->InstValView->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
			this->InstValView->Size = System::Drawing::Size(666, 377);
			this->InstValView->TabIndex = 1;
			// 
			// Coords
			// 
			this->Coords->DataPropertyName = L"Coords";
			this->Coords->HeaderText = L"Coords";
			this->Coords->Name = L"Coords";
			this->Coords->ReadOnly = true;
			// 
			// Values
			// 
			this->Values->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			this->Values->DataPropertyName = L"Value";
			this->Values->HeaderText = L"Values";
			this->Values->Name = L"Values";
			this->Values->ReadOnly = true;
			// 
			// InfoLabel
			// 
			this->InfoLabel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->InfoLabel->Location = System::Drawing::Point(3, 0);
			this->InfoLabel->Name = L"InfoLabel";
			this->InfoLabel->Size = System::Drawing::Size(666, 60);
			this->InfoLabel->TabIndex = 0;
			this->InfoLabel->Text = L"Select A field to view";
			// 
			// MultiValBrowserView
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoSize = true;
			this->Controls->Add(this->topSplit);
			this->Name = L"MultiValBrowserView";
			this->Size = System::Drawing::Size(905, 445);
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MultiValBrowserView::MultiValBrowserView_Paint);
			this->topSplit->Panel1->ResumeLayout(false);
			this->topSplit->Panel2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->topSplit))->EndInit();
			this->topSplit->ResumeLayout(false);
			this->tableLayoutPanel2->ResumeLayout(false);
			this->tableLayoutPanel2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->TreeGridView))->EndInit();
			this->tableLayoutPanel1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->InstValView))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

		/// <summary>Event handler for cell painting in Model component DGV.</summary>
		/// <param name="sender">Object that invoked the event handler.</param>
		/// <param name="e">Arguments that are associated with the event.</param>
private: System::Void TreeGridView_CellPainting(System::Object^  sender, System::Windows::Forms::DataGridViewCellPaintingEventArgs^  e) {
	if (e->RowIndex != -1 && e->ColumnIndex != -1)
	{
		TreeRepNode^ currNode = (TreeRepNode^)m_treeSource[e->RowIndex];
		TreeGridView[e->ColumnIndex, e->RowIndex]->Style->ForeColor = currNode->Color;

		if (e->ColumnIndex == TreeGridView->Columns["name"]->Index)
		{
		if (currNode->ObjectType == OME_ALIAS)
			TreeGridView[e->ColumnIndex, e->RowIndex]->Style->Font = ALS_FONT;
		else
			TreeGridView[e->ColumnIndex, e->RowIndex]->Style->Font = this->Font;
		}
	}
}

		 /// <summary>Event handler for selection changed in Model component DGV.</summary>
		 /// <param name="sender">Object that invoked the event handler.</param>
		 /// <param name="e">Arguments that are associated with the event.</param>
private: System::Void TreeGridView_SelectionChanged(System::Object^  sender, System::EventArgs^  e) {
	
	int selectNum = TreeGridView->SelectedRows->Count;
	m_lastTime = m_sim->SelectedTime;
	if (!m_isPopulating && selectNum > 0)
		{
			BuildList((TreeRepNode^)TreeGridView->SelectedRows[0]->DataBoundItem);
			InstValView->Visible = true;
			InstValView->Refresh();
		}
		else
		{
			InfoLabel->Text = "Select A field to view";
			InstValView->Visible = false;
			m_instSource->Clear();

		}
	
}

		 /// <summary>Event handler for change in data source for Model component DGV.</summary>
		 /// <param name="sender">Object that invoked the event handler.</param>
		 /// <param name="e">Arguments that are associated with the event.</param>
private: System::Void TreeGridView_DataSourceChanged(System::Object^  sender, System::EventArgs^  e) {
	TreeGridView->ClearSelection();
}

		 /// <summary>Event handler for view painting in Component value DGV.</summary>
		 /// <param name="sender">Object that invoked the event handler.</param>
		 /// <param name="e">Arguments that are associated with the event.</param>
private: System::Void MultiValBrowserView_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
	if (TreeGridView->CurrentCell != nullptr && m_lastTime != m_sim->SelectedTime)
	{
		BuildList((TreeRepNode^)TreeGridView->SelectedRows[0]->DataBoundItem);

	}

}
		 /// <summary>Event handler for column header click in Model component DGV.</summary>
		 /// <param name="sender">Object that invoked the event handler.</param>
		 /// <param name="e">Arguments that are associated with the event.</param>
private: System::Void TreeGridView_ColumnHeaderMouseClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellMouseEventArgs^  e) {
	if (TreeGridView->Columns[e->ColumnIndex]->DataPropertyName == "Name")
	{
		if (m_pListControl->SortField == RNControl::SORT_FIELD::NAME)
			m_pListControl->ToggleSortOrder();
		else
			m_pListControl->SortField = RNControl::SORT_FIELD::NAME;
	}
	else if (TreeGridView->Columns[e->ColumnIndex]->DataPropertyName == "ParentModelName")
	{
		if (m_pListControl->SortField == RNControl::SORT_FIELD::PARENT)
			m_pListControl->ToggleSortOrder();
		else
			m_pListControl->SortField = RNControl::SORT_FIELD::PARENT;
	}
	((TreeResultsDetails*)m_sim->ViewMgr->GetActiveDetails())->SetSortAsc(m_pListControl->IsAscending);
	//todo add support for storing column sorting
	UpdateVisibility();
}
		 /// <summary>Event handler for text changing in filter text box.</summary>
		 /// <param name="sender">Object that invoked the event handler.</param>
		 /// <param name="e">Arguments that are associated with the event.</param>
private: System::Void browserFilter_TextChanged(System::Object^  sender, System::EventArgs^  e) {
	UpdateFilter(((TextBox^)sender)->Text);
}

		 /// <summary>Event handler for the completion of data binding in Model component DGV.</summary>
		 /// <param name="sender">Object that invoked the event handler.</param>
		 /// <param name="e">Arguments that are associated with the event.</param>
private: System::Void TreeGridView_DataBindingComplete(System::Object^  sender, System::Windows::Forms::DataGridViewBindingCompleteEventArgs^  e) {
	((DataGridView^)sender)->ClearSelection();
}
};
}
