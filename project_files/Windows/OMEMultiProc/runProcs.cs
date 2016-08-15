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
            string base_name;
            public List<paramLRP> plist = new List<paramLRP>();
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
                                    base_name = line.Replace("*","");
                            }
                        }
                        Console.WriteLine(base_name);
                        Console.WriteLine("##############End of File");


                        Console.WriteLine(path);

                    }
                }
                catch
                {

                }
                
            }
        }

        //LRP model parameter ranges
        public class paramLRP
        {
            string ome_name { get; set; }
            string descriptor { get; set; }
            float default_val { get; set; }
            float min { get; set; }
            float max { get; set; }
            float intrvl { get; set; }
            int iter { get; set; }

        }

 
    


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

            profileCollection modelList = new profileCollection(profile_dir);


            //LaunchOMEMultiProc(engine_args);
            Console.ReadLine();
        }
    }
}
