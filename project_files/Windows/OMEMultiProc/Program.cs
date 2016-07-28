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
            Queue model_queue = new Queue();
            int proc_cntr = 0;

            //List of models to be ran with complete path
            //Add this list to Queue
            string exec_path=AppDomain.CurrentDomain.BaseDirectory;
            string test_path = Path.GetFullPath(Path.Combine(exec_path, @"..\..\OMEMultiProc\test\compiled_tests"));
            string[] file_name_arr = Directory.GetFiles(test_path, "*.omec");
            string[] out_files = new string[file_name_arr.Length];

            for (int j = 0; j < file_name_arr.Length * 1024; j++)
            {
                for (int i = 0; i < file_name_arr.Length; i++)
                {
                    model_queue.Enqueue(file_name_arr[i]);
                }
            }

          

            while (model_queue.Count > 0)
            {   //build argument list 
                Guid g = Guid.NewGuid();
                string input_path = (string)model_queue.Dequeue();
                int idx = input_path.LastIndexOf('\\');
                int len = input_path.LastIndexOf('.') - idx;
               
                string out_file = input_path.Substring(idx + 1, len-1);
                string out_path = input_path.Substring(0, idx) + "\\results\\" + out_file + g + ".csv";
               
                
                int elapsedTime = 0;
                bool eventHandled = false;


                //start a process on the new file
                Process proc_engine = new Process();
                proc_engine.StartInfo.FileName = ".\\OMEEngine.exe"; ;
                proc_engine.StartInfo.Arguments = eng_args + " -c\"" + out_path + "\" \"" + input_path + "\"";
                proc_engine.EnableRaisingEvents = true;
                proc_engine.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
                proc_engine.Exited += (sender, EventArgs) =>
                    {
                        Console.WriteLine(model_queue.Count);
                        if (model_queue.Count == 0)
                        {
                            Console.WriteLine("Time ended: {0}\r\n", DateTime.Now.ToString("h:mm:ss tt"));
                            Console.WriteLine("Exit time:    {0}\r\n", proc_engine.ExitTime);
                        }

                        eventHandled = true;
                        //Console.WriteLine("Exit time:    {0}\r\n", proc_engine.ExitTime);
                        //Console.WriteLine("Exit time:    {0}\r\n" +
                            //"Exit code:    {1}\r\nElapsed time: {2}", proc_engine.ExitTime, proc_engine.ExitCode,proc_engine.ExitTime - proc_engine.StartTime);
                        
                        if (proc_engine.ExitCode != 0)
                            Console.Write("Error occured");

                        //Source:
                        //https://msdn.microsoft.com/en-us/library/system.diagnostics.process.enableraisingevents(v=vs.110).aspx

                    };
                proc_engine.Start();
                //proc_cntr += 1;
                //proc_engine.WaitForExit();


                  
            }



            Console.WriteLine("Finished. Press any key to exit");
            Console.ReadLine(); 






        }





        static void Main(string[] args)
        {

            Console.Write(Environment.NewLine + Environment.NewLine);
            Console.WriteLine("Enter any flags separated only be spaces: ");
            string startTime = DateTime.Now.ToString("h:mm:ss tt");
            string engine_args = "-q";

            Console.WriteLine("Time started: {0}\r\n", startTime);

            LaunchOMEMultiProc(engine_args);

            

        }

    
    }
}
