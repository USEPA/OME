#pragma once
#include "RepNode.h"
#include "ArrNode.h"
#include "omeobjects/VarArray.h"

namespace OMESimRunner {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;


	/// <summary>
	/// View for exploring the contents of an array.
	/// </summary>
	public ref class ArrayBrowser : public System::Windows::Forms::Form
	{
	public:
		ArrayBrowser(void)
		{
			InitializeComponent();

		}

		///Constructor that accepts rep node. 
		/// <param name="node"> The RepNode to use for source data.</param>
		ArrayBrowser(RepNode^ node)
		{
			InitializeComponent();

			m_node = node;
			m_va = (VarArray*)m_node->Object;
			m_selected = m_node->Instance - 1;

			//create array of values for viewing
			m_arraySource = gcnew BindingSource();
			ArrNode^ ar;
			IndVec inds(m_va->GetDimCount());
			for (size_t i = 0; i < m_va->GetSize(); ++i)
			{
				ar = gcnew ArrNode((*m_va)[i]);
				m_va->GetDimsForRawIndex(i, inds);
				ar->Coords = GetStrForInds(inds);
				m_arraySource->Add(ar);

			}
			arrayView->DataSource = m_arraySource;

			//update description.
			System::String^ details = String::Concat("Name: ", m_node->Name, "\nDimensions: {");
			for (size_t i = 0; i < m_va->GetDimCount(); ++i)
				details = String::Concat(details, " ", m_va->GetDim(i).ToString());
			details = String::Concat(details, " }");
			arrayInfo->Text = details;

		}
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ArrayBrowser()
		{
			if (components)
			{
				delete components;
			}
		}

		///The column containing the entry number.
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  rowNum;
			 ///The column containing the checkbox used to determine which value is displayed in the parent view.
	private: System::Windows::Forms::DataGridViewCheckBoxColumn^  Selected;
			 ///The actual values for the array.
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  values;

	protected:
		///Handle to Source for Data grid view.
		BindingSource^ m_arraySource;
		///Handle to source node.
		RepNode^ m_node;
		///Pointer to referenced VarArray object.
		VarArray* m_va;
		///Index of selected value.
		int m_selected;

		/// <summary> Make sure all but one checkbox is unchecked. </summary>
		System::Void UpdateChecks()
		{
			int col = arrayView->Columns["Selected"]->Index;
			for (int i = 0; i < arrayView->RowCount; ++i)
			{
				if (i == m_selected)
					((DataGridViewCheckBoxCell^)arrayView->Rows[i]->Cells[col])->Value = true;
				else
					((DataGridViewCheckBoxCell^)arrayView->Rows[i]->Cells[col])->Value = false;
				//((DataGridViewCheckBoxCell^)arrayView->Rows[i]->Cells[col])->Value = i == m_selected ? true : false;
			}
		}

		/// <summary> Create a string representation of a list of indices. </summary>
		/// <param name="inds"> The collection of indices to represent. </param>
		/// <returns> A string representation of inds</returns>
		String^ GetStrForInds(IndVec & inds)
		{
			StringBuilder^ sb = gcnew StringBuilder();
			sb->Append("{ ");

			for (unsigned int i = 0; i < inds.size(); ++i)
			{
				sb->Append(inds[i] + 1);
				sb->Append(" ");
			}
			sb->Append("}");

			return sb->ToString();
		}

		/// <summary>Layout used for window widgets.</summary>
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
			 /// <summary>Label displaying information about the VarArray object.</summary>
	private: System::Windows::Forms::Label^  arrayInfo;
			 /// <summary>DataGridView containing the arrray entries.</summary>
	private: System::Windows::Forms::DataGridView^  arrayView;

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
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->arrayInfo = (gcnew System::Windows::Forms::Label());
			this->arrayView = (gcnew System::Windows::Forms::DataGridView());
			this->rowNum = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Selected = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->values = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->tableLayoutPanel1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->arrayView))->BeginInit();
			this->SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->ColumnCount = 1;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				100)));
			this->tableLayoutPanel1->Controls->Add(this->arrayInfo, 0, 0);
			this->tableLayoutPanel1->Controls->Add(this->arrayView, 0, 1);
			this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel1->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 2;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(369, 464);
			this->tableLayoutPanel1->TabIndex = 1;
			// 
			// arrayInfo
			// 
			this->arrayInfo->AutoSize = true;
			this->arrayInfo->Dock = System::Windows::Forms::DockStyle::Fill;
			this->arrayInfo->Location = System::Drawing::Point(3, 0);
			this->arrayInfo->Name = L"arrayInfo";
			this->arrayInfo->Size = System::Drawing::Size(363, 40);
			this->arrayInfo->TabIndex = 0;
			this->arrayInfo->Text = L"label1";
			// 
			// arrayView
			// 
			this->arrayView->AllowUserToAddRows = false;
			this->arrayView->AllowUserToDeleteRows = false;
			this->arrayView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->arrayView->BackgroundColor = System::Drawing::SystemColors::Window;
			this->arrayView->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->arrayView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(3) {
				this->rowNum, this->Selected,
					this->values
			});
			this->arrayView->Location = System::Drawing::Point(3, 43);
			this->arrayView->MultiSelect = false;
			this->arrayView->Name = L"arrayView";
			this->arrayView->RowHeadersVisible = false;
			this->arrayView->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
			this->arrayView->ShowEditingIcon = false;
			this->arrayView->ShowRowErrors = false;
			this->arrayView->Size = System::Drawing::Size(363, 418);
			this->arrayView->TabIndex = 1;
			this->arrayView->CellClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &ArrayBrowser::dataGridView1_CellClick);
			this->arrayView->RowsAdded += gcnew System::Windows::Forms::DataGridViewRowsAddedEventHandler(this, &ArrayBrowser::arrayView_RowsAdded);
			// 
			// rowNum
			// 
			this->rowNum->Frozen = true;
			this->rowNum->HeaderText = L"";
			this->rowNum->Name = L"rowNum";
			this->rowNum->ReadOnly = true;
			this->rowNum->Width = 50;
			// 
			// Selected
			// 
			this->Selected->Frozen = true;
			this->Selected->HeaderText = L"Selected";
			this->Selected->Name = L"Selected";
			this->Selected->Width = 60;
			// 
			// values
			// 
			this->values->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			this->values->DataPropertyName = L"value";
			dataGridViewCellStyle1->NullValue = nullptr;
			this->values->DefaultCellStyle = dataGridViewCellStyle1;
			this->values->HeaderText = L"Values";
			this->values->Name = L"values";
			this->values->ReadOnly = true;
			// 
			// ArrayBrowser
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(369, 464);
			this->Controls->Add(this->tableLayoutPanel1);
			this->Name = L"ArrayBrowser";
			this->Text = L"ArrayBrowser";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &ArrayBrowser::ArrayBrowser_FormClosing);
			this->tableLayoutPanel1->ResumeLayout(false);
			this->tableLayoutPanel1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->arrayView))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

		/// <summary>Form closing event handler.</summary>
		/// <param name="sender">The object sending the event.</param>
		/// <param name="e">The event arguments.</param>
	private: System::Void ArrayBrowser_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
				 //change selected in m_node
				 m_node->Instance = m_selected + 1;
	}
			 /// <summary>DGV cell click event handler.</summary>
			 /// <param name="sender">The object sending the event.</param>
			 /// <param name="e">The event arguments.</param>
	private: System::Void dataGridView1_CellClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
				 if (e->RowIndex >= 0 && e->ColumnIndex == arrayView->Columns["Selected"]->Index)
				 {
					 arrayView->ClearSelection();
					 m_selected = e->RowIndex;
					 UpdateChecks();
				 }
	}
			 /// <summary>DGV row added event handler.</summary>
			 /// <param name="sender">The object sending the event.</param>
			 /// <param name="e">The event arguments.</param>
	private: System::Void arrayView_RowsAdded(System::Object^  sender, System::Windows::Forms::DataGridViewRowsAddedEventArgs^  e) {
				 //update check boxes
				 UpdateChecks();
				 int col = arrayView->Columns["rowNum"]->Index;
				 for (int i = 1; i <= arrayView->RowCount; ++i)
					 arrayView[col, i - 1]->Value = i;
	}
	};

}
