using System;
using System.Collections.Generic;
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
using CaptureManagerToCSharpProxy;
using CaptureManagerToCSharpProxy.Interfaces;
using System.Windows.Threading;
using System.Xml;
using Microsoft.Win32;
using System.Reflection;
using System.Diagnostics;
using System.Threading;
using System.IO;
using System.Globalization;
using System.ComponentModel;
using System.Collections.ObjectModel;

namespace WPFVideoAndAudioRecorder
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
	
	
    public class SubTypeNameConverter : IValueConverter
    {
        public object Convert(object value, Type targetType,
            object parameter, CultureInfo culture)
        {
            string l_result = value as String;

            if(l_result != null)
            {
                l_result = l_result.Replace("MFVideoFormat_", "");
                l_result = l_result.Replace("MFAudioFormat_", "");
            }

            return l_result;
        }

        public object ConvertBack(object value, Type targetType,
            object parameter, CultureInfo culture)
        {
            return null;
        }
    }
    
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

        IEVRSinkFactory mEVRSinkFactory = null;

        bool mIsStarted = false;

        string mFilename = null;
        
        public MainWindow()
        {
            mMediaTypesViewSource.Source = mMediaTypeCollection;

            mSubTypesViewSource.Source = mSubTypesCollection;

            InitializeComponent();
        }

        private object mCurrentSource = null;
        
        public object CurrentSource
        {
            get => this.mCurrentSource;
            set
            {
                this.mCurrentSource = value;
                createGroupSubType(value);
            }
        }

        private object mCurrentSubType = null;

        public object CurrentSubType
        {
            get => this.mCurrentSubType;
            set
            {
                this.mCurrentSubType = value;
                createGroupMediaTypes(value);
            }
        }

        ObservableCollection<string> mSubTypesCollection = new ObservableCollection<string>();

        CollectionViewSource mSubTypesViewSource = new CollectionViewSource();

        public ICollectionView SubTypes { get => mSubTypesViewSource.View; }

        ObservableCollection<XmlNode> mMediaTypeCollection = new ObservableCollection<XmlNode>();

        CollectionViewSource mMediaTypesViewSource = new CollectionViewSource();

        public ICollectionView MediaTypes { get => mMediaTypesViewSource.View; }

        private void createGroupSubType(object aCurrentSource)
        {
            var lCurrentSourceNode = aCurrentSource as XmlNode;

            if (lCurrentSourceNode == null)
                return;

            var lSubTypesNode = lCurrentSourceNode.SelectNodes("PresentationDescriptor/StreamDescriptor/MediaTypes/MediaType/MediaTypeItem[@Name='MF_MT_SUBTYPE']/SingleValue/@Value");

            if (lSubTypesNode == null)
                return;

            mSubTypesCollection.Clear();

            foreach (XmlNode item in lSubTypesNode)
            {
                if(!mSubTypesCollection.Contains(item.Value))
                    mSubTypesCollection.Add(item.Value);
            }
        }

        private void createGroupMediaTypes(object aCurrentSubType)
        {
            var lCurrentSubType = aCurrentSubType as string;

            var lCurrentSourceNode = mCurrentSource as XmlNode;

            if (lCurrentSourceNode == null)
                return;

            var lMediaTypesNode = lCurrentSourceNode.SelectNodes("PresentationDescriptor/StreamDescriptor/MediaTypes/MediaType[MediaTypeItem[@Name='MF_MT_SUBTYPE']/SingleValue[@Value='" + lCurrentSubType + "']]");

            if (lMediaTypesNode == null)
                return;

            mMediaTypeCollection.Clear();

            foreach (XmlNode item in lMediaTypesNode)
            {
                mMediaTypeCollection.Add(item);
            }
        }

        private void MainWindow_WriteDelegateEvent(string aMessage)
        {
            MessageBox.Show(aMessage);
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            var t = new Thread(

               delegate ()
               {

                   try
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

                       mSinkControl.createSinkFactory(Guid.Empty, out mEVRMultiSinkFactory);

                       if (mEVRMultiSinkFactory == null)
                           return;

                       mSinkControl.createSinkFactory(Guid.Empty, out mEVRSinkFactory);

                       if (mEVRSinkFactory == null)
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
                
                uint lStreamIndex = 0;
                
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
                
                var t = new Thread(

                   delegate ()
                   {

                       try
                       {
                           Dispatcher.Invoke(
                           DispatcherPriority.Normal,
                           new Action(() => {
                               m_VideoEncodingModeComboBox.IsEnabled = false;
                           }));

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
                            return;



                           XmlDataProvider lXmlEncoderModeDataProvider = (XmlDataProvider)this.Resources["XmlEncoderModeProvider"];

                           if (lXmlEncoderModeDataProvider == null)
                               return;

                           XmlDocument lEncoderModedoc = new XmlDocument();

                           lEncoderModedoc.LoadXml(lMediaTypeCollection);

                           lXmlEncoderModeDataProvider.Document = lEncoderModedoc;


                           Dispatcher.Invoke(
                           DispatcherPriority.Normal,
                           new Action(() => {
                               m_VideoEncodingModeComboBox.IsEnabled = true;
                           }));
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

            } while (false);
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

                var t = new Thread(

                   delegate ()
                   {

                       try
                       {

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
                               return;



                           XmlDataProvider lXmlEncoderModeDataProvider = (XmlDataProvider)this.Resources["XmlAudioEncoderModeProvider"];

                           if (lXmlEncoderModeDataProvider == null)
                               return;

                           XmlDocument lEncoderModedoc = new XmlDocument();

                           lEncoderModedoc.LoadXml(lMediaTypeCollection);

                           lXmlEncoderModeDataProvider.Document = lEncoderModedoc;

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


            } while (false);
        }

        private void m_SelectFileBtn_Click(object sender, RoutedEventArgs e)
        {
            do
            {
            var lselectedNode = m_FileFormatComboBox.SelectedItem as XmlNode;

            if (lselectedNode == null)
                break;

            var lSelectedAttr = lselectedNode.Attributes["Value"];

            if (lSelectedAttr == null)
                break;

            String limageSourceDir = System.IO.Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);

            SaveFileDialog lsaveFileDialog = new SaveFileDialog();

            lsaveFileDialog.InitialDirectory = limageSourceDir;

            lsaveFileDialog.DefaultExt = "." + lSelectedAttr.Value.ToLower();

            lsaveFileDialog.AddExtension = true;

            lsaveFileDialog.CheckFileExists = false;

            lsaveFileDialog.Filter = "Media file (*." + lSelectedAttr.Value.ToLower() + ")|*." + lSelectedAttr.Value.ToLower();

            var lresult = lsaveFileDialog.ShowDialog();

            if (lresult != true)
                break;

            mFilename = lsaveFileDialog.FileName;
                
            lSelectedAttr = lselectedNode.Attributes["GUID"];

            if (lSelectedAttr == null)
                break;

                var t = new Thread(

                   delegate ()
                   {

                       try
                       {

                           mSinkControl.createSinkFactory(
                           Guid.Parse(lSelectedAttr.Value),
                           out mFileSinkFactory);
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

            m_StartStopBtn.IsEnabled = true;
                                
            } while (false);

        }

        private void m_StartStopBtn_Click(object sender, RoutedEventArgs e)
        {
            m_StartStopBtn.IsEnabled = false;

            if (mIsStarted)
            {
                mIsStarted = false;

                var tl = new Thread(

                   delegate ()
                   {

                       try
                       {

                           if (mISession == null)
                               return;

                           mISession.stopSession();

                           mISession.closeSession();

                           mISession = null;

                           Dispatcher.Invoke(
                           DispatcherPriority.Normal,
                           new Action(() => {
                               m_BtnTxtBlk.Text = "Start";

                               m_StartStopBtn.IsEnabled = true;
                           }));
                       }
                       catch (Exception ex)
                       {
                       }
                       finally
                       {
                       }
                   });
                tl.SetApartmentState(ApartmentState.MTA);

                tl.Start();

                return;
            }

            var l_videoStreamEnabled = (bool)m_VideoStreamChkBtn.IsChecked && m_VideoCompressedMediaTypesComboBox.SelectedIndex > -1;

            var l_previewEnabled = (bool)m_VideoStreamPreviewChkBtn.IsChecked;

            bool l_IsWithoutEncoder = m_SubTypeTxtBlk.Text == "MFVideoFormat_H264";

            if (l_IsWithoutEncoder)
                l_videoStreamEnabled = l_IsWithoutEncoder;

            var l_VideoSourceXmlNode = m_VideoSourceComboBox.SelectedItem as XmlNode;
            var l_VideoStreamXmlNode = m_VideoStreamComboBox.SelectedItem as XmlNode;
            var l_VideoSourceMediaTypeXmlNode = m_VideoSourceMediaTypeComboBox.SelectedItem as XmlNode;
            var l_VideoEncodersXmlNode = m_VideoEncodersComboBox.SelectedItem as XmlNode;
            var l_VideoEncodingModeXmlNode = m_VideoEncodingModeComboBox.SelectedItem as XmlNode;
            var l_VideoCompressedMediaTypeSelectedIndex = m_VideoCompressedMediaTypesComboBox.SelectedIndex;


            var l_audioStreamEnabled = (bool)m_AudioStreamChkBtn.IsChecked && m_AudioCompressedMediaTypesComboBox.SelectedIndex > -1;


            var l_AudioSourceXmlNode = m_AudioSourceComboBox.SelectedItem as XmlNode;
            var l_AudioStreamXmlNode = m_AudioStreamComboBox.SelectedItem as XmlNode;
            var l_AudioSourceMediaTypeXmlNode = m_AudioSourceMediaTypeComboBox.SelectedItem as XmlNode;
            var l_AudioEncodersXmlNode = m_AudioEncodersComboBox.SelectedItem as XmlNode;
            var l_AudioEncodingModeXmlNode = m_AudioEncodingModeComboBox.SelectedItem as XmlNode;
            var l_AudioCompressedMediaTypeSelectedIndexXmlNode = m_AudioCompressedMediaTypesComboBox.SelectedIndex;

            var lHandle = m_EVRDisplay.Handle;


            object RenderNode = null;

            if (l_previewEnabled)
            {
                if (mEVRSinkFactory != null)
                    mEVRSinkFactory.createOutputNode(
                        lHandle,
                        out RenderNode);
                
            }

            var t = new Thread(

               delegate ()
               {

                   try
                   {

                       List<object> lCompressedMediaTypeList = new List<object>();

                       if (l_videoStreamEnabled)
                       {
                           object lCompressedMediaType = getCompressedMediaType(
                                   l_VideoSourceXmlNode,
                                   l_VideoStreamXmlNode,
                                   l_VideoSourceMediaTypeXmlNode,
                                   l_VideoEncodersXmlNode,
                                   l_VideoEncodingModeXmlNode,
                                   l_VideoCompressedMediaTypeSelectedIndex,
                                   l_IsWithoutEncoder);

                           if (lCompressedMediaType != null)
                               lCompressedMediaTypeList.Add(lCompressedMediaType);
                       }

                       if (l_audioStreamEnabled)
                       {
                           object lCompressedMediaType = getCompressedMediaType(
                                    l_AudioSourceXmlNode,
                                    l_AudioStreamXmlNode,
                                    l_AudioSourceMediaTypeXmlNode,
                                    l_AudioEncodersXmlNode,
                                    l_AudioEncodingModeXmlNode,
                                    l_AudioCompressedMediaTypeSelectedIndexXmlNode);

                           if (lCompressedMediaType != null)
                               lCompressedMediaTypeList.Add(lCompressedMediaType);
                       }

                       List<object> lOutputNodes = getOutputNodes(lCompressedMediaTypeList);

                       if (lOutputNodes == null || lOutputNodes.Count == 0)
                           return;


                       int lOutputIndex = 0;

                       List<object> lSourceNodes = new List<object>();

                       if (l_videoStreamEnabled)
                       {



                           object lSourceNode = getSourceNode(
                                   l_VideoSourceXmlNode,
                                   l_VideoStreamXmlNode,
                                   l_VideoSourceMediaTypeXmlNode,
                                   l_VideoEncodersXmlNode,
                                   l_VideoEncodingModeXmlNode,
                                   l_VideoCompressedMediaTypeSelectedIndex,
                                   RenderNode,
                                   lOutputNodes[lOutputIndex++],
                                   l_IsWithoutEncoder);

                           if (lSourceNodes != null)
                               lSourceNodes.Add(lSourceNode);
                       }

                       if (l_audioStreamEnabled)
                       {
                           object lSourceNode = getSourceNode(
                            l_AudioSourceXmlNode,
                            l_AudioStreamXmlNode,
                            l_AudioSourceMediaTypeXmlNode,
                            l_AudioEncodersXmlNode,
                            l_AudioEncodingModeXmlNode,
                            l_AudioCompressedMediaTypeSelectedIndexXmlNode,
                           null,
                               lOutputNodes[lOutputIndex++]);

                           if (lSourceNodes != null)
                               lSourceNodes.Add(lSourceNode);
                       }

                       mISession = mISessionControl.createSession(lSourceNodes.ToArray());

                       if (mISession == null)
                           return;

                       if (mISession.startSession(0, Guid.Empty))
                       {
                           Dispatcher.Invoke(
                           DispatcherPriority.Normal,
                           new Action(() => {
                               m_BtnTxtBlk.Text = "Stop";

                               m_StartStopBtn.IsEnabled = true;
                           }));
                       }

                       mIsStarted = true;


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

        private object getCompressedMediaType(
            XmlNode aSourceNode,
            XmlNode aStreamNode,
            XmlNode aMediaTypeNode,
            XmlNode aEncoderNode,
            XmlNode aEncoderModeNode,
            int aCompressedMediaTypeIndex,
            bool aIsWithoutEncoder = false)
        {
            object lresult = null;

            do
            {
                if (aSourceNode == null)
                    break;

                               
                if (aMediaTypeNode == null)
                    break;

                if (aIsWithoutEncoder)
                {
                    var lNode1 = aSourceNode.SelectSingleNode(
                "Source.Attributes/Attribute" +
                "[@Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK' or @Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK']" +
                "/SingleValue/@Value");

                    if (lNode1 == null)
                        break;

                    string lSymbolicLink1 = lNode1.Value;
                                        
                    uint lStreamIndex1 = 0;


                    if (aMediaTypeNode == null)
                        break;

                    lNode1 = aMediaTypeNode.SelectSingleNode("@Index");

                    if (lNode1 == null)
                        break;

                    uint lMediaTypeIndex1 = 0;

                    if (!uint.TryParse(lNode1.Value, out lMediaTypeIndex1))
                    {
                        break;
                    }

                    object lSourceMediaType1 = null;

                    if (!mSourceControl.getSourceOutputMediaType(
                        lSymbolicLink1,
                        lStreamIndex1,
                        lMediaTypeIndex1,
                        out lSourceMediaType1))
                        break;

                    if (lSourceMediaType1 == null)
                        break;

                    lresult = lSourceMediaType1;

                    break;
                }

                if (aCompressedMediaTypeIndex < 0)
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


                var lNode = aSourceNode.SelectSingleNode(
            "Source.Attributes/Attribute" +
            "[@Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK' or @Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK']" +
            "/SingleValue/@Value");

                if (lNode == null)
                    break;

                string lSymbolicLink = lNode.Value;
                
                uint lStreamIndex = 0;
                                
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

                object lCompressedMediaType;

                if (!lEncoderNodeFactory.createCompressedMediaType(
                    lSourceMediaType,
                    lCLSIDEncoderMode,
                    50,
                    (uint)aCompressedMediaTypeIndex,
                    out lCompressedMediaType))
                    break;

                lresult = lCompressedMediaType;
                
            } while (false);                    

            return lresult;
        }

        private List<object> getOutputNodes(List<object> aCompressedMediaTypeList)
        {
            List<object> lresult = new List<object>();

            do
            {
                if (aCompressedMediaTypeList == null)
                    break;

                if (aCompressedMediaTypeList.Count == 0)
                    break;

                if (mFileSinkFactory == null)
                    break;

                if(string.IsNullOrEmpty(mFilename))
                    break;

                mFileSinkFactory.createOutputNodes(
                    aCompressedMediaTypeList,
                    mFilename,
                    out lresult);
                
            } while (false);

            return lresult;
        }
        
        private object getSourceNode(
            XmlNode aSourceNode,
            XmlNode aStreamNode,
            XmlNode aMediaTypeNode,
            XmlNode aEncoderNode,
            XmlNode aEncoderModeNode,
            int aCompressedMediaTypeIndex,
            object PreviewRenderNode,
            object aOutputNode,
            bool aIsWithoutEncoder = false)
        {
            object lresult = null;

            do
            {
                if (aSourceNode == null)
                    break;
                

                if (aMediaTypeNode == null)
                    break;


                if (aIsWithoutEncoder)
                {
                    var lNode1 = aSourceNode.SelectSingleNode(
                "Source.Attributes/Attribute" +
                "[@Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK' or @Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK']" +
                "/SingleValue/@Value");

                    if (lNode1 == null)
                        break;

                    string lSymbolicLink1 = lNode1.Value;
                    
                    uint lStreamIndex1 = 0;
                    
                    if (aMediaTypeNode == null)
                        break;

                    lNode1 = aMediaTypeNode.SelectSingleNode("@Index");

                    if (lNode1 == null)
                        break;

                    uint lMediaTypeIndex1 = 0;

                    if (!uint.TryParse(lNode1.Value, out lMediaTypeIndex1))
                    {
                        break;
                    }
                                       
                    object SpreaderNode1 = aOutputNode;

                    if (PreviewRenderNode != null)
                    {
                        List<object> lOutputNodeList = new List<object>();
                        
                        lOutputNodeList.Add(PreviewRenderNode);

                        lOutputNodeList.Add(aOutputNode);

                        mSpreaderNodeFactory.createSpreaderNode(
                            lOutputNodeList,
                            out SpreaderNode1);

                    }

                    object lSourceNode1;

                    string lextendSymbolicLink1 = lSymbolicLink1 + " --options=" +
        "<?xml version='1.0' encoding='UTF-8'?>" +
        "<Options>" +
            "<Option Type='Cursor' Visiblity='True'>" +
                "<Option.Extensions>" +
                    "<Extension Type='BackImage' Height='100' Width='100' Fill='0x7055ff55' />" +
                "</Option.Extensions>" +
            "</Option>" +
        "</Options>";

                    if (!mSourceControl.createSourceNode(
                        lextendSymbolicLink1,
                        lStreamIndex1,
                        lMediaTypeIndex1,
                        SpreaderNode1,
                        out lSourceNode1))
                        break;

                    lresult = lSourceNode1;

                    break;
                }

                if (aCompressedMediaTypeIndex < 0)
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
                                
                var lNode = aSourceNode.SelectSingleNode(
            "Source.Attributes/Attribute" +
            "[@Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK' or @Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK']" +
            "/SingleValue/@Value");

                if (lNode == null)
                    break;

                string lSymbolicLink = lNode.Value;
                
                uint lStreamIndex = 0;
                
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

                object lEncoderNode;

                if (!lEncoderNodeFactory.createEncoderNode(
                    lSourceMediaType,
                    lCLSIDEncoderMode,
                    50,
                    (uint)aCompressedMediaTypeIndex,
                    aOutputNode,
                    out lEncoderNode))
                    break;


                object SpreaderNode = lEncoderNode;

                if(PreviewRenderNode != null)
                {

                    List<object> lOutputNodeList = new List<object>();

                    lOutputNodeList.Add(PreviewRenderNode);

                    lOutputNodeList.Add(lEncoderNode);

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

                lresult = lSourceNode;

            } while (false);

            return lresult;
        }     
    }
}
