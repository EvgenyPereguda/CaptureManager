using CaptureManagerToCSharpProxy;
using CaptureManagerToCSharpProxy.Interfaces;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Xml;

namespace WPFIRCamShot
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public static CaptureManager mCaptureManager;

        ISampleGrabberCallSinkFactory mSinkFactory = null;

        ISession mISession = null;
        
        Guid MFMediaType_Video = new Guid(
 0x73646976, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

        Guid MFVideoFormat_RGB32 = new Guid(
 22, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

        Guid MFVideoFormat_MJPG = new Guid(
 0x47504A4D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
        
        uint mVideoWidth = 0;

        uint mVideoHeight = 0;

        int mChannels = 0;
        
        Guid mReadMode;

        ISampleGrabberCall mISampleGrabberCall;
        
        uint lsampleByteSize;

        byte[] mData = null;

        byte[] mGray8Data = null;

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

            XmlDataProvider lXmlDataProvider = (XmlDataProvider)this.Resources["XmlSourceProvider"];

            if (lXmlDataProvider == null)
                return;

            System.Xml.XmlDocument doc = new System.Xml.XmlDocument();

            string lxmldoc = "";

            mCaptureManager.getCollectionOfSources(ref lxmldoc);

            if (string.IsNullOrEmpty(lxmldoc))
                return;

            doc.LoadXml(lxmldoc);

            lXmlDataProvider.Document = doc;            
        }
        
        private void mLaunchButton_Click(object sender, RoutedEventArgs e)
        {
            if (mLaunchButton.Content.ToString() == "Stop")
            {
                //mTimer.Stop();

                if (mISession != null)
                    mISession.closeSession();

                mLaunchButton.Content = "Launch";

                mTakePhotoButton.IsEnabled = false;

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

            mGray8Data = new byte[lVideoHeight * lVideoWidth];

            lNode = lSourceNode.SelectSingleNode("MediaTypeItem[@Name='MF_MT_SAMPLE_SIZE']/SingleValue/@Value");

            if (lNode == null)
                return;

            lsampleByteSize = 0;

            if (!uint.TryParse(lNode.Value, out lsampleByteSize))
            {
                return;
            }
            
            mData = new byte[lsampleByteSize];

            var lSinkControl = mCaptureManager.createSinkControl();

            string lxmldoc = "";

            mCaptureManager.getCollectionOfSinks(ref lxmldoc);

            XmlDocument doc = new XmlDocument();

            doc.LoadXml(lxmldoc);

            var lSinkNode = doc.SelectSingleNode("SinkFactories/SinkFactory[@GUID='{759D24FF-C5D6-4B65-8DDF-8A2B2BECDE39}']");

            if (lSinkNode == null)
                return;

            var lContainerNode = lSinkNode.SelectSingleNode("Value.ValueParts/ValuePart[3]");

            if (lContainerNode == null)
                return;

            setContainerFormat(lContainerNode);

            lSinkControl.createSinkFactory(
            mReadMode,
            out mSinkFactory);

            lNode = lSourceNode.SelectSingleNode("MediaTypeItem[@Name='MF_MT_SUBTYPE']/SingleValue/@GUID");

            if (lNode == null)
                return;

            Guid lFormat = Guid.Empty;

            bool lr = Guid.TryParse(lNode.Value, out lFormat);

            if (!lr)
                return;

            if (lFormat != Guid.Empty)
            {
                mSinkFactory.createOutputNode(
                    MFMediaType_Video,
                    lFormat,
                    lsampleByteSize,
                    out mISampleGrabberCall);
            }

            if (mISampleGrabberCall != null)
            {
                var lSampleGrabberCallNode = mISampleGrabberCall.getTopologyNode();

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

                if (!mISession.startSession(0, Guid.Empty))
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
            if (mLaunchButton.Content.ToString() == "Stop")
            {
                //mTimer.Stop();

                if (mISession != null)
                    mISession.closeSession();

                mLaunchButton.Content = "Launch";

                return;
            }
        }

        private void mTakePhotoButton_Click(object sender, RoutedEventArgs e)
        {
            if (mISampleGrabberCall == null)
                return;
            
            uint lByteSize = (uint)mData.Length;

            try
            {

                mISampleGrabberCall.readData(mData, out lByteSize);
            }
            catch (Exception)
            {

            }
            finally
            {

                if (lByteSize > 0)
                {
                    float ldiv = (float)lByteSize / (float)mGray8Data.Length;

                    if (ldiv == 2.0)
                    {
                        for (int i = 0; i < mGray8Data.Length; i++)
                        {
                            mGray8Data[i] = mData[(i * 2) + 1];
                        } 

                        mDisplayImage.Source = FromArray(mGray8Data, mVideoWidth, mVideoHeight, 1);
                    }
                    else if (ldiv == 1.0)
                    {
                        mDisplayImage.Source = FromArray(mData, mVideoWidth, mVideoHeight, 1);
                    }
                }
            }

        }
    }
}
