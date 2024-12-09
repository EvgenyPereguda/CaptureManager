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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CaptureManagerToCSharpProxy.Interfaces;

namespace CaptureManagerToCSharpProxy.WrapClasses
{
    class SampleGrabberCallSinkFactory : ISampleGrabberCallSinkFactory, ISampleGrabberCallSinkFactoryAsync
    {
        private CaptureManagerLibrary.ISampleGrabberCallSinkFactory mISampleGrabberCallSinkFactory;

        public SampleGrabberCallSinkFactory(CaptureManagerLibrary.ISampleGrabberCallSinkFactory aISampleGrabberCallSinkFactory)
        {
            mISampleGrabberCallSinkFactory = aISampleGrabberCallSinkFactory;
        }

        private async Task<SampleGrabberCall> createOutputNodeTask(
            Guid aRefMajorType,
            Guid aRefSubType,
            uint aSampleByteSize,
            bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                SampleGrabberCall lresult = null;

                do
                {
                    if (mISampleGrabberCallSinkFactory == null)
                        break;

                    try
                    {
                        object lIUnknown;

                        mISampleGrabberCallSinkFactory.createOutputNode(
                            aRefMajorType,
                            aRefSubType,
                            aSampleByteSize,
                            typeof(CaptureManagerLibrary.ISampleGrabberCall).GUID,
                            out lIUnknown);


                        if (lIUnknown == null)
                            break;

                        var lSampleGrabberCall = lIUnknown as CaptureManagerLibrary.ISampleGrabberCall;

                        if (lSampleGrabberCall == null)
                            break;

                        lresult = new SampleGrabberCall(
                            lSampleGrabberCall,
                            aSampleByteSize);
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }
        
        public bool createOutputNode(
            Guid aRefMajorType, 
            Guid aRefSubType, 
            uint aSampleByteSize, 
            out ISampleGrabberCall aISampleGrabberCall)
        {
            bool lresult = false;

            aISampleGrabberCall = null;
            
            do
            {
                if (mISampleGrabberCallSinkFactory == null)
                    break;

                var lTask = createOutputNodeTask(aRefMajorType, aRefSubType, aSampleByteSize, false).Result;
                         
                aISampleGrabberCall = lTask as ISampleGrabberCall;

                if (aISampleGrabberCall == null)
                    break;

                lresult = true;

            } while (false);

            return lresult;
        }

        public async Task<ISampleGrabberCallAsync> createOutputNodeAsync(Guid aRefMajorType, Guid aRefSubType, uint aSampleByteSize)
        {
            return await createOutputNodeTask(aRefMajorType, aRefSubType, aSampleByteSize, true);
        }
    }
}
