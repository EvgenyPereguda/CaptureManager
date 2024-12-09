using CaptureManagerToWinRTProxy;
using PhotoDisplayComponent;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Data.Xml.Dom;
using Windows.Foundation;
using Windows.Foundation.Metadata;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media.Imaging;

namespace CameraSession
{    
    sealed class CameraSession: ICameraSession
    {        
        private uint mIndex;

        private uint mWidth;

        private uint mHeight;

        private string mSymbolicLink;

        private string mSubType;

        private ISourceControl mISourceControl;

        private CameraDisplayPanel mCameraDisplayPanel = new CameraDisplayPanel();

        private ISession mISession = null;


        Guid MFMediaType_Video = new Guid(0x73646976, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

        Guid MFVideoFormat_RGB32 = new Guid(22, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

        Guid MFVideoFormat_MJPG = new Guid(0x47504A4D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

        public static CameraSession createICameraSession(
            IXmlNode aIXmlNode, 
            IPhotoDisplayControl aIPhotoDisplayControl)
        {
            CameraSession lresult = null;

            do
            {               

                CameraSession lCameraSession = new CameraSession();

                if (!lCameraSession.init(aIXmlNode, aIPhotoDisplayControl))
                    break;

                lresult = lCameraSession;

            } while (false);

            return lresult;
        }

        private CameraSession()
        {
            mCameraDisplayPanel.StartRecordEvent += mCameraDisplayPanel_StartRecordEvent;

            mCameraDisplayPanel.StopRecordEvent += mCameraDisplayPanel_StopRecordEvent;
        }

        void mCameraDisplayPanel_StopRecordEvent()
        {
            stop();

            close();

            if (initViewSession(mSymbolicLink, mSubType))
            {
                start();

            }
        }


        void mCameraDisplayPanel_StartRecordEvent()
        {
            stop();

            close();

            if (initRecordSession(mSymbolicLink, mSubType))
            {
                start();
            }
        }
        
        private bool init(
            IXmlNode aIXmlNode, 
            IPhotoDisplayControl aIPhotoDisplayControl)
        {
            bool lresult = false;

            do
            {
                var lISourceControl = CaptureManager.getInstance().getISourceControl();

                if(lISourceControl == null)
                    break;

                mISourceControl = lISourceControl;

                var lStreamDescriptorXmlNodeList = aIXmlNode.SelectNodes("PresentationDescriptor/StreamDescriptor");

                if (lStreamDescriptorXmlNodeList.Length == 0)
                    break;

                var lMediaTypeXmlNodeList = lStreamDescriptorXmlNodeList[0].SelectNodes("MediaTypes/MediaType");

                uint lIndex = 0;
                uint lWidth = 0;
                uint lHeight = 0;

                if (!findIndexMaxResolution(
                    ref lIndex,
                    ref lWidth,
                    ref lHeight,
                    lMediaTypeXmlNodeList))
                    break;

                mIndex = lIndex;
                mWidth = lWidth;
                mHeight = lHeight;

                var lMediaTypeXmlNode = lMediaTypeXmlNodeList.Item(mIndex);

                if (lMediaTypeXmlNode == null)
                    break;

                var lAttrSubTypeXmlNode = lMediaTypeXmlNode.SelectSingleNode("MediaTypeItem[@Name='MF_MT_SUBTYPE']/SingleValue/@Value");

                if (lAttrSubTypeXmlNode == null)
                    break;

                var lSubType = lAttrSubTypeXmlNode.NodeValue.ToString();

                if (lSubType == null)
                    break;

                
                var lSymbolicLink = aIXmlNode.SelectSingleNode("Source.Attributes/Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK']/SingleValue/@Value");

                if (lSymbolicLink == null || lSymbolicLink.NodeValue == null)
                    break;

                

                mSymbolicLink = lSymbolicLink.NodeValue.ToString();

                mSubType = lSubType;

                if (!initViewSession(lSymbolicLink.NodeValue.ToString(), lSubType))
                    break;

                if (mCameraDisplayPanel != null)
                {
                    mCameraDisplayPanel.PhotoDisplayControl = aIPhotoDisplayControl;

                    mCameraDisplayPanel.processControl(lSymbolicLink.NodeValue.ToString());
                }

                lresult = true;
                
            } while (false);

            return lresult;
        }
        
        private bool findIndexMaxResolution(
            ref uint aRefIndex,
            ref uint aRefWidth,
            ref uint aRefHeight,
            XmlNodeList aMediaTypeXmlNodeList)
        {
            bool lresult = false;

            do
            {
                if (aMediaTypeXmlNodeList == null)
                    break;


                float lmaxFrameRate = 0.0f;
                
                for (uint lIndex = 0; lIndex < aMediaTypeXmlNodeList.Length; lIndex++)
                {
                    var lMediaTypeXmlNode = aMediaTypeXmlNodeList.Item(lIndex);

                    var MediaTypeItem = lMediaTypeXmlNode.SelectSingleNode("MediaTypeItem[@Name='MF_MT_FRAME_RATE']/RatioValue/@Value");

                    if (MediaTypeItem != null && MediaTypeItem.NodeValue != null)
                    {
                        float ltemp = 0.0f;

                        if (float.TryParse(MediaTypeItem.NodeValue.ToString(), out ltemp))
                        {
                            if (lmaxFrameRate < ltemp)
                                lmaxFrameRate = ltemp;
                        }
                    }
                }

                if (lmaxFrameRate == 0.0f)
                    break;

                string xpath = "MediaTypeItem[@Name='MF_MT_FRAME_RATE']/RatioValue[@Value='" + lmaxFrameRate.ToString() + "']";

                string xpathWidth = "MediaTypeItem[@Name='MF_MT_FRAME_SIZE']/Value.ValueParts/ValuePart[1]/@Value";

                int lmaxWidth = -1;

                IXmlNode lMaxlMediaTypeXmlNode = null;

                for (uint lIndex = 0; lIndex < aMediaTypeXmlNodeList.Length; lIndex++)
                {
                    var lMediaTypeXmlNode = aMediaTypeXmlNodeList.Item(lIndex);

                    var MediaTypeItem = lMediaTypeXmlNode.SelectSingleNode(xpath);

                    if (MediaTypeItem == null)
                        continue;

                    MediaTypeItem = lMediaTypeXmlNode.SelectSingleNode(xpathWidth);

                    if (MediaTypeItem != null && MediaTypeItem.NodeValue != null)
                    {
                        int ltemp = -1;

                        if (int.TryParse(MediaTypeItem.NodeValue.ToString(), out ltemp))
                        {
                            if (lmaxWidth < ltemp)
                            {
                                lmaxWidth = ltemp;

                                lMaxlMediaTypeXmlNode = lMediaTypeXmlNode;
                            }
                        }
                    }

                }

                if (lMaxlMediaTypeXmlNode == null)
                    break;

                var lattr = lMaxlMediaTypeXmlNode.SelectSingleNode("@Index");

                uint ltempValue;

                if (!uint.TryParse(lattr.NodeValue.ToString(), out ltempValue))
                    break;

                aRefIndex = ltempValue;

                lattr = lMaxlMediaTypeXmlNode.SelectSingleNode("MediaTypeItem[@Name='MF_MT_FRAME_SIZE']/Value.ValueParts/ValuePart[1]/@Value");
                
                if (!uint.TryParse(lattr.NodeValue.ToString(), out ltempValue))
                    break;

                aRefWidth = ltempValue;




                lattr = lMaxlMediaTypeXmlNode.SelectSingleNode("MediaTypeItem[@Name='MF_MT_FRAME_SIZE']/Value.ValueParts/ValuePart[2]/@Value");

                if (!uint.TryParse(lattr.NodeValue.ToString(), out ltempValue))
                    break;

                aRefHeight = ltempValue;
                
                lresult = true;
                                               
            } while (false);

            return lresult;
        }

        private bool initViewSession(string aSymbolicLink, string aSubType)
        {
            bool lresult = false;

            do
            {
                int lWidthInBytes = 0;

                var lIStrideForBitmap = CaptureManager.getInstance().getIStrideForBitmap();

                if (lIStrideForBitmap == null)
                    break;

                if (!lIStrideForBitmap.getStrideForBitmap(
                     MFVideoFormat_RGB32,
                     mWidth,
                     out lWidthInBytes))
                    break;

                uint lsampleByteSize = (uint)Math.Abs(lWidthInBytes) * mHeight;
                
                var lISinkControl = CaptureManager.getInstance().getISinkControl();

                if (lISinkControl == null)
                    break;

                var lXMLString = lISinkControl.getCollectionOfSinks();

                if (lXMLString == null)
                    break;


                XmlDocument doc = new XmlDocument();

                doc.LoadXml(lXMLString);


                // create RenderOutputNode
                var lSinkFactoryXmlNode = doc.SelectSingleNode("SinkFactories/SinkFactory[@GUID='{3D64C48E-EDA4-4EE1-8436-58B64DD7CF13}']");

                if (lSinkFactoryXmlNode == null)
                    break;

                var lModeXmlNodeList = lSinkFactoryXmlNode.SelectNodes("Value.ValueParts/ValuePart");

                if (lSinkFactoryXmlNode == null)
                    break;

                var lCount = lModeXmlNodeList.Length;

                if (lCount == 0)
                    break;

                var lModeSinkFactory = lModeXmlNodeList.Item(0).Attributes.GetNamedItem("GUID").NodeValue.ToString();

                if (lSinkFactoryXmlNode == null)
                    break;

                Guid lMode;

                if (!Guid.TryParse(lModeSinkFactory, out lMode))
                    break;

                var lISampleGrabberCallbackSinkFactory = lISinkControl.createISampleGrabberCallbackSinkFactory(
                    lMode);

                if (lISampleGrabberCallbackSinkFactory == null)
                    break;
                
                var lRenderOutputNode = lISampleGrabberCallbackSinkFactory.createOutputNode(
                    MFMediaType_Video,
                    MFVideoFormat_RGB32,
                    (int)mWidth,
                    (int)mHeight,
                    mCameraDisplayPanel);

                if (lRenderOutputNode == null)
                    break;




                // create RenderOutputNode

                lSinkFactoryXmlNode = doc.SelectSingleNode("SinkFactories/SinkFactory[@GUID='{759D24FF-C5D6-4B65-8DDF-8A2B2BECDE39}']");

                if (lSinkFactoryXmlNode == null)
                    break;

                var lContainerNode = lSinkFactoryXmlNode.SelectSingleNode("Value.ValueParts/ValuePart[3]");

                if (lContainerNode == null)
                    break;

                lModeSinkFactory = lContainerNode.Attributes.GetNamedItem("GUID").NodeValue.ToString();

                if (lModeSinkFactory == null)
                    break;

                if (!Guid.TryParse(lModeSinkFactory, out lMode))
                    break;

                var lISampleGrabberCallSinkFactory = lISinkControl.createISampleGrabberCallSinkFactory(
                    lMode);

                if (lISampleGrabberCallSinkFactory == null)
                    break;

                ISampleGrabberCall aISampleGrabberCall = null;

                TakePhotoSampleGrabberCall mTakePhotoSampleGrabberCall = new TakePhotoSampleGrabberCall();

                if (aSubType == "MFVideoFormat_MJPG")
                {
                    aISampleGrabberCall = lISampleGrabberCallSinkFactory.createSampleGrabberCall(
                        MFMediaType_Video,
                        MFVideoFormat_MJPG,
                        lsampleByteSize);

                    mTakePhotoSampleGrabberCall.mIsMJPG = true;

                }
                else
                {
                    aISampleGrabberCall = lISampleGrabberCallSinkFactory.createSampleGrabberCall(
                        MFMediaType_Video,
                        MFVideoFormat_RGB32,
                        lsampleByteSize);

                    mTakePhotoSampleGrabberCall.mIsMJPG = false;
                }

                if(aISampleGrabberCall == null)
                    break;

                mTakePhotoSampleGrabberCall.mISampleGrabberCall = aISampleGrabberCall;

                mTakePhotoSampleGrabberCall.mBufferSize = lsampleByteSize;
                

                mTakePhotoSampleGrabberCall.mImageWidth = mWidth;

                mTakePhotoSampleGrabberCall.mImageHeight = mHeight;

                mCameraDisplayPanel.takePhotoSampleGrabberCall = mTakePhotoSampleGrabberCall;

                
                
                //  create Spread node

                var lIStreamControl = CaptureManager.getInstance().getIStreamControl();

                if (lIStreamControl == null)
                    break;
                
                ISpreaderNodeFactory lSpreaderNodeFactory = lIStreamControl.createSpreaderNodeFactory();

                if (lSpreaderNodeFactory == null)
                    break;



                // Bind output nodes with one source node

                IList<ITopologyNode> lOutputNodeList = new List<ITopologyNode>();

                lOutputNodeList.Add(lRenderOutputNode);

                lOutputNodeList.Add(aISampleGrabberCall.getTopologyNode());

                var lSpreaderNode = lSpreaderNodeFactory.createSpreaderNode(lOutputNodeList);

                if (lSpreaderNode == null)
                    break;



                var lISourceControl = CaptureManager.getInstance().getISourceControl();

                if (lISourceControl == null)
                    break;

                var lSourceNode = lISourceControl.createSourceNodeWithDownStreamConnection(
                     aSymbolicLink,
                     0,
                     mIndex,
                     lSpreaderNode);

                if (lSourceNode == null)
                    break;





                var lISessionControl = CaptureManager.getInstance().getISessionControl();

                if (lISessionControl == null)
                    break;

                IList<ITopologyNode> lArrayPtrSourceNodesOfTopology = new List<ITopologyNode>();

                lArrayPtrSourceNodesOfTopology.Add(lSourceNode);

                mISession = lISessionControl.createSession(lArrayPtrSourceNodesOfTopology);

                if (mISession == null)
                    break;

                lresult = true;
                
            } while (false);

            return lresult;
            
        }

        private bool initRecordSession(string aSymbolicLink, string aSubType)
        {
            bool lresult = false;

            do
            {
                int lWidthInBytes = 0;

                var lIStrideForBitmap = CaptureManager.getInstance().getIStrideForBitmap();

                if (lIStrideForBitmap == null)
                    break;

                if (!lIStrideForBitmap.getStrideForBitmap(
                     MFVideoFormat_RGB32,
                     mWidth,
                     out lWidthInBytes))
                    break;

                uint lsampleByteSize = (uint)Math.Abs(lWidthInBytes) * mHeight;

                var lISinkControl = CaptureManager.getInstance().getISinkControl();

                if (lISinkControl == null)
                    break;

                var lXMLString = lISinkControl.getCollectionOfSinks();

                if (lXMLString == null)
                    break;


                XmlDocument doc = new XmlDocument();

                doc.LoadXml(lXMLString);


                // create RenderOutputNode
                var lSinkFactoryXmlNode = doc.SelectSingleNode("SinkFactories/SinkFactory[@GUID='{3D64C48E-EDA4-4EE1-8436-58B64DD7CF13}']");

                if (lSinkFactoryXmlNode == null)
                    break;

                var lModeXmlNodeList = lSinkFactoryXmlNode.SelectNodes("Value.ValueParts/ValuePart");

                if (lSinkFactoryXmlNode == null)
                    break;

                var lCount = lModeXmlNodeList.Length;

                if (lCount == 0)
                    break;

                var lModeSinkFactory = lModeXmlNodeList.Item(0).Attributes.GetNamedItem("GUID").NodeValue.ToString();

                if (lSinkFactoryXmlNode == null)
                    break;

                Guid lMode;

                if (!Guid.TryParse(lModeSinkFactory, out lMode))
                    break;

                var lISampleGrabberCallbackSinkFactory = lISinkControl.createISampleGrabberCallbackSinkFactory(
                    lMode);

                if (lISampleGrabberCallbackSinkFactory == null)
                    break;

                var lRenderOutputNode = lISampleGrabberCallbackSinkFactory.createOutputNode(
                    MFMediaType_Video,
                    MFVideoFormat_RGB32,
                    (int)mWidth,
                    (int)mHeight,
                    mCameraDisplayPanel);

                if (lRenderOutputNode == null)
                    break;




                // create Take photo aISampleGrabberCall

                lSinkFactoryXmlNode = doc.SelectSingleNode("SinkFactories/SinkFactory[@GUID='{759D24FF-C5D6-4B65-8DDF-8A2B2BECDE39}']");

                if (lSinkFactoryXmlNode == null)
                    break;

                var lContainerNode = lSinkFactoryXmlNode.SelectSingleNode("Value.ValueParts/ValuePart[3]");

                if (lContainerNode == null)
                    break;

                lModeSinkFactory = lContainerNode.Attributes.GetNamedItem("GUID").NodeValue.ToString();

                if (lModeSinkFactory == null)
                    break;

                if (!Guid.TryParse(lModeSinkFactory, out lMode))
                    break;

                var lISampleGrabberCallSinkFactory = lISinkControl.createISampleGrabberCallSinkFactory(
                    lMode);

                if (lISampleGrabberCallSinkFactory == null)
                    break;

                ISampleGrabberCall aISampleGrabberCall = null;

                TakePhotoSampleGrabberCall mTakePhotoSampleGrabberCall = new TakePhotoSampleGrabberCall();

                if (aSubType == "MFVideoFormat_MJPG")
                {
                    aISampleGrabberCall = lISampleGrabberCallSinkFactory.createSampleGrabberCall(
                        MFMediaType_Video,
                        MFVideoFormat_MJPG,
                        lsampleByteSize);

                    mTakePhotoSampleGrabberCall.mIsMJPG = true;

                }
                else
                {
                    aISampleGrabberCall = lISampleGrabberCallSinkFactory.createSampleGrabberCall(
                        MFMediaType_Video,
                        MFVideoFormat_RGB32,
                        lsampleByteSize);

                    mTakePhotoSampleGrabberCall.mIsMJPG = false;
                }

                if (aISampleGrabberCall == null)
                    break;

                mTakePhotoSampleGrabberCall.mISampleGrabberCall = aISampleGrabberCall;

                mTakePhotoSampleGrabberCall.mBufferSize = lsampleByteSize;


                mTakePhotoSampleGrabberCall.mImageWidth = mWidth;

                mTakePhotoSampleGrabberCall.mImageHeight = mHeight;

                mCameraDisplayPanel.takePhotoSampleGrabberCall = mTakePhotoSampleGrabberCall;



                // create Source node
                var lISourceControl = CaptureManager.getInstance().getISourceControl();

                if (lISourceControl == null)
                    break;

                var lSourceMediaType = lISourceControl.getSourceOutputMediaType(
                     aSymbolicLink,
                     0,
                     mIndex);

                if (lSourceMediaType == null)
                    break;



                // create Encoder Media Type
                var lIEncoderControl = CaptureManager.getInstance().getIEncoderControl();

                if (lIEncoderControl == null)
                    break;

                Guid lASFEncoderGUID;

                if (!Guid.TryParse("{7e320092-596a-41b2-bbeb-175d10504eb6}", out lASFEncoderGUID))
                    break;

                var lEncoderNodeFactory = lIEncoderControl.createEncoderNodeFactory(lASFEncoderGUID);

                if (lEncoderNodeFactory == null)
                    break;

                if (!Guid.TryParse("{ee8c3745-f45b-42b3-a8cc-c7a696440955}", out lMode))
                    break;
                                
                var lCompressedMediaType = lEncoderNodeFactory.createCompressedMediaType(
                    lSourceMediaType,
                    lMode,
                    85,
                    0);

                if (lCompressedMediaType == null)
                    break;





                
                // create file sink record video lIFileSinkFactory
                
                if (!Guid.TryParse("{A2A56DA1-EB84-460E-9F05-FEE51D8C81E3}", out lMode))
                    break;

                var lIFileSinkFactory = lISinkControl.createIFileSinkFactory(
                    lMode);

                if (lIFileSinkFactory == null)
                    break;

                IList<ITopologyNode> aArrayOutputNodes = new List<ITopologyNode>();

                IList<IMediaType> aArrayPtrCompressedMediaTypes = new List<IMediaType>();

                aArrayPtrCompressedMediaTypes.Add(lCompressedMediaType);


                StorageFolder localFolder = ApplicationData.Current.LocalFolder;

		        var lLogFilePath = localFolder.Path + "\\video.asf";



                lIFileSinkFactory.createOutputNodes(
                    aArrayPtrCompressedMediaTypes,
                    lLogFilePath,
                    aArrayOutputNodes);

               

                // create Encoder node

                if (!Guid.TryParse("{ee8c3745-f45b-42b3-a8cc-c7a696440955}", out lMode))
                    break;

                var lEncoderNode = lEncoderNodeFactory.createEncoderNode(
                    lSourceMediaType,
                    lMode,
                    85,
                    0,
                    aArrayOutputNodes[0]);

                if (lEncoderNode == null)
                    break;




                //  create Spread node

                var lIStreamControl = CaptureManager.getInstance().getIStreamControl();

                if (lIStreamControl == null)
                    break;

                ISpreaderNodeFactory lSpreaderNodeFactory = lIStreamControl.createSpreaderNodeFactory();

                if (lSpreaderNodeFactory == null)
                    break;



                // Bind output nodes with one source node

                IList<ITopologyNode> lOutputNodeList = new List<ITopologyNode>();

                lOutputNodeList.Add(lRenderOutputNode);

                lOutputNodeList.Add(lEncoderNode);//aISampleGrabberCall.getTopologyNode());

                var lSpreaderNode = lSpreaderNodeFactory.createSpreaderNode(lOutputNodeList);

                if (lSpreaderNode == null)
                    break;

                //lOutputNodeList.Clear();

                //lOutputNodeList.Add(lEncoderNode);

                //lOutputNodeList.Add(lSpreaderNode);


                //lSpreaderNode = lSpreaderNodeFactory.createSpreaderNode(lOutputNodeList);

                //if (lSpreaderNode == null)
                //    break;



                // create Source node

                var lSourceNode = lISourceControl.createSourceNodeWithDownStreamConnection(
                     aSymbolicLink,
                     0,
                     mIndex,
                     lSpreaderNode);

                if (lSourceNode == null)
                    break;





                var lISessionControl = CaptureManager.getInstance().getISessionControl();

                if (lISessionControl == null)
                    break;

                IList<ITopologyNode> lArrayPtrSourceNodesOfTopology = new List<ITopologyNode>();

                lArrayPtrSourceNodesOfTopology.Add(lSourceNode);

                mISession = lISessionControl.createSession(lArrayPtrSourceNodesOfTopology);

                if (mISession == null)
                    break;

                lresult = true;

            } while (false);

            return lresult;

        }
        
        public Windows.UI.Xaml.Controls.Page getDisplayPage()
        {
            return mCameraDisplayPanel;
        }

        public void start()
        {
            do
            {
                if (mISession == null)
                    break;

                mISession.startSession(0, Guid.Empty);
                
            } while (false);
        }

        public void stop()
        {
            do
            {
                if (mISession == null)
                    break;

                mISession.stopSession();

            } while (false);
        }

        public void close()
        {
            do
            {
                if (mISession == null)
                    break;

                mISession.closeSession();

            } while (false);
        }


        public void hideElements()
        {
            if (mCameraDisplayPanel != null)
                mCameraDisplayPanel.hideElements();
        }

        public void showElements()
        {
            if (mCameraDisplayPanel != null)
                mCameraDisplayPanel.showElements();
        }
    }
}
