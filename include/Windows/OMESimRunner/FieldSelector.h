#pragma once
#include "SimWrapper.h"
#include "FieldSelectRep.h"
#include "ScopeSelector.h"

namespace OMESimRunner {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Dialog used to select specific fields for the calling view.
	/// </summary>
	public ref class FieldSelector : public System::Windows::Forms::Form
	{
	public:
		FieldSelector(BaseResultsDetails* pRD, SimWrapper^ sim, String^ upper, String^ lower);
		FieldSelector(BaseResultsDetails* pRD, SimWrapper^ sim, String^ upper, String^ lower, bool redundantSelect, String^ addText, String^ remText);
		bool EnableSFColumn(String^ columnID);
		bool DisableSFColumn(String^ columnID);

		System::Void CalcSelectableRepCoords();

	protected:
		/// <summary>Pointer to the target result details object.</summary>
		BaseResultsDetails* m_pRD;
		/// <summary>Handle to the current SimWrapper.</summary>
		SimWrapper^ m_sim;
		/// <summary>Handle to Binding source containing unselected fields.</summary>
		BindingSource^ m_available;
		/// <summary>Handle to Binding source containing selected fields.</summary>
		BindingSource^ m_selected;

		/// <summary>Filter string to apply to DGV containing available fields.</summary>
		String^ m_upperFilter;
		/// <summary>Filter string to apply to DGV containing selected fields.</summary>
		String^ m_lowerFilter;

		/// <summary>If true, allows the repeated selection of the same field.</summary>
		bool m_redundantSelect;

		/// <summary>Select all fields button.</summary>
	private: System::Windows::Forms::Button^  SelectAll;
			 /// <summary>Select no fields button.</summary>
	private: System::Windows::Forms::Button^  SelectNone;
			 /// <summary>Layout for swap buttons.</summary>
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel3;
			 /// <summary>Removes selected row from selected fields group.</summary>
	private: System::Windows::Forms::Button^  removeButton;
			 /// <summary>Adds selected row from selected fields group.</summary>
	private: System::Windows::Forms::Button^  addButton;
			 /// <summary>Layout for top row of widgets.</summary>
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel4;
			 /// <summary>Upper section label.</summary>
	private: System::Windows::Forms::Label^  label2;
			 /// <summary>Text box for enter upper filter text.</summary>
	private: System::Windows::Forms::TextBox^  upperFilterBox;
			 /// <summary>Upper filter label.</summary>
	private: System::Windows::Forms::Label^  label3;
			 /// <summary>Layout for lower section header widgets.</summary>
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel5;
			 /// <summary>Text box for enter upper filter text.</summary>
	private: System::Windows::Forms::TextBox^  lowerFilterBox;
			 /// <summary>Lower filter label.</summary>
	private: System::Windows::Forms::Label^  label4;
			 /// <summary>Lower section label.</summary>
	private: System::Windows::Forms::Label^  label1;
			 /// <summary>Source Object name column.</summary>
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  SOName;
			 /// <summary>Source Object id column.</summary>
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  SOId;
			 /// <summary>Source Object type column.</summary>
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  omeType;
			 /// <summary>Source Object Parent name column.</summary>
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  SOParent;
			 /// <summary>Selected field color column.</summary>
	private: System::Windows::Forms::DataGridViewButtonColumn^  SFColor;
			 /// <summary>Selected field name column.</summary>
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  SFName;
			 /// <summary>Selected field id column.</summary>
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  SFId;
			 /// <summary>Selected field selected instance column.</summary>
	private: System::Windows::Forms::DataGridViewLinkColumn^  SFInst;
			 /// <summary>Selected field type column.</summary>
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  oType;
			 /// <summary>Selected field parent name column.</summary>
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  SFParent;
			 /// <summary>DGV containing any fields that have not been selected.</summary>
	private: System::Windows::Forms::DataGridView^  selectableObjs;
			 /// <summary>DGV containing any fields that have been selected.</summary>
	private: System::Windows::Forms::DataGridView^  selectedFields;

			 /// <summary>Confirmation button.</summary>
	private: System::Windows::Forms::Button^  okButton;
			 /// <summary>Abort button.</summary>
	private: System::Windows::Forms::Button^  cancelButton;

			 /// <summary>Split container containing the selectables on top and the selected on bottom.</summary>
	private: System::Windows::Forms::SplitContainer^  splitContainer1;
			 /// <summary>Layout for upper split pane.</summary>
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
			 /// <summary>Layout for lower split pane.</summary>
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel2;

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~FieldSelector()
		{
			if (components)
			{
				delete components;
			}
		}

		System::Void SwapRows(DataGridView^ src, DataGridView^ dst);
		System::Void SwapRows(DataGridView^ src, DataGridView^ dst, const bool % chgSrc, const bool % chgDst, const bool % newCopy);
		System::Void InsertRows(DataGridView^ src, DataGridView^ dst, int row);
		System::Void UpdateFilter(DataGridView^ dgv, String^ prefix, String^ filterText);
		System::Void ClearFilter(DataGridView^ dgv);
		System::Void RefreshAllFilters();

		System::Void AddLevelsToScopeSelector(FieldSelectRep^ rep, ScopeSelector^ scpSel);
		System::Void GetIndicesFromScopeSelector(FieldSelectRep^ rep, ScopeSelector^ scpSel);

	private:
		/// <summary>Rect used for dragging operations</summary>
		Drawing::Rectangle m_dragRect;
		/// <summary>Italicized font applied to alias objects.</summary>
		System::Drawing::Font^ ALS_FONT;

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
			this->selectableObjs = (gcnew System::Windows::Forms::DataGridView());
			this->SOName = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->SOId = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->omeType = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->SOParent = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->selectedFields = (gcnew System::Windows::Forms::DataGridView());
			this->SFColor = (gcnew System::Windows::Forms::DataGridViewButtonColumn());
			this->SFName = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->SFId = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->SFInst = (gcnew System::Windows::Forms::DataGridViewLinkColumn());
			this->oType = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->SFParent = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->okButton = (gcnew System::Windows::Forms::Button());
			this->cancelButton = (gcnew System::Windows::Forms::Button());
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->tableLayoutPanel3 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->removeButton = (gcnew System::Windows::Forms::Button());
			this->addButton = (gcnew System::Windows::Forms::Button());
			this->tableLayoutPanel4 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->upperFilterBox = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->tableLayoutPanel2 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->tableLayoutPanel5 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->lowerFilterBox = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->SelectAll = (gcnew System::Windows::Forms::Button());
			this->SelectNone = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->selectableObjs))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->selectedFields))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer1))->BeginInit();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			this->tableLayoutPanel1->SuspendLayout();
			this->tableLayoutPanel3->SuspendLayout();
			this->tableLayoutPanel4->SuspendLayout();
			this->tableLayoutPanel2->SuspendLayout();
			this->tableLayoutPanel5->SuspendLayout();
			this->SuspendLayout();
			// 
			// selectableObjs
			// 
			this->selectableObjs->AllowDrop = true;
			this->selectableObjs->AllowUserToAddRows = false;
			this->selectableObjs->AllowUserToDeleteRows = false;
			this->selectableObjs->AllowUserToOrderColumns = true;
			this->selectableObjs->AllowUserToResizeRows = false;
			this->selectableObjs->BackgroundColor = System::Drawing::Color::White;
			this->selectableObjs->CellBorderStyle = System::Windows::Forms::DataGridViewCellBorderStyle::SingleHorizontal;
			this->selectableObjs->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->selectableObjs->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(4) {
				this->SOName,
					this->SOId, this->omeType, this->SOParent
			});
			this->selectableObjs->Dock = System::Windows::Forms::DockStyle::Fill;
			this->selectableObjs->Location = System::Drawing::Point(3, 28);
			this->selectableObjs->Name = L"selectableObjs";
			this->selectableObjs->RowHeadersVisible = false;
			this->selectableObjs->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
			this->selectableObjs->Size = System::Drawing::Size(822, 216);
			this->selectableObjs->TabIndex = 1;
			this->selectableObjs->CellDoubleClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &FieldSelector::selectableObjs_CellDoubleClick);
			this->selectableObjs->CellPainting += gcnew System::Windows::Forms::DataGridViewCellPaintingEventHandler(this, &FieldSelector::selectableObjs_CellPainting);
			this->selectableObjs->SelectionChanged += gcnew System::EventHandler(this, &FieldSelector::selectableObjs_SelectionChanged);
			this->selectableObjs->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &FieldSelector::DGV_DragDrop);
			this->selectableObjs->DragOver += gcnew System::Windows::Forms::DragEventHandler(this, &FieldSelector::DGV_DragOver);
			this->selectableObjs->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &FieldSelector::DGV_MouseDown);
			this->selectableObjs->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &FieldSelector::DGV_MouseMove);
			// 
			// SOName
			// 
			this->SOName->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			this->SOName->DataPropertyName = L"Name";
			this->SOName->HeaderText = L"Name";
			this->SOName->Name = L"SOName";
			this->SOName->ReadOnly = true;
			// 
			// SOId
			// 
			this->SOId->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			this->SOId->DataPropertyName = L"ID";
			this->SOId->HeaderText = L"ID";
			this->SOId->Name = L"SOId";
			this->SOId->ReadOnly = true;
			// 
			// omeType
			// 
			this->omeType->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			this->omeType->DataPropertyName = L"Type";
			this->omeType->HeaderText = L"Type";
			this->omeType->Name = L"omeType";
			this->omeType->ReadOnly = true;
			// 
			// SOParent
			// 
			this->SOParent->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			this->SOParent->DataPropertyName = L"Path";
			this->SOParent->HeaderText = L"ParentModel";
			this->SOParent->Name = L"SOParent";
			this->SOParent->ReadOnly = true;
			// 
			// selectedFields
			// 
			this->selectedFields->AllowDrop = true;
			this->selectedFields->AllowUserToAddRows = false;
			this->selectedFields->AllowUserToDeleteRows = false;
			this->selectedFields->AllowUserToOrderColumns = true;
			this->selectedFields->BackgroundColor = System::Drawing::Color::White;
			this->selectedFields->CellBorderStyle = System::Windows::Forms::DataGridViewCellBorderStyle::SingleHorizontal;
			this->selectedFields->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->selectedFields->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(6) {
				this->SFColor,
					this->SFName, this->SFId, this->SFInst, this->oType, this->SFParent
			});
			this->selectedFields->Dock = System::Windows::Forms::DockStyle::Fill;
			this->selectedFields->Location = System::Drawing::Point(3, 28);
			this->selectedFields->Name = L"selectedFields";
			this->selectedFields->RowHeadersVisible = false;
			this->selectedFields->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
			this->selectedFields->Size = System::Drawing::Size(822, 147);
			this->selectedFields->TabIndex = 2;
			this->selectedFields->CellClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &FieldSelector::selectedFields_CellClick);
			this->selectedFields->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &FieldSelector::selectedFields_CellContentClick);
			this->selectedFields->CellDoubleClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &FieldSelector::selectedFields_CellDoubleClick);
			this->selectedFields->CellPainting += gcnew System::Windows::Forms::DataGridViewCellPaintingEventHandler(this, &FieldSelector::selectedFields_CellPainting);
			this->selectedFields->SelectionChanged += gcnew System::EventHandler(this, &FieldSelector::selectedFields_SelectionChanged);
			this->selectedFields->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &FieldSelector::DGV_DragDrop);
			this->selectedFields->DragOver += gcnew System::Windows::Forms::DragEventHandler(this, &FieldSelector::DGV_DragOver);
			this->selectedFields->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &FieldSelector::DGV_MouseDown);
			this->selectedFields->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &FieldSelector::DGV_MouseMove);
			// 
			// SFColor
			// 
			this->SFColor->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			dataGridViewCellStyle1->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleCenter;
			dataGridViewCellStyle1->SelectionBackColor = System::Drawing::Color::Transparent;
			dataGridViewCellStyle1->SelectionForeColor = System::Drawing::Color::Transparent;
			this->SFColor->DefaultCellStyle = dataGridViewCellStyle1;
			this->SFColor->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->SFColor->HeaderText = L"Color";
			this->SFColor->Name = L"SFColor";
			// 
			// SFName
			// 
			this->SFName->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			this->SFName->DataPropertyName = L"Name";
			this->SFName->HeaderText = L"Name";
			this->SFName->Name = L"SFName";
			// 
			// SFId
			// 
			this->SFId->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			this->SFId->DataPropertyName = L"ID";
			this->SFId->HeaderText = L"ID";
			this->SFId->Name = L"SFId";
			// 
			// SFInst
			// 
			this->SFInst->DataPropertyName = L"CoordStr";
			this->SFInst->HeaderText = L"Instance";
			this->SFInst->Name = L"SFInst";
			this->SFInst->Visible = false;
			// 
			// oType
			// 
			this->oType->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			this->oType->DataPropertyName = L"Type";
			this->oType->HeaderText = L"Type";
			this->oType->Name = L"oType";
			// 
			// SFParent
			// 
			this->SFParent->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			this->SFParent->DataPropertyName = L"Path";
			this->SFParent->HeaderText = L"ParentModel";
			this->SFParent->Name = L"SFParent";
			// 
			// okButton
			// 
			this->okButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->okButton->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->okButton->Location = System::Drawing::Point(12, 492);
			this->okButton->Name = L"okButton";
			this->okButton->Size = System::Drawing::Size(75, 23);
			this->okButton->TabIndex = 3;
			this->okButton->Text = L"OK";
			this->okButton->UseVisualStyleBackColor = true;
			this->okButton->Click += gcnew System::EventHandler(this, &FieldSelector::okButton_Click);
			// 
			// cancelButton
			// 
			this->cancelButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->cancelButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->cancelButton->Location = System::Drawing::Point(93, 492);
			this->cancelButton->Name = L"cancelButton";
			this->cancelButton->Size = System::Drawing::Size(75, 23);
			this->cancelButton->TabIndex = 4;
			this->cancelButton->Text = L"Cancel";
			this->cancelButton->UseVisualStyleBackColor = true;
			this->cancelButton->Click += gcnew System::EventHandler(this, &FieldSelector::button1_Click);
			// 
			// splitContainer1
			// 
			this->splitContainer1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->splitContainer1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->splitContainer1->Location = System::Drawing::Point(0, 0);
			this->splitContainer1->Margin = System::Windows::Forms::Padding(3, 3, 3, 50);
			this->splitContainer1->Name = L"splitContainer1";
			this->splitContainer1->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer1.Panel1
			// 
			this->splitContainer1->Panel1->Controls->Add(this->tableLayoutPanel1);
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->Controls->Add(this->tableLayoutPanel2);
			this->splitContainer1->Size = System::Drawing::Size(830, 468);
			this->splitContainer1->SplitterDistance = 284;
			this->splitContainer1->TabIndex = 7;
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->ColumnCount = 1;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle()));
			this->tableLayoutPanel1->Controls->Add(this->selectableObjs, 0, 1);
			this->tableLayoutPanel1->Controls->Add(this->tableLayoutPanel3, 0, 2);
			this->tableLayoutPanel1->Controls->Add(this->tableLayoutPanel4, 0, 0);
			this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel1->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 3;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 25)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 35)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(828, 282);
			this->tableLayoutPanel1->TabIndex = 0;
			// 
			// tableLayoutPanel3
			// 
			this->tableLayoutPanel3->ColumnCount = 2;
			this->tableLayoutPanel3->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				50)));
			this->tableLayoutPanel3->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				50)));
			this->tableLayoutPanel3->Controls->Add(this->removeButton, 1, 0);
			this->tableLayoutPanel3->Controls->Add(this->addButton, 0, 0);
			this->tableLayoutPanel3->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel3->Location = System::Drawing::Point(3, 250);
			this->tableLayoutPanel3->Name = L"tableLayoutPanel3";
			this->tableLayoutPanel3->RowCount = 1;
			this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 29)));
			this->tableLayoutPanel3->Size = System::Drawing::Size(822, 29);
			this->tableLayoutPanel3->TabIndex = 7;
			// 
			// removeButton
			// 
			this->removeButton->Dock = System::Windows::Forms::DockStyle::Left;
			this->removeButton->Enabled = false;
			this->removeButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->removeButton->Location = System::Drawing::Point(414, 3);
			this->removeButton->Name = L"removeButton";
			this->removeButton->Size = System::Drawing::Size(75, 23);
			this->removeButton->TabIndex = 0;
			this->removeButton->Text = L"↑";
			this->removeButton->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			this->removeButton->UseVisualStyleBackColor = true;
			this->removeButton->Click += gcnew System::EventHandler(this, &FieldSelector::removeButton_Click);
			// 
			// addButton
			// 
			this->addButton->Dock = System::Windows::Forms::DockStyle::Right;
			this->addButton->Enabled = false;
			this->addButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->addButton->Location = System::Drawing::Point(333, 3);
			this->addButton->Name = L"addButton";
			this->addButton->Size = System::Drawing::Size(75, 23);
			this->addButton->TabIndex = 1;
			this->addButton->Text = L"↓";
			this->addButton->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			this->addButton->UseVisualStyleBackColor = true;
			this->addButton->Click += gcnew System::EventHandler(this, &FieldSelector::addButton_Click);
			// 
			// tableLayoutPanel4
			// 
			this->tableLayoutPanel4->ColumnCount = 3;
			this->tableLayoutPanel4->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				25)));
			this->tableLayoutPanel4->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				20)));
			this->tableLayoutPanel4->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				55)));
			this->tableLayoutPanel4->Controls->Add(this->label2, 0, 0);
			this->tableLayoutPanel4->Controls->Add(this->upperFilterBox, 2, 0);
			this->tableLayoutPanel4->Controls->Add(this->label3, 1, 0);
			this->tableLayoutPanel4->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel4->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel4->Margin = System::Windows::Forms::Padding(0);
			this->tableLayoutPanel4->Name = L"tableLayoutPanel4";
			this->tableLayoutPanel4->RowCount = 1;
			this->tableLayoutPanel4->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel4->Size = System::Drawing::Size(828, 25);
			this->tableLayoutPanel4->TabIndex = 8;
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(3, 12);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(66, 13);
			this->label2->TabIndex = 7;
			this->label2->Text = L"Upper Fields";
			// 
			// upperFilterBox
			// 
			this->upperFilterBox->Dock = System::Windows::Forms::DockStyle::Fill;
			this->upperFilterBox->Location = System::Drawing::Point(375, 3);
			this->upperFilterBox->Name = L"upperFilterBox";
			this->upperFilterBox->Size = System::Drawing::Size(450, 20);
			this->upperFilterBox->TabIndex = 8;
			this->upperFilterBox->TextChanged += gcnew System::EventHandler(this, &FieldSelector::FilterChange);
			// 
			// label3
			// 
			this->label3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(337, 6);
			this->label3->Margin = System::Windows::Forms::Padding(3, 6, 3, 0);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(32, 13);
			this->label3->TabIndex = 9;
			this->label3->Text = L"Filter:";
			// 
			// tableLayoutPanel2
			// 
			this->tableLayoutPanel2->ColumnCount = 1;
			this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle()));
			this->tableLayoutPanel2->Controls->Add(this->selectedFields, 0, 1);
			this->tableLayoutPanel2->Controls->Add(this->tableLayoutPanel5, 0, 0);
			this->tableLayoutPanel2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel2->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel2->Name = L"tableLayoutPanel2";
			this->tableLayoutPanel2->RowCount = 2;
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 25)));
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel2->Size = System::Drawing::Size(828, 178);
			this->tableLayoutPanel2->TabIndex = 0;
			// 
			// tableLayoutPanel5
			// 
			this->tableLayoutPanel5->ColumnCount = 3;
			this->tableLayoutPanel5->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				25)));
			this->tableLayoutPanel5->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				20)));
			this->tableLayoutPanel5->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				55)));
			this->tableLayoutPanel5->Controls->Add(this->lowerFilterBox, 0, 0);
			this->tableLayoutPanel5->Controls->Add(this->label4, 0, 0);
			this->tableLayoutPanel5->Controls->Add(this->label1, 0, 0);
			this->tableLayoutPanel5->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel5->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel5->Margin = System::Windows::Forms::Padding(0);
			this->tableLayoutPanel5->Name = L"tableLayoutPanel5";
			this->tableLayoutPanel5->RowCount = 1;
			this->tableLayoutPanel5->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel5->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 25)));
			this->tableLayoutPanel5->Size = System::Drawing::Size(828, 25);
			this->tableLayoutPanel5->TabIndex = 3;
			// 
			// lowerFilterBox
			// 
			this->lowerFilterBox->Dock = System::Windows::Forms::DockStyle::Fill;
			this->lowerFilterBox->Location = System::Drawing::Point(375, 3);
			this->lowerFilterBox->Name = L"lowerFilterBox";
			this->lowerFilterBox->Size = System::Drawing::Size(450, 20);
			this->lowerFilterBox->TabIndex = 11;
			this->lowerFilterBox->TextChanged += gcnew System::EventHandler(this, &FieldSelector::FilterChange);
			// 
			// label4
			// 
			this->label4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(337, 6);
			this->label4->Margin = System::Windows::Forms::Padding(3, 6, 3, 0);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(32, 13);
			this->label4->TabIndex = 10;
			this->label4->Text = L"Filter:";
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(3, 12);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(63, 13);
			this->label1->TabIndex = 6;
			this->label1->Text = L"Lower fields";
			// 
			// SelectAll
			// 
			this->SelectAll->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->SelectAll->Location = System::Drawing::Point(333, 492);
			this->SelectAll->Name = L"SelectAll";
			this->SelectAll->Size = System::Drawing::Size(75, 23);
			this->SelectAll->TabIndex = 8;
			this->SelectAll->Text = L"Select All";
			this->SelectAll->UseVisualStyleBackColor = true;
			this->SelectAll->Click += gcnew System::EventHandler(this, &FieldSelector::SelectAll_click);
			// 
			// SelectNone
			// 
			this->SelectNone->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->SelectNone->Location = System::Drawing::Point(414, 492);
			this->SelectNone->Name = L"SelectNone";
			this->SelectNone->Size = System::Drawing::Size(75, 23);
			this->SelectNone->TabIndex = 9;
			this->SelectNone->Text = L"Select None";
			this->SelectNone->UseVisualStyleBackColor = true;
			this->SelectNone->Click += gcnew System::EventHandler(this, &FieldSelector::DeSelectAll_Click);
			// 
			// FieldSelector
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(830, 527);
			this->Controls->Add(this->SelectNone);
			this->Controls->Add(this->SelectAll);
			this->Controls->Add(this->splitContainer1);
			this->Controls->Add(this->cancelButton);
			this->Controls->Add(this->okButton);
			this->Name = L"FieldSelector";
			this->Text = L"FieldSelector";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->selectableObjs))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->selectedFields))->EndInit();
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer1))->EndInit();
			this->splitContainer1->ResumeLayout(false);
			this->tableLayoutPanel1->ResumeLayout(false);
			this->tableLayoutPanel3->ResumeLayout(false);
			this->tableLayoutPanel4->ResumeLayout(false);
			this->tableLayoutPanel4->PerformLayout();
			this->tableLayoutPanel2->ResumeLayout(false);
			this->tableLayoutPanel5->ResumeLayout(false);
			this->tableLayoutPanel5->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

		/// <summary>Event handler for Cancel button click.</summary>
		/// <param name="sender">Object that sent the event.</param>
		/// <param name="e">Arguments associated with the event.</param>
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e)
	{
		//cancel button
		Close();
	}
			 /// <summary>Event handler for mouse clicks within the selected fields DGV.</summary>
			 /// <param name="sender">Object that sent the event.</param>
			 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void selectedFields_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e)
	{
				 if (e->RowIndex >= 0 && e->ColumnIndex == selectedFields->Columns["SFInst"]->Index)
				 {
					 ScopeSelector^ scpSel = gcnew ScopeSelector();

					 //add entries
					 AddLevelsToScopeSelector((FieldSelectRep^)m_selected[e->RowIndex], scpSel);
					 scpSel->ShowDialog();
					 GetIndicesFromScopeSelector((FieldSelectRep^)m_selected[e->RowIndex], scpSel);
					 selectedFields->Refresh();
				 }
	}

			 /// <summary>Event handler for OK button click.</summary>
			 /// <param name="sender">Object that sent the event.</param>
			 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void okButton_Click(System::Object^  sender, System::EventArgs^  e)
	{
				 m_sim->ApplyFieldData(m_selected);
				 Close();
	}
			 /// <summary>Event handler for mouse clicks within the selectable objects fields DGV.</summary>
			 /// <param name="sender">Object that sent the event.</param>
			 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void selectableObjs_CellDoubleClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e)
	{
				 SwapRows(selectableObjs, selectedFields, !m_redundantSelect, true, m_redundantSelect);

				 RefreshAllFilters();
	}
			 /// <summary>Event handler for mouse double-clicks within the selected fields DGV.</summary>
			 /// <param name="sender">Object that sent the event.</param>
			 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void selectedFields_CellDoubleClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e)
	{
				 SwapRows(selectedFields, selectableObjs, true, !m_redundantSelect, false);

				 RefreshAllFilters();
	}

			 //drag operations
			 // http://social.msdn.microsoft.com/Forums/windows/en-US/16b0a44e-35a0-4bc8-9ccd-ec2c62c95a55/select-and-drag-a-datagridview-row-with-a-single-click?forum=winforms
			 /// <summary>Event handler for mouse dragover within a DGV.</summary>
			 /// <param name="sender">Object that sent the event.</param>
			 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void DGV_DragOver(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e)
	{
				 e->Effect = DragDropEffects::Move;
	}
			 /// <summary>Event handler for mouse drag & drop within a DGV.</summary>
			 /// <param name="sender">Object that sent the event.</param>
			 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void DGV_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e)
	{
				 DataGridView^ table = (DataGridView^)sender;
				 Point tblPoint = table->PointToClient(Point(e->X, e->Y));
				 int hitRow = table->HitTest(tblPoint.X, tblPoint.Y)->RowIndex;
				 if (e->Effect == DragDropEffects::Move)
				 {
					 DataGridViewSelectedRowCollection^ rows = (DataGridViewSelectedRowCollection^)e->Data->GetData(DataGridViewSelectedRowCollection::typeid);

					 if (rows->Count)
						 InsertRows(rows[0]->DataGridView, table, hitRow);

					 RefreshAllFilters();
				 }
	}

			 /// <summary>Event handler for mouse down within a DGV.</summary>
			 /// <param name="sender">Object that sent the event.</param>
			 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void DGV_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
	{
				 DataGridView^ table = (DataGridView^)sender;
				 if (table->SelectedRows->Count)
				 {
					 Drawing::Size dragSize = SystemInformation::DragSize;
					 m_dragRect = Drawing::Rectangle(Point(e->X - dragSize.Width / 2, e->Y - dragSize.Height / 2), dragSize);
				 }
	}

			 /// <summary>Event handler for mouse move within a DGV.</summary>
			 /// <param name="sender">Object that sent the event.</param>
			 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void DGV_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
	{
				 DataGridView^ table = (DataGridView^)sender;

				 if ((e->Button & ::MouseButtons::Left) == ::MouseButtons::Left && table->SelectedRows->Count)
				 {
					 if (!m_dragRect.IsEmpty && !m_dragRect.Contains(e->X, e->Y))
					 {
						 table->DoDragDrop(table->SelectedRows, DragDropEffects::Move);
					 }
				 }
				 else
					 m_dragRect = Drawing::Rectangle(0, 0, 0, 0);
	}
			 /// <summary>Event handler for Select All button.</summary>
			 /// <param name="sender">Object that sent the event.</param>
			 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void SelectAll_click(System::Object^  sender, System::EventArgs^  e)
	{
				 if (!m_redundantSelect)
				 {
					 for (int i = 0; i < m_available->Count; ++i)
						 m_selected->Add(m_available[i]);

					 m_available->Clear();
				 }
				 else
				 for (int i = 0; i < m_available->Count; ++i)
					 m_selected->Add(gcnew FieldSelectRep((FieldSelectRep^)m_available[i]));
	}
			 /// <summary>Event handler for De-Select All button.</summary>
			 /// <param name="sender">Object that sent the event.</param>
			 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void DeSelectAll_Click(System::Object^  sender, System::EventArgs^  e)
	{
				 if (!m_redundantSelect)
				 {
					 for (int i = 0; i < m_selected->Count; ++i)
						 m_available->Add(m_selected[i]);
				 }

				 m_selected->Clear();
	}

			 /// <summary>Event handler for Add button.</summary>
			 /// <param name="sender">Object that sent the event.</param>
			 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void addButton_Click(System::Object^  sender, System::EventArgs^  e) {

				 DataGridViewSelectedRowCollection^ rows = selectableObjs->SelectedRows;
				 for (int i = 0; i < rows->Count; ++i)
				 {
					 m_selected->Add(rows[i]->DataBoundItem);
					 if (!m_redundantSelect)
						 m_available->Remove(rows[i]->DataBoundItem);
				 }
				 selectableObjs->ClearSelection();
				 RefreshAllFilters();
	}
			 /// <summary>Event handler for Remove button.</summary>
			 /// <param name="sender">Object that sent the event.</param>
			 /// <param name="e">Arguments associated with the event.</param>

	private: System::Void removeButton_Click(System::Object^  sender, System::EventArgs^  e) {

				 DataGridViewSelectedRowCollection^ rows = selectedFields->SelectedRows;
				 for (int i = 0; i < rows->Count; ++i)
				 {
					 if (!m_redundantSelect)
						 m_available->Add(rows[i]->DataBoundItem);
					 m_selected->Remove(rows[i]->DataBoundItem);
				 }

				 selectedFields->ClearSelection();
				 RefreshAllFilters();
	}
			 /// <summary>Event handler for selection change in SelectedFields DGV.</summary>
			 /// <param name="sender">Object that sent the event.</param>
			 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void selectedFields_SelectionChanged(System::Object^  sender, System::EventArgs^  e) {
				 removeButton->Enabled = selectedFields->SelectedRows->Count > 0;
	}
			 /// <summary>Event handler for selection change in Selectable Objects DGV.</summary>
			 /// <param name="sender">Object that sent the event.</param>
			 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void selectableObjs_SelectionChanged(System::Object^  sender, System::EventArgs^  e) {
				 addButton->Enabled = selectableObjs->SelectedRows->Count > 0;
	}
			 /// <summary>Event handler for a DGV filter change.</summary>
			 /// <param name="sender">Object that sent the event.</param>
			 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void FilterChange(System::Object^  sender, System::EventArgs^  e) {

				 DataGridView^ dView = nullptr;
				 String^ prefix;
				 String^ actFilter;
				 actFilter = ((TextBox^)sender)->Text;
				 if (sender == upperFilterBox)
				 {
					 dView = selectableObjs;
					 prefix = "SO";
					 m_upperFilter = actFilter;
				 }
				 else if (sender == lowerFilterBox)
				 {
					 dView = selectedFields;
					 prefix = "SF";
					 m_lowerFilter = actFilter;
				 }

				 if (dView != nullptr)
				 {
					 UpdateFilter(dView, prefix, actFilter);
				 }
	}
			 /// <summary>Event handler for cell painting in SelectedFields DGV.</summary>
			 /// <param name="sender">Object that sent the event.</param>
			 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void selectedFields_CellPainting(System::Object^  sender, System::Windows::Forms::DataGridViewCellPaintingEventArgs^  e) {
				 if (e->RowIndex >= 0)
				 {
					 FieldSelectRep^ currRep = (FieldSelectRep^)m_selected[e->RowIndex];

					 if (currRep)
					 {
						 if (e->ColumnIndex == selectedFields->Columns["SFColor"]->Index)
						 {
							 //paint cell the selected color
							 OMEColor* colorRef = currRep->ColorRef;

							 //selectedFields->CurrentCell->Style->BackColor = System::Drawing::Color::FromArgb(255, colorRef->r * 255, colorRef->g * 255, colorRef->b * 255);
							 //e->PaintContent(e->ClipBounds);

							 selectedFields[e->ColumnIndex, e->RowIndex]->Style->BackColor = System::Drawing::Color::FromArgb(255, colorRef->r * 255, colorRef->g * 255, colorRef->b * 255);
							 selectedFields[e->ColumnIndex, e->RowIndex]->Style->SelectionBackColor = selectedFields[e->ColumnIndex, e->RowIndex]->Style->BackColor;
						 }
						 else
						 {
							 if (currRep->Type == "Alias")
								 selectedFields[e->ColumnIndex, e->RowIndex]->Style->Font = ALS_FONT;
							 else
								 selectedFields[e->ColumnIndex, e->RowIndex]->Style->Font = this->Font;

						 }

					 }
				 }
	}
			 /// <summary>Event handler for cell click in SelectedFields DGV.</summary>
			 /// <param name="sender">Object that sent the event.</param>
			 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void selectedFields_CellClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {

				 if (e->RowIndex >= 0 && e->ColumnIndex == selectedFields->Columns["SFColor"]->Index)
				 {
					 FieldSelectRep^ currRep = (FieldSelectRep^)m_selected[e->RowIndex];

					 ColorDialog^ colDlg = gcnew ColorDialog;
					 colDlg->SolidColorOnly = true;
					 OMEColor* colorRef = currRep->ColorRef;
					 colDlg->Color = System::Drawing::Color::FromArgb(255, colorRef->r * 255, colorRef->g * 255, colorRef->b * 255);
					 if (colDlg->ShowDialog() == System::Windows::Forms::DialogResult::OK)
					 {
						 colorRef->r = colDlg->Color.R / 255.0;
						 colorRef->g = colDlg->Color.G / 255.0;
						 colorRef->b = colDlg->Color.B / 255.0;
						 selectedFields->Refresh();
					 }
				 }
	}

			 /// <summary>Event handler for cell painting in Selectable Objects DGV.</summary>
			 /// <param name="sender">Object that sent the event.</param>
			 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void selectableObjs_CellPainting(System::Object^  sender, System::Windows::Forms::DataGridViewCellPaintingEventArgs^  e) {

				 if (e->RowIndex >= 0)
				 {
					 FieldSelectRep^ currRep = (FieldSelectRep^)m_available[e->RowIndex];

					 if (currRep && currRep->Type == "Alias")
						 selectableObjs[e->ColumnIndex, e->RowIndex]->Style->Font = ALS_FONT;
					 else
						 selectableObjs[e->ColumnIndex, e->RowIndex]->Style->Font = this->Font;

				 }
	}
	};
}
