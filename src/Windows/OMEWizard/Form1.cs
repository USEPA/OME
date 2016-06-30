using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;

namespace OMEWizard
{
    /// <summary>
    /// Root Window for OMEWizard.
    /// </summary>
    public partial class wizardWindow : Form
    {
        #region Multi-tab code
      
        public wizardWindow()
        {
            InitializeComponent();
            //initialize various fields as needed

            paramDataSrc = new BindingSource();
            paramTable.DataSource = paramDataSrc;
            reSolverBox.SelectedIndex = 0;
        }

        /// <summary>
        /// Data source for parameter data.
        /// </summary>
        private BindingSource paramDataSrc;

        /// <summary>
        /// Launch a program using the supplied arguments
        /// </summary>
        /// <param name="name">The name of the program to launch.</param>
        /// <param name="args">The argument string to pass to the program.</param>
        /// <param name="errMsgs">Any configuration errors that exist. If the list is not empty,
        /// The errors will be displayed instead of the program being launched.</param>
        private void LaunchOMEProgram(string name, string args, List<string> errMsgs)
        {
            if (errMsgs.Count == 0)
            {
                //build argument list
                Process exoProc = new Process();
                exoProc.StartInfo.FileName = ".\\" + name + ".exe";
                exoProc.StartInfo.Arguments = args.ToString();
                exoProc.Start();
                exoProc.WaitForExit();
                if (exoProc.ExitCode != 0)
                    MessageBox.Show(name + " reported an unrecoverable error during the conversion process");
            }
            else
            {
                StringBuilder unpacked = new StringBuilder();

                foreach (string err in errMsgs)
                {
                    unpacked.Append("\n   - " + err);
                }
                MessageBox.Show("The following errors must be fixed before conversion can begin:" + unpacked);
            }
        }

        /// <summary>
        /// Ensure that a path is properly constructed for use as an argument.
        /// </summary>
        /// <param name="inStr">The path to modify.</param>
        /// <returns>The modified path.</returns>
        private string PrepPathString(string inStr)
        {
            StringBuilder buff=new StringBuilder();
            buff.Append('"');
            buff.Append(inStr.Replace('\\', '/'));
            buff.Append('"');

            return buff.ToString();
        }

        #endregion
        #region Converter Code

        /// <summary>
        /// Event handler for browsing for converter input files.
        /// </summary>
        /// <param name="sender">The button that sent the event.</param>
        /// <param name="e">The event details.</param>
        private void convInButton_Click(object sender, EventArgs e)
        {
            string path = convInField.Text;

            OpenFileDialog oDlg = new OpenFileDialog();
            oDlg.Filter = "Simile Files (*.sml, *.pl)|*.sml;*.pl|Vensim Files (*.mdl)|*.mdl|XMILE Files (*.xml)|*.xml|All Files (*.*)|*.*";
            oDlg.FilterIndex = 4;
            oDlg.Multiselect = false;
            oDlg.FileName = path;
            oDlg.Title = "Please select a model to convert";

            if(oDlg.ShowDialog() == DialogResult.OK)
            {
                convInField.Text = oDlg.FileName;

                if(convOutField.Text.Length==0)
                {
                    //default to current file name,but with extension
                    convOutField.Text = Path.ChangeExtension(convInField.Text,"omem");

                }
            }

        }

        /// <summary>
        /// Event handler for saving for converter output files.
        /// </summary>
        /// <param name="sender">The button that sent the event.</param>
        /// <param name="e">The event details.</param>
        private void convOutButton_Click(object sender, EventArgs e)
        {
            string path = convOutField.Text;

            SaveFileDialog sDlg = new SaveFileDialog();

            sDlg.Filter = "OME model files (*.omem)|*.omem|All Files (*.*)|*.*";
            sDlg.FilterIndex = 1;
            sDlg.FileName = Path.GetFileName(path);
            sDlg.DefaultExt="omem";

            if(sDlg.ShowDialog()==DialogResult.OK)
            {
                convOutField.Text = sDlg.FileName;
            }
        }

