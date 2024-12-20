﻿/*
MIT License
Copyright(c) 2020 Evgeny Pereguda
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

using System;
using System.Collections.Generic;
using System.Windows;
using CaptureManagerToCSharpProxy;
using CaptureManagerToCSharpProxy.Interfaces;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Threading;
using System.Xml;

namespace WPFWebViewerCallback
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        CaptureManager mCaptureManager;

        ISampleGrabberCallbackSinkFactory mSinkFactory = null;

        ISession mISession = null;

        Guid MFMediaType_Video = new Guid(
 0x73646976, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

        Guid MFVideoFormat_RGB24 = new Guid(
 20, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

        Guid MFVideoFormat_RGB32 = new Guid(
 22, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

        uint mVideoWidth = 0;

        uint mVideoHeight = 0;

        int mChannels = 3;

        IWebCamControl mWebCamControl;

        Guid mReadMode;

        uint lsampleByteSize;

        byte[] mData = null;

        bool mAccess = true;

        public MainWindow()
        {
            InitializeComponent();

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

            string lxmldoc = "";

            mCaptureManager.getCollectionOfSources(ref lxmldoc);

            if (string.IsNullOrEmpty(lxmldoc))
                return;

            doc.LoadXml(lxmldoc);

            lXmlDataProvider.Document = doc;

            mWebCamParametrsTab.AddHandler(Slider.ValueChangedEvent, new RoutedEventHandler(mParametrSlider_ValueChanged));

            mWebCamParametrsTab.AddHandler(CheckBox.CheckedEvent, new RoutedEventHandler(mParametrSlider_Checked));

            mWebCamParametrsTab.AddHandler(CheckBox.UncheckedEvent, new RoutedEventHandler(mParametrSlider_Checked));

        }

        private void mLaunchButton_Click(object sender, RoutedEventArgs e)
        {
            if (mLaunchButton.Content.ToString() == "Stop")
            {

                lock (this)
                {
                    mAccess = false;
                }

                if (mISession != null)
                    mISession.closeSession();

                mLaunchButton.Content = "Launch";

                mISession = null;

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

            lNode = lSourceNode.SelectSingleNode("MediaTypeItem[@Name='MF_MT_FRAME_SIZE']/Value.ValueParts/ValuePart[2]/@Value");

            if (lNode == null)
                return;

            uint lVideoHeight = 0;

            if (!uint.TryParse(lNode.Value, out lVideoHeight))
            {
                return;
            }

            int lWidthInBytes;

            mCaptureManager.getStrideForBitmapInfoHeader(
                MFVideoFormat_RGB24,
                lVideoWidth,
                out lWidthInBytes);

            lsampleByteSize = (uint)Math.Abs(lWidthInBytes) * lVideoHeight;

            mData = new byte[lsampleByteSize];

            var lSinkControl = mCaptureManager.createSinkControl();

            string lxmldoc = "";

            mCaptureManager.getCollectionOfSinks(ref lxmldoc);

            XmlDocument doc = new XmlDocument();

            doc.LoadXml(lxmldoc);

            var lSinkNode = doc.SelectSingleNode("SinkFactories/SinkFactory[@GUID='{3D64C48E-EDA4-4EE1-8436-58B64DD7CF13}']");

            if (lSinkNode == null)
                return;

            var lContainerNode = lSinkNode.SelectSingleNode("Value.ValueParts/ValuePart[1]");

            if (lContainerNode == null)
                return;

            setContainerFormat(lContainerNode);

            lSinkControl.createSinkFactory(
            mReadMode,
            out mSinkFactory);

            ISampleGrabberCallback lISampleGrabberCallback;

            mSinkFactory.createOutputNode(
                MFMediaType_Video,
                MFVideoFormat_RGB24,
                out lISampleGrabberCallback);



            if (lISampleGrabberCallback != null)
            {
                byte[] lData = new byte[lsampleByteSize];

                lISampleGrabberCallback.mUpdateEvent += delegate
                    (byte[] aData, uint aLength)
                {
                    lock (this)
                    {
                        if (!mAccess)
                            return;
                    }

                    Dispatcher.Invoke(
                    DispatcherPriority.Normal,
                    new Action(() => mDisplayImage.Source = FromArray(aData, lVideoWidth, lVideoHeight, mChannels)));
                };

                var lSampleGrabberCallNode = lISampleGrabberCallback.getTopologyNode();

                if (lSampleGrabberCallNode == null)
                    return;

                object lPtrSourceNode;

                var lSourceControl = mCaptureManager.createSourceControl();

                if (lSourceControl == null)
                    return;

                lSourceControl.createSourceNode(
                    lSymbolicLink,
                    lStreamIndex,
                    lMediaTypeIndex,
                    lSampleGrabberCallNode,
                    out lPtrSourceNode);

                List<object> lSourceMediaNodeList = new List<object>();

                lSourceMediaNodeList.Add(lPtrSourceNode);

                var lSessionControl = mCaptureManager.createSessionControl();

                if (lSessionControl == null)
                    return;

                mISession = lSessionControl.createSession(
                    lSourceMediaNodeList.ToArray());

                if (mISession == null)
                    return;

                mISession.startSession(0, Guid.Empty);

                mLaunchButton.Content = "Stop";

                mWebCamControl = lSourceControl.createWebCamControl(lSymbolicLink);

                if (mWebCamControl != null)
                {
                    string lXMLstring;

                    mWebCamControl.getCamParametrs(out lXMLstring);

                    XmlDataProvider lXmlDataProvider = (XmlDataProvider)this.Resources["XmlWebCamParametrsProvider"];

                    if (lXmlDataProvider == null)
                        return;

                    System.Xml.XmlDocument ldoc = new System.Xml.XmlDocument();

                    ldoc.LoadXml(lXMLstring);

                    lXmlDataProvider.Document = ldoc;
                }


                lock (this)
                {
                    mAccess = true;
                }

            }
        }

        private void updateDisplayImage(Window aWindow, byte[] aData, int aLength)
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

            var lslider = e.OriginalSource as Slider;

            if (lslider == null)
                return;

            if (!lslider.IsFocused)
                return;

            var lParametrNode = lslider.Tag as XmlNode;

            if (lParametrNode == null)
                return;

            var lAttr = lParametrNode.Attributes["Index"];

            if (lAttr == null)
                return;

            uint lindex = uint.Parse(lAttr.Value);

            int lvalue = (int)lslider.Value;

            mWebCamControl.setCamParametr(
                lindex,
                lvalue,
                2);

        }

        private void mParametrSlider_Checked(object sender, RoutedEventArgs e)
        {

            var lCheckBox = e.OriginalSource as CheckBox;

            if (lCheckBox == null)
                return;

            if (!lCheckBox.IsFocused)
                return;

            var lAttr = lCheckBox.Tag as XmlAttribute;

            if (lAttr == null)
                return;

            uint lindex = uint.Parse(lAttr.Value);

            int lvalue = (bool)lCheckBox.IsChecked ? 2 : 1;

            int lCurrentValue;
            int lMin;
            int lMax;
            int lStep;
            int lDefault;
            int lFlag;

            mWebCamControl.getCamParametr(
                lindex,
                out lCurrentValue,
                out lMin,
                out lMax,
                out lStep,
                out lDefault,
                out lFlag);

            mWebCamControl.setCamParametr(
                lindex,
                lCurrentValue,
                lvalue);

        }

        private void mShowBtn_Click(object sender, RoutedEventArgs e)
        {
            if (mShowBtn.Content.ToString() == "Show")
            {
                Canvas.SetBottom(mWebCamParametrsPanel, 0);

                mShowBtn.Content = "Hide";
            }
            else if (mShowBtn.Content.ToString() == "Hide")
            {
                Canvas.SetBottom(mWebCamParametrsPanel, -150);

                mShowBtn.Content = "Show";
            }
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

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            lock (this)
            {
                mAccess = false;
            }

            if (mISession != null)
            {
                var ltimer = new DispatcherTimer();

                ltimer.Interval = new TimeSpan(0, 0, 0, 1);

                ltimer.Tick += delegate
                (object sender1, EventArgs e1)
                {
                    if (mLaunchButton.Content.ToString() == "Stop")
                    {
                        if (mISession != null)
                        {
                            mISession.closeSession();
                        }

                        mLaunchButton.Content = "Launch";
                    }

                    mISession = null;

                    Close();

                    (sender1 as DispatcherTimer).Stop();
                };

                ltimer.Start();

                e.Cancel = true;
            }
        }
    }
}
