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
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using CaptureManagerToCSharpProxy.Interfaces;

namespace CaptureManagerToCSharpProxy.WrapClasses
{
    [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1049:TypesThatOwnNativeResourcesShouldBeDisposable")]
    class SampleGrabberCall : ISampleGrabberCall, ISampleGrabberCallAsync
    {
        private CaptureManagerLibrary.ISampleGrabberCall mISampleGrabberCall;

        private IntPtr mPtrSampleBuffer;

        private int mAllocatedSize = 0;

        public SampleGrabberCall(
            CaptureManagerLibrary.ISampleGrabberCall aISampleGrabberCall,
            uint aSampleByteSize)
        {
            mISampleGrabberCall = aISampleGrabberCall;
        }

        ~SampleGrabberCall()
        {
            Marshal.ReleaseComObject(mISampleGrabberCall);

            Marshal.FreeHGlobal(mPtrSampleBuffer);
        }

        private async Task<uint> readDataTask(IntPtr aPtrSampleBuffer, bool aIsAwait)
        {            
            return await Task.Run(() =>
            {
                uint lresult = 0;

                try
                {
                    mISampleGrabberCall.readData(
                        aPtrSampleBuffer,
                        out lresult);
                }
                catch (Exception)
                {
                }

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public void readData(byte[] aData, out uint aByteSize)
        {
            if (mAllocatedSize != aData.Length)
            {
                Marshal.FreeHGlobal(mPtrSampleBuffer);

                mPtrSampleBuffer = Marshal.AllocHGlobal(aData.Length);

                mAllocatedSize = aData.Length;
            }

            aByteSize = readDataTask(mPtrSampleBuffer, false).Result;
            
            Marshal.Copy(mPtrSampleBuffer, aData, 0, (int)aByteSize);
        }

        public object getTopologyNode()
        {
            return mISampleGrabberCall;
        }

        public async Task<uint> readDataAsync(byte[] aData)
        {
            if (mAllocatedSize != aData.Length)
            {
                Marshal.FreeHGlobal(mPtrSampleBuffer);

                mPtrSampleBuffer = Marshal.AllocHGlobal(aData.Length);

                mAllocatedSize = aData.Length;
            }

            uint lresult = await readDataTask(mPtrSampleBuffer, true);

            Marshal.Copy(mPtrSampleBuffer, aData, 0, (int)lresult);

            return lresult;
        }
    }
}
