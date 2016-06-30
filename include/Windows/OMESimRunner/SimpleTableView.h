#pragma once
#include "SimWrapper.h"
#include "TblRepNode.h"
#include "ScopeSelector.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace OMESimRunner {

	/// <summary>
	/// View for showing values in a simple table configuration.
	/// </summary>
	public ref class SimpleTableView : public System::Windows::Forms::UserControl
	{
	public:
		/// <summary>Detailed constructor.</summary>
		/// <param name="sim">Handle to the active SimWrapper.</param>
		SimpleTableView(SimWrapper^ sim)
         :m_sim(sim)
		{
			InitializeComponent();
			m_tableSource = gcnew BindingSource();
			TableView->DataSource = m_tableSource;
			TableView->AutoGenerateColumns = false;

			ALS_FONT = gcnew System::Drawing::Font(this->Font, FontStyle::Italic);
		}

      void ReloadData();

	  /// <summary>DGV Column containing a field's name.</summary>
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  name;
			 /// <summary>DGV Column containing a field's instance.</summary>
	private: System::Windows::Forms::DataGridViewLinkColumn^  Instance;
			 /// <summary>DGV Column containing a field's value.</summary>
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  currVal;
			 /// <summary>A DGV that contains the selected table entries.</summary>
	private: System::Windows::Forms::DataGridView^  TableView;

	protected:
		/// <summary>Handle to associated SimWrapper.</summary>
		SimWrapper^ m_sim;
		/// <summary>Italicized Font used to identify Aliases.</summary>
		System::Drawing::Font^ ALS_FONT;
		/// <summary> Container for data used with DGV.</summary>
		BindingSource^ m_tableSource;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~SimpleTableView()
		{
			if (components)
			{
				delete components;
			}
		}

		System::Void AddLevelsToScopeSelector(TblRepNode^ rep, ScopeSelector^ scpSel);
		System::Void GetIndicesFromScopeSelector(TblRepNode^ rep, ScopeSelector^ scpSel,int index);

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
			this->TableView = (gcnew System::Windows::Forms::DataGridView());
			this->name = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Instance = (gcnew System::Windows::Forms::DataGridViewLinkColumn());
			this->currVal = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->TableView))->BeginInit();
			this->SuspendLayout();
			// 
			// TableView
			// 
			this->TableView->AllowUserToAddRows = false;
			this->TableView->AllowUserToDeleteRows = false;
			this->TableView->AllowUserToResizeRows = false;
			this->TableView->BackgroundColor = System::Drawing::SystemColors::Window;
			this->TableView->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->TableView->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->TableView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(3) {
				this->name, this->Instance,
					this->currVal
			});
			this->TableView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->TableView->Location = System::Drawing::Point(0, 0);
			this->TableView->MultiSelect = false;
			this->TableView->Name = L"TableView";
			this->TableView->RowHeadersVisible = false;
			this->TableView->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
			this->TableView->Size = System::Drawing::Size(736, 441);
			this->TableView->TabIndex = 1;
			this->TableView->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &SimpleTableView::TableView_CellContentClick);
			this->TableView->CellFormatting += gcnew System::Windows::Forms::DataGridViewCellFormattingEventHandler(this, &SimpleTableView::TableView_CellFormatting);
			this->TableView->CellPainting += gcnew System::Windows::Forms::DataGridViewCellPaintingEventHandler(this, &SimpleTableView::TableView_CellPainting);
			// 
			// name
			// 
			this->name->DataPropertyName = L"Name";
			this->name->HeaderText = L"Name";
			this->name->Name = L"name";
			this->name->Width = 300;
			// 
			// Instance
			// 
			this->Instance->DataPropertyName = L"CoordStr";
			this->Instance->HeaderText = L"Instance";
			this->Instance->Name = L"Instance";
			this->Instance->ReadOnly = true;
			// 
			// currVal
			// 
			this->currVal->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			this->currVal->DataPropertyName = L"LookupValStr";
			this->currVal->HeaderText = L"Value";
			this->currVal->Name = L"currVal";
			// 
			// SimpleTableView
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoSize = true;
			this->BackColor = System::Drawing::SystemColors::MenuHighlight;
			this->Controls->Add(this->TableView);
			this->Name = L"SimpleTableView";
			this->Size = System::Drawing::Size(736, 441);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->TableView))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

		/// <summary>Event handler for cell painting in TableView.</summary>
		/// <param name="sender">Object that invoked the event handler.</param>
		/// <param name="e">Arguments that are associated with the event.</param>
private: System::Void TableView_CellPainting(System::Object^  sender, System::Windows::Forms::DataGridViewCellPaintingEventArgs^  e) 
{
	if (e->RowIndex >= 0)
	{
		TblRepNode^ currNode = (TblRepNode^)m_tableSource[e->RowIndex];
		
		if (e->ColumnIndex == TableView->Columns["name"]->Index)
		{	
			if (currNode->ObjectType == OME_ALIAS)
				TableView[e->ColumnIndex, e->RowIndex]->Style->Font = ALS_FONT;
			else
				TableView[e->ColumnIndex, e->RowIndex]->Style->Font = this->Font;
			TableView[e->ColumnIndex, e->RowIndex]->Style->ForeColor = currNode->Color;
		}
		else if (e->ColumnIndex == TableView->Columns["currVal"]->Index)
		{
			if (!m_sim->HasRun)
			{
				e->Handled = true;
				e->PaintBackground(e->ClipBounds, true);
			}
		}
	}
}

		 /// <summary>Event handler for cell formatting in TableView.</summary>
		 /// <param name="sender">Object that invoked the event handler.</param>
		 /// <param name="e">Arguments that are associated with the event.</param>
private: System::Void TableView_CellFormatting(System::Object^  sender, System::Windows::Forms::DataGridViewCellFormattingEventArgs^  e) 
{
	if(e->ColumnIndex == TableView->Columns["Instance"]->Index && !TableView[e->ColumnIndex, e->RowIndex]->IsInEditMode)
	{

		TblRepNode^ currNode = (TblRepNode^)m_tableSource[e->RowIndex];
		if (currNode->CoordCount)
		{
			StringBuilder^ sb = gcnew StringBuilder("{");
			for (unsigned int i = 0; i < currNode->CoordCount; ++i)
				sb->AppendFormat(" {0}", currNode->Coord[i]);

			sb->Append(" }");

			e->Value = sb->ToString();
		}
		else
			e->Value = "";
		e->FormattingApplied = true;
	}
}

		 /// <summary>Event handler for cell click in TableView.</summary>
		 /// <param name="sender">Object that invoked the event handler.</param>
		 /// <param name="e">Arguments that are associated with the event.</param>
private: System::Void TableView_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) 
{
	if (e->RowIndex >= 0 && e->ColumnIndex == TableView->Columns["instance"]->Index)
	{
		DataGridViewTextBoxCell^ tb;
		TableView->ClearSelection();
		TblRepNode^ currNode = (TblRepNode^)m_tableSource[e->RowIndex];
		
		ScopeSelector^ scpSel = gcnew ScopeSelector();

		//add entries
		AddLevelsToScopeSelector(currNode, scpSel);
		scpSel->ShowDialog();
		GetIndicesFromScopeSelector(currNode, scpSel,e->RowIndex);
		TableView->Refresh();
	}
}
};
}
