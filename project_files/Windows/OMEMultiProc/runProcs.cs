using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.IO;

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


            //List that will hold the profiles
            public List<modelProfile> profile_prints;


            //parse each of the profiles in the given subfolder path
            //Add the new modelProfile object to the list
            public void BaseProfileFactory(string directory_path)
            {

                string[] file_paths = Directory.GetFiles(directory_path, "profile*");

                //We get each file path, need to open it and parse it, filling in all the values in the modelProfile
                foreach(string each in file_paths)
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
            public List<paramLRP> plist = new List<paramLRP>();
            String[] temp_params;

            //This MEGAMAP is a monstrous creature used to speed up the process
            //of creating output files from the input files. The dicts are built
            //on a per profile basis to add flexibility in case of differing defaults
            public Dictionary<string, string> name_map = new Dictionary<string, string>();

            public void fill_profile(string path)
            {
                try
                {
                    using (StreamReader sr = new StreamReader(path))
                    {
                        string line;       
                        while ((line = sr.ReadLine()) != null)
                        {
                            if ( line[0] != '#')
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
                catch(Exception e)
                {
                    Console.WriteLine(e.GetType().FullName);
                    Console.WriteLine(e.Message);
                    Console.WriteLine("Press any button to exit, program will now terminate");
                    Console.ReadLine();
                    Environment.Exit(1);
                }
                
            }
        }




        /*LRP model parameter ranges
         ** 
         ** 
         */
        public class paramLRP
        {
            //Structure of the profiles used is as follows
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
                    iter = int.Parse(prams[6]);
                    if (iter == 0) { iter = 1; }
                }
                catch(IndexOutOfRangeException)
                {
                    Console.WriteLine("Index was out of range, check the commas for variable {0}", ome_name );
                    Console.WriteLine("\n\n Program will now exit, press any button");
                    Console.ReadLine();
                    Environment.Exit(1);
                }
                catch(InvalidCastException)
                {
                    Console.WriteLine("Invalid cast, check the fields in the profile for variable {0}", ome_name);
                    Console.WriteLine("\n\n Program will now exit, press any button");
                    Console.ReadLine();
                    Environment.Exit(1);
                }

            }

            public string ome_name { get; set; }
            public string descriptor { get; set; }
            public float default_val { get; set; }
            float min { get; set; }
            float max { get; set; }
            float intrvl { get; set; }
            public int iter { get; set; }

            //Returns a list of all the values the parameter can take
            //In OME_input -o format (See OMEEngine.cpp PrintHelp())
            //Also inserts naming scheme into the dictionary used for the profile
            public List<string> enumVals(Dictionary<string, string> OMEtoLRP_map)
            {
                List<string> var_vals = new List<string>();
                float varr;
                string temp_name, temp_desc;

                if (iter == 1)
                {
                    temp_name = String.Format("-o\"{0}\"={1} ", ome_name, default_val);
                    OMEtoLRP_map.Add(temp_name, ""); //When default, not in LRP name.
                    var_vals.Add(temp_name);
                    return var_vals;
                }    

                for(int i = 0; i < iter; i++)
                {
                    varr = min + intrvl * i;
                    temp_name = String.Format("-o\"{0}\"={1} ", ome_name, varr);
                    if (varr == default_val)
                        temp_desc = ""; //When default, not in name.
                    else
                        temp_desc = String.Format("_{0}{1} ", varr, descriptor);
                    OMEtoLRP_map.Add(temp_name, temp_desc.Trim());
                    var_vals.Add(temp_name);
                }
                //var_vals.ForEach(i => Console.Write("{0}\t", i));
                //Console.Write("\n\n\n");
                return var_vals;
            }

            
        }

        
        
        
        
        
        
        
        //Calculate all combinations, fill in output file names and input string to engine accordingly
        public static void nameCreator(modelProfile model_data, List<string> build_input, List<string> build_output)
        {
            
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


        private static List<List<T>> AddExtraSet<T> (List<List<T>> combinations, List<T> set)
        {
            var newCombinations = from value in set
                                  from combination in combinations
                                  select new List<T>(combination) { value };

            return newCombinations.ToList();
        }

        //Concatenate the OME_Input and corresponding output using the dictionary
        //This keyword extends IEnumerable with 'string' properties
        public static string Concat(this IEnumerable<string> source, Dictionary<string,string> map_csv)
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







        //The engine arguments and each base model along with its parameters are passed in
        public static void LaunchOMEMultiProc(string result_Path, string eng_args, modelProfile model)
        {
            string startTime = DateTime.Now.ToString("h:mm:ss tt");
            Console.WriteLine("Time started: {0}\r\n", startTime);
            Queue model_queue = new Queue();
            short proc_cntr = 0;
            int file_count = 1;
            Console.WriteLine("Amount of Processers: {0}", Environment.ProcessorCount);
            //First get the amount of combinations to get the file count
            foreach(paramLRP var in model.plist )
                file_count *= var.iter;

            //List of models to be ran with complete path
            //Add this list to Queue
            string exec_path=AppDomain.CurrentDomain.BaseDirectory;
            string test_path = Path.GetFullPath(Path.Combine(exec_path, @"..\..\OMEMultiProc\items\compiled_tests"));
            string main_path = Directory.GetFiles(test_path, "*.omec")[0];
            List<string> out_fnames = new List<string>();
            List<string> input_params = new List<string>();

            //Fill all the input/output names. 
            nameCreator(model, input_params, out_fnames);


            /*
            foreach (var merp in out_files)
            {
                Console.WriteLine(merp);
                Console.WriteLine("\n\n\n");
            }
            foreach (var derp in input_params)
            {
                Console.WriteLine(derp);
                Console.WriteLine("\n\n\n");
            }*/


            // Merge both lists into a list of tuples and chuck it into a queue
            IEnumerable<Tuple<string, string>> in_out = input_params.Zip(out_fnames, (a, b) => Tuple.Create(a, b));
            foreach (var atuple in in_out)
            {
                model_queue.Enqueue(atuple);
                /*
                if (atuple.Item2 == "_0CD_4RDD_1GreenRD_0PTF_0LMA_0.8PofG_0.8MPG")
                {
                    string error = atuple.Item1;
                    System.IO.File.WriteAllText(@"C:\Users\vpredovi\Desktop\error_cause.txt", error);
                    Environment.Exit(0);
                }*/
            }
            Console.WriteLine("Finished processing files, starting the Engines...");
            //Console.ReadLine();
            int idx = main_path.LastIndexOf('\\'); 
            while (model_queue.Count > 0)
            {   
                
                //Retrieve the next item and build the output string 
                Tuple<string, string> int_out = (Tuple<string, string>)model_queue.Dequeue();
                string out_path = result_Path + "\\..\\LRP_results\\" + model.base_name + int_out.Item2 + ".csv";
                //Console.WriteLine(model_queue.Count);
                
                 //Prepare a process to start up OMEengine with the proper filepaths and flags
                 Process proc_engine = new Process();
                 proc_engine.StartInfo.FileName = ".\\OMEEngine.exe"; ;
                //Console.WriteLine(eng_args + "-f\"" + out_path + "\" " + int_out.Item1 + "\"" + main_path + "\"");
                //string error = eng_args + "-f\"" + out_path + "\" " + int_out.Item1 + "\"" + main_path + "\"";
               // error += Environment.NewLine + int_out.Item1;
                //System.IO.File.WriteAllText(@"C:\Users\User\Desktop\WriteLines.txt", error);

                //Console.WriteLine(int_out.Item1);
                 proc_engine.StartInfo.Arguments = eng_args + "-f\"" + out_path + "\" " + int_out.Item1 + "\"" + main_path + "\"";
                 proc_engine.EnableRaisingEvents = true;
                 proc_engine.StartInfo.CreateNoWindow = true;
                 proc_engine.StartInfo.UseShellExecute = false;
                 //proc_engine.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
                 //proc_engine.StartInfo.RedirectStandardOutput = true;
                 //proc_engine.StartInfo.RedirectStandardError = true;

                 //Manage the amount of processes running at once to maximize performace
                 Console.Write(proc_cntr);
                 while (proc_cntr > Environment.ProcessorCount)
                {
                    System.Threading.Thread.Sleep(2000);
                    Console.WriteLine("Waiting for process to exit....Sleeping for 2 seconds");
                }

                 //listener for each process, signals when they exit
                 proc_engine.Exited += (sender, EventArgs) =>
                     {
                         proc_cntr -= 1;
                         Console.WriteLine("Current count at process exit: {0}\r\n", proc_cntr);

                         if (proc_cntr == 0)
                         {
                             Console.WriteLine("Time started: {0}\r\n", startTime);
                             Console.WriteLine("Time ended: {0}\r\n", DateTime.Now.ToString("h:mm:ss tt"));
                             Console.WriteLine("Exit time:    {0}\r\n", proc_engine.ExitTime);
                         }

                         //Console.WriteLine(proc_engine.StandardError.ReadToEnd());
                         //Console.WriteLine(proc_engine.StandardOutput.ReadToEnd());

                         
                         if (proc_engine.ExitCode != 0)
                         {
                             Console.Write("Error occured");
                             
                         }

                         //Source:
                         //https://msdn.microsoft.com/en-us/library/system.diagnostics.process.enableraisingevents(v=vs.110).aspx

                     };
                 //System.Threading.Thread.Sleep(2000);

                 //Start up the given subprocess consisting of a call to OMEENGINE
                 proc_engine.Start();
                 //proc_engine.WaitForExit();
                 proc_cntr += 1;
                 //Console.ReadLine();

            }

            Console.WriteLine("Finished starting last set of processes. Please wait for these to complete, then press any button to exit");
            Console.WriteLine("Time started: {0}\r\n", startTime);
            Console.WriteLine("Time ended: {0}\r\n", DateTime.Now.ToString("h:mm:ss tt"));


        }




        static void Main(string[] args)
        {
            Console.Write(Environment.NewLine + Environment.NewLine);
            Console.WriteLine("Enter any flags separated only be spaces: ");

            //Run quietly to improve performance
            //Hardcode for testing purposes.
            string engine_args = "-q ";

            string exec_path=AppDomain.CurrentDomain.BaseDirectory;
            string profile_dir = Path.GetFullPath(Path.Combine(exec_path, @"..\..\..\..\..\..\LRP\MultiRun\profiles"));

            profileCollection model_lst = new profileCollection(profile_dir);
            //model_lst.profile_prints.ForEach(item => Console.WriteLine(item.base_name));

            //Iterate through each profile, launching the program
            for (int i = 0; i < model_lst.profile_prints.Count; i++)
                LaunchOMEMultiProc(profile_dir, engine_args, model_lst.profile_prints[i]);
    

            Console.ReadLine();
        }
    }
}















/*    OLD VERSION FOR TESTING, MAYBE REIMPLEMENT AS TESTING/SINGULAR OPTION
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