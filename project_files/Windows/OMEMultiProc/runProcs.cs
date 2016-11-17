using System;
using System.Windows.Forms;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.IO;
using System.Threading;

//Developer: Vlad Predovic
//Description: Used for testing/optimizing the amount of instances of OMEEngine to run on a system at once. 
//Requirements: Omec file in compiled_tests folder, DLL and omem files in test folder,
//Output: csv files in results folder, start and end times in the command prompt. 
//Instructions: Change the amount of files to create using the outer-forloop 
//Change the amount of processes to run at once in the inner while loop 

namespace MultiProc
{



    public static class runProcs
    {

        /** A single profileCollection holds a list of the profiles collected 
         ** which consists of the default values and possible values 
         ** of each variable. list: 'profile_prints'
         */
        public class profileCollection
        {

            //Constructor, pass in the directory
            public profileCollection(string dir_path)
            {
                try
                {
                    profile_prints = new List<modelProfile>();
                    BaseProfileFactory(dir_path);
                }
                catch (Exception e)
                {
                    throw e;
                }
            }


            //! List that will hold the profiles
            public List<modelProfile> profile_prints;


            //! A void member taking in path as string
            //Add the new modelProfile object to the list
            public void BaseProfileFactory(string directory_path)
            {

                string[] file_paths = Directory.GetFiles(directory_path, "profile*");

                //We get each file path, need to open it and parse it, filling in all the values in the modelProfile
                foreach (string each in file_paths)
                {
                    modelProfile prof = new modelProfile();
                    prof.fill_profile(each);
                    profile_prints.Add(prof);
                }


            }
        }

        //Parses the text profiles to creat the different models
        //Parameters: base_name -> base model type
        //plist -> list of all variables and their possible range of setting
        public class modelProfile
        {
            public string base_name;
            private int _proc_cntr;
            public int ProcCounter { get { return _proc_cntr; } }
            public void IncrementProcCounter() { Interlocked.Increment(ref _proc_cntr); }
            public void DecrementProcCounter() { Interlocked.Decrement(ref _proc_cntr); }


            public List<paramLRP> plist = new List<paramLRP>();
            String[] temp_params;

            //This MEGAMAP is used to speed up the process
            //of creating output files from the input files. The dicts are built
            //on a per profile basis to add flexibility in case of differing defaults
            public Dictionary<string, string> name_map = new Dictionary<string, string>();

            //! A void member taking one argument 
            /*!
              \param path String argument to the profile.
            */
            public void fill_profile(string path)
            {
                try
                {   // scraping paramaters form the profile text file
                    using (StreamReader sr = new StreamReader(path))
                    {
                        string line;
                        while ((line = sr.ReadLine()) != null)
                        {
                            if (line[0] != '#')
                            {
                                if (line.Substring(0, 2) == "**")
                                {
                                    base_name = line.Replace("*", "");
                                    Console.WriteLine("Loading base profile: {0}", base_name);
                                }
                                else
                                {
                                    temp_params = line.Replace("\t", String.Empty).Split(',');
                                    paramLRP new_entry = new paramLRP(temp_params, name_map);
                                    plist.Add(new_entry);
                                }
                            }
                        }
                        Console.WriteLine("Load Complete");
                    }
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.GetType().FullName);
                    Console.WriteLine(e.Message);
                    Console.WriteLine("Press any button to exit, program will now terminate");
                    Console.ReadLine();
                    Environment.Exit(1);
                }

            }
        }

        //
        //! Base Class for parameter profiles. 
        /*!
            Used in setting up the structure for your outputs. Variables get populated from profile text file 
        */
        public abstract class paramCreator
        {

            public virtual string ome_name { get; set; }
            public virtual string descriptor { get; set; }
            public virtual float default_val { get; set; }
            protected virtual float min { get; set; }
            protected virtual float max { get; set; }
            protected virtual float intrvl { get; set; }
            public virtual int iter { get; set; }

