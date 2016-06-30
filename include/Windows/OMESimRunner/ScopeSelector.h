#pragma once

namespace OMESimRunner {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>Node used for ScopeSelector list.</summary>
	public ref class SSNode
	{
	public:

		SSNode()
			:m_lvl(""), m_ind(1) {}

		/// <summary>Detailed constructor.</summary>
		/// <param name="lvl">String representing the path to the index.</param>
		/// <param name="ind">Absolute index of represented value.</param>
		SSNode(String^ lvl, size_t ind)
			:m_lvl(lvl), m_ind(ind) {}

		/// <summary>String representing the path to the index.</summary>
		property String^ Level
		{
			String^ get()
			{
				return m_lvl;
			}
			System::Void set(String^ str)
			{
				m_lvl = str;
			}
		}

		/// <summary>Absolute index of represented value.</summary>
		property size_t Index
		{
			size_t get()
			{
				return m_ind;
			}

			System::Void set(size_t ind)
			{
				m_ind = ind;
			}
		}

	private:
		/// <summary>String representing the path to the index.</summary>
		String^ m_lvl;
		/// <summary>Absolute index of represented value.</summary>
		size_t m_ind;
	};

//////////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// Dialog for selecting specific instance of a represented object.
	/// </summary>
	public ref class ScopeSelector : public System::Windows::Forms::Form
	{
	public:
		/// <summary> The number of levels of indexing associated with the parent object. </summary>
		property unsigned int LevelCount
		{
			unsigned int get()
			{
				return m_entries->Count;
			}
		}

		/// <summary> Accessor for specific index. </summary>
		property size_t Index[unsigned int]
		{
			size_t get(unsigned int i)
			{
				return ((SSNode^)m_entries[i])->Index;
			}
		}

		void AddEntry(String^ level, const size_t & index);
		void AddLevel(String^ level);

		ScopeSelector(void)
		{
			InitializeComponent();

			m_entries = gcnew BindingSource();
			ScopeTable->DataSource = m_entries;
		}

		/// <summary> Column for displaying index level. </summary>
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  level;
			 /// <summary> Column for displaying active index for a given level. </summary>
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  index;
			 /// <summary> Info label </summary>
	private: System::Windows::Forms::Label^  label1;
			 /// <summary> Base layout for window. </summary>
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
			 /// <summary> DGV for showing the indices as applied to different scoping levels. </summary>
	private: System::Windows::Forms::DataGridView^  ScopeTable;
			 /// <summary> Layout for dismissal buttons. </summary>
	private: System::Windows::Forms::FlowLayoutPanel^  flowLayoutPanel1;
			 /// <summary> The OK button. </summary>
	private: System::Windows::Forms::Button^  OKButton;
			 /// <summary> The cancel button. </summary>
	private: System::Windows::Forms::Button^  CancelButton;

	protected:

		/// <summary> Data container for DGV. </summary>
		BindingSource^ m_entries;


		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ScopeSelector()
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
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->flowLayoutPanel1 = (gcnew System::Windows::Forms::FlowLayoutPanel());
			this->OKButton = (gcnew System::Windows::Forms::Button());
			this->CancelButton = (gcnew System::Windows::Forms::Button());
			this->ScopeTable = (gcnew System::Windows::Forms::DataGridView());
			this->level = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->index = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->tableLayoutPanel1->SuspendLayout();
			this->flowLayoutPanel1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ScopeTable))->BeginInit();
			this->SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->ColumnCount = 1;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				100)));
			this->tableLayoutPanel1->Controls->Add(this->flowLayoutPanel1, 0, 2);
			this->tableLayoutPanel1->Controls->Add(this->ScopeTable, 0, 1);
			this->tableLayoutPanel1->Controls->Add(this->label1, 0, 0);
			this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel1->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 3;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 35)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(468, 383);
			this->tableLayoutPanel1->TabIndex = 0;
			// 
			// flowLayoutPanel1
			// 
			this->flowLayoutPanel1->Controls->Add(this->CancelButton);
			this->flowLayoutPanel1->Controls->Add(this->OKButton);
			this->flowLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->flowLayoutPanel1->FlowDirection = System::Windows::Forms::FlowDirection::RightToLeft;
			this->flowLayoutPanel1->Location = System::Drawing::Point(3, 351);
			this->flowLayoutPanel1->Name = L"flowLayoutPanel1";
			this->flowLayoutPanel1->Size = System::Drawing::Size(462, 29);
			this->flowLayoutPanel1->TabIndex = 2;
			// 
			// OKButton
			// 
			this->OKButton->Location = System::Drawing::Point(303, 3);
			this->OKButton->Name = L"OKButton";
			this->OKButton->Size = System::Drawing::Size(75, 23);
			this->OKButton->TabIndex = 0;
			this->OKButton->Text = L"OK";
			this->OKButton->UseVisualStyleBackColor = true;
			this->OKButton->Click += gcnew System::EventHandler(this, &ScopeSelector::OKButton_Click);
			// 
			// CancelButton
			// 
			this->CancelButton->Location = System::Drawing::Point(384, 3);
			this->CancelButton->Name = L"CancelButton";
			this->CancelButton->Size = System::Drawing::Size(75, 23);
			this->CancelButton->TabIndex = 1;
			this->CancelButton->Text = L"Cancel";
			this->CancelButton->UseVisualStyleBackColor = true;
			this->CancelButton->Click += gcnew System::EventHandler(this, &ScopeSelector::CancelButton_Click);
			// 
			// ScopeTable
			// 
			this->ScopeTable->AllowUserToAddRows = false;
			this->ScopeTable->AllowUserToDeleteRows = false;
			this->ScopeTable->BackgroundColor = System::Drawing::Color::White;
			this->ScopeTable->ColumnHeadersBorderStyle = System::Windows::Forms::DataGridViewHeaderBorderStyle::Single;
			this->ScopeTable->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->ScopeTable->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(2) { this->level, this->index });
			this->ScopeTable->Dock = System::Windows::Forms::DockStyle::Fill;
			this->ScopeTable->Location = System::Drawing::Point(3, 23);
			this->ScopeTable->Name = L"ScopeTable";
			this->ScopeTable->RowHeadersVisible = false;
			this->ScopeTable->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
			this->ScopeTable->Size = System::Drawing::Size(462, 322);
			this->ScopeTable->TabIndex = 1;
			// 
			// level
			// 
			this->level->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			this->level->DataPropertyName = L"Level";
			this->level->HeaderText = L"Level";
			this->level->Name = L"level";
			this->level->ReadOnly = true;
			// 
			// index
			// 
			this->index->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			this->index->DataPropertyName = L"Index";
			this->index->HeaderText = L"Index";
			this->index->Name = L"index";
			this->index->Resizable = System::Windows::Forms::DataGridViewTriState::True;
			// 
			// label1
			// 
			this->label1->Anchor = System::Windows::Forms::AnchorStyles::Left;
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(3, 3);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(176, 13);
			this->label1->TabIndex = 3;
			this->label1->Text = L"Edit an Index Value by Clicking on it";
			// 
			// ScopeSelector
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(468, 383);
			this->Controls->Add(this->tableLayoutPanel1);
			this->Name = L"ScopeSelector";
			this->Text = L"ScopeSelector";
			this->tableLayoutPanel1->ResumeLayout(false);
			this->tableLayoutPanel1->PerformLayout();
			this->flowLayoutPanel1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ScopeTable))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
		/// <summary>Event handler for Ok button click.</summary>
		/// <param name="sender">Object that invoked the event handler.</param>
		/// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void OKButton_Click(System::Object^  sender, System::EventArgs^  e)
	{
				 //apply

				 Close();
	}
			 /// <summary>Event handler for Cancel button click.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void CancelButton_Click(System::Object^  sender, System::EventArgs^  e)
	{
				 m_entries->Clear();
				 Close();
	}
	};


	
}