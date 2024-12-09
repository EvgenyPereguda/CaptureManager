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

using CaptureManagerToCSharpProxy.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CaptureManagerToCSharpProxy.WrapClasses
{
    class CaptureProcessor : CaptureManagerLibrary.ICaptureProcessor
    {
        class InitilaizeCaptureSource : IInitilaizeCaptureSource
        {
            public string mPresentationDescriptor;

            public void setPresentationDescriptor(string aPresentationDescriptor)
            {
                mPresentationDescriptor = aPresentationDescriptor;
            }
        }

        class CurrentMediaType: ICurrentMediaType
        {

            public uint mStreamIndex = 0;

            public uint mMediaTypeIndex = 0;

            public object mMediaType = null;

            public void getMediaType(out object aPtrMediaType)
            {
                aPtrMediaType = mMediaType;
            }  

            public void getMediaTypeIndex(out uint aPtrMediaTypeIndex)
            {
                aPtrMediaTypeIndex = mMediaTypeIndex;
            }  
   
            public void getStreamIndex(out uint aPtrStreamIndex)
            {
                aPtrStreamIndex = mStreamIndex;
            }            
        }

        class SourceRequestResult: ISourceRequestResult
        {
            public CaptureManagerLibrary.ISourceRequestResult mISourceRequestResult = null;

            public void getStreamIndex(out uint aPtrStreamIndex)
            {
                aPtrStreamIndex = 0;

                if(mISourceRequestResult != null)
                    mISourceRequestResult.getStreamIndex(out aPtrStreamIndex);
            }

            public void setData(IntPtr aPtrData, uint aByteSize, int aIsKeyFrame)
            {
                if (mISourceRequestResult != null)
                    mISourceRequestResult.setData(aPtrData, aByteSize, aIsKeyFrame);
            }
        }

        ICaptureProcessor mICaptureProcessor = null;

        InitilaizeCaptureSource mInitilaizeCaptureSource = new InitilaizeCaptureSource();

        SourceRequestResult mSourceRequestResult = new SourceRequestResult();

        public CaptureProcessor(ICaptureProcessor aICaptureProcessor)
        {
            mICaptureProcessor = aICaptureProcessor;
        }

        public void initilaize(object aPtrIInitilaizeCaptureSource)
        {
            if (mICaptureProcessor == null)
                return;

            if (aPtrIInitilaizeCaptureSource == null)
                throw new ArgumentNullException();

            var lInitilaizeCaptureSource = aPtrIInitilaizeCaptureSource as CaptureManagerLibrary.IInitilaizeCaptureSource;

            if (lInitilaizeCaptureSource == null)
                return;
            
            mICaptureProcessor.initilaize(mInitilaizeCaptureSource);

            lInitilaizeCaptureSource.setPresentationDescriptor(mInitilaizeCaptureSource.mPresentationDescriptor); 
        }

        public void pause()
        {
            throw new NotImplementedException();
        }

        public void setCurrentMediaType(object aPtrICurrentMediaType)
        {
            if (aPtrICurrentMediaType == null)
                return;

            var lCurrentMediaType = aPtrICurrentMediaType as CaptureManagerLibrary.ICurrentMediaType;

            if (lCurrentMediaType == null)
                return;

            CurrentMediaType lICurrentMediaType = new CurrentMediaType();
            
            lCurrentMediaType.getStreamIndex(out lICurrentMediaType.mStreamIndex);

            lCurrentMediaType.getMediaTypeIndex(out lICurrentMediaType.mMediaTypeIndex);

            lCurrentMediaType.getMediaType(out lICurrentMediaType.mMediaType);

            if (lICurrentMediaType == null)
                return;

            mICaptureProcessor.setCurrentMediaType(lICurrentMediaType);
        }

        public void shutdown()
        {
            if (mICaptureProcessor == null)
                return;

            mICaptureProcessor.shutdown();
        }

        public void sourceRequest(object aPtrISourceRequestResult)
        {
            if (mICaptureProcessor == null)
                return;

            var lSourceRequestResult = aPtrISourceRequestResult as CaptureManagerLibrary.ISourceRequestResult;

            if (lSourceRequestResult == null)
                return;

            mSourceRequestResult.mISourceRequestResult = lSourceRequestResult;

            mICaptureProcessor.sourceRequest(mSourceRequestResult);
        }

        public void start(long aStartPositionInHundredNanosecondUnits, ref Guid aGUIDTimeFormat)
        {
            if (mICaptureProcessor == null)
                return;

            mICaptureProcessor.start(aStartPositionInHundredNanosecondUnits, ref aGUIDTimeFormat);
        }

        public void stop()
        {
            if (mICaptureProcessor == null)
                return;

            mICaptureProcessor.stop();
        }
    }
}