            //Returns a list of all the values the parameter can take
            //In OME_input -o format (See OMEEngine.cpp PrintHelp())
            //Also inserts naming scheme into the dictionary used for the profile
            //Representation will vary for any implementation, hence abstract
            public abstract List<string> enumVals(Dictionary<string, string> OMEtoLRP_map);

            //Define output file format based on implementation needed in the derived class. 
            //Cannot inherit static members
            //abstract public string outputStr();

        }

        //! LRP model Class. 
        /*!
            Functionality provided is based on output naming schemes and 
         * variables provided by the LRP team.
         \see paramCreator()
        */
        public sealed class paramLRP : paramCreator
        {
            //! A constructor.
            /*!
              Initializes current parameter in the profile with all the info provided.
            */
            public paramLRP(string[] prams, Dictionary<string, string> OMEtoLRP_map)
            {
                try
                {
                    ome_name = prams[0];
                    descriptor = prams[1];
                    default_val = float.Parse(prams[2]);
                    min = float.Parse(prams[3]);
                    max = float.Parse(prams[4]);
                    intrvl = float.Parse(prams[5]);
                    if (intrvl == 0) { intrvl = 1; }
                    iter = int.Parse(prams[6]);
                    if (iter == 0) { iter = 1; }
                }
                catch (IndexOutOfRangeException)
                {
                    Console.WriteLine("Index was out of range, check the commas for variable {0}", ome_name);
                    Console.WriteLine("\n\n Program will now exit, press any button");
                    Console.ReadLine();
                    Environment.Exit(1);
                }
                catch (InvalidCastException)
                {
                    Console.WriteLine("Invalid cast, check the fields in the profile for variable {0}", ome_name);
                    Console.WriteLine("\n\n Program will now exit, press any button");
                    Console.ReadLine();
                    Environment.Exit(1);
                }

            }

            //Returns a list of all the values the parameter can take
            //In OME_input -o format (See OMEEngine.cpp PrintHelp())
            //Also inserts naming scheme into the dictionary used for the profile

            //! A normal member taking a map <string, string> and returning a list <string>.
            /*!
              \param OMEtoLRP_map a map connecting the OME input variable names to the desired LRP output names.
              \return var_vals
              \see paramLRP()
            */
            public override List<string> enumVals(Dictionary<string, string> OMEtoLRP_map)
            {
                List<string> var_vals = new List<string>();
                float varr;
                int deflt_index;
                string temp_name, temp_desc;

                if (iter == 1)
                {
                    deflt_index = (int)(default_val / intrvl - min / intrvl);
                    temp_name = String.Format("-o\"{0}\"={1} ", ome_name, default_val);
                    temp_desc = String.Format("{0}", deflt_index);
                    OMEtoLRP_map.Add(temp_name, temp_desc.Trim()); //When default, not in LRP name.
                    var_vals.Add(temp_name);
                    return var_vals;
                }
                else
                {
                    for (int i = 0; i < iter; i++)
                    {
                        varr = min + intrvl * i;
                        temp_name = String.Format("-o\"{0}\"={1} ", ome_name, varr);
                        temp_desc = String.Format("{0}", i);
                        OMEtoLRP_map.Add(temp_name, temp_desc.Trim());
                        var_vals.Add(temp_name);
                    }
                }
                return var_vals;

            }

            //LRP implementation for file output scheme
            public static string outputStr(string output_path, string model_base, string file_name)
            {
                string shaved_base = file_name.Substring(2);
                string final_outpath = output_path + model_base + "_" + shaved_base.Substring(0, 4);
                Directory.CreateDirectory(final_outpath);
                string result_path = final_outpath + "//" + model_base + "_" + shaved_base + ".csv";
                return result_path;
            }


        }



