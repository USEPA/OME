namespace OMEWizard
{
    partial class wizardWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.toolTabs = new System.Windows.Forms.TabControl();
            this.converterPage = new System.Windows.Forms.TabPage();
            this.convQuietCB = new System.Windows.Forms.CheckBox();
            this.label7 = new System.Windows.Forms.Label();
            this.removeParamButton = new System.Windows.Forms.Button();
            this.addParamButton = new System.Windows.Forms.Button();
            this.convButton = new System.Windows.Forms.Button();
            this.paramTable = new System.Windows.Forms.DataGridView();
            this.paramCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.mdlCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.convOutButton = new System.Windows.Forms.Button();
            this.convOutField = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.convInButton = new System.Windows.Forms.Button();
            this.convInField = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.MCBPage = new System.Windows.Forms.TabPage();
            this.mcbQuietCB = new System.Windows.Forms.CheckBox();
            this.label8 = new System.Windows.Forms.Label();
            this.mcbRunButton = new System.Windows.Forms.Button();
            this.mcbClassField = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.mcbOutButton = new System.Windows.Forms.Button();
            this.mcbOutField = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.mcbInButton = new System.Windows.Forms.Button();
            this.mcbInField = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.enginePage = new System.Windows.Forms.TabPage();
            this.reQuietCB = new System.Windows.Forms.CheckBox();
            this.label9 = new System.Windows.Forms.Label();
            this.reRunButton = new System.Windows.Forms.Button();
            this.reInButton = new System.Windows.Forms.Button();
            this.reInField = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.reSolverBox = new System.Windows.Forms.ComboBox();
            this.label10 = new System.Windows.Forms.Label();
            this.reUseReportCB = new System.Windows.Forms.CheckBox();
            this.reReportFileButton = new System.Windows.Forms.Button();
            this.reReportFileField = new System.Windows.Forms.TextBox();
            this.reLogButton = new System.Windows.Forms.Button();
            this.reLogField = new System.Windows.Forms.TextBox();
            this.reLogCB = new System.Windows.Forms.CheckBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.reNoCsvExport = new System.Windows.Forms.RadioButton();
            this.reCSVAllCB = new System.Windows.Forms.RadioButton();
            this.reCSVOneCB = new System.Windows.Forms.RadioButton();
            this.reExportButton = new System.Windows.Forms.Button();
            this.reExportField = new System.Windows.Forms.TextBox();
            this.toolTabs.SuspendLayout();
            this.converterPage.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.paramTable)).BeginInit();
            this.MCBPage.SuspendLayout();
            this.enginePage.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // toolTabs
            // 
            this.toolTabs.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.toolTabs.Controls.Add(this.converterPage);
            this.toolTabs.Controls.Add(this.MCBPage);
            this.toolTabs.Controls.Add(this.enginePage);
            this.toolTabs.Location = new System.Drawing.Point(12, 12);
            this.toolTabs.Name = "toolTabs";
            this.toolTabs.SelectedIndex = 0;
            this.toolTabs.Size = new System.Drawing.Size(559, 362);
            this.toolTabs.TabIndex = 0;
            // 
            // converterPage
            // 
            this.converterPage.Controls.Add(this.convQuietCB);
            this.converterPage.Controls.Add(this.label7);
            this.converterPage.Controls.Add(this.removeParamButton);
            this.converterPage.Controls.Add(this.addParamButton);
            this.converterPage.Controls.Add(this.convButton);
            this.converterPage.Controls.Add(this.paramTable);
            this.converterPage.Controls.Add(this.convOutButton);
            this.converterPage.Controls.Add(this.convOutField);
            this.converterPage.Controls.Add(this.label2);
            this.converterPage.Controls.Add(this.convInButton);
            this.converterPage.Controls.Add(this.convInField);
            this.converterPage.Controls.Add(this.label1);
            this.converterPage.Location = new System.Drawing.Point(4, 22);
            this.converterPage.Name = "converterPage";
            this.converterPage.Padding = new System.Windows.Forms.Padding(3);
            this.converterPage.Size = new System.Drawing.Size(551, 336);
            this.converterPage.TabIndex = 0;
            this.converterPage.Text = "Converter";
            this.converterPage.UseVisualStyleBackColor = true;
            // 
            // convQuietCB
            // 
            this.convQuietCB.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.convQuietCB.AutoSize = true;
            this.convQuietCB.Location = new System.Drawing.Point(5, 311);
            this.convQuietCB.Name = "convQuietCB";
            this.convQuietCB.Size = new System.Drawing.Size(81, 17);
            this.convQuietCB.TabIndex = 11;
            this.convQuietCB.Text = "Run Quietly";
            this.convQuietCB.UseVisualStyleBackColor = true;
            // 
            // label7
            // 
            this.label7.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label7.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label7.Location = new System.Drawing.Point(5, 301);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(541, 2);
            this.label7.TabIndex = 10;
            // 
            // removeParamButton
            // 
            this.removeParamButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.removeParamButton.Enabled = false;
            this.removeParamButton.Location = new System.Drawing.Point(89, 275);
            this.removeParamButton.Name = "removeParamButton";
            this.removeParamButton.Size = new System.Drawing.Size(75, 23);
            this.removeParamButton.TabIndex = 9;
            this.removeParamButton.Text = "Remove";
            this.removeParamButton.UseVisualStyleBackColor = true;
            this.removeParamButton.Click += new System.EventHandler(this.removeParamButton_Click);
            // 
            // addParamButton
            // 
            this.addParamButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.addParamButton.Location = new System.Drawing.Point(7, 275);
            this.addParamButton.Name = "addParamButton";
            this.addParamButton.Size = new System.Drawing.Size(75, 23);
            this.addParamButton.TabIndex = 8;
            this.addParamButton.Text = "Add";
            this.addParamButton.UseVisualStyleBackColor = true;
            this.addParamButton.Click += new System.EventHandler(this.addParamButton_Click);
            // 
            // convButton
            // 
            this.convButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.convButton.Location = new System.Drawing.Point(470, 307);
            this.convButton.Name = "convButton";
            this.convButton.Size = new System.Drawing.Size(75, 23);
            this.convButton.TabIndex = 7;
            this.convButton.Text = "Convert";
            this.convButton.UseVisualStyleBackColor = true;
            this.convButton.Click += new System.EventHandler(this.convButton_Click);
            // 
            // paramTable
            // 
            this.paramTable.AllowUserToAddRows = false;
            this.paramTable.AllowUserToDeleteRows = false;
            this.paramTable.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.paramTable.BackgroundColor = System.Drawing.SystemColors.Window;
            this.paramTable.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.paramTable.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.paramCol,
            this.mdlCol});
            this.paramTable.Location = new System.Drawing.Point(6, 84);
            this.paramTable.Name = "paramTable";
            this.paramTable.RowHeadersVisible = false;
            this.paramTable.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.paramTable.Size = new System.Drawing.Size(539, 184);
            this.paramTable.TabIndex = 6;
            this.paramTable.SelectionChanged += new System.EventHandler(this.paramTable_SelectionChanged);
            // 
            // paramCol
            // 
            this.paramCol.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.paramCol.DataPropertyName = "Item1";
            this.paramCol.HeaderText = "Parameter File Path";
            this.paramCol.Name = "paramCol";
            // 
            // mdlCol
            // 
            this.mdlCol.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.mdlCol.DataPropertyName = "Item2";
            this.mdlCol.HeaderText = "Model Name";
            this.mdlCol.Name = "mdlCol";
            // 
            // convOutButton
            // 
            this.convOutButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.convOutButton.Location = new System.Drawing.Point(470, 56);
            this.convOutButton.Name = "convOutButton";
            this.convOutButton.Size = new System.Drawing.Size(75, 23);
            this.convOutButton.TabIndex = 5;
            this.convOutButton.Text = "Browse";
            this.convOutButton.UseVisualStyleBackColor = true;
            this.convOutButton.Click += new System.EventHandler(this.convOutButton_Click);
            // 
            // convOutField
            // 
            this.convOutField.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.convOutField.Location = new System.Drawing.Point(6, 58);
            this.convOutField.Name = "convOutField";
            this.convOutField.Size = new System.Drawing.Size(458, 20);
            this.convOutField.TabIndex = 4;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 42);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(93, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "Output Model File:";
            // 
            // convInButton
            // 
            this.convInButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.convInButton.Location = new System.Drawing.Point(470, 17);
            this.convInButton.Name = "convInButton";
            this.convInButton.Size = new System.Drawing.Size(75, 23);
            this.convInButton.TabIndex = 2;
            this.convInButton.Text = "Browse";
            this.convInButton.UseVisualStyleBackColor = true;
            this.convInButton.Click += new System.EventHandler(this.convInButton_Click);
            // 
            // convInField
            // 
            this.convInField.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.convInField.Location = new System.Drawing.Point(6, 19);
            this.convInField.Name = "convInField";
            this.convInField.Size = new System.Drawing.Size(458, 20);
            this.convInField.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 3);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(85, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Input Model File:";
            // 
            // MCBPage
            // 
            this.MCBPage.Controls.Add(this.mcbQuietCB);
            this.MCBPage.Controls.Add(this.label8);
            this.MCBPage.Controls.Add(this.mcbRunButton);
            this.MCBPage.Controls.Add(this.mcbClassField);
            this.MCBPage.Controls.Add(this.label5);
            this.MCBPage.Controls.Add(this.mcbOutButton);
            this.MCBPage.Controls.Add(this.mcbOutField);
            this.MCBPage.Controls.Add(this.label4);
            this.MCBPage.Controls.Add(this.mcbInButton);
            this.MCBPage.Controls.Add(this.mcbInField);
            this.MCBPage.Controls.Add(this.label3);
            this.MCBPage.Location = new System.Drawing.Point(4, 22);
            this.MCBPage.Name = "MCBPage";
            this.MCBPage.Padding = new System.Windows.Forms.Padding(3);
            this.MCBPage.Size = new System.Drawing.Size(551, 336);
            this.MCBPage.TabIndex = 1;
            this.MCBPage.Text = "Model Class Builder";
            this.MCBPage.UseVisualStyleBackColor = true;
            // 
            // mcbQuietCB
            // 
            this.mcbQuietCB.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.mcbQuietCB.AutoSize = true;
            this.mcbQuietCB.Location = new System.Drawing.Point(5, 311);
            this.mcbQuietCB.Name = "mcbQuietCB";
            this.mcbQuietCB.Size = new System.Drawing.Size(81, 17);
            this.mcbQuietCB.TabIndex = 13;
            this.mcbQuietCB.Text = "Run Quietly";
            this.mcbQuietCB.UseVisualStyleBackColor = true;
            // 
            // label8
            // 
            this.label8.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label8.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label8.Location = new System.Drawing.Point(5, 301);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(541, 2);
            this.label8.TabIndex = 12;
            // 
            // mcbRunButton
            // 
            this.mcbRunButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.mcbRunButton.Location = new System.Drawing.Point(396, 307);
            this.mcbRunButton.Name = "mcbRunButton";
            this.mcbRunButton.Size = new System.Drawing.Size(148, 23);
            this.mcbRunButton.TabIndex = 11;
            this.mcbRunButton.Text = "Generate Source Code";
            this.mcbRunButton.UseVisualStyleBackColor = true;
            this.mcbRunButton.Click += new System.EventHandler(this.mcbRunButton_Click);
            // 
            // mcbClassField
            // 
            this.mcbClassField.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mcbClassField.Location = new System.Drawing.Point(6, 97);
            this.mcbClassField.Name = "mcbClassField";
            this.mcbClassField.Size = new System.Drawing.Size(458, 20);
            this.mcbClassField.TabIndex = 10;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(6, 81);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(238, 13);
            this.label5.TabIndex = 9;
            this.label5.Text = "Class name (leave blank for default model name):";
            // 
            // mcbOutButton
            // 
            this.mcbOutButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.mcbOutButton.Location = new System.Drawing.Point(470, 56);
            this.mcbOutButton.Name = "mcbOutButton";
            this.mcbOutButton.Size = new System.Drawing.Size(75, 23);
            this.mcbOutButton.TabIndex = 8;
            this.mcbOutButton.Text = "Browse";
            this.mcbOutButton.UseVisualStyleBackColor = true;
            this.mcbOutButton.Click += new System.EventHandler(this.mcbOutButton_Click);
            // 
            // mcbOutField
            // 
            this.mcbOutField.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mcbOutField.Location = new System.Drawing.Point(6, 58);
            this.mcbOutField.Name = "mcbOutField";
            this.mcbOutField.Size = new System.Drawing.Size(458, 20);
            this.mcbOutField.TabIndex = 7;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(6, 42);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(249, 13);
            this.label4.TabIndex = 6;
            this.label4.Text = "Output Directory (leave blank for working directory):";
            // 
            // mcbInButton
            // 
            this.mcbInButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.mcbInButton.Location = new System.Drawing.Point(470, 17);
            this.mcbInButton.Name = "mcbInButton";
            this.mcbInButton.Size = new System.Drawing.Size(75, 23);
            this.mcbInButton.TabIndex = 5;
            this.mcbInButton.Text = "Browse";
            this.mcbInButton.UseVisualStyleBackColor = true;
            this.mcbInButton.Click += new System.EventHandler(this.mcbInButton_Click);
            // 
            // mcbInField
            // 
            this.mcbInField.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mcbInField.Location = new System.Drawing.Point(5, 19);
            this.mcbInField.Name = "mcbInField";
            this.mcbInField.Size = new System.Drawing.Size(459, 20);
            this.mcbInField.TabIndex = 4;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(6, 3);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(85, 13);
            this.label3.TabIndex = 3;
            this.label3.Text = "Input Model File:";
            // 
            // enginePage
            // 
            this.enginePage.Controls.Add(this.groupBox1);
            this.enginePage.Controls.Add(this.reLogButton);
            this.enginePage.Controls.Add(this.reLogField);
            this.enginePage.Controls.Add(this.reLogCB);
            this.enginePage.Controls.Add(this.reReportFileButton);
            this.enginePage.Controls.Add(this.reReportFileField);
            this.enginePage.Controls.Add(this.reUseReportCB);
            this.enginePage.Controls.Add(this.label10);
            this.enginePage.Controls.Add(this.reSolverBox);
            this.enginePage.Controls.Add(this.reQuietCB);
            this.enginePage.Controls.Add(this.label9);
            this.enginePage.Controls.Add(this.reRunButton);
            this.enginePage.Controls.Add(this.reInButton);
            this.enginePage.Controls.Add(this.reInField);
            this.enginePage.Controls.Add(this.label6);
            this.enginePage.Location = new System.Drawing.Point(4, 22);
            this.enginePage.Name = "enginePage";
            this.enginePage.Padding = new System.Windows.Forms.Padding(3);
            this.enginePage.Size = new System.Drawing.Size(551, 336);
            this.enginePage.TabIndex = 2;
            this.enginePage.Text = "OME Run";
            this.enginePage.UseVisualStyleBackColor = true;
            // 
            // reQuietCB
            // 
            this.reQuietCB.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.reQuietCB.AutoSize = true;
            this.reQuietCB.Location = new System.Drawing.Point(5, 311);
            this.reQuietCB.Name = "reQuietCB";
            this.reQuietCB.Size = new System.Drawing.Size(81, 17);
            this.reQuietCB.TabIndex = 14;
            this.reQuietCB.Text = "Run Quietly";
            this.reQuietCB.UseVisualStyleBackColor = true;
            // 
            // label9
            // 
            this.label9.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label9.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label9.Location = new System.Drawing.Point(5, 301);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(541, 2);
            this.label9.TabIndex = 13;
            // 
            // reRunButton
            // 
            this.reRunButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.reRunButton.Location = new System.Drawing.Point(470, 307);
            this.reRunButton.Name = "reRunButton";
            this.reRunButton.Size = new System.Drawing.Size(75, 23);
            this.reRunButton.TabIndex = 9;
            this.reRunButton.Text = "Run";
            this.reRunButton.UseVisualStyleBackColor = true;
            this.reRunButton.Click += new System.EventHandler(this.reRunButton_Click);
            // 
            // reInButton
            // 
            this.reInButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.reInButton.Location = new System.Drawing.Point(470, 17);
            this.reInButton.Name = "reInButton";
            this.reInButton.Size = new System.Drawing.Size(75, 23);
            this.reInButton.TabIndex = 8;
            this.reInButton.Text = "Browse";
            this.reInButton.UseVisualStyleBackColor = true;
            this.reInButton.Click += new System.EventHandler(this.reInButton_Click);
            // 
            // reInField
            // 
            this.reInField.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.reInField.Location = new System.Drawing.Point(6, 19);
            this.reInField.Name = "reInField";
            this.reInField.Size = new System.Drawing.Size(458, 20);
            this.reInField.TabIndex = 7;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(4, 3);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(69, 13);
            this.label6.TabIndex = 6;
            this.label6.Text = "Model to run:";
            // 
            // reSolverBox
            // 
            this.reSolverBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.reSolverBox.FormattingEnabled = true;
            this.reSolverBox.Items.AddRange(new object[] {
            "Default",
            "Euler",
            "RK4"});
            this.reSolverBox.Location = new System.Drawing.Point(52, 45);
            this.reSolverBox.Name = "reSolverBox";
            this.reSolverBox.Size = new System.Drawing.Size(121, 21);
            this.reSolverBox.TabIndex = 15;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(6, 48);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(40, 13);
            this.label10.TabIndex = 16;
            this.label10.Text = "Solver:";
            // 
            // reUseReportCB
            // 
            this.reUseReportCB.AutoSize = true;
            this.reUseReportCB.Location = new System.Drawing.Point(7, 72);
            this.reUseReportCB.Name = "reUseReportCB";
            this.reUseReportCB.Size = new System.Drawing.Size(178, 17);
            this.reUseReportCB.TabIndex = 17;
            this.reUseReportCB.Text = "Only Display Fields Listed in File:";
            this.reUseReportCB.UseVisualStyleBackColor = true;
            this.reUseReportCB.CheckedChanged += new System.EventHandler(this.reUseReportCB_CheckedChanged);
            // 
            // reReportFileButton
            // 
            this.reReportFileButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.reReportFileButton.Enabled = false;
            this.reReportFileButton.Location = new System.Drawing.Point(470, 90);
            this.reReportFileButton.Name = "reReportFileButton";
            this.reReportFileButton.Size = new System.Drawing.Size(75, 23);
            this.reReportFileButton.TabIndex = 19;
            this.reReportFileButton.Text = "Browse";
            this.reReportFileButton.UseVisualStyleBackColor = true;
            this.reReportFileButton.Click += new System.EventHandler(this.reReportFileButton_Click);
            // 
            // reReportFileField
            // 
            this.reReportFileField.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.reReportFileField.Enabled = false;
            this.reReportFileField.Location = new System.Drawing.Point(6, 92);
            this.reReportFileField.Name = "reReportFileField";
            this.reReportFileField.Size = new System.Drawing.Size(458, 20);
            this.reReportFileField.TabIndex = 18;
            // 
            // reLogButton
            // 
            this.reLogButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.reLogButton.Enabled = false;
            this.reLogButton.Location = new System.Drawing.Point(470, 135);
            this.reLogButton.Name = "reLogButton";
            this.reLogButton.Size = new System.Drawing.Size(75, 23);
            this.reLogButton.TabIndex = 22;
            this.reLogButton.Text = "Browse";
            this.reLogButton.UseVisualStyleBackColor = true;
            this.reLogButton.Click += new System.EventHandler(this.reLogButton_Click);
            // 
            // reLogField
            // 
            this.reLogField.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.reLogField.Enabled = false;
            this.reLogField.Location = new System.Drawing.Point(6, 137);
            this.reLogField.Name = "reLogField";
            this.reLogField.Size = new System.Drawing.Size(458, 20);
            this.reLogField.TabIndex = 21;
            // 
            // reLogCB
            // 
            this.reLogCB.AutoSize = true;
            this.reLogCB.Location = new System.Drawing.Point(7, 117);
            this.reLogCB.Name = "reLogCB";
            this.reLogCB.Size = new System.Drawing.Size(132, 17);
            this.reLogCB.TabIndex = 20;
            this.reLogCB.Text = "Write output to log file:";
            this.reLogCB.UseVisualStyleBackColor = true;
            this.reLogCB.CheckedChanged += new System.EventHandler(this.reLogCB_CheckedChanged);
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.reExportButton);
            this.groupBox1.Controls.Add(this.reCSVOneCB);
            this.groupBox1.Controls.Add(this.reExportField);
            this.groupBox1.Controls.Add(this.reCSVAllCB);
            this.groupBox1.Controls.Add(this.reNoCsvExport);
            this.groupBox1.Location = new System.Drawing.Point(7, 163);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(538, 113);
            this.groupBox1.TabIndex = 23;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "CSV export:";
            // 
            // reNoCsvExport
            // 
            this.reNoCsvExport.AutoSize = true;
            this.reNoCsvExport.Checked = true;
            this.reNoCsvExport.Location = new System.Drawing.Point(6, 19);
            this.reNoCsvExport.Name = "reNoCsvExport";
            this.reNoCsvExport.Size = new System.Drawing.Size(51, 17);
            this.reNoCsvExport.TabIndex = 0;
            this.reNoCsvExport.TabStop = true;
            this.reNoCsvExport.Text = "None";
            this.reNoCsvExport.UseVisualStyleBackColor = true;
            this.reNoCsvExport.CheckedChanged += new System.EventHandler(this.reNoCsvExport_CheckedChanged);
            // 
            // reCSVAllCB
            // 
            this.reCSVAllCB.AutoSize = true;
            this.reCSVAllCB.Location = new System.Drawing.Point(6, 65);
            this.reCSVAllCB.Name = "reCSVAllCB";
            this.reCSVAllCB.Size = new System.Drawing.Size(92, 17);
            this.reCSVAllCB.TabIndex = 1;
            this.reCSVAllCB.Text = "All Time Steps";
            this.reCSVAllCB.UseVisualStyleBackColor = true;
            // 
            // reCSVOneCB
            // 
            this.reCSVOneCB.AutoSize = true;
            this.reCSVOneCB.Location = new System.Drawing.Point(6, 42);
            this.reCSVOneCB.Name = "reCSVOneCB";
            this.reCSVOneCB.Size = new System.Drawing.Size(96, 17);
            this.reCSVOneCB.TabIndex = 2;
            this.reCSVOneCB.Text = "Last Time Step";
            this.reCSVOneCB.UseVisualStyleBackColor = true;
            // 
            // reExportButton
            // 
            this.reExportButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.reExportButton.Enabled = false;
            this.reExportButton.Location = new System.Drawing.Point(463, 86);
            this.reExportButton.Name = "reExportButton";
            this.reExportButton.Size = new System.Drawing.Size(75, 23);
            this.reExportButton.TabIndex = 25;
            this.reExportButton.Text = "Browse";
            this.reExportButton.UseVisualStyleBackColor = true;
            this.reExportButton.Click += new System.EventHandler(this.reExportButton_Click);
            // 
            // reExportField
            // 
            this.reExportField.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.reExportField.Enabled = false;
            this.reExportField.Location = new System.Drawing.Point(2, 88);
            this.reExportField.Name = "reExportField";
            this.reExportField.Size = new System.Drawing.Size(455, 20);
            this.reExportField.TabIndex = 24;
            // 
            // wizardWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(583, 385);
            this.Controls.Add(this.toolTabs);
            this.MinimumSize = new System.Drawing.Size(330, 400);
            this.Name = "wizardWindow";
            this.Text = "OME Wizard";
            this.toolTabs.ResumeLayout(false);
            this.converterPage.ResumeLayout(false);
            this.converterPage.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.paramTable)).EndInit();
            this.MCBPage.ResumeLayout(false);
            this.MCBPage.PerformLayout();
            this.enginePage.ResumeLayout(false);
            this.enginePage.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl toolTabs;
        private System.Windows.Forms.TabPage converterPage;
        private System.Windows.Forms.TabPage MCBPage;
        private System.Windows.Forms.TabPage enginePage;
        private System.Windows.Forms.Button convOutButton;
        private System.Windows.Forms.TextBox convOutField;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button convInButton;
        private System.Windows.Forms.TextBox convInField;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button convButton;
        private System.Windows.Forms.DataGridView paramTable;
        private System.Windows.Forms.Button mcbRunButton;
        private System.Windows.Forms.TextBox mcbClassField;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button mcbOutButton;
        private System.Windows.Forms.TextBox mcbOutField;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button mcbInButton;
        private System.Windows.Forms.TextBox mcbInField;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button reRunButton;
        private System.Windows.Forms.Button reInButton;
        private System.Windows.Forms.TextBox reInField;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button removeParamButton;
        private System.Windows.Forms.Button addParamButton;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.DataGridViewTextBoxColumn paramCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn mdlCol;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.CheckBox convQuietCB;
        private System.Windows.Forms.CheckBox mcbQuietCB;
        private System.Windows.Forms.CheckBox reQuietCB;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.ComboBox reSolverBox;
        private System.Windows.Forms.Button reLogButton;
        private System.Windows.Forms.TextBox reLogField;
        private System.Windows.Forms.CheckBox reLogCB;
        private System.Windows.Forms.Button reReportFileButton;
        private System.Windows.Forms.TextBox reReportFileField;
        private System.Windows.Forms.CheckBox reUseReportCB;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button reExportButton;
        private System.Windows.Forms.RadioButton reCSVOneCB;
        private System.Windows.Forms.TextBox reExportField;
        private System.Windows.Forms.RadioButton reCSVAllCB;
        private System.Windows.Forms.RadioButton reNoCsvExport;
    }
}

