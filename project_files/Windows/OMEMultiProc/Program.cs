using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.IO;

namespace MultiProc
{
    class Program
    {


        /// <summary>
        /// Launch a program using the supplied arguments
        /// </summary>
        /// <param name="args">The executable path for OMEENGINE and flags passed to it.</param>
        private static void LaunchOMEMultiProc(string eng_args)
        {
            Queue model_queue = new Queue();


            //List of models to be ran with complete path
            //Add this list to Queue
            string exec_path=AppDomain.CurrentDomain.BaseDirectory;
            string test_path = Path.GetFullPath(Path.Combine(exec_path, @"..\..\OMEMultiProc\test"));
            string[] file_name_arr = Directory.GetFiles(test_path, "*.omem");
            string[] out_files = new string[file_name_arr.Length];
            for (int i = 0; i < file_name_arr.Length; i++)
            {
                //Console.Write(Environment.NewLine);
                //Console.WriteLine(file_name_arr[i]);
                //Console.Write(Environment.NewLine);
                model_queue.Enqueue(file_name_arr[i]);
                
                //Console.Write(Environment.NewLine);
                //Console.WriteLine(out_files[i]);
                //Console.Write(Environment.NewLine);
            }
            Console.WriteLine(eng_args + " -c" + out_files[1] + file_name_arr[1]);
            //while(Queue)
            //build argument list
            while (model_queue.Count > 0)
            {
                string input_path = (string)model_queue.Dequeue();
                //input_path = input_path.Replace(" ","\\ ");
                Console.WriteLine(input_path);
                string out_path = input_path.Split('/').Last().Split('.')[0] + ".csv";
                
                
                Process exoProc = new Process();
                exoProc.StartInfo.FileName = ".\\OMEEngine.exe"; ;
                exoProc.StartInfo.Arguments = eng_args + " -c\"" + out_path + "\" \"" + input_path + "\"";
                exoProc.Start();
                exoProc.WaitForExit();
                if (exoProc.ExitCode != 0)
                    Console.Write("Unrecoverable error");
                 
                
            }
            
        }


        static void Main(string[] args)
        {

            Console.Write(Environment.NewLine + Environment.NewLine);
            Console.WriteLine("Enter any flags separated only be spaces: ");
            string engine_args = Console.ReadLine();

            LaunchOMEMultiProc(engine_args);

            string derp = Console.ReadLine();

        }

    
    }
}