        //! A top-level function taking in a complete profile and two lists <string>.
        /*!
          \param model_data Object representing the model and all it's possible parameter values.
          \param build_input List<string> List of the OME input overrides written in OME format
          \param build_output List<string> List of LRP output strings.
        */
        public static void nameCreator(modelProfile model_data, List<string> build_input, List<string> build_output)
        {
            //Calculate all combinations, fill in output file names and input string to engine accordingly
            List<paramLRP> pd = model_data.plist;
            List<List<string>> all_combos = new List<List<string>>();
            List<List<string>> combo_result = new List<List<string>>();
           
            //Create the list consisting of list's of values each paramater can take up
            //In OME input format. Additionally hashes the corresponding output var name
            foreach (var entry in pd)
            {
                all_combos.Add(entry.enumVals(model_data.name_map));
            }

            //Now develop each combination and add it to the build inputs and outputs.
            combo_result = AllCombinationsOf(all_combos);

            //Merge each list of strings into the command for OME Engine
            //and the output string for the csv files
            foreach (var string_list in combo_result)
            {
                build_input.Add(Concat(string_list, null));
                build_output.Add(Concat(string_list, model_data.name_map));
            }
        }


        //! A top-level function taking in a List of lists <T> and returning the permuation of these lists.
        /*!
          \param sets A list containing lists of all the parameter combinations available in OME format.
          \param build_input List<string> List of the permuted OME input overrides written in OME format
          \param build_output List<string> List of permuted LRP output strings.
        */
        public static List<List<T>> AllCombinationsOf<T>(List<List<T>> sets)
        {
            // need array bounds checking etc for production
            var combinations = new List<List<T>>();

            // prime the data
            foreach (var value in sets[0])
                combinations.Add(new List<T> { value });

            foreach (var set in sets.Skip(1))
                combinations = AddExtraSet(combinations, set);

            return combinations;
            //Thanks to Garry Shutler for the combinations algorithm, edited for list<list> use:
            //http://stackoverflow.com/questions/545703/combination-of-listlistint
        }



        private static List<List<T>> AddExtraSet<T>(List<List<T>> combinations, List<T> set)
        {
            var newCombinations = from value in set
                                  from combination in combinations
                                  select new List<T>(combination) { value };

            return newCombinations.ToList();
        }

        //Concatenate the OME_Input and corresponding output using the dictionary
        //This keyword extends IEnumerable with 'string' properties
        //! A top-level function taking in a string and a dictionary <string, string>
        /*!
          \param source key LRP-OME connection list to be parsed and fed into map or retured post concatenation.
          \param map_csv <string,string> OME input mapped to LRP output strings.
          \return sb concatenated string to be fed into OME engine as variable overrides or as LRP file names
        */
        public static string Concat(this IEnumerable<string> source, Dictionary<string, string> map_csv)
        {
            StringBuilder sb = new StringBuilder();

            if (map_csv == null)
            {
                foreach (string s in source)
                {
                    sb.Append(s);
                }
                return sb.ToString();
            }

            foreach (string s in source)
            {
                sb.Append(map_csv[s]);
            }
            return sb.ToString();
        }




        /**

         */
        //!  A class for a logfile . 
        /*!
           Interface that is primarily used to identify simulations that may have failed.
           All results that appear in the log file should be checked for errors and re-entered 
           manually into OME Engine if any are found.
        */
        public class Logger
        {

            //! A constructor.
            /*!
              Initializes log file /w time stamp .
            */
            public Logger(string log_dir_path, int init_count)
            {
                alert_time = st_time = DateTime.Now;
                this.start_count = init_count;

                string temp = "log" + DateTime.Now.ToShortTimeString() + ".txt";
                log_file = Path.GetFullPath(Path.Combine(log_dir_path, temp.Replace(":", "_")));
                if (!File.Exists(log_file))
                {
                    // Create a file to write to.
                    using (StreamWriter sw = File.CreateText(log_file))
                    {
                        sw.WriteLine("Log Begin: " + DateTime.Now.ToShortDateString() + "   " + DateTime.Now.ToShortTimeString());
                    }
                }
            }


