#pragma once
#include "SimWrapper.h"
#include "FieldSelector.h"
#include "SimpleTreeView.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace OMESimRunner {

	/// <summary>
	/// Preference panel for SimpleTreeView.
	/// </summary>
	public ref class TreeViewPrefPanel : public System::Windows::Forms::UserControl
	{
	public:
		/// <summary>Detailed constructor.</summary>
		/// <param name="sim">Handle to Active SimWrapper.</param>
		/// <param name="tv">Handle to SimpleTreeView affected by preference pane.</param>
		TreeViewPrefPanel(SimWrapper^ sim,SimpleTreeView^ tv)
			:m_sim(sim),m_treeView(tv)
		{
			InitializeComponent();
		}

		/// <summary>Reload Data from source.</summary>
		System::Void ReloadData()
		{
			TreeResultsDetails* pDetails=(TreeResultsDetails*)m_sim->ViewMgr->GetActiveDetails();
			msclr::interop::marshal_context context;
			viewNameBox->Text=context.marshal_as<String^>(pDetails->GetViewName());
			filterSelect->Checked=pDetails->GetUseSelected();

			//update from pDetails once added
			sortModeCBox->SelectedIndex = (int)pDetails->GetSortMode();
		}
		
	protected:
		/// <summary>Handle to Active SimWrapper.</summary>
		SimWrapper^ m_sim;
		/// <summary>Reference to old name during name change.</summary>
		String^ m_oldName;
		/// <summary>Handle to SimpleTreeView affected by preference pane.</summary>
		SimpleTreeView^ m_treeView;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~TreeViewPrefPanel()
		{
			if (components)
			{
				delete components;
			}
		}

		/// <summary>Button that invokes field selector.</summary>
	private: System::Windows::Forms::Button^  selectorButton;
			 /// <summary>Name field label</summary>
	private: System::Windows::Forms::Label^  label1;
			 /// <summary>Box containing view name.</summary>
	private: System::Windows::Forms::TextBox^  viewNameBox;
			 /// <summary>Checkbox indicating whether or not to filter fields based on selections.</summary>
	private: System::Windows::Forms::CheckBox^  filterSelect;
			 /// <summary>ComboBox that sets sort order of list contents.</summary>
	private: System::Windows::Forms::ComboBox^  sortModeCBox;
			 /// <summary>Label for sorting combo box.</summary>
	private: System::Windows::Forms::Label^  label2;

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
			this->selectorButton = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->viewNameBox = (gcnew System::Windows::Forms::TextBox());
			this->filterSelect = (gcnew System::Windows::Forms::CheckBox());
			this->sortModeCBox = (gcnew System::Windows::Forms::ComboBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// selectorButton
			// 
			this->selectorButton->Location = System::Drawing::Point(6, 79);
			this->selectorButton->Name = L"selectorButton";
			this->selectorButton->Size = System::Drawing::Size(93, 23);
			this->selectorButton->TabIndex = 0;
			this->selectorButton->Text = L"Select Fields...";
			this->selectorButton->UseVisualStyleBackColor = true;
			this->selectorButton->Click += gcnew System::EventHandler(this, &TreeViewPrefPanel::button1_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(3, 6);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(64, 13);
			this->label1->TabIndex = 3;
			this->label1->Text = L"View Name:";
			// 
			// viewNameBox
			// 
			this->viewNameBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->viewNameBox->Location = System::Drawing::Point(73, 3);
			this->viewNameBox->Name = L"viewNameBox";
			this->viewNameBox->Size = System::Drawing::Size(524, 20);
			this->viewNameBox->TabIndex = 4;
			this->viewNameBox->Enter += gcnew System::EventHandler(this, &TreeViewPrefPanel::viewNameBox_Enter);
			this->viewNameBox->Leave += gcnew System::EventHandler(this, &TreeViewPrefPanel::viewNameBox_Leave);
			// 
			// filterSelect
			// 
			this->filterSelect->AutoSize = true;
			this->filterSelect->Location = System::Drawing::Point(6, 29);
			this->filterSelect->Name = L"filterSelect";
			this->filterSelect->Size = System::Drawing::Size(144, 17);
			this->filterSelect->TabIndex = 5;
			this->filterSelect->Text = L"Use Only Selected Fields";
			this->filterSelect->UseVisualStyleBackColor = true;
			this->filterSelect->CheckedChanged += gcnew System::EventHandler(this, &TreeViewPrefPanel::filterSelect_CheckedChanged);
			// 
			// sortModeCBox
			// 
			this->sortModeCBox->FormattingEnabled = true;
			this->sortModeCBox->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"Components First", L"Models First", L"Mixed" });
			this->sortModeCBox->Location = System::Drawing::Point(90, 52);
			this->sortModeCBox->Name = L"sortModeCBox";
			this->sortModeCBox->Size = System::Drawing::Size(189, 21);
			this->sortModeCBox->TabIndex = 7;
			this->sortModeCBox->SelectedIndexChanged += gcnew System::EventHandler(this, &TreeViewPrefPanel::comboBox1_SelectedIndexChanged);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(11, 54);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(73, 13);
			this->label2->TabIndex = 8;
			this->label2->Text = L"Sorting Mode:";
			// 
			// TreeViewPrefPanel
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->label2);
			this->Controls->Add(this->sortModeCBox);
			this->Controls->Add(this->filterSelect);
			this->Controls->Add(this->viewNameBox);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->selectorButton);
			this->Name = L"TreeViewPrefPanel";
			this->Size = System::Drawing::Size(600, 174);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		/// <summary>Event handler for Select Fields button click.</summary>
		/// <param name="sender">Handle to the object invoking the event handler.</param>
		/// <param name="e">Arguments associated with the event.</param>
		private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) 
				{
					FieldSelector^ selectForm=gcnew FieldSelector(m_sim->ViewMgr->GetActiveDetails(),m_sim,
						"Available Fields:","Select Fields by Dragging them to this table:");
					msclr::interop::marshal_context context;
					selectForm->Text=String::Concat(context.marshal_as<String^>(m_sim->ViewMgr->GetActiveDetails()->GetViewName())," - ",selectForm->Text);
					if(selectForm->ShowDialog()==DialogResult::OK)
						m_treeView->RefreshActive();
				}
	
				 /// <summary>Event handler for beginning of editing in viewNameBox.</summary>
				 /// <param name="sender">Handle to the object invoking the event handler.</param>
				 /// <param name="e">Arguments associated with the event.</param>
