using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using CaptureManagerToWinRTProxy;
using Windows.Storage;
using Windows.Data.Xml.Dom;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace IPCameraViewer
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page, ISessionCallback
    {
        ISession mISession = null;

        IEVRStreamControl mIEVRStreamControl = null;

        IList<ITopologyNode> mArrayPtrTopologyOutputNodes = new List<ITopologyNode>();

        public MainPage()
        {
            this.InitializeComponent();
            
        }

        void onDragDelta(object sender, DragDeltaEventArgs e)
        {
            var thumb = sender as Thumb;
            Canvas.SetLeft(thumb, Canvas.GetLeft(thumb) + e.HorizontalChange);

            setStreamPosition();
        }

        private void setStreamPosition()
        {
            var lLeftPos = Canvas.GetLeft(m_Thumb);

            var lProp = lLeftPos / m_VideoRenderPanel.ActualWidth;

            if (mIEVRStreamControl != null)
            mIEVRStreamControl.setPosition(
                mArrayPtrTopologyOutputNodes[0],
                0.0f,
                (float)lProp,
                0.0f,
                1.0f);

        }

        private async void Button_Click(object sender, RoutedEventArgs e)
        {

            do
            {

                if (mLaunchButton.Content.ToString() == "Stop")
                {
                    if (mISession != null)
                    {
                        mISession.closeSession();

                        mLaunchButton.Content = "Start";
                    }

                    mISession = null;

                    return;
                }

                m_WaitControl.Visibility = Windows.UI.Xaml.Visibility.Visible;

                m_WaitControl.startWaitAnimation();

                var lCaptureProcessor = await IPCameraMJPEGCaptureProcessor.createCaptureProcessor();

                if (lCaptureProcessor == null)
                    break;

                mIEVRStreamControl = CaptureManager.getInstance().getIEVRStreamControl();

                var lSinkControl = CaptureManager.getInstance().getISinkControl();

                if (lSinkControl == null)
                    break;

                var IEVRMultiSinkFactory = lSinkControl.createIEVRMultiSinkFactory();

                if (IEVRMultiSinkFactory == null)
                    break;

                mArrayPtrTopologyOutputNodes.Clear();

                IIntPtr lIIntPtr = m_VideoRenderPanel.getSwapChain();
                
                IEVRMultiSinkFactory.createOutputNodes(
                    lIIntPtr,
                    mArrayPtrTopologyOutputNodes);

                //string lSymbolicLink = "CaptureManager///Software///Sources///ScreenCapture///ScreenCapture";

                //string lextendSymbolicLink = lSymbolicLink + " --options=" +
                //    "<?xml version='1.0' encoding='UTF-8'?>" +
                //    "<Options>" +
                //        "<Option Type='Cursor' Visiblity='True'>" +
                //            "<Option.Extensions>" +
                //                "<Extension Type='BackImage' Height='100' Width='100' Fill='0x7055ff55' />" +
                //            "</Option.Extensions>" +
                //        "</Option>" +
                //    "</Options>";

                //uint lStreamIndex = 0;

                //uint lMediaTypeIndex = 4;


                uint lStreamIndex = 0;

                uint lMediaTypeIndex = 0;


                var lISourceControl = CaptureManager.getInstance().getISourceControl();
                
                var lCaptureProcessorMediaSource = lISourceControl.createMediaSource(lCaptureProcessor);

                if (lCaptureProcessorMediaSource == null)
                    break;

                var lSourceNode = lISourceControl.createSourceNodeFromExternalSourceWithDownStreamConnection(
                    lCaptureProcessorMediaSource,
                    lStreamIndex,
                    lMediaTypeIndex,
                    mArrayPtrTopologyOutputNodes[0]);


                //var lSourceNode = lISourceControl.createSourceNodeWithDownStreamConnection(
                //    lextendSymbolicLink,
                //    lStreamIndex,
                //    lMediaTypeIndex,
                //    mArrayPtrTopologyOutputNodes[0]);

                if (lSourceNode == null)
                    break;



                var lISessionControl = CaptureManager.getInstance().getISessionControl();

                if (lISessionControl == null)
                    break;

                IList<ITopologyNode> lArrayPtrTopologySourceNodes = new List<ITopologyNode>();

                lArrayPtrTopologySourceNodes.Add(lSourceNode);

                mISession = lISessionControl.createSession(
                    lArrayPtrTopologySourceNodes);

                if (mISession == null)
                    break;

                mISession.attachISessionCallback(this);

                mISession.startSession(0, Guid.Empty);


                var lLeftPos = Canvas.GetLeft(m_Thumb);

                var lProp = lLeftPos / m_VideoRenderPanel.ActualWidth;

                mIEVRStreamControl.setPosition(
                    mArrayPtrTopologyOutputNodes[0],
                    0.0f,
                    (float)lProp,
                    0.0f,
                    1.0f);

                mLaunchButton.Content = "Stop";
                
            } while (false);
        }

        private void Page_Loaded(object sender, RoutedEventArgs e)
        {
            initLogPrintOut();

            m_Thumb.Height = m_VideoRenderPanel.ActualHeight;

            Canvas.SetLeft(m_Thumb, m_VideoRenderPanel.ActualWidth / 2.0);
            
        }

        private void initLogPrintOut()
        {
	        var lILogPrintOutControl = CaptureManager.getInstance().getILogPrintOutControl();

	        do
	        {
                if (lILogPrintOutControl == null)
                    break;

        #if DEBUG

		        StorageFolder localFolder = ApplicationData.Current.LocalFolder;

		        var lLogFilePath = localFolder.Path + "\\Log.txt";

		        lILogPrintOutControl.addPrintOutDestination(0, lLogFilePath);

		        lILogPrintOutControl.addPrintOutDestination(1, lLogFilePath);
        #endif

	        } while (false);
        }


        public enum SessionCallbackEventCode : int
        {
            Unknown = 0,
            Error = Unknown + 1,
            Status_Error = Error + 1,
            Execution_Error = Status_Error + 1,
            ItIsReadyToStart = Execution_Error + 1,
            ItIsStarted = ItIsReadyToStart + 1,
            ItIsPaused = ItIsStarted + 1,
            ItIsStopped = ItIsPaused + 1,
            ItIsEnded = ItIsStopped + 1,
            ItIsClosed = ItIsEnded + 1,
            VideoCaptureDeviceRemoved = ItIsClosed + 1
        };

        public bool invoke(uint aCallbackEventCode, uint aSessionDescriptor)
        {
            SessionCallbackEventCode k = (SessionCallbackEventCode)aCallbackEventCode;

            switch (k)
            {
                case SessionCallbackEventCode.Unknown:
                    break;
                case SessionCallbackEventCode.Error:
                    break;
                case SessionCallbackEventCode.Status_Error:
                    break;
                case SessionCallbackEventCode.Execution_Error:
                    break;
                case SessionCallbackEventCode.ItIsReadyToStart:
                    break;
                case SessionCallbackEventCode.ItIsStarted:
                    {
                        Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Low,
                            stopWaitAnimation);
                            
                    }
                    break;
                case SessionCallbackEventCode.ItIsPaused:
                    break;
                case SessionCallbackEventCode.ItIsStopped:
                    break;
                case SessionCallbackEventCode.ItIsEnded:
                    break;
                case SessionCallbackEventCode.ItIsClosed:
                    break;
                case SessionCallbackEventCode.VideoCaptureDeviceRemoved:
                    {


                        //Dispatcher.Invoke(
                        //DispatcherPriority.Normal,
                        //new Action(() => mLaunchButton_Click(null, null)));

                    }
                    break;
                default:
                    break;
            }

            return true;
        }

        void stopWaitAnimation()
        {

            m_WaitControl.stopWaitAnimation();

            m_WaitControl.Visibility = Windows.UI.Xaml.Visibility.Collapsed;

        }
    }
}
