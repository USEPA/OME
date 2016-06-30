#pragma once
#include "SimWrapper.h"

namespace OMESimRunner {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public delegate void ParamDialogOkDelegate(bool);

	/// <summary>
	/// Dialog for modifying simulation parameters.
	/// </summary>
	public ref class ParamDialog : public System::Windows::Forms::Form
	{
	public:

		/// <summary>Detailed constructor. </summary>
		/// <param name="sim"> Handle to active SimWrapper.</param>
		/// <param name="onOkFunc"> Handle to ok delegate function.</param>
		ParamDialog(SimWrapper^ sim, ParamDialogOkDelegate^ onOkFunc)
		{
			InitializeComponent();

			m_sim = sim;
			m_onOK = onOkFunc;
			LoadRunValues();
			LoadParamValues();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ParamDialog()
		{
			if (components)
			{
				delete components;
			}
		}

		/// <summary> Handle to associated SimWrapper. </summary>
		SimWrapper^ m_sim;
		/// <summary> Handle to function to call when OK button is hit. </summary>
		ParamDialogOkDelegate^ m_onOK;

		/// <summary> Root layout for panel.</summary>
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
			 /// <summary> Layout for OK/Cancel buttons. </summary>
	private: System::Windows::Forms::FlowLayoutPanel^  buttonflow;
			 /// <summary> Dialog cancel button. </summary>
	private: System::Windows::Forms::Button^  cancelButton;
			 /// <summary> Dialog OK button. </summary>
	private: System::Windows::Forms::Button^  okButton;
			 /// <summary> Tabbed view for Run Settings and Parameters. </summary>
	private: System::Windows::Forms::TabControl^  tabLayout;
			 /// <summary> Tab containing simulation run variables. </summary>
	private: System::Windows::Forms::TabPage^  runTab;
			 /// <summary> Tab containing parameter settings. </summary>
	private: System::Windows::Forms::TabPage^  paramTab;
			 /// <summary> Layout for Run Tab. </summary>
	private: System::Windows::Forms::TableLayoutPanel^  runTable;
			 /// <summary> Layout for Labels in run tab. </summary>
	private: System::Windows::Forms::FlowLayoutPanel^  runLabelFlow;
			 /// <summary> Layout for setting widgets in run tab. </summary>
	private: System::Windows::Forms::FlowLayoutPanel^  runValFlow;
			 /// <summary> Integration Widget label. </summary>
	private: System::Windows::Forms::Label^  label1;
			 /// <summary> Start time Widget label. </summary>
	private: System::Windows::Forms::Label^  label2;
			 /// <summary> Stop time Widget label. </summary>
	private: System::Windows::Forms::Label^  label3;
			 /// <summary> Interval Widget label. </summary>
	private: System::Windows::Forms::Label^  label4;
			 /// <summary> Relative Step Widget label. </summary>
	private: System::Windows::Forms::Label^  label5;
			 /// <summary> Integration setting widget. </summary>
	private: System::Windows::Forms::ComboBox^  integrationCombo;
			 /// <summary> Start time setting widget. </summary>
	private: System::Windows::Forms::TextBox^  startTimeBox;
			 /// <summary> Stop time setting widget. </summary>
	private: System::Windows::Forms::TextBox^  stopTimeBox;
			 /// <summary> Interval setting widget. </summary>
	private: System::Windows::Forms::TextBox^  intervalBox;
			 /// <summary> Relative setting widget. </summary>
	private: System::Windows::Forms::TextBox^  relativeBox;
			 /// <summary> Layout for parameter widgets. </summary>
	private: System::Windows::Forms::TableLayoutPanel^  paramTable;
			 /// <summary> Checkbox for using minimal record keeping. </summary>
	private: System::Windows::Forms::CheckBox^  minRecordCB;
			 /// <summary> Solver Widget label. </summary>
	private: System::Windows::Forms::Label^  label6;
			 /// <summary> Solver setting widget. </summary>
	private: System::Windows::Forms::ComboBox^  SolverCombo;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

		/// <summary> Load simulation run values from associated SimWrapper.</summary>
		System::Void LoadRunValues()
		{
			integrationCombo->SelectedIndex = m_sim->IntMethod == OMEIntegrationBlock::IM_RK4 ? 1 : 0;
			startTimeBox->Text = m_sim->StartTime.ToString();
			stopTimeBox->Text = m_sim->StopTime.ToString();
			intervalBox->Text = m_sim->Interval.ToString();
			relativeBox->Text = m_sim->StepSize.ToString();
			minRecordCB->Checked = m_sim->UseMinRecord;
			SolverCombo->SelectedIndex = m_sim->SolverType;
		}

		/// <summary>
		/// Load modifiable parameters to allow modification.
		/// </summary>
		/// <remarks> This function is incomplete, and does not work 
		/// in its current state. There shouldn't be too much more work
		/// to get the function up and running </remarks>
		System::Void LoadParamValues()
		{

			/// @todo finish this function
			/// @todo add initialization to model loading process instead of model run
			EvalArray evals;
			m_sim->RootModel->GetAllEvals(evals);
			msclr::interop::marshal_context context;

			int rowCount=0;
			//count rows
			paramTable->RowCount = rowCount;
			for (auto itr = evals.begin(); itr != evals.end(); ++itr)
			{
				if ((*itr)->GetAskVal())
				{
					Label^ newLabel = gcnew Label();
					newLabel->Text = context.marshal_as<String^>((*itr)->GetName());
					newLabel->AutoSize = true;
					

					
					paramTable->RowCount=+1;
					paramTable->Controls->Add(newLabel, 0, rowCount);
					
					TextBox^ valBox = gcnew TextBox();
					//valBox->Text = (*itr)->GetValue().ToString();
					valBox->Dock = System::Windows::Forms::DockStyle::Fill;
					paramTable->Controls->Add(valBox, 1, rowCount);
					
					++rowCount;
					if ((*itr)->IsOMEType(OME_VARIABLE) && ((Variable*)(*itr))->GetMinMaxLimit() == Variable::HAS_MINMAX)
					{
						TrackBar^ limBar = gcnew TrackBar();
						limBar->Dock = System::Windows::Forms::DockStyle::Fill;
						limBar->TickStyle = System::Windows::Forms::TickStyle::None;
						limBar->Minimum = ((Variable*)(*itr))->GetMin();
						limBar->Maximum = ((Variable*)(*itr))->GetMax();
						paramTable->RowCount += 1;
						paramTable->Controls->Add(limBar, 1, rowCount);
						++rowCount;
					}
				}
			}
		}

		/// <summary> Apply UI simulation run values to associated SimWrapper.</summary>
		System::Void ApplyRunValues()
		{
			m_sim->IntMethod = integrationCombo->SelectedIndex == 1 ? OMEIntegrationBlock::IM_RK4 : OMEIntegrationBlock::IM_EULER;
			m_sim->StartTime = System::Convert::ToDouble(startTimeBox->Text);
			m_sim->StopTime = System::Convert::ToDouble(stopTimeBox->Text);
			m_sim->Interval = System::Convert::ToDouble(intervalBox->Text);
			m_sim->StepSize = System::Convert::ToDouble(relativeBox->Text);
			m_sim->UseMinRecord = minRecordCB->Checked;
			m_sim->SolverType = (SimManager::SOLVER_TYPE)SolverCombo->SelectedIndex;
		}

		/// <summary> Validate user values from parameter input.</summary>
		/// <remarks> Presently unimplemented. Some sort of check should be applied
		/// once LoadParamValues is fully operational.</remarks>
		System::Boolean ValidateValues()
		{
			//fill in
			return true;
		}

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->buttonflow = (gcnew System::Windows::Forms::FlowLayoutPanel());
			this->cancelButton = (gcnew System::Windows::Forms::Button());
			this->okButton = (gcnew System::Windows::Forms::Button());
			this->tabLayout = (gcnew System::Windows::Forms::TabControl());
			this->runTab = (gcnew System::Windows::Forms::TabPage());
			this->runTable = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->runLabelFlow = (gcnew System::Windows::Forms::FlowLayoutPanel());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->runValFlow = (gcnew System::Windows::Forms::FlowLayoutPanel());
			this->integrationCombo = (gcnew System::Windows::Forms::ComboBox());
			this->startTimeBox = (gcnew System::Windows::Forms::TextBox());
			this->stopTimeBox = (gcnew System::Windows::Forms::TextBox());
			this->intervalBox = (gcnew System::Windows::Forms::TextBox());
			this->relativeBox = (gcnew System::Windows::Forms::TextBox());
			this->minRecordCB = (gcnew System::Windows::Forms::CheckBox());
			this->paramTab = (gcnew System::Windows::Forms::TabPage());
			this->paramTable = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->SolverCombo = (gcnew System::Windows::Forms::ComboBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->tableLayoutPanel1->SuspendLayout();
			this->buttonflow->SuspendLayout();
			this->tabLayout->SuspendLayout();
			this->runTab->SuspendLayout();
			this->runTable->SuspendLayout();
			this->runLabelFlow->SuspendLayout();
			this->runValFlow->SuspendLayout();
			this->paramTab->SuspendLayout();
			this->SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->ColumnCount = 1;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
				100)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				20)));
			this->tableLayoutPanel1->Controls->Add(this->buttonflow, 0, 1);
			this->tableLayoutPanel1->Controls->Add(this->tabLayout, 0, 0);
			this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel1->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 2;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 35)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(452, 427);
			this->tableLayoutPanel1->TabIndex = 1;
			// 
			// buttonflow
			// 
			this->buttonflow->Controls->Add(this->cancelButton);
			this->buttonflow->Controls->Add(this->okButton);
			this->buttonflow->Dock = System::Windows::Forms::DockStyle::Fill;
			this->buttonflow->FlowDirection = System::Windows::Forms::FlowDirection::RightToLeft;
			this->buttonflow->Location = System::Drawing::Point(3, 395);
			this->buttonflow->Name = L"buttonflow";
			this->buttonflow->Size = System::Drawing::Size(446, 29);
			this->buttonflow->TabIndex = 0;
			// 
			// cancelButton
			// 
			this->cancelButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->cancelButton->Location = System::Drawing::Point(368, 3);
			this->cancelButton->Name = L"cancelButton";
			this->cancelButton->Size = System::Drawing::Size(75, 23);
			this->cancelButton->TabIndex = 0;
			this->cancelButton->Text = L"Cancel";
			this->cancelButton->UseVisualStyleBackColor = true;
			this->cancelButton->Click += gcnew System::EventHandler(this, &ParamDialog::cancelButton_Click);
			// 
			// okButton
			// 
			this->okButton->Location = System::Drawing::Point(287, 3);
			this->okButton->Name = L"okButton";
			this->okButton->Size = System::Drawing::Size(75, 23);
			this->okButton->TabIndex = 1;
			this->okButton->Text = L"OK";
			this->okButton->UseVisualStyleBackColor = true;
			this->okButton->Click += gcnew System::EventHandler(this, &ParamDialog::okButton_Click);
			// 
			// tabLayout
			// 
			this->tabLayout->Controls->Add(this->runTab);
			this->tabLayout->Controls->Add(this->paramTab);
			this->tabLayout->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabLayout->Location = System::Drawing::Point(3, 3);
			this->tabLayout->Name = L"tabLayout";
			this->tabLayout->SelectedIndex = 0;
			this->tabLayout->Size = System::Drawing::Size(446, 386);
			this->tabLayout->TabIndex = 1;
			// 
			// runTab
			// 
			this->runTab->Controls->Add(this->runTable);
			this->runTab->Location = System::Drawing::Point(4, 22);
			this->runTab->Name = L"runTab";
			this->runTab->Padding = System::Windows::Forms::Padding(3);
			this->runTab->Size = System::Drawing::Size(438, 360);
			this->runTab->TabIndex = 0;
			this->runTab->Text = L"Run Settings";
			this->runTab->UseVisualStyleBackColor = true;
			// 
			// runTable
			// 
			this->runTable->ColumnCount = 2;
			this->runTable->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute, 111)));
			this->runTable->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->runTable->Controls->Add(this->runLabelFlow, 0, 0);
			this->runTable->Controls->Add(this->runValFlow, 1, 0);
			this->runTable->Dock = System::Windows::Forms::DockStyle::Fill;
			this->runTable->Location = System::Drawing::Point(3, 3);
			this->runTable->Name = L"runTable";
			this->runTable->RowCount = 1;
			this->runTable->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->runTable->Size = System::Drawing::Size(432, 354);
			this->runTable->TabIndex = 0;
			// 
			// runLabelFlow
			// 
			this->runLabelFlow->Controls->Add(this->label1);
			this->runLabelFlow->Controls->Add(this->label2);
			this->runLabelFlow->Controls->Add(this->label3);
			this->runLabelFlow->Controls->Add(this->label4);
			this->runLabelFlow->Controls->Add(this->label5);
			this->runLabelFlow->Controls->Add(this->label6);
			this->runLabelFlow->Dock = System::Windows::Forms::DockStyle::Fill;
			this->runLabelFlow->FlowDirection = System::Windows::Forms::FlowDirection::TopDown;
			this->runLabelFlow->Location = System::Drawing::Point(3, 3);
			this->runLabelFlow->Name = L"runLabelFlow";
			this->runLabelFlow->Size = System::Drawing::Size(105, 348);
			this->runLabelFlow->TabIndex = 0;
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(3, 0);
			this->label1->Margin = System::Windows::Forms::Padding(3, 0, 0, 0);
			this->label1->Name = L"label1";
			this->label1->Padding = System::Windows::Forms::Padding(0, 6, 0, 7);
			this->label1->Size = System::Drawing::Size(99, 26);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Integration Method:";
			this->label1->TextAlign = System::Drawing::ContentAlignment::TopRight;
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(3, 26);
			this->label2->Margin = System::Windows::Forms::Padding(3, 0, 0, 0);
			this->label2->Name = L"label2";
			this->label2->Padding = System::Windows::Forms::Padding(0, 6, 0, 7);
			this->label2->Size = System::Drawing::Size(99, 26);
			this->label2->TabIndex = 1;
			this->label2->Text = L"Start Time:";
			this->label2->TextAlign = System::Drawing::ContentAlignment::TopRight;
			// 
			// label3
			// 
			this->label3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(3, 52);
			this->label3->Margin = System::Windows::Forms::Padding(3, 0, 0, 0);
			this->label3->Name = L"label3";
			this->label3->Padding = System::Windows::Forms::Padding(0, 6, 0, 7);
			this->label3->Size = System::Drawing::Size(99, 26);
			this->label3->TabIndex = 2;
			this->label3->Text = L"Stop Time:";
			this->label3->TextAlign = System::Drawing::ContentAlignment::TopRight;
			// 
			// label4
			// 
			this->label4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(3, 78);
			this->label4->Margin = System::Windows::Forms::Padding(3, 0, 0, 0);
			this->label4->Name = L"label4";
			this->label4->Padding = System::Windows::Forms::Padding(0, 6, 0, 7);
			this->label4->Size = System::Drawing::Size(99, 26);
			this->label4->TabIndex = 3;
			this->label4->Text = L"Interval:";
			this->label4->TextAlign = System::Drawing::ContentAlignment::TopRight;
			// 
			// label5
			// 
			this->label5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(3, 104);
			this->label5->Margin = System::Windows::Forms::Padding(3, 0, 0, 0);
			this->label5->Name = L"label5";
			this->label5->Padding = System::Windows::Forms::Padding(0, 6, 0, 7);
			this->label5->Size = System::Drawing::Size(99, 26);
			this->label5->TabIndex = 4;
			this->label5->Text = L"Relative Step:";
			this->label5->TextAlign = System::Drawing::ContentAlignment::TopRight;
			// 
			// runValFlow
			// 
			this->runValFlow->Controls->Add(this->integrationCombo);
			this->runValFlow->Controls->Add(this->startTimeBox);
			this->runValFlow->Controls->Add(this->stopTimeBox);
			this->runValFlow->Controls->Add(this->intervalBox);
			this->runValFlow->Controls->Add(this->relativeBox);
			this->runValFlow->Controls->Add(this->SolverCombo);
			this->runValFlow->Controls->Add(this->minRecordCB);
			this->runValFlow->Dock = System::Windows::Forms::DockStyle::Fill;
			this->runValFlow->FlowDirection = System::Windows::Forms::FlowDirection::TopDown;
			this->runValFlow->Location = System::Drawing::Point(114, 3);
			this->runValFlow->Name = L"runValFlow";
			this->runValFlow->Size = System::Drawing::Size(315, 348);
			this->runValFlow->TabIndex = 1;
			// 
			// integrationCombo
			// 
			this->integrationCombo->FormattingEnabled = true;
			this->integrationCombo->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"Euler", L"RK4" });
			this->integrationCombo->Location = System::Drawing::Point(3, 3);
			this->integrationCombo->Name = L"integrationCombo";
			this->integrationCombo->Size = System::Drawing::Size(121, 21);
			this->integrationCombo->TabIndex = 0;
			// 
			// startTimeBox
			// 
			this->startTimeBox->Location = System::Drawing::Point(3, 30);
			this->startTimeBox->Name = L"startTimeBox";
			this->startTimeBox->Size = System::Drawing::Size(121, 20);
			this->startTimeBox->TabIndex = 1;
			// 
			// stopTimeBox
			// 
			this->stopTimeBox->Location = System::Drawing::Point(3, 56);
			this->stopTimeBox->Name = L"stopTimeBox";
			this->stopTimeBox->Size = System::Drawing::Size(121, 20);
			this->stopTimeBox->TabIndex = 2;
			// 
			// intervalBox
			// 
			this->intervalBox->Location = System::Drawing::Point(3, 82);
			this->intervalBox->Name = L"intervalBox";
			this->intervalBox->Size = System::Drawing::Size(121, 20);
			this->intervalBox->TabIndex = 3;
			// 
			// relativeBox
			// 
			this->relativeBox->Location = System::Drawing::Point(3, 108);
			this->relativeBox->Name = L"relativeBox";
			this->relativeBox->Size = System::Drawing::Size(121, 20);
			this->relativeBox->TabIndex = 4;
			// 
			// minRecordCB
			// 
			this->minRecordCB->AutoSize = true;
			this->minRecordCB->Location = System::Drawing::Point(3, 161);
			this->minRecordCB->Name = L"minRecordCB";
			this->minRecordCB->Size = System::Drawing::Size(163, 17);
			this->minRecordCB->TabIndex = 5;
			this->minRecordCB->Text = L"Use Minimal Record Keeping";
			this->minRecordCB->UseVisualStyleBackColor = true;
			// 
			// paramTab
			// 
			this->paramTab->AutoScroll = true;
			this->paramTab->Controls->Add(this->paramTable);
			this->paramTab->Location = System::Drawing::Point(4, 22);
			this->paramTab->Name = L"paramTab";
			this->paramTab->Padding = System::Windows::Forms::Padding(3);
			this->paramTab->Size = System::Drawing::Size(438, 360);
			this->paramTab->TabIndex = 1;
			this->paramTab->Text = L"Parameters";
			this->paramTab->UseVisualStyleBackColor = true;
			// 
			// paramTable
			// 
			this->paramTable->AutoScroll = true;
			this->paramTable->AutoSize = true;
			this->paramTable->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->paramTable->ColumnCount = 2;
			this->paramTable->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 50)));
			this->paramTable->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 50)));
			this->paramTable->Dock = System::Windows::Forms::DockStyle::Fill;
			this->paramTable->Location = System::Drawing::Point(3, 3);
			this->paramTable->Name = L"paramTable";
			this->paramTable->RowCount = 1;
			this->paramTable->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->paramTable->Size = System::Drawing::Size(432, 354);
			this->paramTable->TabIndex = 0;
			// 
			// SolverCombo
			// 
			this->SolverCombo->FormattingEnabled = true;
			this->SolverCombo->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"Default", L"HiRes" });
			this->SolverCombo->Location = System::Drawing::Point(3, 134);
			this->SolverCombo->Name = L"SolverCombo";
			this->SolverCombo->Size = System::Drawing::Size(121, 21);
			this->SolverCombo->TabIndex = 6;
			// 
			// label6
			// 
			this->label6->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(3, 130);
			this->label6->Margin = System::Windows::Forms::Padding(3, 0, 0, 0);
			this->label6->Name = L"label6";
			this->label6->Padding = System::Windows::Forms::Padding(0, 6, 0, 7);
			this->label6->Size = System::Drawing::Size(99, 26);
			this->label6->TabIndex = 5;
			this->label6->Text = L"Solver:";
			this->label6->TextAlign = System::Drawing::ContentAlignment::TopRight;
			// 
			// ParamDialog
			// 
			this->AcceptButton = this->okButton;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->cancelButton;
			this->ClientSize = System::Drawing::Size(452, 427);
			this->ControlBox = false;
			this->Controls->Add(this->tableLayoutPanel1);
			this->Name = L"ParamDialog";
			this->Text = L"Adjust Parameters";
			this->tableLayoutPanel1->ResumeLayout(false);
			this->buttonflow->ResumeLayout(false);
			this->tabLayout->ResumeLayout(false);
			this->runTab->ResumeLayout(false);
			this->runTable->ResumeLayout(false);
			this->runLabelFlow->ResumeLayout(false);
			this->runLabelFlow->PerformLayout();
			this->runValFlow->ResumeLayout(false);
			this->runValFlow->PerformLayout();
			this->paramTab->ResumeLayout(false);
			this->paramTab->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	
		/// <summary>Event handler for Ok button click.</summary>
		/// <param name="sender">Object that invoked the event handler.</param>
		/// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void okButton_Click(System::Object^  sender, System::EventArgs^  e) {
		if (ValidateValues())
		{
			ApplyRunValues();
			this->Close();
			//call OK function delegate
			m_onOK(true);
		}
	}
			 /// <summary>Event handler for Cancel button click.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void cancelButton_Click(System::Object^  sender, System::EventArgs^  e) {
		this->Close();
	}

};
};