        /// <summary>
        /// Event handler for enabling/disabling the remove button when a selection is
        /// present/absent.
        /// </summary>
        /// <param name="sender">The table that sent the event.</param>
        /// <param name="e">The event details.</param>
        private void paramTable_SelectionChanged(object sender, EventArgs e)
        {
            removeParamButton.Enabled = paramTable.SelectedRows.Count > 0;
        }

        /// <summary>
        /// Event handler for adding a parameter file.
        /// </summary>
        /// <param name="sender">The button that sent the event.</param>
        /// <param name="e">The event details.</param>
        private void addParamButton_Click(object sender, EventArgs e)
        {
            ParamSelector dlg = new ParamSelector();

            if(dlg.ShowDialog()==DialogResult.OK)
            {
                if(dlg.ParamPath.Length>0)
                {
                    paramDataSrc.Add(Tuple.Create<string, string>(dlg.ParamPath, dlg.TargetModel));
                }
            }
        }

        /// <summary>
        /// Event handler for removing a parameter file.
        /// </summary>
        /// <param name="sender">The button that sent the event.</param>
        /// <param name="e">The event details.</param>
        private void removeParamButton_Click(object sender, EventArgs e)
        {
            DataGridViewSelectedRowCollection rows = paramTable.SelectedRows;

            for (int i = 0; i < rows.Count; ++i)
                paramDataSrc.Remove(rows[i].DataBoundItem);
        }

        /// <summary>
        /// Event handler for configuring and running UniversalConverter.
        /// </summary>
        /// <param name="sender">The button that sent the event.</param>
        /// <param name="e">The event details.</param>
        private void convButton_Click(object sender, EventArgs e)
        {
            List<string> errMsgs=new List<string>();
            if (convInField.Text.Length == 0)
                errMsgs.Add("Source model is missing.");

            if (convOutField.Text.Length == 0)
                errMsgs.Add("Destination Model name is missing.");

            //build argument list
            StringBuilder args = new StringBuilder();

            //parameter file mappings
            for(int i=0; i<paramDataSrc.Count; ++i)
            {
                Tuple<string,string> entry=(Tuple<string,string>)paramDataSrc[i];
                args.Append(" -p\"");
                if(entry.Item2.Length>0)
                {
                    args.Append(entry.Item2);
                    args.Append("::");
                }
                args.Append(entry.Item1.Replace('\\', '/'));

                args.Append('"');
            }

            //quiet mode
            if (convQuietCB.Checked)
                args.Append(" -q");

            
            args.Append(' ');
            args.Append(PrepPathString(convInField.Text));
            args.Append(' ');
            args.Append(PrepPathString(convOutField.Text));

            LaunchOMEProgram("UniversalConverter", args.ToString(), errMsgs);
        }
#endregion
        #region ModelClassBuilder Code
        /// <summary>
        /// Event handler for selecting an input model for the ModelClass Builder utility.
        /// </summary>
        /// <param name="sender">The button that sent the event.</param>
        /// <param name="e">The event details.</param>
        private void mcbInButton_Click(object sender, EventArgs e)
        {
            string path = mcbInField.Text;

            OpenFileDialog oDlg = new OpenFileDialog();
            oDlg.Filter = "OME Model Files (*.omem)|*.omem|OME Control Files (*.omec)|*.omec|All OME Files (*.omem, *.omec)|*.omem; *.omec";
            oDlg.FilterIndex = 3;
            oDlg.Multiselect = false;
            oDlg.FileName = path;
            oDlg.Title = "Please select a source model:";

            if (oDlg.ShowDialog() == DialogResult.OK)
            {
                mcbInField.Text = oDlg.FileName;

                if (mcbOutField.Text.Length == 0)
                {
                    mcbOutField.Text = Path.GetDirectoryName(oDlg.FileName);
                }
                
                if(mcbClassField.Text.Length==0)
                {
                    //TODO: use full name scrubbing here
                    mcbClassField.Text = Path.GetFileNameWithoutExtension(oDlg.FileName).Replace(" ","_");
                }
            }
        }

