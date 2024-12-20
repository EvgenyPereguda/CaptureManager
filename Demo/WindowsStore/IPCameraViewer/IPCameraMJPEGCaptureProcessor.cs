﻿using CaptureManagerToWinRTProxy;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Graphics.Imaging;
using Windows.System.Threading;

namespace IPCameraViewer
{

    internal class ByteArrayUtils
    {
        // Check if the array contains needle on specified position
        public static bool Compare(byte[] array, byte[] needle, int startIndex)
        {
            int needleLen = needle.Length;
            // compare
            for (int i = 0, p = startIndex; i < needleLen; i++, p++)
            {
                if (array[p] != needle[i])
                {
                    return false;
                }
            }
            return true;
        }

        // Find subarray in array
        public static int Find(byte[] array, byte[] needle, int startIndex, int count)
        {
            int needleLen = needle.Length;
            int index;

            while (count >= needleLen)
            {
                index = Array.IndexOf(array, needle[0], startIndex, count - needleLen + 1);

                if (index == -1)
                    return -1;

                int i, p;
                // check for needle
                for (i = 0, p = index; i < needleLen; i++, p++)
                {
                    if (array[p] != needle[i])
                    {
                        break;
                    }
                }

                if (i == needleLen)
                {
                    // found needle
                    return index;
                }

                count -= (index - startIndex + 1);
                startIndex = index + 1;
            }
            return -1;
        }
    }

    class IPCameraMJPEGCaptureProcessor : ICaptureProcessor
    {
        static Guid MFVideoFormat_MJPG = new Guid(
 0x47504A4D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);


        private static int bufSize = 1024 * 512;

        private static int readSize = 1024;

        string mPresentationDescriptor = "";

        HttpWebRequest req = null;
        
        private ManualResetEvent stopEvent = null;
        private ManualResetEvent reloadEvent = null;
        
        byte[] mFrameBuffer = null;

        uint mWidth = 0;

        uint mHeight = 0;

        //static private string mURL = "http://123.209.83.115:80/mjpg/video.mjpg";

        //static private string mURL = "http://144.139.80.151:8080/mjpg/video.mjpg";

        static private string mURL = "http://mx.cafesydney.com:8888/mjpg/video.mjpg";

        

        private IPCameraMJPEGCaptureProcessor() { }

