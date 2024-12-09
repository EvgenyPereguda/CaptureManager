using CaptureManagerToCSharpProxy.Interfaces;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

namespace WPFImageRecorder
{
    class ImageCaptureProcessor : ICaptureProcessor
    {
                static Guid MFVideoFormat_RGB24 = new Guid(
 20, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

        static Guid MFVideoFormat_RGB32 = new Guid(
 22, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

        string mPresentationDescriptor = "";


        byte[] mPixels = null;

        IntPtr mRawData = IntPtr.Zero;

        private ImageCaptureProcessor() { }

        static public ICaptureProcessor createCaptureProcessor()
        {

            string lPresentationDescriptor = "<?xml version='1.0' encoding='UTF-8'?>" +
            "<PresentationDescriptor StreamCount='1'>" +
                "<PresentationDescriptor.Attributes Title='Attributes of Presentation'>" +
                    "<Attribute Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK' GUID='{58F0AAD8-22BF-4F8A-BB3D-D2C4978C6E2F}' Title='The symbolic link for a video capture driver.' Description='Contains the unique symbolic link for a video capture driver.'>" +
                        "<SingleValue Value='ImageCaptureProcessor' />" +
                    "</Attribute>" +
                    "<Attribute Name='MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME' GUID='{60D0E559-52F8-4FA2-BBCE-ACDB34A8EC01}' Title='The display name for a device.' Description='The display name is a human-readable string, suitable for display in a user interface.'>" + 
                        "<SingleValue Value='Image Capture Processor' />" +
                    "</Attribute>" +
                "</PresentationDescriptor.Attributes>" + 
                "<StreamDescriptor Index='0' MajorType='MFMediaType_Video' MajorTypeGUID='{73646976-0000-0010-8000-00AA00389B71}'>" + 
                    "<MediaTypes TypeCount='1'>" + 
                        "<MediaType Index='0'>" +
                            "<MediaTypeItem Name='MF_MT_FRAME_SIZE' GUID='{1652C33D-D6B2-4012-B834-72030849A37D}' >" +
                                "<Value.ValueParts>" +
                                    "<ValuePart Title='Width' Value='720' />" +
                                    "<ValuePart Title='Height' Value='540' />" +
                                "</Value.ValueParts>" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_AVG_BITRATE' GUID='{20332624-FB0D-4D9E-BD0D-CBF6786C102E}' >" +
                                "<SingleValue Value='186624000' />" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_YUV_MATRIX' GUID='{3E23D450-2C75-4D25-A00E-B91670D12327}' >" +
                                "<SingleValue Value='MFVideoTransferMatrix_BT601' />" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_MAJOR_TYPE' GUID='{48EBA18E-F8C9-4687-BF11-0A74C9F96A8F}' >" +
                                   "<SingleValue Value='MFMediaType_Video' GUID='{73646976-0000-0010-8000-00AA00389B71}' />" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_VIDEO_LIGHTING' GUID='{53A0529C-890B-4216-8BF9-599367AD6D20}' >" +
                                   "<SingleValue Value='MFVideoLighting_dim' />" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_DEFAULT_STRIDE' GUID='{644B4E48-1E02-4516-B0EB-C01CA9D49AC6}' >" +
                             "<SingleValue Value='0' />" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_VIDEO_CHROMA_SITING' GUID='{65DF2370-C773-4C33-AA64-843E068EFB0C}' >" +
                                   "<SingleValue Value='MFVideoChromaSubsampling_DV_PAL' />" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_AM_FORMAT_TYPE' GUID='{73D1072D-1870-4174-A063-29FF4FF6C11E}' >" +
                             "<SingleValue Value='{F72A76A0-EB0A-11D0-ACE4-0000C0CC16BA}' GUID='{F72A76A0-EB0A-11D0-ACE4-0000C0CC16BA}' />" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_FIXED_SIZE_SAMPLES' GUID='{B8EBEFAF-B718-4E04-B0A9-116775E3321B}' >" +
                             "<SingleValue Value='True' />" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_VIDEO_NOMINAL_RANGE' GUID='{C21B8EE5-B956-4071-8DAF-325EDF5CAB11}' >" +
                                   "<SingleValue Value='MFNominalRange_16_235' />" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_FRAME_RATE' GUID='{C459A2E8-3D2C-4E44-B132-FEE5156C7BB0}' >" +
                                   "<RatioValue Value='10' >" +
                                       "<Value.ValueParts>" +
                                           "<ValuePart Title='Numerator' Value='10' />" +
                               "<ValuePart Title='Denominator' Value='1' />" +
                                       "</Value.ValueParts>" +
                                   "</RatioValue>" +
                               "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_PIXEL_ASPECT_RATIO' GUID='{C6376A1E-8D0A-4027-BE45-6D9A0AD39BB6}' >" +
                             "<RatioValue Value='1'>" +
                                       "<Value.ValueParts>" +
                                           "<ValuePart Title='Numerator' Value='1' />" +
                               "<ValuePart Title='Denominator' Value='1' />" +
                                       "</Value.ValueParts>" +
                                   "</RatioValue>" +
                               "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_ALL_SAMPLES_INDEPENDENT' GUID='{C9173739-5E56-461C-B713-46FB995CB95F}' >" +
                             "<SingleValue Value='True' />" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_FRAME_RATE_RANGE_MIN' GUID='{D2E7558C-DC1F-403F-9A72-D28BB1EB3B5E}' >" +
                             "<RatioValue Value='10'>" +
                                       "<Value.ValueParts>" +
                                           "<ValuePart Title='Numerator' Value='10' />" +
                               "<ValuePart Title='Denominator' Value='1' />" +
                                       "</Value.ValueParts>" +
                                   "</RatioValue>" +
                               "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_SAMPLE_SIZE' GUID='{DAD3AB78-1990-408B-BCE2-EBA673DACC10}' >" +
                             "<SingleValue Value='777600' />" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_VIDEO_PRIMARIES' GUID='{DBFBE4D7-0740-4EE0-8192-850AB0E21935}' >" +
                             "<SingleValue Value='MFVideoPrimaries_BT709' />" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_INTERLACE_MODE' GUID='{E2724BB8-E676-4806-B4B2-A8D6EFB44CCD}' >" +
                             "<SingleValue Value='MFVideoInterlace_Progressive' />" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_FRAME_RATE_RANGE_MAX' GUID='{E3371D41-B4CF-4A05-BD4E-20B88BB2C4D6}' >" +
                             "<RatioValue Value='10'>" +
                                       "<Value.ValueParts>" +
                                           "<ValuePart Title='Numerator' Value='10' />" +
                               "<ValuePart Title='Denominator' Value='1' />" +
                                       "</Value.ValueParts>" +
                                   "</RatioValue>" +
                               "</MediaTypeItem>" +
                        "<MediaTypeItem Name='MF_MT_SUBTYPE' GUID='{F7E34C9A-42E8-4714-B74B-CB29D72C35E5}' >" +
                         "<SingleValue Value='MFVideoFormat_YUY2' GUID='{32595559-0000-0010-8000-00AA00389B71}' />" +
                        "</MediaTypeItem>" +
                        "</MediaType>" +
                    "</MediaTypes>" +
                "</StreamDescriptor>" +
            "</PresentationDescriptor>";

            ImageCaptureProcessor lICaptureProcessor = new ImageCaptureProcessor();
                       
            lICaptureProcessor.mPixels = new byte[777600];

            for (int i = 0; i < lICaptureProcessor.mPixels.Length; i++)
            {
                lICaptureProcessor.mPixels[i] = 250;
            }

            lICaptureProcessor.mPresentationDescriptor = lPresentationDescriptor;

            return lICaptureProcessor;
        }
        

        public void initilaize(IInitilaizeCaptureSource IInitilaizeCaptureSource)
        {
            if (IInitilaizeCaptureSource != null)
            {
                IInitilaizeCaptureSource.setPresentationDescriptor(mPresentationDescriptor);
            }
        }

        public void pause()
        {
        }

        public void setCurrentMediaType(ICurrentMediaType aICurrentMediaType)
        {
            if (aICurrentMediaType == null)
                throw new NotImplementedException();

            uint lStreamIndex = 0;

            uint lMediaTypeIndex = 0;

            aICurrentMediaType.getStreamIndex(out lStreamIndex);

            aICurrentMediaType.getMediaTypeIndex(out lMediaTypeIndex);

            if (lStreamIndex != 0 || lMediaTypeIndex != 0)
                throw new NotImplementedException();
        }

        public void shutdown()
        {
            if (mRawData != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(mRawData);

                mRawData = IntPtr.Zero;
            }
        }

        public void sourceRequest(ISourceRequestResult aISourceRequestResult)
        {
            System.Threading.Thread.Sleep(100);

            if (aISourceRequestResult == null)
                return;

            uint lStreamIndex = 0;

            aISourceRequestResult.getStreamIndex(out lStreamIndex);

            if (lStreamIndex == 0)
                aISourceRequestResult.setData(mRawData, (uint)mPixels.Length, 1);
        }

        public void start(long aStartPositionInHundredNanosecondUnits, ref Guid aGUIDTimeFormat)
        {

            if (mPixels == null)
                return;

            if (mRawData != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(mRawData);

                mRawData = IntPtr.Zero;
            }

            mRawData = Marshal.AllocHGlobal(mPixels.Length);
            
            Marshal.Copy(mPixels, 0, mRawData, mPixels.Length);                        
        }

        public void stop()
        {
        }
    }
}
