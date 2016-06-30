#pragma once
#include "SimWrapper.h"
#include "SimpleTableView.h"
#include "SimpleViewPrefPanel.h"
#include "SimpleTreeView.h"
#include "TreeViewPrefPanel.h"
#include "MultiValBrowserView.h"
#include "MultiValBrowserPrefs.h"
#include "ParamDialog.h"

//#using "OMERuntime.dll"

namespace OMESimRunner {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Root Window for OMESimRunner
	/// </summary>
	public ref class SimWindow : public System::Windows::Forms::Form
	{
	public:
		SimWindow(void)
		{
			m_inPBarDrag = -1;
			m_loadedPath = nullptr;
			InitializeComponent();
			InitResPanels();
			m_listSrc = gcnew BindingSource();
			viewList->DataSource = m_listSrc;
			viewList->AutoGenerateColumns = false;
			UpdateModelSensitiveControls(false);
		}

		/// <summary>Detailed Constructor. </summary>
		/// <param name="mngr">Handle to SimWrapper object that interfaces with SimManager.</param>
		SimWindow(SimWrapper^ mngr)
		{
			m_inPBarDrag = -1;
			m_loadedPath = nullptr;
			InitializeComponent();
			m_sim = mngr;
			InitResPanels();
			m_listSrc = gcnew BindingSource();
			viewList->DataSource = m_listSrc;
			viewList->AutoGenerateColumns = false;

			UpdateModelSensitiveControls(false);


			//open_Click(nullptr,nullptr);

			UpdateBindings();

			m_updateDelegate = gcnew ParamDialogOkDelegate(this, &SimWindow::UpdateModelSensitiveControls);
			//GCHandle delegateGCH = GCHandle::Alloc(m_updateDelegate);
		}

		/// <summary>Prepare Widgets for a simulation run.</summary>
		System::Void PrepRun()
		{
			runBar->Visible = true;
			runButton->Enabled = false;
			PlayBar->Visible = false;
			tEndBox->ReadOnly = true;
		}

		/// <summary>Prepare Widgets for the conclusion of a simulation.</summary>
		System::Void FinishRun()
		{
			runButton->Enabled = true;
			if (!m_sim->UseMinRecord)
			{
				runBar->Visible = false;
				PlayBar->Visible = true;
				PlayBar->Value = (m_sim->StopTime - m_sim->StartTime) / m_sim->Interval;
			}
			else
				runBar->Value = runBar->Maximum;
			tEndBox->ReadOnly = false;
			UpdateModelSensitiveControls(true);
		}

		/// <summary>Handle to progress bar widget which shows simulation progress. </summary>
		property ProgressBar^ RunProgressBar
		{
			ProgressBar^ get() { return runBar; }
		}
		/// <summary>Text box that displays the current time step. </summary>
		property TextBox^ RunTextBox
		{
			TextBox^ get() { return tEndBox; }
		}

		/// <summary>Update Text box with model information.</summary>
		System::Void UpdateLabel()
		{
			ModelInfo->Text = m_sim->GetModelDescription();
		}

		/// <summary>Menu strip for main window.</summary>
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
			 /// <summary>File menu.</summary>
	private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
			 /// <summary>Open .omec file menu item.</summary>
	private: System::Windows::Forms::ToolStripMenuItem^  openControlFileToolStripMenuItem;
			 /// <summary>Save .omec file menu item.</summary>
	private: System::Windows::Forms::ToolStripMenuItem^  saveControlFileToolStripMenuItem;
			 /// <summary>Save As .omec file menu item.</summary>
	private: System::Windows::Forms::ToolStripMenuItem^  saveControlFileAsToolStripMenuItem;
			 /// <summary>Exit file menu item.</summary>
	private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
			 /// <summary>Edit menu.</summary>
	private: System::Windows::Forms::ToolStripMenuItem^  editToolStripMenuItem;
			 /// <summary>Run menu.</summary>
	private: System::Windows::Forms::ToolStripMenuItem^  runToolStripMenuItem;
			 /// <summary>Run menu item.</summary>
	private: System::Windows::Forms::ToolStripMenuItem^  runToolStripMenuItem1;
			 /// <summary>Parameters dialog menu item.</summary>
	private: System::Windows::Forms::ToolStripMenuItem^  overrideParametersToolStripMenuItem;
			 /// <summary>Export current values menu item.</summary>
	private: System::Windows::Forms::ToolStripMenuItem^  exportValuesToolStripMenuItem;
			 /// <summary>Export all values menu item.</summary>
	private: System::Windows::Forms::ToolStripMenuItem^  exportAllValuesToolStripMenuItem;
			 /// <summary>Split that separates playback controls from the rest of the window.</summary>
	private: System::Windows::Forms::SplitContainer^  playbackSplit;
			 /// <summary>Split between results view and selection panel.</summary>
	private: System::Windows::Forms::SplitContainer^  centralSplit;
			 /// <summary>Split between info panel and selection panel.</summary>
	private: System::Windows::Forms::SplitContainer^  selectSplit;
			 /// <summary>Label that displays information about the loaded model.</summary>
	private: System::Windows::Forms::Label^  ModelInfo;
			 /// <summary>Layout for selection buttons.</summary>
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel3;
			 /// <summary>Sub-Layout for selection buttons.</summary>
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
			 /// <summary>Button for adding a results view.</summary>
	private: System::Windows::Forms::Button^  addRButton;
			 /// <summary>Button for removing a results view.</summary>
	private: System::Windows::Forms::Button^  RemoveRButton;
			 /// <summary>ComboBox for selecting the type of results view to add.</summary>
	private: System::Windows::Forms::ComboBox^  resultTypeBox;
			 /// <summary>Split between results view and preference panel.</summary>
	private: System::Windows::Forms::SplitContainer^  resultsSplit;
			 /// <summary>Layout for playback controls.</summary>
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel2;
			 /// <summary>Track bar for selecting specific timesteps.</summary>
	private: System::Windows::Forms::TrackBar^  PlayBar;
			 /// <summary>Text box that holds the presently selected time step.</summary>
	private: System::Windows::Forms::TextBox^  tEndBox;
			 /// <summary>Button which executes simulation run.</summary>
	private: System::Windows::Forms::Button^  runButton;
			 /// <summary>DGV containing List of loaded views.</summary>
	private: System::Windows::Forms::DataGridView^  viewList;
			 /// <summary>DGV column that displays the name of a view.</summary>
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  view;
			 /// <summary>DGV column that displays the type of a view.</summary>
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  viewType;
			 /// <summary>Tracking bar that shows progress of simulation run.</summary>
	private: System::Windows::Forms::ProgressBar^  runBar;
			 /// <summary>Placeholder for results view preferences.</summary>
	private: System::Windows::Forms::Panel^  panel1;
			 /// <summary>Popup that displays during selection of a timestep.</summary>
	private: System::Windows::Forms::ToolTip^  PlayTip;
			 /// <summary>Informational text.</summary>
	private: System::Windows::Forms::Label^  label1;
			 /// <summary>Displays the total number of timesteps.</summary>
	private: System::Windows::Forms::Label^  tEndLabel;
			 /// <summary>Displays start date.</summary>
	private: System::Windows::Forms::Label^  tStartLabel;
			 /// <summary>Menu seperator.</summary>
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
			 /// <summary>Menu seperator.</summary>
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator2;
			 /// <summary>Button that shifts selected view up.</summary>
	private: System::Windows::Forms::Button^  viewUpButton;
			 /// <summary>Button that shifts selected view down.</summary>
	private: System::Windows::Forms::Button^  viewDownButton;

	protected:
		/// <summary>Handle to active SimWrapper.</summary>
		SimWrapper^ m_sim;
		/// <summary>Handle to SimpleTableView object.</summary>
		SimpleTableView^ m_simpleTableView;
		/// <summary>Handle to SimpleTableView preferences object.</summary>
		SimpleViewPrefPanel^ m_simplePrefPanel;
		/// <summary>Handle to SimpleTreeView preferences object.</summary>
		SimpleTreeView^ m_simpleTreeView;
		/// <summary>Handle to SimpleTreeView object.</summary>
		TreeViewPrefPanel^ m_treePrefPanel;
		/// <summary>Handle to MultivalBrowserView object.</summary>
		MultiValBrowserView^ m_multiView;
		/// <summary>Handle to MultivalBrowserView preferences object.</summary>
		MultiValBrowserPrefs^ m_multiPrefPanel;

		/// <summary>Flag indicating if Playbar is being dragged.</summary>
		int m_inPBarDrag;
		/// <summary>Path to loaded .omec file.</summary>
		String^ m_loadedPath;
		/// <summary>Update delegate to call whenever there is a state change.</summary>
		ParamDialogOkDelegate^ m_updateDelegate;

		/// <summary>Data container for viewList DGV.</summary>
		BindingSource^ m_listSrc;
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~SimWindow()
		{
			if (components)
			{
				delete components;
			}
		}

		/// <summary>Enable/disable any widgets that are dependent on a model being loaded.</summary>
		/// <param name="enable">Enable or disable Model sensitive controls.</param>
		System::Void UpdateModelSensitiveControls(bool enable)
		{
			runButton->Enabled = enable;
			runToolStripMenuItem->Enabled = enable;
			saveControlFileToolStripMenuItem->Enabled = enable;
			saveControlFileAsToolStripMenuItem->Enabled = enable;
			exportValuesToolStripMenuItem->Enabled = enable && m_sim->HasRun;
			exportAllValuesToolStripMenuItem->Enabled = enable && m_sim->HasRun && !m_sim->UseMinRecord;
			UpdateLabel();

			if (enable)
			{
				if (m_sim->HasRun)
					runButton->Text = "ReRun";
				else
					runButton->Text = "Run";

				tStartLabel->Text = "Start: " + m_sim->StartTime.ToString();
				tEndLabel->Text = "of " + m_sim->StopTime.ToString();
				//tEndBox->Text = m_sim->StopTime.ToString();

				PlayBar->Minimum = 0;
				PlayBar->Maximum = (m_sim->StopTime - m_sim->StartTime) / m_sim->Interval;
				tEndBox->Text = m_sim->StopTime.ToString();
			}
			else
				viewList->ClearSelection();

			addRButton->Enabled = enable;
			viewUpButton->Enabled = enable;
			viewDownButton->Enabled = enable;
			//reset menus
			resultTypeBox->SelectedIndex = 0;

			UpdateBindings();
			Refresh();
		}

		/// <summary>Reload Result view bindings.</summary>
		System::Void UpdateBindings()
		{
			m_listSrc->Clear();
			for (size_t i = 0; i < m_sim->ViewMgr->GetViewCount(); i++)
				m_listSrc->Add(gcnew ViewWrapper(m_sim->ViewMgr->GetDetails(i)));
		}

		/// <summary>Initialize the various results views panels.</summary>
		System::Void InitResPanels()
		{
			m_simpleTableView = gcnew SimpleTableView(m_sim);
			m_simplePrefPanel = gcnew SimpleViewPrefPanel(m_sim, m_simpleTableView);
			m_simpleTreeView = gcnew SimpleTreeView(m_sim);
			m_treePrefPanel = gcnew TreeViewPrefPanel(m_sim, m_simpleTreeView);
			m_multiView = gcnew MultiValBrowserView(m_sim);
			m_multiPrefPanel = gcnew MultiValBrowserPrefs(m_sim, m_multiView);

			m_simpleTableView->Dock = DockStyle::Fill;
			m_simpleTreeView->Dock = DockStyle::Fill;
			m_multiView->Dock = DockStyle::Fill;
		}

		/// <summary>Activate and display the selected results view.</summary>
		System::Void LoadResultsView()
		{
			LoadResultsView(m_sim->ViewMgr->GetActiveDetails());
		}

		/// <summary>Activate and display the specified results view.</summary>
		/// <param name="pRD">Pointer to the BaseResultsDetails that provides a discription of the results view to load.</param>
		System::Void LoadResultsView(BaseResultsDetails* pRD)
		{
			resultsSplit->Panel1->Controls->Clear();
			resultsSplit->Panel2->Controls->Clear();
			if (pRD != NULL)
			{
				/// @todo create baseclass/interface/protocol for prefpanes and result views, use with common funcs.
				switch (pRD->GetRVType())
				{
				case RVT_TABLE:
					resultsSplit->Panel1->Controls->Add(m_simplePrefPanel);
					m_simplePrefPanel->ReloadData();
					resultsSplit->Panel2->Controls->Add(m_simpleTableView);
					m_simpleTableView->ReloadData();
					break;
				case RVT_TREE:
					resultsSplit->Panel1->Controls->Add(m_treePrefPanel);
					m_treePrefPanel->ReloadData();
					resultsSplit->Panel2->Controls->Add(m_simpleTreeView);
					m_simpleTreeView->ReloadData();
					m_simpleTreeView->UpdateVisibility(true, true);
					break;
				case RVT_MULTVAL:
					resultsSplit->Panel1->Controls->Add(m_multiPrefPanel);
					m_multiPrefPanel->ReloadData();
					resultsSplit->Panel2->Controls->Add(m_multiView);
					m_multiView->ReloadData();
					break;
				}
			}
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
	private: System::ComponentModel::IContainer^  components;

#pragma region Windows Form Designer generated code
			 /// <summary>
			 /// Required method for Designer support - do not modify
			 /// the contents of this method with the code editor.
			 /// </summary>
			 void InitializeComponent(void)
			 {
				 this->components = (gcnew System::ComponentModel::Container());
				 this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
				 this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				 this->openControlFileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				 this->saveControlFileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				 this->saveControlFileAsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				 this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
				 this->exportValuesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				 this->exportAllValuesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				 this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
				 this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				 this->editToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				 this->runToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				 this->runToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
				 this->overrideParametersToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				 this->playbackSplit = (gcnew System::Windows::Forms::SplitContainer());
				 this->centralSplit = (gcnew System::Windows::Forms::SplitContainer());
				 this->selectSplit = (gcnew System::Windows::Forms::SplitContainer());
				 this->ModelInfo = (gcnew System::Windows::Forms::Label());
				 this->tableLayoutPanel3 = (gcnew System::Windows::Forms::TableLayoutPanel());
				 this->label1 = (gcnew System::Windows::Forms::Label());
				 this->resultTypeBox = (gcnew System::Windows::Forms::ComboBox());
				 this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
				 this->addRButton = (gcnew System::Windows::Forms::Button());
				 this->RemoveRButton = (gcnew System::Windows::Forms::Button());
				 this->viewUpButton = (gcnew System::Windows::Forms::Button());
				 this->viewDownButton = (gcnew System::Windows::Forms::Button());
				 this->viewList = (gcnew System::Windows::Forms::DataGridView());
				 this->view = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
				 this->viewType = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
				 this->resultsSplit = (gcnew System::Windows::Forms::SplitContainer());
				 this->tableLayoutPanel2 = (gcnew System::Windows::Forms::TableLayoutPanel());
				 this->tEndBox = (gcnew System::Windows::Forms::TextBox());
				 this->panel1 = (gcnew System::Windows::Forms::Panel());
				 this->PlayBar = (gcnew System::Windows::Forms::TrackBar());
				 this->runBar = (gcnew System::Windows::Forms::ProgressBar());
				 this->tEndLabel = (gcnew System::Windows::Forms::Label());
				 this->tStartLabel = (gcnew System::Windows::Forms::Label());
				 this->runButton = (gcnew System::Windows::Forms::Button());
				 this->PlayTip = (gcnew System::Windows::Forms::ToolTip(this->components));
				 this->menuStrip1->SuspendLayout();
				 (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->playbackSplit))->BeginInit();
				 this->playbackSplit->Panel1->SuspendLayout();
				 this->playbackSplit->Panel2->SuspendLayout();
				 this->playbackSplit->SuspendLayout();
				 (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->centralSplit))->BeginInit();
				 this->centralSplit->Panel1->SuspendLayout();
				 this->centralSplit->Panel2->SuspendLayout();
				 this->centralSplit->SuspendLayout();
				 (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->selectSplit))->BeginInit();
				 this->selectSplit->Panel1->SuspendLayout();
				 this->selectSplit->Panel2->SuspendLayout();
				 this->selectSplit->SuspendLayout();
				 this->tableLayoutPanel3->SuspendLayout();
				 this->tableLayoutPanel1->SuspendLayout();
				 (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->viewList))->BeginInit();
				 (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->resultsSplit))->BeginInit();
				 this->resultsSplit->SuspendLayout();
				 this->tableLayoutPanel2->SuspendLayout();
				 this->panel1->SuspendLayout();
				 (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->PlayBar))->BeginInit();
				 this->SuspendLayout();
				 // 
				 // menuStrip1
				 // 
				 this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
					 this->fileToolStripMenuItem,
						 this->editToolStripMenuItem, this->runToolStripMenuItem
				 });
				 this->menuStrip1->Location = System::Drawing::Point(0, 0);
				 this->menuStrip1->Name = L"menuStrip1";
				 this->menuStrip1->RenderMode = System::Windows::Forms::ToolStripRenderMode::System;
				 this->menuStrip1->Size = System::Drawing::Size(852, 24);
				 this->menuStrip1->TabIndex = 0;
				 this->menuStrip1->Text = L"menuStrip1";
				 // 
				 // fileToolStripMenuItem
				 // 
				 this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(8) {
					 this->openControlFileToolStripMenuItem,
						 this->saveControlFileToolStripMenuItem, this->saveControlFileAsToolStripMenuItem, this->toolStripSeparator1, this->exportValuesToolStripMenuItem,
						 this->exportAllValuesToolStripMenuItem, this->toolStripSeparator2, this->exitToolStripMenuItem
				 });
				 this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
				 this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
				 this->fileToolStripMenuItem->Text = L"File";
				 // 
				 // openControlFileToolStripMenuItem
				 // 
				 this->openControlFileToolStripMenuItem->Name = L"openControlFileToolStripMenuItem";
				 this->openControlFileToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::O));
				 this->openControlFileToolStripMenuItem->Size = System::Drawing::Size(259, 22);
				 this->openControlFileToolStripMenuItem->Text = L"Open Control File...";
				 this->openControlFileToolStripMenuItem->Click += gcnew System::EventHandler(this, &SimWindow::open_Click);
				 // 
				 // saveControlFileToolStripMenuItem
				 // 
				 this->saveControlFileToolStripMenuItem->Name = L"saveControlFileToolStripMenuItem";
				 this->saveControlFileToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::S));
				 this->saveControlFileToolStripMenuItem->Size = System::Drawing::Size(259, 22);
				 this->saveControlFileToolStripMenuItem->Text = L"Save Control File...";
				 this->saveControlFileToolStripMenuItem->Click += gcnew System::EventHandler(this, &SimWindow::save_Click);
				 // 
				 // saveControlFileAsToolStripMenuItem
				 // 
				 this->saveControlFileAsToolStripMenuItem->Name = L"saveControlFileAsToolStripMenuItem";
				 this->saveControlFileAsToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>(((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Shift)
					 | System::Windows::Forms::Keys::S));
				 this->saveControlFileAsToolStripMenuItem->Size = System::Drawing::Size(259, 22);
				 this->saveControlFileAsToolStripMenuItem->Text = L"Save Control File As...";
				 this->saveControlFileAsToolStripMenuItem->Click += gcnew System::EventHandler(this, &SimWindow::saveAs_Click);
				 // 
				 // toolStripSeparator1
				 // 
				 this->toolStripSeparator1->Name = L"toolStripSeparator1";
				 this->toolStripSeparator1->Size = System::Drawing::Size(256, 6);
				 // 
				 // exportValuesToolStripMenuItem
				 // 
				 this->exportValuesToolStripMenuItem->Name = L"exportValuesToolStripMenuItem";
				 this->exportValuesToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::E));
				 this->exportValuesToolStripMenuItem->Size = System::Drawing::Size(259, 22);
				 this->exportValuesToolStripMenuItem->Text = L"Export Current Values...";
				 this->exportValuesToolStripMenuItem->Click += gcnew System::EventHandler(this, &SimWindow::Export_Click);
				 // 
				 // exportAllValuesToolStripMenuItem
				 // 
				 this->exportAllValuesToolStripMenuItem->Name = L"exportAllValuesToolStripMenuItem";
				 this->exportAllValuesToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>(((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Shift)
					 | System::Windows::Forms::Keys::E));
				 this->exportAllValuesToolStripMenuItem->Size = System::Drawing::Size(259, 22);
				 this->exportAllValuesToolStripMenuItem->Text = L"Export All Values...";
				 this->exportAllValuesToolStripMenuItem->Click += gcnew System::EventHandler(this, &SimWindow::Export_all_click);
				 // 
				 // toolStripSeparator2
				 // 
				 this->toolStripSeparator2->Name = L"toolStripSeparator2";
				 this->toolStripSeparator2->Size = System::Drawing::Size(256, 6);
				 // 
				 // exitToolStripMenuItem
				 // 
				 this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
				 this->exitToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Alt | System::Windows::Forms::Keys::F4));
				 this->exitToolStripMenuItem->Size = System::Drawing::Size(259, 22);
				 this->exitToolStripMenuItem->Text = L"Exit";
				 this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &SimWindow::exitToolStripMenuItem_Click);
				 // 
				 // editToolStripMenuItem
				 // 
				 this->editToolStripMenuItem->Name = L"editToolStripMenuItem";
				 this->editToolStripMenuItem->Size = System::Drawing::Size(39, 20);
				 this->editToolStripMenuItem->Text = L"Edit";
				 // 
				 // runToolStripMenuItem
				 // 
				 this->runToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
					 this->runToolStripMenuItem1,
						 this->overrideParametersToolStripMenuItem
				 });
				 this->runToolStripMenuItem->Name = L"runToolStripMenuItem";
				 this->runToolStripMenuItem->Size = System::Drawing::Size(40, 20);
				 this->runToolStripMenuItem->Text = L"Run";
				 // 
				 // runToolStripMenuItem1
				 // 
				 this->runToolStripMenuItem1->Name = L"runToolStripMenuItem1";
				 this->runToolStripMenuItem1->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::R));
				 this->runToolStripMenuItem1->Size = System::Drawing::Size(263, 22);
				 this->runToolStripMenuItem1->Text = L"Run";
				 this->runToolStripMenuItem1->Click += gcnew System::EventHandler(this, &SimWindow::runToolStripMenuItem1_Click);
				 // 
				 // overrideParametersToolStripMenuItem
				 // 
				 this->overrideParametersToolStripMenuItem->Name = L"overrideParametersToolStripMenuItem";
				 this->overrideParametersToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>(((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Shift)
					 | System::Windows::Forms::Keys::P));
				 this->overrideParametersToolStripMenuItem->Size = System::Drawing::Size(263, 22);
				 this->overrideParametersToolStripMenuItem->Text = L"Override Parameters...";
				 this->overrideParametersToolStripMenuItem->Click += gcnew System::EventHandler(this, &SimWindow::overrideParametersToolStripMenuItem_Click);
				 // 
				 // playbackSplit
				 // 
				 this->playbackSplit->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
				 this->playbackSplit->Dock = System::Windows::Forms::DockStyle::Fill;
				 this->playbackSplit->FixedPanel = System::Windows::Forms::FixedPanel::Panel2;
				 this->playbackSplit->Location = System::Drawing::Point(0, 24);
				 this->playbackSplit->Name = L"playbackSplit";
				 this->playbackSplit->Orientation = System::Windows::Forms::Orientation::Horizontal;
				 // 
				 // playbackSplit.Panel1
				 // 
				 this->playbackSplit->Panel1->Controls->Add(this->centralSplit);
				 // 
				 // playbackSplit.Panel2
				 // 
				 this->playbackSplit->Panel2->Controls->Add(this->tableLayoutPanel2);
				 this->playbackSplit->Size = System::Drawing::Size(852, 557);
				 this->playbackSplit->SplitterDistance = 503;
				 this->playbackSplit->TabIndex = 1;
				 // 
				 // centralSplit
				 // 
				 this->centralSplit->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
				 this->centralSplit->Dock = System::Windows::Forms::DockStyle::Fill;
				 this->centralSplit->FixedPanel = System::Windows::Forms::FixedPanel::Panel1;
				 this->centralSplit->Location = System::Drawing::Point(0, 0);
				 this->centralSplit->Name = L"centralSplit";
				 // 
				 // centralSplit.Panel1
				 // 
				 this->centralSplit->Panel1->Controls->Add(this->selectSplit);
				 this->centralSplit->Panel1MinSize = 131;
				 // 
				 // centralSplit.Panel2
				 // 
				 this->centralSplit->Panel2->Controls->Add(this->resultsSplit);
				 this->centralSplit->Size = System::Drawing::Size(852, 503);
				 this->centralSplit->SplitterDistance = 131;
				 this->centralSplit->TabIndex = 0;
				 // 
				 // selectSplit
				 // 
				 this->selectSplit->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
				 this->selectSplit->Dock = System::Windows::Forms::DockStyle::Fill;
				 this->selectSplit->FixedPanel = System::Windows::Forms::FixedPanel::Panel1;
				 this->selectSplit->Location = System::Drawing::Point(0, 0);
				 this->selectSplit->Name = L"selectSplit";
				 this->selectSplit->Orientation = System::Windows::Forms::Orientation::Horizontal;
				 // 
				 // selectSplit.Panel1
				 // 
				 this->selectSplit->Panel1->Controls->Add(this->ModelInfo);
				 // 
				 // selectSplit.Panel2
				 // 
				 this->selectSplit->Panel2->Controls->Add(this->tableLayoutPanel3);
				 this->selectSplit->Size = System::Drawing::Size(131, 503);
				 this->selectSplit->SplitterDistance = 90;
				 this->selectSplit->TabIndex = 0;
				 // 
				 // ModelInfo
				 // 
				 this->ModelInfo->AutoSize = true;
				 this->ModelInfo->Dock = System::Windows::Forms::DockStyle::Fill;
				 this->ModelInfo->Location = System::Drawing::Point(0, 0);
				 this->ModelInfo->Name = L"ModelInfo";
				 this->ModelInfo->Size = System::Drawing::Size(35, 13);
				 this->ModelInfo->TabIndex = 0;
				 this->ModelInfo->Text = L"label1";
				 // 
				 // tableLayoutPanel3
				 // 
				 this->tableLayoutPanel3->ColumnCount = 1;
				 this->tableLayoutPanel3->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle()));
				 this->tableLayoutPanel3->Controls->Add(this->label1, 0, 0);
				 this->tableLayoutPanel3->Controls->Add(this->resultTypeBox, 0, 1);
				 this->tableLayoutPanel3->Controls->Add(this->tableLayoutPanel1, 0, 2);
				 this->tableLayoutPanel3->Controls->Add(this->viewList, 0, 3);
				 this->tableLayoutPanel3->Dock = System::Windows::Forms::DockStyle::Fill;
				 this->tableLayoutPanel3->Location = System::Drawing::Point(0, 0);
				 this->tableLayoutPanel3->Name = L"tableLayoutPanel3";
				 this->tableLayoutPanel3->RowCount = 4;
				 this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 13)));
				 this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 21)));
				 this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 68)));
				 this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
				 this->tableLayoutPanel3->Size = System::Drawing::Size(129, 407);
				 this->tableLayoutPanel3->TabIndex = 0;
				 // 
				 // label1
				 // 
				 this->label1->AutoSize = true;
				 this->label1->Location = System::Drawing::Point(3, 0);
				 this->label1->Name = L"label1";
				 this->label1->Size = System::Drawing::Size(71, 13);
				 this->label1->TabIndex = 0;
				 this->label1->Text = L"View To Add:";
				 // 
				 // resultTypeBox
				 // 
				 this->resultTypeBox->Dock = System::Windows::Forms::DockStyle::Fill;
				 this->resultTypeBox->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
				 this->resultTypeBox->FormattingEnabled = true;
				 this->resultTypeBox->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"Table View", L"Tree View", L"MultiVal View" });
				 this->resultTypeBox->Location = System::Drawing::Point(3, 16);
				 this->resultTypeBox->Name = L"resultTypeBox";
				 this->resultTypeBox->Size = System::Drawing::Size(126, 21);
				 this->resultTypeBox->TabIndex = 3;
				 // 
				 // tableLayoutPanel1
				 // 
				 this->tableLayoutPanel1->ColumnCount = 2;
				 this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
					 50)));
				 this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
					 50)));
				 this->tableLayoutPanel1->Controls->Add(this->addRButton, 0, 0);
				 this->tableLayoutPanel1->Controls->Add(this->RemoveRButton, 1, 0);
				 this->tableLayoutPanel1->Controls->Add(this->viewUpButton, 0, 1);
				 this->tableLayoutPanel1->Controls->Add(this->viewDownButton, 1, 1);
				 this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
				 this->tableLayoutPanel1->Location = System::Drawing::Point(3, 37);
				 this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
				 this->tableLayoutPanel1->RowCount = 2;
				 this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
				 this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
				 this->tableLayoutPanel1->Size = System::Drawing::Size(126, 62);
				 this->tableLayoutPanel1->TabIndex = 4;
				 // 
				 // addRButton
				 // 
				 this->addRButton->Dock = System::Windows::Forms::DockStyle::Fill;
				 this->addRButton->Location = System::Drawing::Point(3, 3);
				 this->addRButton->Name = L"addRButton";
				 this->addRButton->Size = System::Drawing::Size(57, 25);
				 this->addRButton->TabIndex = 0;
				 this->addRButton->Text = L"Add";
				 this->addRButton->UseVisualStyleBackColor = true;
				 this->addRButton->Click += gcnew System::EventHandler(this, &SimWindow::addRButton_Click);
				 // 
				 // RemoveRButton
				 // 
				 this->RemoveRButton->Dock = System::Windows::Forms::DockStyle::Fill;
				 this->RemoveRButton->Enabled = false;
				 this->RemoveRButton->Location = System::Drawing::Point(66, 3);
				 this->RemoveRButton->Name = L"RemoveRButton";
				 this->RemoveRButton->Size = System::Drawing::Size(57, 25);
				 this->RemoveRButton->TabIndex = 1;
				 this->RemoveRButton->Text = L"Remove";
				 this->RemoveRButton->UseVisualStyleBackColor = true;
				 this->RemoveRButton->Click += gcnew System::EventHandler(this, &SimWindow::RemoveRButton_Click);
				 // 
				 // viewUpButton
				 // 
				 this->viewUpButton->Location = System::Drawing::Point(3, 34);
				 this->viewUpButton->Name = L"viewUpButton";
				 this->viewUpButton->Size = System::Drawing::Size(57, 23);
				 this->viewUpButton->TabIndex = 2;
				 this->viewUpButton->Text = L"Up";
				 this->viewUpButton->UseVisualStyleBackColor = true;
				 this->viewUpButton->Click += gcnew System::EventHandler(this, &SimWindow::ShiftButton_click);
				 // 
				 // viewDownButton
				 // 
				 this->viewDownButton->Location = System::Drawing::Point(66, 34);
				 this->viewDownButton->Name = L"viewDownButton";
				 this->viewDownButton->Size = System::Drawing::Size(57, 23);
				 this->viewDownButton->TabIndex = 3;
				 this->viewDownButton->Text = L"Down";
				 this->viewDownButton->UseVisualStyleBackColor = true;
				 this->viewDownButton->Click += gcnew System::EventHandler(this, &SimWindow::ShiftButton_click);
				 // 
				 // viewList
				 // 
				 this->viewList->AllowUserToAddRows = false;
				 this->viewList->AllowUserToDeleteRows = false;
				 this->viewList->AllowUserToResizeRows = false;
				 this->viewList->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::Fill;
				 this->viewList->BackgroundColor = System::Drawing::Color::White;
				 this->viewList->BorderStyle = System::Windows::Forms::BorderStyle::None;
				 this->viewList->CellBorderStyle = System::Windows::Forms::DataGridViewCellBorderStyle::SingleVertical;
				 this->viewList->ClipboardCopyMode = System::Windows::Forms::DataGridViewClipboardCopyMode::Disable;
				 this->viewList->ColumnHeadersBorderStyle = System::Windows::Forms::DataGridViewHeaderBorderStyle::Single;
				 this->viewList->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
				 this->viewList->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(2) { this->view, this->viewType });
				 this->viewList->Dock = System::Windows::Forms::DockStyle::Fill;
				 this->viewList->Location = System::Drawing::Point(3, 105);
				 this->viewList->MultiSelect = false;
				 this->viewList->Name = L"viewList";
				 this->viewList->ReadOnly = true;
				 this->viewList->RowHeadersVisible = false;
				 this->viewList->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
				 this->viewList->ShowCellErrors = false;
				 this->viewList->ShowCellToolTips = false;
				 this->viewList->ShowEditingIcon = false;
				 this->viewList->ShowRowErrors = false;
				 this->viewList->Size = System::Drawing::Size(126, 333);
				 this->viewList->TabIndex = 5;
				 this->viewList->SelectionChanged += gcnew System::EventHandler(this, &SimWindow::viewList_SelectionChanged);
				 // 
				 // view
				 // 
				 this->view->DataPropertyName = L"ViewName";
				 this->view->HeaderText = L"Name";
				 this->view->Name = L"view";
				 this->view->ReadOnly = true;
				 // 
				 // viewType
				 // 
				 this->viewType->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
				 this->viewType->DataPropertyName = L"ViewTypeStr";
				 this->viewType->HeaderText = L"Type";
				 this->viewType->Name = L"viewType";
				 this->viewType->ReadOnly = true;
				 // 
				 // resultsSplit
				 // 
				 this->resultsSplit->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
				 this->resultsSplit->Dock = System::Windows::Forms::DockStyle::Fill;
				 this->resultsSplit->FixedPanel = System::Windows::Forms::FixedPanel::Panel1;
				 this->resultsSplit->Location = System::Drawing::Point(0, 0);
				 this->resultsSplit->Name = L"resultsSplit";
				 this->resultsSplit->Orientation = System::Windows::Forms::Orientation::Horizontal;
				 this->resultsSplit->Size = System::Drawing::Size(717, 503);
				 this->resultsSplit->SplitterDistance = 109;
				 this->resultsSplit->TabIndex = 0;
				 // 
				 // tableLayoutPanel2
				 // 
				 this->tableLayoutPanel2->ColumnCount = 5;
				 this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
					 75)));
				 this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
					 60)));
				 this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
					 100)));
				 this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
					 50)));
				 this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
					 60)));
				 this->tableLayoutPanel2->Controls->Add(this->tEndBox, 3, 0);
				 this->tableLayoutPanel2->Controls->Add(this->panel1, 2, 0);
				 this->tableLayoutPanel2->Controls->Add(this->tEndLabel, 4, 0);
				 this->tableLayoutPanel2->Controls->Add(this->tStartLabel, 1, 0);
				 this->tableLayoutPanel2->Controls->Add(this->runButton, 0, 0);
				 this->tableLayoutPanel2->Dock = System::Windows::Forms::DockStyle::Fill;
				 this->tableLayoutPanel2->Location = System::Drawing::Point(0, 0);
				 this->tableLayoutPanel2->Name = L"tableLayoutPanel2";
				 this->tableLayoutPanel2->RowCount = 2;
				 this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
				 this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
				 this->tableLayoutPanel2->Size = System::Drawing::Size(850, 48);
				 this->tableLayoutPanel2->TabIndex = 0;
				 // 
				 // tEndBox
				 // 
				 this->tEndBox->Dock = System::Windows::Forms::DockStyle::Fill;
				 this->tEndBox->Location = System::Drawing::Point(743, 13);
				 this->tEndBox->Margin = System::Windows::Forms::Padding(3, 13, 3, 3);
				 this->tEndBox->Name = L"tEndBox";
				 this->tEndBox->Size = System::Drawing::Size(44, 20);
				 this->tEndBox->TabIndex = 10;
				 this->tEndBox->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &SimWindow::tEndBox_KeyUp);
				 this->tEndBox->Leave += gcnew System::EventHandler(this, &SimWindow::tEndBox_TextChanged);
				 // 
				 // panel1
				 // 
				 this->panel1->Controls->Add(this->PlayBar);
				 this->panel1->Controls->Add(this->runBar);
				 this->panel1->Dock = System::Windows::Forms::DockStyle::Fill;
				 this->panel1->Location = System::Drawing::Point(138, 3);
				 this->panel1->Name = L"panel1";
				 this->panel1->Size = System::Drawing::Size(599, 45);
				 this->panel1->TabIndex = 13;
				 // 
				 // PlayBar
				 // 
				 this->PlayBar->Cursor = System::Windows::Forms::Cursors::Arrow;
				 this->PlayBar->Dock = System::Windows::Forms::DockStyle::Fill;
				 this->PlayBar->LargeChange = 1;
				 this->PlayBar->Location = System::Drawing::Point(0, 0);
				 this->PlayBar->Name = L"PlayBar";
				 this->PlayBar->Size = System::Drawing::Size(599, 45);
				 this->PlayBar->TabIndex = 11;
				 this->PlayBar->TickStyle = System::Windows::Forms::TickStyle::Both;
				 this->PlayBar->Visible = false;
				 this->PlayBar->Scroll += gcnew System::EventHandler(this, &SimWindow::PlayBar_Scroll);
				 this->PlayBar->ValueChanged += gcnew System::EventHandler(this, &SimWindow::PlayBar_ValueChanged);
				 this->PlayBar->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &SimWindow::PlayBar_MouseDown);
				 this->PlayBar->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &SimWindow::PlayBar_MouseUp);
				 // 
				 // runBar
				 // 
				 this->runBar->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->runBar->Location = System::Drawing::Point(0, 10);
				 this->runBar->Name = L"runBar";
				 this->runBar->Size = System::Drawing::Size(599, 20);
				 this->runBar->TabIndex = 12;
				 // 
				 // tEndLabel
				 // 
				 this->tEndLabel->AutoSize = true;
				 this->tEndLabel->Dock = System::Windows::Forms::DockStyle::Fill;
				 this->tEndLabel->Location = System::Drawing::Point(793, 0);
				 this->tEndLabel->Name = L"tEndLabel";
				 this->tEndLabel->Size = System::Drawing::Size(54, 51);
				 this->tEndLabel->TabIndex = 14;
				 this->tEndLabel->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
				 // 
				 // tStartLabel
				 // 
				 this->tStartLabel->AutoSize = true;
				 this->tStartLabel->Dock = System::Windows::Forms::DockStyle::Fill;
				 this->tStartLabel->Location = System::Drawing::Point(78, 0);
				 this->tStartLabel->Name = L"tStartLabel";
				 this->tStartLabel->Size = System::Drawing::Size(54, 51);
				 this->tStartLabel->TabIndex = 15;
				 this->tStartLabel->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
				 // 
				 // runButton
				 // 
				 this->runButton->Location = System::Drawing::Point(3, 13);
				 this->runButton->Margin = System::Windows::Forms::Padding(3, 13, 3, 3);
				 this->runButton->Name = L"runButton";
				 this->runButton->Size = System::Drawing::Size(69, 23);
				 this->runButton->TabIndex = 8;
				 this->runButton->Text = L"Run";
				 this->runButton->UseVisualStyleBackColor = true;
				 this->runButton->Click += gcnew System::EventHandler(this, &SimWindow::runButton_Click);
				 // 
				 // PlayTip
				 // 
				 this->PlayTip->AutomaticDelay = 100;
				 // 
				 // SimWindow
				 // 
				 this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				 this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				 this->ClientSize = System::Drawing::Size(852, 581);
				 this->Controls->Add(this->playbackSplit);
				 this->Controls->Add(this->menuStrip1);
				 this->MinimumSize = System::Drawing::Size(100, 250);
				 this->Name = L"SimWindow";
				 this->Text = L"OMERun";
				 this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &SimWindow::SimWindow_FormClosing);
				 this->menuStrip1->ResumeLayout(false);
				 this->menuStrip1->PerformLayout();
				 this->playbackSplit->Panel1->ResumeLayout(false);
				 this->playbackSplit->Panel2->ResumeLayout(false);
				 (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->playbackSplit))->EndInit();
				 this->playbackSplit->ResumeLayout(false);
				 this->centralSplit->Panel1->ResumeLayout(false);
				 this->centralSplit->Panel2->ResumeLayout(false);
				 (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->centralSplit))->EndInit();
				 this->centralSplit->ResumeLayout(false);
				 this->selectSplit->Panel1->ResumeLayout(false);
				 this->selectSplit->Panel1->PerformLayout();
				 this->selectSplit->Panel2->ResumeLayout(false);
				 (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->selectSplit))->EndInit();
				 this->selectSplit->ResumeLayout(false);
				 this->tableLayoutPanel3->ResumeLayout(false);
				 this->tableLayoutPanel3->PerformLayout();
				 this->tableLayoutPanel1->ResumeLayout(false);
				 (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->viewList))->EndInit();
				 (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->resultsSplit))->EndInit();
				 this->resultsSplit->ResumeLayout(false);
				 this->tableLayoutPanel2->ResumeLayout(false);
				 this->tableLayoutPanel2->PerformLayout();
				 this->panel1->ResumeLayout(false);
				 this->panel1->PerformLayout();
				 (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->PlayBar))->EndInit();
				 this->ResumeLayout(false);
				 this->PerformLayout();

			 }
