#pragma once
#include "SimWrapper.h"
#include "FieldSelector.h"
#include "TableResultsDetails.h"
#include "SimpleTableView.h"
#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace OMESimRunner {

	/// <summary>
	/// Base Panel for SimpleTableView preferences
	/// </summary>
	public ref class SimpleViewPrefPanel : public System::Windows::Forms::UserControl
	{
	public:
		/// <summary>Detailed constructor.</summary>
		/// <param name="sim">The active SimWrapper.</param>
		/// <param name="stv">The SimpleTableView to apply settings to.</param>
		SimpleViewPrefPanel(SimWrapper^ sim, SimpleTableView^ stv)
			:m_sim(sim), m_tableView(stv)
		{
			InitializeComponent();

		}

		/// <summary>Reload the source data.</summary>
		System::Void ReloadData()
		{
			BaseResultsDetails* pDetails = m_sim->ViewMgr->GetActiveDetails();
			msclr::interop::marshal_context context;
			viewNameBox->Text = context.marshal_as<String^>(pDetails->GetViewName());

			infoLabel->Visible = pDetails->GetPartialRepCount() == 0;
		}

	protected:
		/// <summary>Handle to active SimWrapper.</summary>
		SimWrapper^ m_sim;
		/// <summary>Handle to SimpleTableView that is influenced by preference choices.</summary>
		SimpleTableView^ m_tableView;
		/// <summary>Handle to old name during name change.</summary>
		String^ m_oldName;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~SimpleViewPrefPanel()
		{
			if (components)
			{
				delete components;
			}
		}
		/// <summary>Label used to provide auxiliary information.</summary>
	private: System::Windows::Forms::Label^  infoLabel;
			 /// <summary>Button used to invoke field selector.</summary>
	private: System::Windows::Forms::Button^  selectorButton;
			 /// <summary>Label of name field.</summary>
	private: System::Windows::Forms::Label^  label2;
			 /// <summary>Box that contains the view's name.</summary>
	private: System::Windows::Forms::TextBox^  viewNameBox;

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
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->viewNameBox = (gcnew System::Windows::Forms::TextBox());
			this->infoLabel = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// selectorButton
			// 
			this->selectorButton->Location = System::Drawing::Point(2, 27);
			this->selectorButton->Name = L"selectorButton";
			this->selectorButton->Size = System::Drawing::Size(93, 23);
			this->selectorButton->TabIndex = 0;
			this->selectorButton->Text = L"Select Fields...";
			this->selectorButton->UseVisualStyleBackColor = true;
			this->selectorButton->Click += gcnew System::EventHandler(this, &SimpleViewPrefPanel::button1_Click);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(3, 6);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(64, 13);
			this->label2->TabIndex = 3;
			this->label2->Text = L"View Name:";
			// 
			// viewNameBox
			// 
			this->viewNameBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->viewNameBox->Location = System::Drawing::Point(73, 3);
			this->viewNameBox->Name = L"viewNameBox";
			this->viewNameBox->Size = System::Drawing::Size(524, 20);
			this->viewNameBox->TabIndex = 4;
			this->viewNameBox->Enter += gcnew System::EventHandler(this, &SimpleViewPrefPanel::viewNameBox_Enter);
			this->viewNameBox->Leave += gcnew System::EventHandler(this, &SimpleViewPrefPanel::viewNameBox_Leave);
			// 
			// infoLabel
			// 
			this->infoLabel->AutoSize = true;
			this->infoLabel->Location = System::Drawing::Point(-1, 67);
			this->infoLabel->Name = L"infoLabel";
			this->infoLabel->Size = System::Drawing::Size(338, 13);
			this->infoLabel->TabIndex = 5;
			this->infoLabel->Text = L"No fields selected. Click on \"Select Fields...\" to choose values to view";
			// 
			// SimpleViewPrefPanel
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->infoLabel);
			this->Controls->Add(this->viewNameBox);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->selectorButton);
			this->Name = L"SimpleViewPrefPanel";
			this->Size = System::Drawing::Size(600, 174);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		/// <summary>Event handler for Field selector button click.</summary>
		/// <param name="sender">Object that invoked the event handler.</param>
		/// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e)
	{
				 FieldSelector^ selectForm = gcnew FieldSelector(m_sim->ViewMgr->GetActiveDetails(), m_sim,
					 "Drag to reorder included fields:", "Select the objects you would like for this table:", true, "Add New", "Remove");
				 msclr::interop::marshal_context context;
				 selectForm->CalcSelectableRepCoords();
				 selectForm->Text = String::Concat(context.marshal_as<String^>(m_sim->ViewMgr->GetActiveDetails()->GetViewName()), " - ", selectForm->Text);
				 selectForm->EnableSFColumn("SFInst");
				 selectForm->DisableSFColumn("SFId");

				 if (selectForm->ShowDialog() == DialogResult::OK)
				 {
					 this->ReloadData();
					 m_tableView->ReloadData();
				 }
	}

			 /// <summary>Event handler for focus on viewNameBox.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void viewNameBox_Enter(System::Object^  sender, System::EventArgs^  e)
	{
				 m_oldName = viewNameBox->Text;
	}

			 /// <summary>Event handler for defocus on viewNameBox.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
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

	};
}