        static async public Task<ICaptureProcessor> createCaptureProcessor()
        {


            string lPresentationDescriptor = "<?xml version='1.0' encoding='UTF-8'?>" +
            "<PresentationDescriptor StreamCount='1'>" +
                "<PresentationDescriptor.Attributes Title='Attributes of Presentation'>" +
                    "<Attribute Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK' GUID='{58F0AAD8-22BF-4F8A-BB3D-D2C4978C6E2F}' Title='The symbolic link for a video capture driver.' Description='Contains the unique symbolic link for a video capture driver.'>" +
                        "<SingleValue Value='MJPEGCaptureProcessor' />" +
                    "</Attribute>" +
                    "<Attribute Name='MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME' GUID='{60D0E559-52F8-4FA2-BBCE-ACDB34A8EC01}' Title='The display name for a device.' Description='The display name is a human-readable string, suitable for display in a user interface.'>" +
                        "<SingleValue Value='MJPEG Capture Processor' />" +
                    "</Attribute>" +
                "</PresentationDescriptor.Attributes>" +
                "<StreamDescriptor Index='0' MajorType='MFMediaType_Video' MajorTypeGUID='{73646976-0000-0010-8000-00AA00389B71}'>" +
                    "<MediaTypes TypeCount='1'>" +
                        "<MediaType Index='0'>" +
                            "<MediaTypeItem Name='MF_MT_FRAME_SIZE' GUID='{1652C33D-D6B2-4012-B834-72030849A37D}' Title='Width and height of the video frame.' Description='Width and height of a video frame, in pixels.'>" +
                                "<Value.ValueParts>" +
                                    "<ValuePart Title='Width' Value='Temp_Width' />" +
                                    "<ValuePart Title='Height' Value='Temp_Height' />" +
                                "</Value.ValueParts>" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_AVG_BITRATE' GUID='{20332624-FB0D-4D9E-BD0D-CBF6786C102E}' Title='Approximate data rate of the video stream.' Description='Approximate data rate of the video stream, in bits per second, for a video media type.'>" +
                                "<SingleValue  Value='33570816' />" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_MAJOR_TYPE' GUID='{48EBA18E-F8C9-4687-BF11-0A74C9F96A8F}' Title='Major type GUID for a media type.' Description='The major type defines the overall category of the media data.'>" +
                                "<SingleValue Value='MFMediaType_Video' GUID='{73646976-0000-0010-8000-00AA00389B71}' />" +
                            "</MediaTypeItem>" +
                            //"<MediaTypeItem Name='MF_MT_DEFAULT_STRIDE' GUID='{644B4E48-1E02-4516-B0EB-C01CA9D49AC6}' Title='Default surface stride.' Description='Default surface stride, for an uncompressed video media type. Stride is the number of bytes needed to go from one row of pixels to the next.'>" + 
                            //    "<SingleValue Value='Temp_Stride' />" + 
                            //"</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_FIXED_SIZE_SAMPLES' GUID='{B8EBEFAF-B718-4E04-B0A9-116775E3321B}' Title='The fixed size of samples in stream.' Description='Specifies for a media type whether the samples have a fixed size.'>" +
                                "<SingleValue Value='False' />" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_FRAME_RATE' GUID='{C459A2E8-3D2C-4E44-B132-FEE5156C7BB0}' Title='Frame rate.' Description='Frame rate of a video media type, in frames per second.'>" +
                                "<RatioValue Value='1.0'>" +
                                    "<Value.ValueParts>" +
                                        "<ValuePart Title='Numerator'  Value='1' />" +
                                        "<ValuePart Title='Denominator'  Value='1' />" +
                                    "</Value.ValueParts>" +
                                "</RatioValue>" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_PIXEL_ASPECT_RATIO' GUID='{C6376A1E-8D0A-4027-BE45-6D9A0AD39BB6}' Title='Pixel aspect ratio.' Description='Pixel aspect ratio for a video media type.'>" +
                                "<RatioValue  Value='1'>" +
                                    "<Value.ValueParts>" +
                                        "<ValuePart Title='Numerator'  Value='1' />" +
                                        "<ValuePart Title='Denominator'  Value='1' />" +
                                    "</Value.ValueParts>" +
                                "</RatioValue>" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_ALL_SAMPLES_INDEPENDENT' GUID='{C9173739-5E56-461C-B713-46FB995CB95F}' Title='Independent of samples.' Description='Specifies for a media type whether each sample is independent of the other samples in the stream.'>" +
                                "<SingleValue Value='True' />" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_SAMPLE_SIZE' GUID='{DAD3AB78-1990-408B-BCE2-EBA673DACC10}' Title='The fixed size of each sample in stream.' Description='Specifies the size of each sample, in bytes, in a media type.'>" +
                                "<SingleValue Value='Temp_SampleSize' />" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_INTERLACE_MODE' GUID='{E2724BB8-E676-4806-B4B2-A8D6EFB44CCD}' Title='Describes how the frames are interlaced.' Description='Describes how the frames in a video media type are interlaced.'>" +
                                "<SingleValue Value='MFVideoInterlace_Progressive' />" +
                            "</MediaTypeItem>" +
                            "<MediaTypeItem Name='MF_MT_SUBTYPE' GUID='{F7E34C9A-42E8-4714-B74B-CB29D72C35E5}' Title='Subtype GUID for a media type.' Description='The subtype GUID defines a specific media format type within a major type.'>" +
                                "<SingleValue GUID='{Temp_SubTypeGUID}' />" +
                            "</MediaTypeItem>" +
                        "</MediaType>" +
                    "</MediaTypes>" +
                "</StreamDescriptor>" +
            "</PresentationDescriptor>";

            IPCameraMJPEGCaptureProcessor lICaptureProcessor = new IPCameraMJPEGCaptureProcessor();
                        
            {
                HttpWebRequest req = (HttpWebRequest)WebRequest.Create(IPCameraMJPEGCaptureProcessor.mURL);

                var resp = await req.GetResponseAsync();



                // check content type
                string ct = resp.ContentType;
                if (ct.IndexOf("multipart/x-mixed-replace") == -1)
                    return null;

                byte[] boundary = null;

                int boundaryLen, delimiterLen = 0, delimiter2Len = 0;

                Stream stream = null;

                // get boundary



                UTF8Encoding encoding = new UTF8Encoding();
                boundary = encoding.GetBytes(ct.Substring(ct.IndexOf("boundary=", 0) + 9));
                boundaryLen = boundary.Length;

                // get response stream
                stream = resp.GetResponseStream();

                int read, todo = 0, total = 0, pos = 0, align = 1;
                int start = 0, stop = 0;

                byte[] buffer = new byte[bufSize];

                byte[] delimiter = null;
                byte[] delimiter2 = null;

                MemoryStream lMemoryStream = null;

                do
                {
                    if ((read = stream.Read(buffer, total, readSize)) == 0)
                        return null;

                    total += read;
                    todo += read;

                    // does we know the delimiter ?
                    if (delimiter == null)
                    {
                        // find boundary
                        pos = ByteArrayUtils.Find(buffer, boundary, pos, todo);

                        if (pos == -1)
                        {
                            // was not found
                            todo = boundaryLen - 1;
                            pos = total - todo;
                            continue;
                        }

                        todo = total - pos;

                        if (todo < 2)
                            continue;

                        // check new line delimiter type
                        if (buffer[pos + boundaryLen] == 10)
                        {
                            delimiterLen = 2;
                            delimiter = new byte[2] { 10, 10 };
                            delimiter2Len = 1;
                            delimiter2 = new byte[1] { 10 };
                        }
                        else
                        {
                            delimiterLen = 4;
                            delimiter = new byte[4] { 13, 10, 13, 10 };
                            delimiter2Len = 2;
                            delimiter2 = new byte[2] { 13, 10 };
                        }

                        pos += boundaryLen + delimiter2Len;
                        todo = total - pos;
                    }

                    // search for image
                    if (align == 1)
                    {
                        start = ByteArrayUtils.Find(buffer, delimiter, pos, todo);
                        if (start != -1)
                        {
                            // found delimiter
                            start += delimiterLen;
                            pos = start;
                            todo = total - pos;
                            align = 2;
                        }
                        else
                        {
                            // delimiter not found
                            todo = delimiterLen - 1;
                            pos = total - todo;
                        }
                    }

                    bool lout = false;

                    // search for image end
                    while ((align == 2) && (todo >= boundaryLen))
                    {
                        stop = ByteArrayUtils.Find(buffer, boundary, pos, todo);
                        if (stop != -1)
                        {
                            pos = stop;
                            todo = total - pos;
                            
                            lMemoryStream = new MemoryStream(buffer, start, stop - start);
                            
                            BitmapDecoder decoder = await BitmapDecoder.CreateAsync(lMemoryStream.AsRandomAccessStream()).AsTask().ConfigureAwait(false);

                            BitmapFrame frame = await decoder.GetFrameAsync(0).AsTask().ConfigureAwait(false);

                            if (frame != null)
                            {
                                lICaptureProcessor.mWidth = decoder.PixelWidth;
                                lICaptureProcessor.mHeight = decoder.PixelHeight;

                                lout = true;

                                break;
                            };

                                                                                    
                            // shift array
                            pos = stop + boundaryLen;
                            todo = total - pos;

                            total = todo;
                            pos = 0;
                            align = 1;
                        }
                        else
                        {
                            // delimiter not found
                            todo = boundaryLen - 1;
                            pos = total - todo;
                        }
                    }

                    if (lout)
                        break;

                } while (true);

                stream.Dispose();

                resp.Dispose();

                
                int lStride = 0;

                lock (lICaptureProcessor)
                {
                    if (lICaptureProcessor.mFrameBuffer != null)
                    {
                        lICaptureProcessor.mFrameBuffer = null;
                    }

                    lICaptureProcessor.mFrameBuffer = new byte[stop - start];

                    Array.Copy(buffer, start, lICaptureProcessor.mFrameBuffer, 0, lICaptureProcessor.mFrameBuffer.Length);
                }




                
                lPresentationDescriptor = lPresentationDescriptor.Replace("Temp_Width", ((uint)lICaptureProcessor.mWidth).ToString());

                lPresentationDescriptor = lPresentationDescriptor.Replace("Temp_Height", ((uint)lICaptureProcessor.mHeight).ToString());

                lPresentationDescriptor = lPresentationDescriptor.Replace("Temp_Stride", lStride.ToString());

                lPresentationDescriptor = lPresentationDescriptor.Replace("Temp_SampleSize", ((uint)(lICaptureProcessor.mWidth * lICaptureProcessor.mHeight * 4)).ToString());

                lPresentationDescriptor = lPresentationDescriptor.Replace("Temp_SubTypeGUID", MFVideoFormat_MJPG.ToString());                    

                lICaptureProcessor.mPresentationDescriptor = lPresentationDescriptor;
            }


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

            lStreamIndex = aICurrentMediaType.getStreamIndex();

            lMediaTypeIndex = aICurrentMediaType.getMediaTypeIndex();

            if (lStreamIndex != 0 || lMediaTypeIndex != 0)
                throw new NotImplementedException();
        }