#pragma endregion

			 /// <summary>Event handler for Run button click.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void runButton_Click(System::Object^  sender, System::EventArgs^  e)
	{
				 m_sim->RunSimulation(this);

	}

			 /// <summary>Event handler for Open menu click.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void open_Click(System::Object^  sender, System::EventArgs^  e)
	{
				 OpenFileDialog^ openDialog = gcnew OpenFileDialog;

				 openDialog->InitialDirectory = "c:\\";
				 openDialog->Filter = "OME Control files (*.omec)|*.omec|All files (*.*)|*.*";
				 openDialog->FilterIndex = 1;
				 openDialog->RestoreDirectory = true;
				 openDialog->Multiselect = false;

				 if (openDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
				 {
					 if ((m_loadedPath = openDialog->FileName) != nullptr)
					 {
						 ModelInfo->Text = "Loading Model...";
						 ModelInfo->Refresh();
						 m_sim->LoadControlFile(m_loadedPath);
						 UpdateModelSensitiveControls(true);
					 }
				 }
	}
			 /// <summary>Event handler for Add Results button click.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void addRButton_Click(System::Object^  sender, System::EventArgs^  e)
	{
				 static int suffix = 0;
				 String^ result = resultTypeBox->Text;
				 msclr::interop::marshal_context context;

				 BaseResultsDetails* pView = NULL;
				 if (result == "Table View")
				 {
					 pView = m_sim->ViewMgr->NewRD(RVT_TABLE, context.marshal_as<STLString>("Table_" + (suffix++).ToString()));
				 }
				 else if (result == "Tree View")
				 {
					 pView = m_sim->ViewMgr->NewRD(RVT_TREE, context.marshal_as<STLString>("Tree_" + (suffix++).ToString()));
				 }
				 else if (result == "MultiVal View")
				 {
					 pView = m_sim->ViewMgr->NewRD(RVT_MULTVAL, context.marshal_as<STLString>("MultiVal_" + (suffix++).ToString()));
				 }

				 if (pView)
				 {
					 m_listSrc->Add(gcnew ViewWrapper(pView));
					 viewList->CurrentCell = viewList[0, m_listSrc->Count - 1];
				 }
	}
			 /// <summary>Event handler for Remove Results button click.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void RemoveRButton_Click(System::Object^  sender, System::EventArgs^  e)
	{
				 if (viewList->SelectedRows->Count > 0)
				 {
					 ViewWrapper^ selected = (ViewWrapper^)viewList->SelectedRows[0]->DataBoundItem;
					 m_listSrc->Remove(selected);
					 m_sim->ViewMgr->RemoveRD(selected->RawPtr);
				 }
	}

			 /// <summary>Event handler for viewList DGV selection changed.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void viewList_SelectionChanged(System::Object^  sender, System::EventArgs^  e)
	{
				 bool selected = (viewList->SelectedRows->Count > 0);
				 RemoveRButton->Enabled = selected;
				 viewUpButton->Enabled = selected;
				 viewDownButton->Enabled = selected;

				 if (selected)
				 {
					 if (m_sim->ViewMgr->GetActiveDetails() != ((ViewWrapper^)viewList->SelectedRows[0]->DataBoundItem)->RawPtr)
					 {
						 m_sim->ViewMgr->SetActiveDetails(((ViewWrapper^)viewList->SelectedRows[0]->DataBoundItem)->RawPtr);
						 LoadResultsView();
					 }

				 }
				 else
				 {
					 m_sim->ViewMgr->ClearActiveDetails();
					 LoadResultsView();
				 }
	}

			 /// <summary>Event handler for PlayBar scrolling.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void PlayBar_Scroll(System::Object^  sender, System::EventArgs^  e) {
				 OME_SCALAR currTime = m_sim->StartTime + (PlayBar->Value*m_sim->Interval);
				 PlayTip->SetToolTip(PlayBar, (gcnew System::Double(currTime))->ToString());
	}

			 /// <summary>Event handler for PlayBar value changing.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void PlayBar_ValueChanged(System::Object^  sender, System::EventArgs^  e) {

				 OME_SCALAR newTime = (OME_SCALAR)m_sim->StartTime + (PlayBar->Value*m_sim->Interval);
				 if (m_inPBarDrag < 0 && m_sim->SelectedTime != newTime)
				 {
					 m_sim->SelectedTime = newTime;
					 tEndBox->Text = newTime.ToString();
					 this->Refresh();
				 }
	}
			 /// <summary>Event handler for mouse down over PlayBar.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void PlayBar_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
	{
				 m_inPBarDrag = PlayBar->Value;
	}

			 /// <summary>Event handler for mouse up over PlayBar.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void PlayBar_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
	{
				 if (m_inPBarDrag >= 0 && m_inPBarDrag != PlayBar->Value)
				 {
					 m_inPBarDrag = -1; //needs to be set before value changed.
					 this->PlayBar_ValueChanged(sender, e);
				 }
				 else m_inPBarDrag = -1;
	}

			 /// <summary>Event handler for Save menu click.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void save_Click(System::Object^  sender, System::EventArgs^  e)
	{
				 if (m_loadedPath)
				 {
					 /// \todo check for error
					 m_sim->SaveControlFile(m_loadedPath);
				 }
				 else
					 this->saveAs_Click(sender, e);


	}

			 /// <summary>Event handler for SaveAs menu click.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void saveAs_Click(System::Object^  sender, System::EventArgs^  e)
	{
				 SaveFileDialog^ saveDialog = gcnew SaveFileDialog;

				 saveDialog->InitialDirectory = "c:\\";
				 saveDialog->Filter = "OME Control files (*.omec)|*.omec|All files (*.*)|*.*";
				 saveDialog->FilterIndex = 1;
				 saveDialog->RestoreDirectory = true;

				 if (saveDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
				 {
					 if ((m_loadedPath = saveDialog->FileName) != nullptr)
						 save_Click(sender, e);

				 }
	}

			 /// <summary>Event handler for Export menu click.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void Export_Click(System::Object^  sender, System::EventArgs^  e) {
				 SaveFileDialog^ saveDialog = gcnew SaveFileDialog;

				 msclr::interop::marshal_context context;
				 saveDialog->Title = "Export Current Values";
				 //saveDialog->InitialDirectory = "c:\\";
				 saveDialog->Filter = "Comma-Separated Values (*.csv)|*.csv|All files (*.*)|*.*";
				 saveDialog->FilterIndex = 1;
				 saveDialog->RestoreDirectory = true;

				 saveDialog->FileName = String::Concat(context.marshal_as<String^>(m_sim->RootModel->GetName()), "_", m_sim->SelectedTime.ToString()->Replace(".", "_"));


				 if (saveDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
				 {
					 if ((m_loadedPath = saveDialog->FileName) != nullptr)
					 {
						 Application::UseWaitCursor = true;
						 if (!m_sim->ExportValues(saveDialog->FileName))
						 {
							 //err
						 }
						 Application::UseWaitCursor = false;
					 }
				 }
	}
			 /// <summary>Event handler for Export All menu click.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void Export_all_click(System::Object^  sender, System::EventArgs^  e) {
				 SaveFileDialog^ saveDialog = gcnew SaveFileDialog;

				 msclr::interop::marshal_context context;
				 saveDialog->Title = "Export All Values";
				 //saveDialog->InitialDirectory = "c:\\";
				 saveDialog->Filter = "Comma-Separated Values (*.csv)|*.csv|All files (*.*)|*.*";
				 saveDialog->FilterIndex = 1;
				 saveDialog->RestoreDirectory = true;

				 saveDialog->FileName = String::Concat(context.marshal_as<String^>(m_sim->RootModel->GetName()), "_all");


				 if (saveDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
				 {
					 if ((m_loadedPath = saveDialog->FileName) != nullptr)
					 {
						 Application::UseWaitCursor = true;
						 if (!m_sim->ExportAllValues(saveDialog->FileName))
						 {
							 //err
						 }
						 Application::UseWaitCursor = false;
					 }
				 }
	}

			 /// <summary>Event handler for main window closing.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void SimWindow_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {

				 switch (e->CloseReason)
				 {
				 case CloseReason::ApplicationExitCall:
				 case CloseReason::UserClosing:
				 case CloseReason::WindowsShutDown:
					 if (m_sim->ViewMgr->SaveIsNeeded())
					 {
						 switch (MessageBox::Show("Would you like to save changed views?", "Save Views?", MessageBoxButtons::YesNoCancel))
						 {
						 case ::DialogResult::Yes:
							 save_Click(sender, nullptr);
							 break;
						 case ::DialogResult::Cancel:
							 e->Cancel = true;
							 break;
						 }
					 }
					 break;
				 }
	}

			 /// <summary>Event handler for changing value in tEndBox.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void tEndBox_TextChanged(System::Object^  sender, System::EventArgs^  e) {
				 //jump to specified index, bounded by slider limits.
				 if (!tEndBox->ReadOnly)
				 {
					 if (!tEndBox->Text->Length)
						 tEndBox->Text = PlayBar->Value.ToString();
					 int newTime = System::Double::Parse(tEndBox->Text) / m_sim->Interval;
					 if (newTime != PlayBar->Value)
					 {
						 if (newTime < PlayBar->Minimum)
						 {
							 newTime = PlayBar->Minimum;
							 tEndBox->Text = newTime.ToString();
						 }
						 else if (newTime>PlayBar->Maximum)
						 {
							 newTime = PlayBar->Maximum;
							 tEndBox->Text = newTime.ToString();
						 }

						 PlayBar->Value = newTime;
					 }
				 }
	}

			 /// <summary>Event handler for key-up value in tEndBox.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void tEndBox_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
				 if (e->KeyCode == Keys::Return)
					 tEndBox_TextChanged(sender, e);
	}

			 /// <summary>Event handler for Exit menu click.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void exitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->Close();
	}
			 /// <summary>Event handler for Run menu click.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void runToolStripMenuItem1_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->runButton_Click(sender, e);
	}

			 /// <summary>Event handler for Show parameters dialog menu click.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void overrideParametersToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

				 ParamDialog^ selectForm = gcnew ParamDialog(m_sim, m_updateDelegate);
				 selectForm->Show();

	}

			 /// <summary>Event handler for reposition buttons clicks.</summary>
			 /// <param name="sender">Object that invoked the event handler.</param>
			 /// <param name="e">Arguments that are associated with the event.</param>
	private: System::Void ShiftButton_click(System::Object^  sender, System::EventArgs^  e) {

				 int oldInd = viewList->CurrentRow->Index;
				 int newInd = oldInd + (sender == viewUpButton ? -1 : 2);

				 if (newInd >= 0 && newInd <= viewList->RowCount)
				 {
					 m_listSrc->Insert(newInd, m_listSrc[oldInd]);
					 viewList->CurrentCell = viewList[0, newInd];
					 if (newInd < oldInd)
						 ++oldInd;
					 m_listSrc->RemoveAt(oldInd);
				 }
	}

	};
}

