using CaptureManagerToCSharpProxy;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace WPFAreaScreenRecorder
{

    class Program
    {

        public static CaptureManager mCaptureManager = null;

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            ProcessPriorityClass lpriority = Process.GetCurrentProcess().PriorityClass;

            Process.GetCurrentProcess().PriorityClass = ProcessPriorityClass.RealTime;

            try
            {
                mCaptureManager = new CaptureManager("CaptureManager.dll");
            }
            catch (System.Exception)
            {
                try
                {
                    mCaptureManager = new CaptureManager();
                }
                catch (System.Exception)
                {

                }
            }

            new System.Windows.Application().Run(new ControlWindow());
        }
    }
}
