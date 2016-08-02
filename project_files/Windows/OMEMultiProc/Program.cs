using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.IO;


//using System.Collections.Generic

namespace MultiProc
{
    class Program
    {

        //List<int> elapsedTime = new List<int>();
        //List<bool> eventHandled = new List<bool>();


        /// <summary>
        /// Launch a program using the supplied arguments
        /// </summary>
        /// <param name="args">The executable path for OMEENGINE and flags passed to it.</param>
        public static void LaunchOMEMultiProc(string eng_args)
        {
            string startTime = DateTime.Now.ToString("h:mm:ss tt");
            Console.WriteLine("Time started: {0}\r\n", startTime);
            Queue model_queue = new Queue();
            int proc_cntr = 0;

            //List of models to be ran with complete path
            //Add this list to Queue
            string exec_path=AppDomain.CurrentDomain.BaseDirectory;
            string test_path = Path.GetFullPath(Path.Combine(exec_path, @"..\..\OMEMultiProc\test\compiled_tests"));
            string[] file_name_arr = Directory.GetFiles(test_path, "*.omec");
            string[] out_files = new string[file_name_arr.Length];

            for (int j = 0; j < file_name_arr.Length * 128; j++)
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
                proc_engine.StartInfo.Arguments = eng_args + " -c\"" + out_path + "\" \"" + input_path + "\"";
                proc_engine.EnableRaisingEvents = true;
                proc_engine.StartInfo.CreateNoWindow = true;
                proc_engine.StartInfo.UseShellExecute = false;
                //proc_engine.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;


                //Only run as many processes as we have cores
                while (proc_cntr > Environment.ProcessorCount * 2 - 2)
               {
                   System.Threading.Thread.Sleep(2000);
                   Console.WriteLine("Waiting for process to exit....Sleeping for 2 seconds");
               }

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


                        if (proc_engine.ExitCode != 0)
                            Console.Write("Error occured");

                        //Source:
                        //https://msdn.microsoft.com/en-us/library/system.diagnostics.process.enableraisingevents(v=vs.110).aspx

                    };

                //Start the subprocess
                proc_engine.Start();
                //proc_engine.WaitForExit();
                proc_cntr += 1;
                Console.WriteLine("Current count at process start: {0}\r\n", proc_cntr);



                  
            }



            Console.WriteLine("Finished starting last set of processes. Please wait for these to complete, then press any button to exit");
            Console.ReadLine(); 






        }





        static void Main(string[] args)
        {

            Console.Write(Environment.NewLine + Environment.NewLine);
            Console.WriteLine("Enter any flags separated only be spaces: ");

            string engine_args = " -q";



            LaunchOMEMultiProc(engine_args);

            

        }

    
    }
}