            int start_count;
            DateTime alert_time;
            DateTime st_time;
            string log_file { get; set; }

            Queue<string> q_log = new Queue<string>();

            //! A void member taking one argument.
            /*!
              \param message string containing OME combination that caused error.
              \sa Logger(), printLog()
            */
            public void appendLog(string message)
            {
                while (true)
                {
                    try
                    {
                        q_log.Enqueue(message + '\n');
                        break;
                    }
                    catch
                    {
                        continue;
                    }
                }
            }

            //! A void member taking three arguments.
            /*!
              \param counter Amount of OMEEngine processes currently running.
              \param queue_size Integer defining error messages in queue.
              \param profile_name The name given in the profile text file.
              \sa Logger(), appendLog()
            */
            public void printLog(int counter, int queue_size, string profile_name)
            {
                if (DateTime.Now.Subtract(alert_time).TotalMinutes > 5)
                {
                    this.alert_time = DateTime.Now;
                    Console.WriteLine("\n\n\n\nThe time is: {0}, I started at: {1}", this.alert_time.ToShortTimeString(), this.st_time.ToShortTimeString());
                    Console.WriteLine("Currently running results for the profile {0}. Next update in 5 minutes.", profile_name);
                    Console.WriteLine("At this moment in time {0} processes are currently running", counter);
                    Console.WriteLine("Your overall rate since the last checkup is {0} files per minute", (this.start_count - queue_size) / 5);
                }
                while (q_log.Count > 0)
                {
                    using (StreamWriter sw = File.AppendText(log_file))
                        sw.WriteLine(q_log.Dequeue());
                }
            }
        }

        //! A top level function taking in 5 arguments.
        /*!
          \param args Input parameters for OMEEngine (see OMEEngine.cpp).
          \param output_fp File path for output data as a csv.
          \param model_base Profile object, used primarily for keeping track of running processes.
          \param log_file Log file in case of an error occuring.
        */
        public static void launchEngines(string args, string output_fp, modelProfile model_base, Logger log_file)
        {
            //Prepare a process to start up OMEengine with the proper filepaths and flags
            Process proc_engine = new Process();
            proc_engine.StartInfo.FileName = "OMEEngine.exe"; ;

            proc_engine.StartInfo.Arguments = args;
            proc_engine.EnableRaisingEvents = true;
            proc_engine.StartInfo.CreateNoWindow = true;
            proc_engine.StartInfo.UseShellExecute = false;
            //proc_engine.StartInfo.RedirectStandardOutput = true;
            //proc_engine.StartInfo.RedirectStandardError = true;

            //Manage the amount of processes running at once to maximize performace
            //Console.Write(process_cntr);
            while (model_base.ProcCounter > Environment.ProcessorCount)
            {
                System.Threading.Thread.Sleep(1000);
            }

            //listener for each process, signals when they exit
            proc_engine.Exited += (sender, EventArgs) =>
            {
                model_base.DecrementProcCounter();


                if (proc_engine.ExitCode != 0)
                {
                    Console.WriteLine("Error occured, check log for details.");
                    //log_file.appendLog(proc_engine.StandardError.ReadToEnd());
                    //log_file.appendLog(proc_engine.StandardOutput.ReadToEnd());
                    log_file.appendLog(Environment.NewLine + "Error occured with the following input: \n\n " + proc_engine.StartInfo.Arguments
                    + Environment.NewLine + Environment.NewLine + "The file in question is: " + output_fp.Split('\\').Last());
                }
                //Source:
                //https://msdn.microsoft.com/en-us/library/system.diagnostics.process.enableraisingevents(v=vs.110).aspx
            };
            //Start up the given subprocess consisting of a call to OMEENGINE
            proc_engine.Start();
            //Console.ReadLine();
        }