private: System::Void viewNameBox_Enter(System::Object^  sender, System::EventArgs^  e) 
			{
				m_oldName=viewNameBox->Text;
			}

		 /// <summary>Event handler for ending of editing in viewNameBox.</summary>
		 /// <param name="sender">Handle to the object invoking the event handler.</param>
		 /// <param name="e">Arguments associated with the event.</param>
private: System::Void viewNameBox_Leave(System::Object^  sender, System::EventArgs^  e) 
			{
				String^ newName=viewNameBox->Text;
				if(m_oldName!=newName)
				{
					msclr::interop::marshal_context context;
					if(m_sim->ViewMgr->NameIsUnique(context.marshal_as<STLString>(newName),m_sim->ViewMgr->GetActiveDetails()))
						m_sim->ViewMgr->GetActiveDetails()->SetViewName(context.marshal_as<STLString>(newName));
					else
					{
						DialogResult r6 = MessageBox::Show( this, String::Concat("The name '",newName,"' is already in use by another view."), "Name in use", MessageBoxButtons::OK, MessageBoxIcon::Error, MessageBoxDefaultButton::Button1, (MessageBoxOptions)0);
						viewNameBox->Text=m_oldName;
					}
				}
				m_oldName="";
			}

		 /// <summary>Event handler for checking/unchecking Filter selection checkbox.</summary>
		 /// <param name="sender">Handle to the object invoking the event handler.</param>
		 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void filterSelect_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
			 {
				 TreeResultsDetails* pDetails=(TreeResultsDetails*)m_sim->ViewMgr->GetActiveDetails();
				 pDetails->SetUseSelected(filterSelect->Checked);
				 m_treeView->UpdateVisibility(false,true);
			 }

			 /// <summary>Event handler for Sort order combo box selection change.</summary>
			 /// <param name="sender">Handle to the object invoking the event handler.</param>
			 /// <param name="e">Arguments associated with the event.</param>
private: System::Void comboBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
	//changed value here
	TreeResultsDetails* pDetails = (TreeResultsDetails*)m_sim->ViewMgr->GetActiveDetails();
	pDetails->SetSortMode((RN_SORTMODE)sortModeCBox->SelectedIndex);
	m_treeView->UpdateVisibility(true,true);
}
};
}
 