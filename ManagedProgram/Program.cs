using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Windows.Forms;

namespace ManagedProgram
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());
        }

        static int AltMain(string arg)
        {
            //If you want to log unhandled exceptions
            AppDomain.CurrentDomain.UnhandledException += Program.CurrentDomain_UnhandledException;

            Thread thread = new Thread(delegate ()
            {
                Main();
            });

            //Use this if you want to use some COM stuff (also for Xaml Islands (WinRT is just glorified COM))
            //also Xaml Islands works only as packaged because you can't embed app.manifest with maxversiontested without breaking RuntimeBroker signature.
            thread.SetApartmentState(ApartmentState.STA);
            thread.Start();
            thread.Join();

            return 0;
        }

        private static void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            MessageBox.Show((e.ExceptionObject as Exception).Message + "\n\n" + (e.ExceptionObject as Exception).StackTrace, "Unhandled Exception");
        }
    }
}