        //! A top level function taking in 5 arguments.
        /*!
          \param args Input parameters for OMEEngine (see OMEEngine.cpp).
          \param output_fp File path for output data as a csv.
          \param model_base Profile object, used primarily for keeping track of running processes.
          \param log_file Log file in case of an error occuring.
        */
        //The engine arguments and each base model along with its parameters are passed in
        public static void LaunchOMEMultiProc(string result_Path, string eng_args, modelProfile model)
        {
            string startTime = DateTime.Now.ToString("h:mm:ss tt");
            Console.WriteLine("Time started: {0}\r\n", startTime);
            Queue model_queue = new Queue();
            Console.WriteLine("Amount of Processers: {0}", Environment.ProcessorCount);
            //First get the amount of combinations to get the file count

            //List of models to be ran with complete path
            //Add this list to Queue
            string exec_path = AppDomain.CurrentDomain.BaseDirectory;
            string test_path = Path.GetFullPath(Path.Combine(exec_path, @"..\..\OMEMultiProc\items\compiled_tests"));
            string main_path = Directory.GetFiles(test_path, "*.omec")[0];
            List<string> out_fnames = new List<string>();
            List<string> input_params = new List<string>();
            Logger log_file = new Logger(test_path, model_queue.Count);
            //Fill all the input/output names. 
            nameCreator(model, input_params, out_fnames);


            // Merge both lists into a list of tuples and chuck it into a queue
            IEnumerable<Tuple<string, string>> in_out = input_params.Zip(out_fnames, (a, b) => Tuple.Create(a, b));
            foreach (var atuple in in_out)
            {
                model_queue.Enqueue(atuple);
            }

            Console.WriteLine("Finished processing files, starting the Engines...");
            int idx = main_path.LastIndexOf('\\');
            while (model_queue.Count > 0)
            {

                //Retrieve the next item and build the output string 
                Tuple<string, string> int_out = (Tuple<string, string>)model_queue.Dequeue();
                string out_path = paramLRP.outputStr(result_Path + "\\..\\LRP_results\\", model.base_name, int_out.Item2);

                string in_path = eng_args + "-f\"" + out_path + "\" " + int_out.Item1 + "\"" + main_path + "\"";

                launchEngines(in_path, out_path, model, log_file);
                model.IncrementProcCounter();
                log_file.printLog(model.ProcCounter, model_queue.Count, model.base_name);
            }

            Console.WriteLine("Finished starting last set of processes. Please wait for these to complete, then press any button to exit");
            Console.WriteLine("Time started: {0}\r\n", startTime);
            Console.WriteLine("Time ended: {0}\r\n", DateTime.Now.ToString("h:mm:ss tt"));
        }



        [STAThread]
        static void Main(string[] args)
        {


            Console.WriteLine("Welcome to OME MultiProc. Please enter the folder to save all the results to.");

            // Prepare a dummy string, thos would appear in the dialog
            string dummyFileName = "Save Here";
            string savePath;        //Path set by user for saving results

            SaveFileDialog sf = new SaveFileDialog();
            // Feed the dummy name to the save dialog
            sf.FileName = dummyFileName;

            if (sf.ShowDialog() != DialogResult.OK)
            {
                Console.Write("Must select a save folder, press any button to exit");
                Console.ReadLine();
                return;                 //end program
            }

            // Now here's our save folder
            savePath = Path.GetDirectoryName(sf.FileName) + "\\";

            //Run quietly to improve performance
            string engine_args = "-q ";

            string exec_path = AppDomain.CurrentDomain.BaseDirectory;

            Console.WriteLine("\n\nCreating all combinations for each profile given...");
            string profile_dir = Path.GetFullPath(Path.Combine(exec_path, @"..\..\..\..\..\..\LRP\MultiRun\profiles"));
            profileCollection model_lst = new profileCollection(profile_dir);

            //Iterate through each profile, launching the program
            Console.WriteLine("Creating input and output lists...\n\n ");
            for (int i = 0; i < model_lst.profile_prints.Count; i++)
                LaunchOMEMultiProc(savePath, engine_args, model_lst.profile_prints[i]);


            Console.ReadLine();

        }
    }
}















