using CaptureManagerToCSharpProxy;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;

namespace WPFVideoAndAudioRecorder
{

    class Program
    {

        public static CaptureManager mCaptureManager = null;

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [MTAThread]
        static void Main(string[] args)
        {
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


            var t = new Thread(

               delegate ()
               {

                   try
                   {
                       var lApplication = new System.Windows.Application();

                       lApplication.Run(new MainWindow());
                   }
                   catch (Exception ex)
                   {
                   }
                   finally
                   {
                   }
               });
            t.SetApartmentState(ApartmentState.STA);

            t.Start();

        }
    }
}

