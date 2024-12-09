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
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CaptureManagerToCSharpProxy.WrapClasses
{
    class WebCamControl : IWebCamControl, IWebCamControlAsync
    {

        private CaptureManagerLibrary.IWebCamControl mIWebCamControl;

        public WebCamControl(CaptureManagerLibrary.IWebCamControl aIWebCamControl)
        {
            mIWebCamControl = aIWebCamControl;
        }

        private async Task<WebCamParametr> getCamParametrTask(
            uint aParametrIndex, 
            bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                WebCamParametr lresult = new WebCamParametr();
                
                do
                {
                    if (mIWebCamControl == null)
                    {
                        break;
                    }

                    try
                    {

                        mIWebCamControl.getCamParametr(
                            aParametrIndex,
                            out lresult.mCurrentValue,
                            out lresult.mMin,
                            out lresult.mMax,
                            out lresult.mStep,
                            out lresult.mDefault,
                            out lresult.mFlag);
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);
                
                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<WebCamParametr> getCamParametrAsync(uint aParametrIndex)
        {
            return await getCamParametrTask(aParametrIndex, true);
        }

        public void getCamParametr(
            uint aParametrIndex, 
            out int aCurrentValue, 
            out int aMin, 
            out int aMax, 
            out int aStep, 
            out int aDefault, 
            out int aFlag)
        {
            var lParametr = getCamParametrTask(aParametrIndex, false).Result;

            aCurrentValue = lParametr.mCurrentValue;
            aMin = lParametr.mMin;
            aMax = lParametr.mMax;
            aStep = lParametr.mStep;
            aDefault = lParametr.mDefault;
            aFlag = lParametr.mFlag;
        }

        private async Task<string> getCamParametrsTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                string lresult = "";
                               

                IntPtr lPtrXMLstring = IntPtr.Zero;

                do
                {
                    if (mIWebCamControl == null)
                    {
                        break;
                    }

                    try
                    {
                        (mIWebCamControl as IWebCamControlInner).getCamParametrs(out lPtrXMLstring);

                        if (lPtrXMLstring != IntPtr.Zero)
                            lresult = Marshal.PtrToStringBSTR(lPtrXMLstring);
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                if (lPtrXMLstring != IntPtr.Zero)
                    Marshal.FreeBSTR(lPtrXMLstring);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<string> getCamParametrsAsync()
        {
            return await getCamParametrsTask(true);
        }

        public void getCamParametrs(
            out string aXMLstring)
        {
            aXMLstring = getCamParametrsTask(false).Result;
        }
        
        private async Task setCamParametrTask(
            uint aParametrIndex,
            int aNewValue,
            int aFlag,
            bool aIsAwait)
        {
            await Task.Run(() =>
            {
                do
                {
                    if (mIWebCamControl == null)
                    {
                        break;
                    }

                    mIWebCamControl.setCamParametr(
                        aParametrIndex,
                        aNewValue,
                        aFlag);

                } while (false);

            }).ConfigureAwait(aIsAwait);
        }

        public async Task setCamParametrAsync(
            uint aParametrIndex,
            int aNewValue,
            int aFlag)
        {
            await setCamParametrTask(
                aParametrIndex,
                aNewValue,
                aFlag,
                false);
        }

        public void setCamParametr(
            uint aParametrIndex,
            int aNewValue,
            int aFlag)
        {
            setCamParametrTask(
                aParametrIndex,
                aNewValue,
                aFlag,
                false);
        }
    }
}