/*    OLD VERSION, COULD REIMPLEMENT AS TESTING/SINGULAR OPTION
      public static void LaunchOMEMultiProc(string eng_args)
        {
            string startTime = DateTime.Now.ToString("h:mm:ss tt");
            Console.WriteLine("Time started: {0}\r\n", startTime);
            Queue model_queue = new Queue();
            int proc_cntr = 0;

            //List of models to be ran with complete path
            //Add this list to Queue
            string exec_path=AppDomain.CurrentDomain.BaseDirectory;
            string test_path = Path.GetFullPath(Path.Combine(exec_path, @"..\..\OMEMultiProc\items\compiled_tests"));
            string[] file_name_arr = Directory.GetFiles(test_path, "*.omec");
            string[] out_files = new string[file_name_arr.Length];

            //CHANGE AMT OF FILES HERE
            for (int j = 0; j < file_name_arr.Length * 1024; j++)
            {
                    model_queue.Enqueue(file_name_arr[i]);
            }


            
            while (model_queue.Count > 0)
            {   
                
                //build argument list 
                Guid g = Guid.NewGuid();
                string input_path = (string)model_queue.Dequeue();
                int idx = input_path.LastIndexOf('\\');
                int len = input_path.LastIndexOf('.') - idx;              
                string out_file = input_path.Substring(idx + 1, len-1);
                string out_path = input_path.Substring(0, idx) + "\\results\\" + out_file + g + ".csv";
               
               
                //Prepare a process to start up OMEengine with the proper filepaths and flags
                Process proc_engine = new Process();
                proc_engine.StartInfo.FileName = ".\\OMEEngine.exe"; ;
                //Console.WriteLine(eng_args + " -c\"" + out_path + "\" \"" + input_path + "\"");
                //Console.ReadLine();
                proc_engine.StartInfo.Arguments = eng_args + " -c\"" + out_path + "\" \"" + input_path + "\"";
                proc_engine.EnableRaisingEvents = true;
                proc_engine.StartInfo.CreateNoWindow = true;
                proc_engine.StartInfo.UseShellExecute = false;
                //proc_engine.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;


                //Manage the amount of processes running at once to maximize performace
                while (proc_cntr > Environment.ProcessorCount)
               {
                   System.Threading.Thread.Sleep(500);
                   //Console.WriteLine("Waiting for process to exit....Sleeping for 2 seconds");
               }

                //listener for each process, signals when they exit
                proc_engine.Exited += (sender, EventArgs) =>
                    {
                        proc_cntr -= 1;
                        //Console.WriteLine("Current count at process exit: {0}\r\n", proc_cntr);

                        if (proc_cntr == 0)
                        {
                            Console.WriteLine("Time started: {0}\r\n", startTime);
                            Console.WriteLine("Time ended: {0}\r\n", DateTime.Now.ToString("h:mm:ss tt"));
                            Console.WriteLine("Exit time:    {0}\r\n", proc_engine.ExitTime);
                        }


                        if (proc_engine.ExitCode != 0)
                            Console.Write("Error occured");

                        //Source:
                        //https://msdn.microsoft.com/en-us/library/system.diagnostics.process.enableraisingevents(v=vs.110).aspx

                    };

                //Start up the given subprocess consisting of a call to OMEENGINE
                proc_engine.Start();
                //proc_engine.WaitForExit();
                proc_cntr += 1;
                //Console.WriteLine("Current count at process start: {0}\r\n", proc_cntr);
                  
            }

            Console.WriteLine("Finished starting last set of processes. Please wait for these to complete, then press any button to exit");
            Console.WriteLine("Time started: {0}\r\n", startTime);
            Console.WriteLine("Time ended: {0}\r\n", DateTime.Now.ToString("h:mm:ss tt"));
            Console.ReadLine(); 

        }*/