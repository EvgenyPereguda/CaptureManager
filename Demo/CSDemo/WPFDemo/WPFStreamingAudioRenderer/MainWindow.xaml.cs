/*
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

using CaptureManagerToCSharpProxy;
using CaptureManagerToCSharpProxy.Interfaces;
using System;
using System.Collections.Generic;
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

namespace WPFStreamingAudioRenderer
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

        ISARSinkFactory mSARSinkFactory = null;

        ISARVolumeControl mISARVolumeControl = null;

        public MainWindow()
        {
            InitializeComponent();

            try
            {
                mCaptureManager = new CaptureManager("CaptureManager.dll");
            }
            catch (Exception)
            {
                mCaptureManager = new CaptureManager();
            }

            LogManager.getInstance().WriteDelegateEvent += MainWindow_WriteDelegateEvent;

            if (mCaptureManager == null)
                return;

            mSourceControl = mCaptureManager.createSourceControl();

            if (mSourceControl == null)
                return;
            
            mSinkControl = mCaptureManager.createSinkControl();

            if (mSinkControl == null)
                return;

            mISessionControl = mCaptureManager.createSessionControl();

            if (mISessionControl == null)
                return;

            mISARVolumeControl = mCaptureManager.createSARVolumeControl();

            if (mISARVolumeControl == null)
                return;



            XmlDataProvider lXmlDataProvider = (XmlDataProvider)this.Resources["XmlSources"];

            if (lXmlDataProvider == null)
                return;

            XmlDocument doc = new XmlDocument();

            string lxmldoc = "";

            mCaptureManager.getCollectionOfSources(ref lxmldoc);

            doc.LoadXml(lxmldoc);

            lXmlDataProvider.Document = doc;



            mSinkControl.createSinkFactory(Guid.Empty, out mSARSinkFactory);

        }

        private void MainWindow_WriteDelegateEvent(string aMessage)
        {
            MessageBox.Show(aMessage);
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (mISession != null)
            {
                mISession.stopSession();

                mISession.closeSession();

                mISession = null;
                
                mTitleTxtBlk.Text = "Start playing";

                return;
            }
                                 
            List<object> lSourceNodes = new List<object>();

            object lSARSinkOutputNode = null;

            mSARSinkFactory.createOutputNode(out lSARSinkOutputNode);

            if (lSARSinkOutputNode == null)
                return;


            var l_AudioSourceXmlNode = m_AudioSourceComboBox.SelectedItem as XmlNode;

            if (l_AudioSourceXmlNode == null)
                return;

            var lNode = l_AudioSourceXmlNode.SelectSingleNode(
"Source.Attributes/Attribute" +
"[@Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK']" +
"/SingleValue/@Value");

            if (lNode == null)
                return;

            string lSymbolicLink = lNode.Value;

            object lSourceNode = null;

            mSourceControl.createSourceNode(
                lSymbolicLink,
                0,
                0,
                lSARSinkOutputNode,
                out lSourceNode);

            if (lSourceNode == null)
                return;

            lSourceNodes.Add(lSourceNode);

            mISession = mISessionControl.createSession(lSourceNodes.ToArray());

            if (mISession == null)
                return;

            mISession.registerUpdateStateDelegate(UpdateStateDelegate);

            if (mISession.startSession(0, Guid.Empty))
            {
                mTitleTxtBlk.Text = "Stop playing";

                uint lChannelCount = 0;

                mISARVolumeControl.getChannelCount(lSARSinkOutputNode, out lChannelCount);

                if(lChannelCount > 0)
                {
                    float lLevel = 0;

                    mISARVolumeControl.getChannelVolume(lSARSinkOutputNode, 0, out lLevel);
                    
                    mRVolume.ValueChanged += (s,ev)=> {
                        mISARVolumeControl.setChannelVolume(lSARSinkOutputNode, 0, (float)ev.NewValue);
                    };

                    mRVolume.Value = lLevel;

                    if (lChannelCount > 1)
                    {
                        lLevel = 0;

                        mISARVolumeControl.getChannelVolume(lSARSinkOutputNode, 1, out lLevel);

                        mLVolume.ValueChanged += (s, ev) => {
                            mISARVolumeControl.setChannelVolume(lSARSinkOutputNode, 1, (float)ev.NewValue);
                        };

                        mLVolume.Value = lLevel;
                    }
                }
            }
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


                    }
                    break;
                default:
                    break;
            }
        }

    }
}