        public void shutdown()
        {
            Free();

            mFrameBuffer = null;
        }
        
        public void sourceRequest(ISourceRequestResult aISourceRequestResult)
        {
            if (aISourceRequestResult == null)
                return;

            uint lStreamIndex = 0;

            lStreamIndex = aISourceRequestResult.getStreamIndex();

            if (lStreamIndex == 0)
            {
                lock (this)
                {
                    aISourceRequestResult.setData(mFrameBuffer, true);
                }
            }
        }



        void workItemHandler(IAsyncAction operation)
        {
            WorkerThread(); 
        }


        public void start(long aStartPositionInHundredNanosecondUnits, Guid aGUIDTimeFormat)
        {
            
            // create events
            stopEvent = new ManualResetEvent(false);
            reloadEvent = new ManualResetEvent(false);
            
            ThreadPool.RunAsync(workItemHandler, WorkItemPriority.Low, WorkItemOptions.None);
        }

        public void stop()
        {
            SignalToStop();
        }


        // Signal thread to stop work
        public void SignalToStop()
        {
            // stop thread
            stopEvent.Set();
        }


        // Free resources
        private void Free()
        {
            // release events
            //stopEvent.Dispose();
            //stopEvent = null;
            reloadEvent.Dispose();
            reloadEvent = null;
        }

        
        // Thread entry point
        public async void WorkerThread()
        {
            byte[] buffer = new byte[bufSize];	// buffer to read stream

            while (true)
            {
                // reset reload event
                reloadEvent.Reset();

                HttpWebRequest req = null;
                WebResponse resp = null;
                Stream stream = null;
                byte[] delimiter = null;
                byte[] delimiter2 = null;
                byte[] boundary = null;
                int boundaryLen, delimiterLen = 0, delimiter2Len = 0;
                int read, todo = 0, total = 0, pos = 0, align = 1;
                int start = 0, stop = 0;

                // align
                //  1 = searching for image start
                //  2 = searching for image end
                try
                {
                    // create request
                    req = (HttpWebRequest)WebRequest.Create(mURL);
                    // set login and password
                    //if ((login != null) && (password != null) && (login != ""))
                    //    req.Credentials = new NetworkCredential(login, password);
                    // get response
                    resp = await req.GetResponseAsync();

                    // check content type
                    string ct = resp.ContentType;
                    if (ct.IndexOf("multipart/x-mixed-replace") == -1)
                        return;

                    // get boundary
                    UTF8Encoding encoding = new UTF8Encoding();
                    boundary = encoding.GetBytes(ct.Substring(ct.IndexOf("boundary=", 0) + 9));
                    boundaryLen = boundary.Length;

                    // get response stream
                    stream = resp.GetResponseStream();

                    // loop
                    while ((!stopEvent.WaitOne(0)) && (!reloadEvent.WaitOne(0)))
                    {
                        // check total read
                        if (total > bufSize - readSize)
                        {
                            total = pos = todo = 0;
                        }

                        // read next portion from stream
                        if ((read = stream.Read(buffer, total, readSize)) == 0)
                            return;

                        total += read;
                        todo += read;
                        
                        // does we know the delimiter ?
                        if (delimiter == null)
                        {
                            // find boundary
                            pos = ByteArrayUtils.Find(buffer, boundary, pos, todo);

                            if (pos == -1)
                            {
                                // was not found
                                todo = boundaryLen - 1;
                                pos = total - todo;
                                continue;
                            }

                            todo = total - pos;

                            if (todo < 2)
                                continue;

                            // check new line delimiter type
                            if (buffer[pos + boundaryLen] == 10)
                            {
                                delimiterLen = 2;
                                delimiter = new byte[2] { 10, 10 };
                                delimiter2Len = 1;
                                delimiter2 = new byte[1] { 10 };
                            }
                            else
                            {
                                delimiterLen = 4;
                                delimiter = new byte[4] { 13, 10, 13, 10 };
                                delimiter2Len = 2;
                                delimiter2 = new byte[2] { 13, 10 };
                            }

                            pos += boundaryLen + delimiter2Len;
                            todo = total - pos;
                        }

                        // search for image
                        if (align == 1)
                        {
                            start = ByteArrayUtils.Find(buffer, delimiter, pos, todo);
                            if (start != -1)
                            {
                                // found delimiter
                                start += delimiterLen;
                                pos = start;
                                todo = total - pos;
                                align = 2;
                            }
                            else
                            {
                                // delimiter not found
                                todo = delimiterLen - 1;
                                pos = total - todo;
                            }
                        }

                        // search for image end
                        while ((align == 2) && (todo >= boundaryLen))
                        {
                            stop = ByteArrayUtils.Find(buffer, boundary, pos, todo);
                            if (stop != -1)
                            {
                                pos = stop;
                                todo = total - pos;
                                
                                // image at stop

                                lock (this)
                                {
                                    mFrameBuffer = null;

                                    mFrameBuffer = new byte[stop - start];

                                    Array.Copy(buffer, start, mFrameBuffer, 0, mFrameBuffer.Length);

                                }

                                // shift array
                                pos = stop + boundaryLen;
                                todo = total - pos;
                                Array.Copy(buffer, pos, buffer, 0, todo);

                                total = todo;
                                pos = 0;
                                align = 1;
                            }
                            else
                            {
                                // delimiter not found
                                todo = boundaryLen - 1;
                                pos = total - todo;
                            }
                        }
                    }
                }
                catch (WebException ex)
                {
                    System.Diagnostics.Debug.WriteLine("=============: " + ex.Message);
                }
                catch (Exception ex)
                {
                    System.Diagnostics.Debug.WriteLine("=============: " + ex.Message);
                }
                finally
                {
                    // abort request
                    if (req != null)
                    {
                        req.Abort();
                        req = null;
                    }
                    // close response stream
                    if (stream != null)
                    {
                        stream.Dispose();
                        stream = null;
                    }
                    // close response
                    if (resp != null)
                    {
                        resp.Dispose();
                        resp = null;
                    }
                }

                // need to stop ?
                if (stopEvent.WaitOne(0))
                    break;
            }
        }
    }
}
