﻿using CaptureManagerToCSharpProxy;
using CaptureManagerToCSharpProxy.Interfaces;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;
using System.Xml;

namespace WPFWebCamShotAsync
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public static CaptureManager mCaptureManager;

        ISessionAsync mISession = null;

        Guid MFMediaType_Video = new Guid(
 0x73646976, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

        Guid MFVideoFormat_RGB32 = new Guid(
 22, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

        Guid MFVideoFormat_MJPG = new Guid(
 0x47504A4D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

        bool mIsMJPG = false;

        uint mVideoWidth = 0;

        uint mVideoHeight = 0;

        int mChannels = 0;

        IWebCamControlAsync mWebCamControl;

        Guid mReadMode;

        ISampleGrabberCallAsync mISampleGrabberCall;

        //DispatcherTimer mTimer = new DispatcherTimer();

        uint m_sampleByteSize;

        public MainWindow()
        {
            InitializeComponent();

            //mTimer.Interval = new TimeSpan(100000);

            mWebCamParametrsTab.AddHandler(Slider.ValueChangedEvent, new RoutedEventHandler(mParametrSlider_ValueChanged));

            mWebCamParametrsTab.AddHandler(CheckBox.CheckedEvent, new RoutedEventHandler(mParametrSlider_Checked));

            mWebCamParametrsTab.AddHandler(CheckBox.UncheckedEvent, new RoutedEventHandler(mParametrSlider_Checked));

            //mTimer.Tick += mTimer_Tick;
        }

        //void mTimer_Tick(object sender, EventArgs e)
        //{
        //    if (mISampleGrabber == null)
        //        return;

        //    uint lByteSize = (uint)mData.Length;

        //    try
        //    {

        //        mISampleGrabber.readData(mData, out lByteSize);
        //    }
        //    catch (Exception exc)
        //    {

        //    }
        //    finally
        //    {

        //        updateDisplayImage(this, mData, lByteSize);
        //    }
        //}

        private async void mLaunchButton_Click(object sender, RoutedEventArgs e)
        {
            if (mLaunchButton.Content.ToString() == "Stop")
            {
                //mTimer.Stop();

                if (mISession != null)
                    await mISession.closeSessionAsync();

                mLaunchButton.Content = "Launch";

                mTakePhotoButton.IsEnabled = false;

                if (mISampleGrabberCall != null)
                    System.Runtime.InteropServices.Marshal.ReleaseComObject(mISampleGrabberCall.getTopologyNode());

                return;
            }

            var lSourceNode = mSourcesComboBox.SelectedItem as XmlNode;

            if (lSourceNode == null)
                return;

            var lNode = lSourceNode.SelectSingleNode("Source.Attributes/Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK']/SingleValue/@Value");

            if (lNode == null)
                return;

            string lSymbolicLink = lNode.Value;

            lSourceNode = mStreamsComboBox.SelectedItem as XmlNode;

            if (lSourceNode == null)
                return;

            lNode = lSourceNode.SelectSingleNode("@Index");

            if (lNode == null)
                return;

            uint lStreamIndex = 0;

            if (!uint.TryParse(lNode.Value, out lStreamIndex))
            {
                return;
            }

            lSourceNode = mMediaTypesComboBox.SelectedItem as XmlNode;

            if (lSourceNode == null)
                return;

            lNode = lSourceNode.SelectSingleNode("@Index");

            if (lNode == null)
                return;

            uint lMediaTypeIndex = 0;

            if (!uint.TryParse(lNode.Value, out lMediaTypeIndex))
            {
                return;
            }

            lNode = lSourceNode.SelectSingleNode("MediaTypeItem[@Name='MF_MT_FRAME_SIZE']/Value.ValueParts/ValuePart[1]/@Value");

            if (lNode == null)
                return;

            uint lVideoWidth = 0;

            if (!uint.TryParse(lNode.Value, out lVideoWidth))
            {
                return;
            }

            mVideoWidth = lVideoWidth;

            lNode = lSourceNode.SelectSingleNode("MediaTypeItem[@Name='MF_MT_FRAME_SIZE']/Value.ValueParts/ValuePart[2]/@Value");

            if (lNode == null)
                return;

            uint lVideoHeight = 0;

            if (!uint.TryParse(lNode.Value, out lVideoHeight))
            {
                return;
            }

            mVideoHeight = lVideoHeight;

            int lWidthInBytes = await mCaptureManager.getStrideForBitmapInfoHeaderAsync(
                MFVideoFormat_RGB32,
                lVideoWidth);

            m_sampleByteSize = (uint)Math.Abs(lWidthInBytes) * lVideoHeight;

            var lSinkControl = await mCaptureManager.createSinkControlAsync();

            string lxmldoc = await mCaptureManager.getCollectionOfSinksAsync();

            XmlDocument doc = new XmlDocument();

            doc.LoadXml(lxmldoc);

            var lSinkNode = doc.SelectSingleNode("SinkFactories/SinkFactory[@GUID='{759D24FF-C5D6-4B65-8DDF-8A2B2BECDE39}']");

            if (lSinkNode == null)
                return;

            var lContainerNode = lSinkNode.SelectSingleNode("Value.ValueParts/ValuePart[3]");

            if (lContainerNode == null)
                return;

            setContainerFormat(lContainerNode);

            ISampleGrabberCallSinkFactoryAsync mSinkFactory = await lSinkControl.createSampleGrabberCallSinkFactoryAsync(
            mReadMode);

            lNode = lSourceNode.SelectSingleNode("MediaTypeItem[@Name='MF_MT_SUBTYPE']/SingleValue/@Value");

            if (lNode == null)
                return;

            if (lNode.Value == "MFVideoFormat_MJPG")
            {
                mISampleGrabberCall = await mSinkFactory.createOutputNodeAsync(
                    MFMediaType_Video,
                    MFVideoFormat_MJPG,
                    m_sampleByteSize);

                mIsMJPG = true;

            }
            else
            {
                mISampleGrabberCall = await mSinkFactory.createOutputNodeAsync(
                    MFMediaType_Video,
                    MFVideoFormat_RGB32,
                    m_sampleByteSize);
            }

            if (mISampleGrabberCall != null)
            {
                byte[] lData = new byte[m_sampleByteSize];

                var lSampleGrabberCallNode = mISampleGrabberCall.getTopologyNode();

                if (lSampleGrabberCallNode == null)
                    return;

                var lSourceControl = await mCaptureManager.createSourceControlAsync();

                if (lSourceControl == null)
                    return;

                object lPtrSourceNode = await lSourceControl.createSourceNodeWithDownStreamConnectionAsync(
                    lSymbolicLink,
                    lStreamIndex,
                    lMediaTypeIndex,
                    lSampleGrabberCallNode);

                List<object> lSourceMediaNodeList = new List<object>();

                lSourceMediaNodeList.Add(lPtrSourceNode);

                var lSessionControl = await mCaptureManager.createSessionControlAsync();

                if (lSessionControl == null)
                    return;

                mISession = await lSessionControl.createSessionAsync(
                    lSourceMediaNodeList.ToArray());

                if (mISession == null)
                    return;

                if (! await mISession.startSessionAsync(0, Guid.Empty))
                    return;

                mLaunchButton.Content = "Stop";
                
                mTakePhotoButton.IsEnabled = true;
            }
        }

        private void updateDisplayImage(Window aWindow, byte[] aData, uint aLength)
        {
            mDisplayImage.Source = FromArray(aData, mVideoWidth, mVideoHeight, mChannels);
        }


        private static BitmapSource FromArray(byte[] data, uint w, uint h, int ch)
        {
            PixelFormat format = PixelFormats.Default;

            if (ch == 1) format = PixelFormats.Gray8; //grey scale image 0-255
            if (ch == 3) format = PixelFormats.Bgr24; //RGB
            if (ch == 4) format = PixelFormats.Bgr32; //RGB + alpha

            WriteableBitmap wbm = new WriteableBitmap((int)w, (int)h, 96, 96, format, null);
            wbm.WritePixels(new Int32Rect(0, 0, (int)w, (int)h), data, ch * (int)w, 0);

            return wbm;
        }

        private void mParametrSlider_ValueChanged(object sender, RoutedEventArgs e)//(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
        }

        private void mParametrSlider_Checked(object sender, RoutedEventArgs e)
        {
        }

        private void mShowBtn_Click(object sender, RoutedEventArgs e)
        {
        }


        private void setContainerFormat(XmlNode aXmlNode)
        {
            do
            {
                if (aXmlNode == null)
                    break;

                var lAttrNode = aXmlNode.SelectSingleNode("@Value");

                if (lAttrNode == null)
                    break;

                lAttrNode = aXmlNode.SelectSingleNode("@GUID");

                if (lAttrNode == null)
                    break;

                Guid lContainerFormatGuid;

                if (Guid.TryParse(lAttrNode.Value, out lContainerFormatGuid))
                {
                    mReadMode = lContainerFormatGuid;
                }

            } while (false);

        }

        private async void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (mLaunchButton.Content.ToString() == "Stop")
            {
                //mTimer.Stop();

                if (mISession != null)
                    await mISession.closeSessionAsync();

                mLaunchButton.Content = "Launch";

                return;
            }
        }

        private async void mTakePhotoButton_Click(object sender, RoutedEventArgs e)
        {
            if (mISampleGrabberCall == null)
                return;

            byte[] lData = new byte[m_sampleByteSize];

            uint lByteSize = (uint)m_sampleByteSize;

            try
            {

                lByteSize = await mISampleGrabberCall.readDataAsync(lData);
            }
            catch (Exception)
            {

            }
            finally
            {

                if (lByteSize > 0)
                {
                    if (mIsMJPG)
                    {
                        Stream imageStreamSource = new MemoryStream(lData, 0, (int)lByteSize, false);
                        JpegBitmapDecoder decoder = new JpegBitmapDecoder(imageStreamSource, BitmapCreateOptions.PreservePixelFormat, BitmapCacheOption.Default);
                        BitmapSource bitmapSource = decoder.Frames[0];

                        mDisplayImage.Source = bitmapSource;
                    }
                    else
                    {
                        mDisplayImage.Source = FromArray(lData, mVideoWidth, mVideoHeight, 4);
                    }
                }
            }

        }

        private async void Window_Loaded(object sender, RoutedEventArgs e)
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

            if (mCaptureManager == null)
                return;

            XmlDataProvider lXmlDataProvider = (XmlDataProvider)this.Resources["XmlLogProvider"];

            if (lXmlDataProvider == null)
                return;

            System.Xml.XmlDocument doc = new System.Xml.XmlDocument();

            string lxmldoc = await mCaptureManager.getCollectionOfSourcesAsync();

            if (string.IsNullOrEmpty(lxmldoc))
                return;

            doc.LoadXml(lxmldoc);

            lXmlDataProvider.Document = doc;

        }
    }
}