        /// <summary>
        /// Event handler for selecting an output directory for the ModelClass Builder utility.
        /// </summary>
        /// <param name="sender">The button that sent the event.</param>
        /// <param name="e">The event details.</param>
        private void mcbOutButton_Click(object sender, EventArgs e)
        {
            string path = mcbOutField.Text;
            FolderBrowserDialog fDlg = new FolderBrowserDialog();
            fDlg.SelectedPath = path;
            fDlg.ShowNewFolderButton = true;

            if(fDlg.ShowDialog()==DialogResult.OK)
            {
                mcbOutField.Text = fDlg.SelectedPath;
            }
        }

        /// <summary>
        /// Event handler for running the ModelClass Builder utility.
        /// </summary>
        /// <param name="sender">The button that sent the event.</param>
        /// <param name="e">The event details.</param>
        private void mcbRunButton_Click(object sender, EventArgs e)
        {
            List<string> errMsgs = new List<string>();
            if (mcbInField.Text.Length == 0)
                errMsgs.Add("Source model is missing.");

            //build argument list
            StringBuilder args = new StringBuilder();

            if(mcbOutField.Text.Length>0)
                args.Append(" -o" + PrepPathString(mcbOutField.Text));

            if (mcbClassField.Text.Length > 0)
                args.Append(" -c" + mcbClassField.Text);

            if (mcbQuietCB.Checked)
                args.Append(" -q");

            args.Append(' ');
            args.Append(PrepPathString(mcbInField.Text));

            LaunchOMEProgram("ModelClassBuilder", args.ToString(), errMsgs);
        }

        #endregion
        #region OMEEngine Code

        /// <summary>
        /// Event handler for selecting a model to run.
        /// </summary>
        /// <param name="sender">The button that sent the event.</param>
        /// <param name="e">The event details.</param>
        private void reInButton_Click(object sender, EventArgs e)
        {
            string path = reInField.Text;

            OpenFileDialog oDlg = new OpenFileDialog();
            oDlg.Filter = "OME Model Files (*.omem)|*.omem|OME Control Files (*.omec)|*.omec|All OME Files (*.omem, *.omec)|*.omem; *.omec";
            oDlg.FilterIndex = 3;
            oDlg.Multiselect = false;
            oDlg.FileName = path;
            oDlg.Title = "Please select a model to run:";

            if (oDlg.ShowDialog() == DialogResult.OK)
            {
                reInField.Text = oDlg.FileName;
            }
        }

        /// <summary>
        /// Event handler for enabling/disabling fields based on the status of the 
        /// use report checkbox.
        /// </summary>
        /// <param name="sender">The checkbox that sent the event.</param>
        /// <param name="e">The event details.</param>
        private void reUseReportCB_CheckedChanged(object sender, EventArgs e)
        {
            bool enabled = reUseReportCB.Checked;
            reReportFileField.Enabled = enabled;
            reReportFileButton.Enabled = enabled;
        }

        /// <summary>
        /// Event handler for enabling/disabling fields based on the status of the 
        /// use log checkbox.
        /// </summary>
        /// <param name="sender">The checkbox that sent the event.</param>
        /// <param name="e">The event details.</param>
        private void reLogCB_CheckedChanged(object sender, EventArgs e)
        {
            bool enabled = reLogCB.Checked;
            reLogField.Enabled = enabled;
            reLogButton.Enabled = enabled;
        }

        /// <summary>
        /// Event handler for enabling/disabling fields based on the status of the 
        /// No CSV export radio button.
        /// </summary>
        /// <param name="sender">The radio button that sent the event.</param>
        /// <param name="e">The event details.</param>
        private void reNoCsvExport_CheckedChanged(object sender, EventArgs e)
        {
            bool enabled = !reNoCsvExport.Checked;
            reExportField.Enabled = enabled;
            reExportButton.Enabled = enabled;
        }

