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



    class runProcs
    {


        public class profileCollection
        {
        
            //Constructor, pass in the directory
            public profileCollection(string dir_path)
            {
                try 
                { 
                    BaseProfileFactory(dir_path); 
                }
                catch (Exception e)
                {  
                    throw e;
                }
            }


            //List that will hold the profiles
            public List<modelProfile> profile_prints = new List<modelProfile>();


            //parse each of the profiles in the given subfolder path
            //Create a new profile given the path to the file
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
            //Dictionary<key,val>;

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
                                    paramLRP new_entry = new paramLRP(temp_params);
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

        //LRP model parameter ranges
        public class paramLRP
        {
            //Structure of the profiles used is as follows
            public paramLRP(string[] prams)
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
            public float min { get; set; }
            public float max { get; set; }
            public float intrvl { get; set; }
            public int iter { get; set; }

        }

        
        
        
        
        
        
        
        //Calculate all combinations, fill in output file names and input string to engine accordingly
        public static void nameCreator(modelProfile model_data, string[] build_input, string[] build_output)
        {
            List<paramLRP> pd = model_data.plist;

            var combinations = from a in pd[0].min.ToString()
                               from b in pd[1].min.ToString()
                               from c in pd[2].min.ToString()
                               orderby a, b, c
                               select new List<int> { a, b, c };

            var x = combinations.ToList();
 

        }

        //The engine arguments and each base model along with its parameters are passed in
        public static void LaunchOMEMultiProc(string eng_args, modelProfile model)
        {
            string startTime = DateTime.Now.ToString("h:mm:ss tt");
            Console.WriteLine("Time started: {0}\r\n", startTime);
            Queue model_queue = new Queue();
            short proc_cntr = 0;
            int file_count = 1;

            //First get the amount of combinations to get the file count
            foreach(paramLRP var in model.plist )
                file_count *= var.iter;

            //List of models to be ran with complete path
            //Add this list to Queue
            string exec_path=AppDomain.CurrentDomain.BaseDirectory;
            string test_path = Path.GetFullPath(Path.Combine(exec_path, @"..\..\OMEMultiProc\items\compiled_tests"));
            string[] file_name_arr = Directory.GetFiles(test_path, "*.omec");
            string[] out_files = new string[file_count];
            string[] input_params = new string[file_count];


            //Call a function to fill all the input/output names. 
            nameCreator(model, input_params, out_files);



                //CHANGE AMT OF FILES HERE
                for (int j = 0; j < file_name_arr.Length * 1024; j++)
                {
                    for (int i = 0; i < file_name_arr.Length; i++)
                    {
                        model_queue.Enqueue(file_name_arr[i]);
                    }
                }

            Console.WriteLine("Finished processing files, starting Engines...");
            /*
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
                  
            }*/

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
            string engine_args = " -q";

            string exec_path=AppDomain.CurrentDomain.BaseDirectory;
            string profile_dir = Path.GetFullPath(Path.Combine(exec_path, @"..\..\OMEMultiProc\items\profiles"));

            profileCollection model_lst = new profileCollection(profile_dir);
            //model_lst.profile_prints.ForEach(item => Console.WriteLine(item.base_name));

            //Iterate through each profile, launching the program
            for (int i = 0; i < model_lst.profile_prints.Count; i++)
                LaunchOMEMultiProc(engine_args, model_lst.profile_prints[i]);
    

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
                for (int i = 0; i < file_name_arr.Length; i++)
                {
                    model_queue.Enqueue(file_name_arr[i]);
                }
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