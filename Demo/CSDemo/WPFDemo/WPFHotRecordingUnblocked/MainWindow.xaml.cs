using CaptureManagerToCSharpProxy;
using CaptureManagerToCSharpProxy.Interfaces;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading;
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
using System.Windows.Threading;
using System.Xml;

namespace WPFHotRecordingUnblocked
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        CaptureManager mCaptureManager = null;

        ISessionControl mISessionControl = null;

        ISession mISession = null;

        ISinkControl mSinkControl = null;

        ISourceControl mSourceControl = null;

        IEncoderControl mEncoderControl = null;

        IFileSinkFactory mFileSinkFactory = null;

        IStreamControl mStreamControl = null;

        ISpreaderNodeFactory mSpreaderNodeFactory = null;

        IEVRMultiSinkFactory mEVRMultiSinkFactory = null;

        ISwitcherControl mSwitcherControl = null;

        object mVideoSourceMediaType = null;

        object mAudioSourceMediaType = null;

        object mVideoSwitcherNode = null;

        object mAudioSwitcherNode = null;

        enum State
        {
            Stopped, Started, Paused
        }

        State mState = State.Stopped;

        uint lFileCount = 0;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void MainWindow_WriteDelegateEvent(string aMessage)
        {
            MessageBox.Show(aMessage);
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            mCaptureManager = Program.mCaptureManager;

            LogManager.getInstance().WriteDelegateEvent += MainWindow_WriteDelegateEvent;

            if (mCaptureManager == null)
                return;

            mSourceControl = mCaptureManager.createSourceControl();

            if (mSourceControl == null)
                return;

            mEncoderControl = mCaptureManager.createEncoderControl();

            if (mEncoderControl == null)
                return;

            mSinkControl = mCaptureManager.createSinkControl();

            if (mSinkControl == null)
                return;

            mISessionControl = mCaptureManager.createSessionControl();

            if (mISessionControl == null)
                return;

            mStreamControl = mCaptureManager.createStreamControl();

            if (mStreamControl == null)
                return;

            mStreamControl.createStreamControlNodeFactory(ref mSpreaderNodeFactory);

            if (mSpreaderNodeFactory == null)
                return;


            var t = new Thread(

               delegate()
               {

                   try
                   {

                       mSinkControl.createSinkFactory(Guid.Empty, out mEVRMultiSinkFactory);

                       if (mEVRMultiSinkFactory == null)
                           return;



                       mSwitcherControl = mCaptureManager.createSwitcherControl();

                       if (mSwitcherControl == null)
                           return;





                       XmlDataProvider lXmlDataProvider = (XmlDataProvider)this.Resources["XmlSources"];

                       if (lXmlDataProvider == null)
                           return;

                       XmlDocument doc = new XmlDocument();

                       string lxmldoc = "";

                       mCaptureManager.getCollectionOfSources(ref lxmldoc);

                       doc.LoadXml(lxmldoc);

                       lXmlDataProvider.Document = doc;

                       lXmlDataProvider = (XmlDataProvider)this.Resources["XmlEncoders"];

                       if (lXmlDataProvider == null)
                           return;

                       doc = new XmlDocument();

                       mCaptureManager.getCollectionOfEncoders(ref lxmldoc);

                       doc.LoadXml(lxmldoc);

                       lXmlDataProvider.Document = doc;




                       mCaptureManager.getCollectionOfSinks(ref lxmldoc);


                       lXmlDataProvider = (XmlDataProvider)this.Resources["XmlContainerTypeProvider"];

                       if (lXmlDataProvider == null)
                           return;

                       doc = new XmlDocument();

                       doc.LoadXml(lxmldoc);

                       lXmlDataProvider.Document = doc;

                   }
                   catch (Exception ex)
                   {
                   }
                   finally
                   {
                   }
               });
            t.SetApartmentState(ApartmentState.MTA);

            t.Start();

        }

        void detachRecorder()
        {
            mSwitcherControl.detachSwitchers(mISession);
        }

        private void m_VideoEncodersComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            do
            {
                if (mEncoderControl == null)
                    break;

                var lselectedNode = m_VideoEncodersComboBox.SelectedItem as XmlNode;

                if (lselectedNode == null)
                    break;

                var lCLSIDEncoderAttr = lselectedNode.Attributes["CLSID"];

                if (lCLSIDEncoderAttr == null)
                    break;

                Guid lCLSIDEncoder;

                if (!Guid.TryParse(lCLSIDEncoderAttr.Value, out lCLSIDEncoder))
                    break;



                var lSourceNode = m_VideoSourceComboBox.SelectedItem as XmlNode;

                if (lSourceNode == null)
                    return;

                var lNode = lSourceNode.SelectSingleNode(
            "Source.Attributes/Attribute" +
            "[@Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK' or @Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK']" +
            "/SingleValue/@Value");

                if (lNode == null)
                    return;

                string lSymbolicLink = lNode.Value;

                lSourceNode = m_VideoStreamComboBox.SelectedItem as XmlNode;

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

                lSourceNode = m_VideoSourceMediaTypeComboBox.SelectedItem as XmlNode;

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



                object lOutputMediaType;

                if (mSourceControl == null)
                    return;

                mSourceControl.getSourceOutputMediaType(
                    lSymbolicLink,
                    lStreamIndex,
                    lMediaTypeIndex,
                    out lOutputMediaType);

                string lMediaTypeCollection;

                if (!mEncoderControl.getMediaTypeCollectionOfEncoder(
                    lOutputMediaType,
                    lCLSIDEncoder,
                    out lMediaTypeCollection))
                    break;



                XmlDataProvider lXmlEncoderModeDataProvider = (XmlDataProvider)this.Resources["XmlEncoderModeProvider"];

                if (lXmlEncoderModeDataProvider == null)
                    return;

                XmlDocument lEncoderModedoc = new XmlDocument();

                lEncoderModedoc.LoadXml(lMediaTypeCollection);

                lXmlEncoderModeDataProvider.Document = lEncoderModedoc;


            } while (false);
        }

        private string createFileName()
        {
            return string.Format("Video_{0}", ++lFileCount);
        }

        private void m_StartBtn_Click(object sender, RoutedEventArgs e)
        {
            if (mState == State.Paused && mISession != null)
            {
                mSwitcherControl.resumeSwitchers(mISession);

                mState = State.Started;

                m_PauseBtn.IsEnabled = true;

                m_StopBtn.IsEnabled = true;

                m_StartStopBtn.IsEnabled = false;

                return;
            }


            mVideoSwitcherNode = null;

            mAudioSwitcherNode = null;

            object RenderNode = null;


            var l_videoStreamEnabled = (bool)m_VideoStreamChkBtn.IsChecked;
            
            
            XmlNode l_VideoSourceXmlNode = m_VideoSourceComboBox.SelectedItem as XmlNode;
            XmlNode l_VideoStreamXmlNode = m_VideoStreamComboBox.SelectedItem as XmlNode;
            XmlNode l_VideoSourceMediaTypeXmlNode = m_VideoSourceMediaTypeComboBox.SelectedItem as XmlNode;
            XmlNode l_VideoEncoderXmlNode = m_VideoEncodersComboBox.SelectedItem as XmlNode;
            XmlNode l_VideoEncodingModeXmlNode = m_VideoEncodingModeComboBox.SelectedItem as XmlNode;
            var l_VideoCompressedMediaType = m_VideoCompressedMediaTypesComboBox.SelectedIndex;





            var l_AudioStreamEnabled = (bool)m_AudioStreamChkBtn.IsChecked;


            XmlNode l_AudioSourceXmlNode = m_AudioSourceComboBox.SelectedItem as XmlNode;
            XmlNode l_AudioStreamXmlNode = m_AudioStreamComboBox.SelectedItem as XmlNode;
            XmlNode l_AudioSourceMediaTypeXmlNode = m_AudioSourceMediaTypeComboBox.SelectedItem as XmlNode;
            XmlNode l_AudioEncoderXmlNode = m_AudioEncodersComboBox.SelectedItem as XmlNode;
            XmlNode l_AudioEncodingModeXmlNode = m_AudioEncodingModeComboBox.SelectedItem as XmlNode;
            var l_AudioCompressedMediaType = m_AudioCompressedMediaTypesComboBox.SelectedIndex;

            
            var t = new Thread(

               delegate()
               {

                   try
                   {


                       List<object> lRenderOutputNodesList = new List<object>();

                       if (mEVRMultiSinkFactory != null)
                           mEVRMultiSinkFactory.createOutputNodes(
                               m_EVRDisplay.SharedResource.SharedHandle,
                               null,
                               1,
                               out lRenderOutputNodesList);

                       if (lRenderOutputNodesList.Count == 1)
                       {
                           RenderNode = lRenderOutputNodesList[0];
                       }

                       List<object> lSourceNodes = new List<object>();

                       if (l_videoStreamEnabled)
                       {

                           mVideoSwitcherNode = createSwitcher(null);

                           if (mVideoSwitcherNode == null)
                               return;

                           var lSourceNode = getSourceNode(
                                l_VideoSourceXmlNode,
                                l_VideoStreamXmlNode,
                                l_VideoSourceMediaTypeXmlNode,
                                l_VideoEncoderXmlNode,
                                l_VideoEncodingModeXmlNode,
                                l_VideoCompressedMediaType,
                               RenderNode,
                               mVideoSwitcherNode);

                           if (lSourceNodes != null && lSourceNode != null)
                           {
                               lSourceNodes.Add(lSourceNode.Item1);

                               mVideoSourceMediaType = lSourceNode.Item2;
                           }
                           else
                               mVideoSwitcherNode = null;
                       }

                       if (l_AudioStreamEnabled)
                       {

                           mAudioSwitcherNode = createSwitcher(null);

                           if (mAudioSwitcherNode == null)
                               return;

                           var lSourceNode = getSourceNode(
                                l_AudioSourceXmlNode,
                                l_AudioStreamXmlNode,
                                l_AudioSourceMediaTypeXmlNode,
                                l_AudioEncoderXmlNode,
                                l_AudioEncodingModeXmlNode,
                                l_AudioCompressedMediaType,
                               null,
                               mAudioSwitcherNode);

                           if (lSourceNodes != null && lSourceNode != null)
                           {
                               lSourceNodes.Add(lSourceNode.Item1);

                               mAudioSourceMediaType = lSourceNode.Item2;
                           }
                           else
                               mAudioSwitcherNode = null;
                       }


                       mISession = mISessionControl.createSession(lSourceNodes.ToArray());

                       //mISession = mISessionControl.createUnblockedSession(lSourceNodes.ToArray());

                       if (mISession == null)
                           return;

                       mISession.registerUpdateStateDelegate(UpdateStateDelegate);

                       if (mISession.startSession(0, Guid.Empty))
                       {
                           mState = State.Started;


                           Dispatcher.Invoke(
                           DispatcherPriority.Normal,
                           new Action(() =>
                           {

                               mStateLabel.Visibility = System.Windows.Visibility.Collapsed;

                               m_PauseBtn.IsEnabled = true;

                               m_StopBtn.IsEnabled = true;

                               m_StartStopBtn.IsEnabled = false;

                               mSourceGrid.IsEnabled = false;
                           
                           }));
                       }


                   }
                   catch (Exception ex)
                   {
                   }
                   finally
                   {
                   }
               });
            t.SetApartmentState(ApartmentState.MTA);

            t.Start();            
        }

        void UpdateStateDelegate(uint aCallbackEventCode, uint aSessionDescriptor)
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


                        Dispatcher.Invoke(
                        DispatcherPriority.Normal,
                        new Action(() => m_StopBtn_Click(null, null)));

                    }
                    break;
                default:
                    break;
            }
        }

        private void m_PauseBtn_Click(object sender, RoutedEventArgs e)
        {
                        
            XmlNode l_VideoEncoderXmlNode = m_VideoEncodersComboBox.SelectedItem as XmlNode;
            XmlNode l_VideoEncodingModeXmlNode = m_VideoEncodingModeComboBox.SelectedItem as XmlNode;
            int l_VideoCompressedMediaTypeIndex = m_VideoCompressedMediaTypesComboBox.SelectedIndex;    
            
        
            XmlNode l_AudioEncoderXmlNode = m_AudioEncodersComboBox.SelectedItem as XmlNode;
            XmlNode l_AudioEncodingModeXmlNode = m_AudioEncodingModeComboBox.SelectedItem as XmlNode;
            int l_AudioCompressedMediaTypeIndex = m_AudioCompressedMediaTypesComboBox.SelectedIndex;



            XmlNode l_FileFormatXmlNode = m_FileFormatComboBox.SelectedItem as XmlNode;

            m_PauseBtn.IsEnabled = false;

            m_StopBtn.IsEnabled = false;

            var t = new Thread(

               delegate()
               {

                   try
                   {
                       detachRecorder();

                       attachRecorder(
                           l_VideoEncoderXmlNode,
                           l_VideoEncodingModeXmlNode,
                           l_VideoCompressedMediaTypeIndex,
                           l_AudioEncoderXmlNode,
                           l_AudioEncodingModeXmlNode,
                           l_AudioCompressedMediaTypeIndex,
                           l_FileFormatXmlNode);
                   }
                   catch (Exception ex)
                   {
                   }
                   finally
                   {

                       Dispatcher.Invoke(
                       DispatcherPriority.Normal,
                       new Action(() => {

                           m_PauseBtn.IsEnabled = true;

                           m_StopBtn.IsEnabled = true;
                       }));
                   }
               });
            t.SetApartmentState(ApartmentState.MTA);

            t.Start();
        }

        void attachRecorder(
            XmlNode a_VideoEncoderXmlNode,
            XmlNode a_VideoEncodingModeXmlNode,
            int a_VideoCompressedMediaTypeIndex,
            XmlNode a_AudioEncoderXmlNode,
            XmlNode a_AudioEncodingModeXmlNode,
            int a_AudioCompressedMediaTypeIndex,
            XmlNode a_FileFormatXmlNode)
        {
            List<object> lEncoderNodes = createEncoderNodes(
            a_VideoEncoderXmlNode,
            a_VideoEncodingModeXmlNode,
            a_VideoCompressedMediaTypeIndex,
            a_AudioEncoderXmlNode,
            a_AudioEncodingModeXmlNode,
            a_AudioCompressedMediaTypeIndex,
            a_FileFormatXmlNode);

            if (lEncoderNodes == null)
                return;

            int lencoderCount = 0;

            if (mVideoSwitcherNode != null)
                mSwitcherControl.atttachSwitcher(mVideoSwitcherNode, lEncoderNodes[lencoderCount++]);

            if (mAudioSwitcherNode != null)
                mSwitcherControl.atttachSwitcher(mAudioSwitcherNode, lEncoderNodes[lencoderCount++]);

            mSwitcherControl.resumeSwitchers(mISession);
        }

        private List<object> createEncoderNodes(
            XmlNode a_VideoEncoderXmlNode,
            XmlNode a_VideoEncodingModeXmlNode,
            int a_VideoCompressedMediaTypeIndex,
            XmlNode a_AudioEncoderXmlNode,
            XmlNode a_AudioEncodingModeXmlNode,
            int a_AudioCompressedMediaTypeIndex, 
            XmlNode a_FileFormatXmlNode
            )
        {

            List<object> lCompressedMediaTypeList = new List<object>();


            object lVideoCompressedMediaType = getCompressedMediaType(
                mVideoSourceMediaType,
                a_VideoEncoderXmlNode,
                a_VideoEncodingModeXmlNode,
                a_VideoCompressedMediaTypeIndex);
            
            if (lVideoCompressedMediaType != null)
                lCompressedMediaTypeList.Add(lVideoCompressedMediaType);


            object lAudioCompressedMediaType = getCompressedMediaType(
                mAudioSourceMediaType,
                a_AudioEncoderXmlNode,
                a_AudioEncodingModeXmlNode,
                a_AudioCompressedMediaTypeIndex);

            if (lAudioCompressedMediaType != null)
                lCompressedMediaTypeList.Add(lAudioCompressedMediaType);

            List<object> lOutputNodes = getOutputNodes(lCompressedMediaTypeList, a_FileFormatXmlNode, createFileName());

            if (lOutputNodes == null || lOutputNodes.Count == 0)
                return null;

            int lOutputIndex = 0;

            List<object> lEncoderNodes = new List<object>();

            object lEncoderNode = null;

            if (lVideoCompressedMediaType != null)
                lEncoderNode = getEncoder(
                    mVideoSourceMediaType,
                    a_VideoEncoderXmlNode,
                    a_VideoEncodingModeXmlNode,
                    a_VideoCompressedMediaTypeIndex,
                    lOutputNodes[lOutputIndex++]);

            if (lEncoderNode != null)
                lEncoderNodes.Add(lEncoderNode);

            lEncoderNode = null;

            if (lAudioCompressedMediaType != null)
                lEncoderNode = getEncoder(
                    mAudioSourceMediaType,
                    a_AudioEncoderXmlNode,
                    a_AudioEncodingModeXmlNode,
                    a_AudioCompressedMediaTypeIndex,
                    lOutputNodes[lOutputIndex++]);

            if (lEncoderNode != null)
                lEncoderNodes.Add(lEncoderNode);

            return lEncoderNodes;
        }

        private void m_StopBtn_Click(object sender, RoutedEventArgs e)
        {
            if (mState != State.Stopped)
            {

                mSourceGrid.IsEnabled = true;

                mState = State.Stopped;

                if (mISession == null)
                    return;

                mStateLabel.Visibility = System.Windows.Visibility.Collapsed;

                mISession.stopSession();

                mISession.closeSession();

                mISession = null;


                m_PauseBtn.IsEnabled = false;

                m_StopBtn.IsEnabled = false;

                m_StartStopBtn.IsEnabled = true;

                m_PauseBtn.Content = "Start recording";
            }
        }

        private object getCompressedMediaType(
            object aSourceMediaType,
            XmlNode aEncoderNode,
            XmlNode aEncoderModeNode,
            int aCompressedMediaTypeIndex)
        {
            object lresult = null;

            do
            {
                if (aCompressedMediaTypeIndex < 0)
                    break;

                if (aSourceMediaType == null)
                    break;

                if (aEncoderNode == null)
                    break;


                if (aEncoderModeNode == null)
                    break;

                var lEncoderGuidAttr = aEncoderNode.Attributes["CLSID"];

                if (lEncoderGuidAttr == null)
                    break;

                Guid lCLSIDEncoder;

                if (!Guid.TryParse(lEncoderGuidAttr.Value, out lCLSIDEncoder))
                    break;

                var lEncoderModeGuidAttr = aEncoderModeNode.Attributes["GUID"];

                if (lEncoderModeGuidAttr == null)
                    break;

                Guid lCLSIDEncoderMode;

                if (!Guid.TryParse(lEncoderModeGuidAttr.Value, out lCLSIDEncoderMode))
                    break;



                string lk = "";

                mCaptureManager.parseMediaType(aSourceMediaType, out lk);

                IEncoderNodeFactory lEncoderNodeFactory;

                if (!mEncoderControl.createEncoderNodeFactory(
                    lCLSIDEncoder,
                    out lEncoderNodeFactory))
                    break;

                if (lEncoderNodeFactory == null)
                    break;

                object lCompressedMediaType;

                if (!lEncoderNodeFactory.createCompressedMediaType(
                    aSourceMediaType,
                    lCLSIDEncoderMode,
                    50,
                    (uint)aCompressedMediaTypeIndex,
                    out lCompressedMediaType))
                    break;

                lresult = lCompressedMediaType;

            } while (false);

            return lresult;
        }

        private List<object> getOutputNodes(List<object> aCompressedMediaTypeList, XmlNode a_FileFormatXmlNode, string aFileName)
        {
            List<object> lresult = new List<object>();

            do
            {
                if (aCompressedMediaTypeList == null)
                    break;

                if (aCompressedMediaTypeList.Count == 0)
                    break;

                var lselectedNode = a_FileFormatXmlNode;

                if (lselectedNode == null)
                    break;

                var lSelectedAttr = lselectedNode.Attributes["Value"];

                if (lSelectedAttr == null)
                    break;
                aFileName += "." + lSelectedAttr.Value.ToLower();

                lSelectedAttr = lselectedNode.Attributes["GUID"];

                if (lSelectedAttr == null)
                    break;

                mFileSinkFactory = null;

                mSinkControl.createSinkFactory(
                    Guid.Parse(lSelectedAttr.Value),
                    out mFileSinkFactory);

                if (string.IsNullOrEmpty(aFileName))
                    break;


                Dispatcher.Invoke(
                DispatcherPriority.Background,
                new Action(() => mFileList.Items.Add(aFileName)));
               
                String limageSourceDir = System.IO.Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);

                mFileSinkFactory.createOutputNodes(
                    aCompressedMediaTypeList,
                    limageSourceDir + @"\" + aFileName,
                    out lresult);

            } while (false);

            return lresult;
        }

        private object getEncoder(
            object aSourceMediaType,
            XmlNode aEncoderNode,
            XmlNode aEncoderModeNode,
            int aCompressedMediaTypeIndex,
            object aOutputNode)
        {
            object lresult = null;

            do
            {
                if (aCompressedMediaTypeIndex < 0)
                    break;


                if (aSourceMediaType == null)
                    break;

                if (aEncoderNode == null)
                    break;

                if (aEncoderModeNode == null)
                    break;

                var lEncoderGuidAttr = aEncoderNode.Attributes["CLSID"];

                if (lEncoderGuidAttr == null)
                    break;

                Guid lCLSIDEncoder;

                if (!Guid.TryParse(lEncoderGuidAttr.Value, out lCLSIDEncoder))
                    break;

                var lEncoderModeGuidAttr = aEncoderModeNode.Attributes["GUID"];

                if (lEncoderModeGuidAttr == null)
                    break;

                Guid lCLSIDEncoderMode;

                if (!Guid.TryParse(lEncoderModeGuidAttr.Value, out lCLSIDEncoderMode))
                    break;

                IEncoderNodeFactory lEncoderNodeFactory;

                if (!mEncoderControl.createEncoderNodeFactory(
                    lCLSIDEncoder,
                    out lEncoderNodeFactory))
                    break;

                if (lEncoderNodeFactory == null)
                    break;

                object lEncoderNode;

                if (!lEncoderNodeFactory.createEncoderNode(
                    aSourceMediaType,
                    lCLSIDEncoderMode,
                    50,
                    (uint)aCompressedMediaTypeIndex,
                    aOutputNode,
                    out lEncoderNode))
                    break;

                lresult = lEncoderNode;

            } while (false);

            return lresult;
        }

        private object createSwitcher(
            object aEncoderNode)
        {
            object lresult = null;

            do
            {

                ISwitcherNodeFactory lSwitcherNodeFactory = null;

                if (!mStreamControl.createStreamControlNodeFactory(ref lSwitcherNodeFactory))
                    break;

                if (lSwitcherNodeFactory == null)
                    break;

                lSwitcherNodeFactory.createSwitcherNode(aEncoderNode, out lresult);

            } while (false);

            return lresult;
        }

        private Tuple<object, object> getSourceNode(
            XmlNode aSourceNode,
            XmlNode aStreamNode,
            XmlNode aMediaTypeNode,
            XmlNode aEncoderNode,
            XmlNode aEncoderModeNode,
            int aCompressedMediaTypeIndex,
            object PreviewRenderNode,
            object aSwitcherNode)
        {
            Tuple<object, object> lresult = null;

            do
            {
                if (aCompressedMediaTypeIndex < 0)
                    break;


                if (aSourceNode == null)
                    break;


                if (aStreamNode == null)
                    break;


                if (aMediaTypeNode == null)
                    break;


                if (aEncoderNode == null)
                    break;


                if (aEncoderModeNode == null)
                    break;

                var lEncoderGuidAttr = aEncoderNode.Attributes["CLSID"];

                if (lEncoderGuidAttr == null)
                    break;

                Guid lCLSIDEncoder;

                if (!Guid.TryParse(lEncoderGuidAttr.Value, out lCLSIDEncoder))
                    break;

                var lEncoderModeGuidAttr = aEncoderModeNode.Attributes["GUID"];

                if (lEncoderModeGuidAttr == null)
                    break;

                Guid lCLSIDEncoderMode;

                if (!Guid.TryParse(lEncoderModeGuidAttr.Value, out lCLSIDEncoderMode))
                    break;



                if (aSourceNode == null)
                    break;

                var lNode = aSourceNode.SelectSingleNode(
            "Source.Attributes/Attribute" +
            "[@Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK' or @Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK']" +
            "/SingleValue/@Value");

                if (lNode == null)
                    break;

                string lSymbolicLink = lNode.Value;

                if (aStreamNode == null)
                    break;

                lNode = aStreamNode.SelectSingleNode("@Index");

                if (lNode == null)
                    break;

                uint lStreamIndex = 0;

                if (!uint.TryParse(lNode.Value, out lStreamIndex))
                {
                    break;
                }

                if (aMediaTypeNode == null)
                    break;

                lNode = aMediaTypeNode.SelectSingleNode("@Index");

                if (lNode == null)
                    break;

                uint lMediaTypeIndex = 0;

                if (!uint.TryParse(lNode.Value, out lMediaTypeIndex))
                {
                    break;
                }

                object lSourceMediaType = null;

                if (!mSourceControl.getSourceOutputMediaType(
                    lSymbolicLink,
                    lStreamIndex,
                    lMediaTypeIndex,
                    out lSourceMediaType))
                    break;

                if (lSourceMediaType == null)
                    break;

                IEncoderNodeFactory lEncoderNodeFactory;

                if (!mEncoderControl.createEncoderNodeFactory(
                    lCLSIDEncoder,
                    out lEncoderNodeFactory))
                    break;

                if (lEncoderNodeFactory == null)
                    break;

                object SpreaderNode = aSwitcherNode;

                if (PreviewRenderNode != null)
                {

                    List<object> lOutputNodeList = new List<object>();

                    lOutputNodeList.Add(PreviewRenderNode);

                    lOutputNodeList.Add(aSwitcherNode);

                    mSpreaderNodeFactory.createSpreaderNode(
                        lOutputNodeList,
                        out SpreaderNode);

                }

                object lSourceNode;

                string lextendSymbolicLink = lSymbolicLink + " --options=" +
    "<?xml version='1.0' encoding='UTF-8'?>" +
    "<Options>" +
        "<Option Type='Cursor' Visiblity='True'>" +
            "<Option.Extensions>" +
                "<Extension Type='BackImage' Height='100' Width='100' Fill='0x7055ff55' />" +
            "</Option.Extensions>" +
        "</Option>" +
    "</Options>";

                if (!mSourceControl.createSourceNode(
                    lextendSymbolicLink,
                    lStreamIndex,
                    lMediaTypeIndex,
                    SpreaderNode,
                    out lSourceNode))
                    break;

                lresult = Tuple.Create<object, object>(lSourceNode, lSourceMediaType);

            } while (false);

            return lresult;
        }

        private void m_AudioEncodersComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            do
            {
                if (mEncoderControl == null)
                    break;

                var lselectedNode = m_AudioEncodersComboBox.SelectedItem as XmlNode;

                if (lselectedNode == null)
                    break;

                var lCLSIDEncoderAttr = lselectedNode.Attributes["CLSID"];

                if (lCLSIDEncoderAttr == null)
                    break;

                Guid lCLSIDEncoder;

                if (!Guid.TryParse(lCLSIDEncoderAttr.Value, out lCLSIDEncoder))
                    break;



                var lSourceNode = m_AudioSourceComboBox.SelectedItem as XmlNode;

                if (lSourceNode == null)
                    return;

                var lNode = lSourceNode.SelectSingleNode(
            "Source.Attributes/Attribute" +
            "[@Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK' or @Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK']" +
            "/SingleValue/@Value");

                if (lNode == null)
                    return;

                string lSymbolicLink = lNode.Value;

                lSourceNode = m_AudioStreamComboBox.SelectedItem as XmlNode;

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

                lSourceNode = m_AudioSourceMediaTypeComboBox.SelectedItem as XmlNode;

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



                object lOutputMediaType;

                if (mSourceControl == null)
                    return;

                mSourceControl.getSourceOutputMediaType(
                    lSymbolicLink,
                    lStreamIndex,
                    lMediaTypeIndex,
                    out lOutputMediaType);

                string lMediaTypeCollection;

                if (!mEncoderControl.getMediaTypeCollectionOfEncoder(
                    lOutputMediaType,
                    lCLSIDEncoder,
                    out lMediaTypeCollection))
                    break;



                XmlDataProvider lXmlEncoderModeDataProvider = (XmlDataProvider)this.Resources["XmlAudioEncoderModeProvider"];

                if (lXmlEncoderModeDataProvider == null)
                    return;

                XmlDocument lEncoderModedoc = new XmlDocument();

                lEncoderModedoc.LoadXml(lMediaTypeCollection);

                lXmlEncoderModeDataProvider.Document = lEncoderModedoc;


            } while (false);
        }

    }
}