        /// <summary>
        /// Event handler for browsing for a file listing the fields to display
        /// during a run.
        /// </summary>
        /// <param name="sender">The button that sent the event.</param>
        /// <param name="e">The event details.</param>
        private void reReportFileButton_Click(object sender, EventArgs e)
        {
            string path = reReportFileField.Text;

            OpenFileDialog oDlg = new OpenFileDialog();
            oDlg.Filter = "Text files (*.txt)|*.txt|All Files (*.*)|*.*";
            oDlg.FilterIndex = 0;
            oDlg.Multiselect = false;
            oDlg.FileName = path;
            oDlg.Title = "Please select a report mapping to apply:";

            if (oDlg.ShowDialog() == DialogResult.OK)
            {
                reReportFileField.Text = oDlg.FileName;
            }
        }

        /// <summary>
        /// Event handler for browsing for a file to write to during a run.
        /// </summary>
        /// <param name="sender">The button that sent the event.</param>
        /// <param name="e">The event details.</param>
        private void reLogButton_Click(object sender, EventArgs e)
        {
            string path = reLogField.Text;

            SaveFileDialog sDlg = new SaveFileDialog();

            sDlg.Filter = "Text files (*.txt)|*.txt|All Files (*.*)|*.*";
            sDlg.FilterIndex = 0;
            sDlg.FileName = Path.GetFileName(path);
            sDlg.DefaultExt = "txt";

            if (sDlg.ShowDialog() == DialogResult.OK)
            {
                reLogField.Text = sDlg.FileName;
            }
        }

        /// <summary>
        /// Event handler for browsing for a save location for exported CSV 
        /// values at the conclusion of a simulation run.
        /// </summary>
        /// <param name="sender">The button that sent the event.</param>
        /// <param name="e">The event details.</param>
        private void reExportButton_Click(object sender, EventArgs e)
        {
            string path = reExportField.Text;

            SaveFileDialog sDlg = new SaveFileDialog();

            sDlg.Filter = "csv files (*.csv)|*.csv|All Files (*.*)|*.*";
            sDlg.FilterIndex = 0;
            sDlg.FileName = Path.GetFileName(path);
            sDlg.DefaultExt = "csv";

            if (sDlg.ShowDialog() == DialogResult.OK)
            {
                reExportField.Text = sDlg.FileName;
            }
        }

        /// <summary>
        /// Event handler for running OMEEngine.
        /// </summary>
        /// <param name="sender">The button that sent the event.</param>
        /// <param name="e">The event details.</param>
        private void reRunButton_Click(object sender, EventArgs e)
        {
            List<string> errMsgs = new List<string>();
            if (reInField.Text.Length == 0)
                errMsgs.Add("Source model is missing.");

            //build argument list
            StringBuilder args = new StringBuilder();

            //quiet code
            if (reQuietCB.Checked)
                args.Append(" -q");

            //solver override
            if (reSolverBox.SelectedIndex > 0)
            {
                args.Append(" -s");
                args.Append(reSolverBox.SelectedIndex == 1 ? "euler" : "rk4");
            }

            //optional list of values to report on
            if (reUseReportCB.Checked)
            {
                args.Append(" -r");
                args.Append(PrepPathString(reReportFileField.Text));
            }

            //optional log file
            if (reLogCB.Checked)
            {
                args.Append(" -l");
                args.Append(PrepPathString(reLogField.Text));
            }

            //optional results output
            if(!reNoCsvExport.Checked)
            {
                args.Append(reCSVOneCB.Checked ? " -c" : " -f");
                args.Append(PrepPathString(reExportField.Text));
            }

            //model file
            args.Append(' ');
            args.Append(PrepPathString(reInField.Text));


            LaunchOMEProgram("OMEEngine", args.ToString(), errMsgs);
        }

        #endregion
    }
}
