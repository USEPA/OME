#pragma once
#include "SimWrapper.h"
#include "MultiValBrowserView.h"
#include "FieldSelector.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace OMESimRunner {

	/// <summary>
	/// View of preference controls for multi-value views.
	/// </summary>
	public ref class MultiValBrowserPrefs : public System::Windows::Forms::UserControl
	{
	public:
		/// <summary> Detailed constructor.</summary>
		/// <param name="sim">Handle to active SimWrapper.</param>
		/// <param name="mv">Handle to associated MultiValBrowserView affected by controls.</param>
		MultiValBrowserPrefs(SimWrapper^ sim, MultiValBrowserView^ mv)
			:m_sim(sim), m_multiView(mv)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}
		
		/// <summary> Reload associated data from source.</summary>
		System::Void ReloadData()
		{
			MultiValResultsDetails* pDetails = (MultiValResultsDetails*)m_sim->ViewMgr->GetActiveDetails();
			msclr::interop::marshal_context context;
			viewNameBox->Text = context.marshal_as<String^>(pDetails->GetViewName());
			
		}
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MultiValBrowserPrefs()
		{
			if (components)
			{
				delete components;
			}
		}

		/// <summary> Handle to active SimWrapper.</summary>
		SimWrapper^ m_sim;
		/// <summary> Storage for original name (in case of name change).</summary>
		String^ m_oldName;
		/// <summary>Handle to associated MultiValBrowserView.</summary>
		MultiValBrowserView^ m_multiView;

		/// <summary>Editable box containing name.</summary>
	private: System::Windows::Forms::TextBox^  viewNameBox;
			 /// <summary>Label for view field.</summary>
	private: System::Windows::Forms::Label^  label1;
			 /// <summary>Checkbox used to toggle field filtering.</summary>
	private: System::Windows::Forms::CheckBox^  fieldCB;
			 /// <summary>Button that invokes the field selector window.</summary>
	private: System::Windows::Forms::Button^  selectorButton;

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
			this->viewNameBox = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->fieldCB = (gcnew System::Windows::Forms::CheckBox());
			this->selectorButton = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// viewNameBox
			// 
			this->viewNameBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->viewNameBox->Location = System::Drawing::Point(73, 3);
			this->viewNameBox->Name = L"viewNameBox";
			this->viewNameBox->Size = System::Drawing::Size(524, 20);
			this->viewNameBox->TabIndex = 6;
			this->viewNameBox->Enter += gcnew System::EventHandler(this, &MultiValBrowserPrefs::viewNameBox_Enter);
			this->viewNameBox->Leave += gcnew System::EventHandler(this, &MultiValBrowserPrefs::viewNameBox_Leave);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(3, 6);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(64, 13);
			this->label1->TabIndex = 5;
			this->label1->Text = L"View Name:";
			// 
			// fieldCB
			// 
			this->fieldCB->AutoSize = true;
			this->fieldCB->Location = System::Drawing::Point(6, 29);
			this->fieldCB->Name = L"fieldCB";
			this->fieldCB->Size = System::Drawing::Size(144, 17);
			this->fieldCB->TabIndex = 7;
			this->fieldCB->Text = L"Use Only Selected Fields";
			this->fieldCB->UseVisualStyleBackColor = true;
			this->fieldCB->CheckedChanged += gcnew System::EventHandler(this, &MultiValBrowserPrefs::fieldCB_CheckedChanged);
			// 
			// selectorButton
			// 
			this->selectorButton->Location = System::Drawing::Point(6, 52);
			this->selectorButton->Name = L"selectorButton";
			this->selectorButton->Size = System::Drawing::Size(89, 23);
			this->selectorButton->TabIndex = 8;
			this->selectorButton->Text = L"Select Fields...";
			this->selectorButton->UseVisualStyleBackColor = true;
			this->selectorButton->Click += gcnew System::EventHandler(this, &MultiValBrowserPrefs::selectorButton_Click);
			// 
			// MultiValBrowserPrefs
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->selectorButton);
			this->Controls->Add(this->fieldCB);
			this->Controls->Add(this->viewNameBox);
			this->Controls->Add(this->label1);
			this->Name = L"MultiValBrowserPrefs";
			this->Size = System::Drawing::Size(737, 150);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

		/// <summary>Event handler for pre-editing activity in name box.</summary>
		/// <param name="sender">Handle to the object invoking the event handler.</param>
		/// <param name="e">Arguments associated with the event.</param>
	private: System::Void viewNameBox_Enter(System::Object^  sender, System::EventArgs^  e)
	{
		m_oldName = viewNameBox->Text;
	}

			 /// <summary>Event handler for post-editing activity in name box.</summary>
			 /// <param name="sender">Handle to the object invoking the event handler.</param>
			 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void viewNameBox_Leave(System::Object^  sender, System::EventArgs^  e)
	{
		String^ newName = viewNameBox->Text;
		if (m_oldName != newName)
		{
			msclr::interop::marshal_context context;
			if (m_sim->ViewMgr->NameIsUnique(context.marshal_as<STLString>(newName), m_sim->ViewMgr->GetActiveDetails()))
				m_sim->ViewMgr->GetActiveDetails()->SetViewName(context.marshal_as<STLString>(newName));
			else
			{
				DialogResult r6 = MessageBox::Show(this, String::Concat("The name '", newName, "' is already in use by another view."), "Name in use", MessageBoxButtons::OK, MessageBoxIcon::Error, MessageBoxDefaultButton::Button1, (MessageBoxOptions)0);
				viewNameBox->Text = m_oldName;
			}
		}
		m_oldName = "";
	}
			 /// <summary>Event handler for toggles of the field checkbox.</summary>
			 /// <param name="sender">Handle to the object invoking the event handler.</param>
			 /// <param name="e">Arguments associated with the event.</param>
	private: System::Void fieldCB_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		MultiValResultsDetails* pDetails = (MultiValResultsDetails*)m_sim->ViewMgr->GetActiveDetails();
		pDetails->SetUseSelected(fieldCB->Checked);
		m_multiView->UpdateVisibility();
	}

			 /// <summary>Event handler for selector button.</summary>
			 /// <param name="sender">Handle to the object invoking the event handler.</param>
			 /// <param name="e">Arguments associated with the event.</param>
private: System::Void selectorButton_Click(System::Object^  sender, System::EventArgs^  e) {
	FieldSelector^ selectForm = gcnew FieldSelector(m_sim->ViewMgr->GetActiveDetails(), m_sim,
		"Selected Fields:", "Available Fields:");
	msclr::interop::marshal_context context;
	selectForm->Text = String::Concat(context.marshal_as<String^>(m_sim->ViewMgr->GetActiveDetails()->GetViewName()), " - ", selectForm->Text);
	if (selectForm->ShowDialog() == DialogResult::OK)
		m_multiView->RefreshActive();
}
};
}
