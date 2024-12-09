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

    class VersionControl : IVersionControl, IVersionControlAsync
    {
        CaptureManagerLibrary.IVersionControl mVersionControl = null;

        public VersionControl(CaptureManagerLibrary.IVersionControl aVersionControl)
        {
            mVersionControl = aVersionControl;
        }

        private async Task<VersionStruct> getVersionTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                VersionStruct lresult = new VersionStruct();
                
                IntPtr lPtrXMLstring = IntPtr.Zero;

                do
                {
                    try
                    {
                        if (mVersionControl == null)
                            break;

                        (mVersionControl as IVersionControlInner).getVersion(
                            out lresult.mMAJOR,
                            out lresult.mMINOR,
                            out lresult.mPATCH,
                            out lPtrXMLstring);

                        if (lPtrXMLstring != IntPtr.Zero)
                            lresult.mAdditionalLabel = Marshal.PtrToStringBSTR(lPtrXMLstring);

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

        public bool getVersion(ref VersionStruct aVersionStruct)
        {
            bool lresult = false;

            aVersionStruct = getVersionTask(false).Result;

            lresult = !string.IsNullOrWhiteSpace(aVersionStruct.mAdditionalLabel);

            return lresult;
        }

        public async Task<VersionStruct> getVersionAsync()
        {
            return await getVersionTask(true);
        }

        private async Task<bool> checkVersionTask(VersionStruct aVersionStruct, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                bool lresult = false;

                do
                {
                    try
                    {
                        if (mVersionControl == null)
                            break;

                        sbyte lcheckResult = 0;

                        (mVersionControl as IVersionControlInner).checkVersion(
                            aVersionStruct.mMAJOR,
                            aVersionStruct.mMINOR,
                            aVersionStruct.mPATCH,
                            out lcheckResult);


                        if (lcheckResult > 0)
                            lresult = true;

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public bool checkVersion(VersionStruct aVersionStruct)
        {
            return checkVersionTask(aVersionStruct, false).Result;
        }

        public async Task<bool> checkVersionAsync(VersionStruct aVersionStruct)
        {
            return await checkVersionTask(aVersionStruct, true);
        }

        private async Task<string> getXMLStringVersionTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                string lresult = "";

                IntPtr lPtrXMLstring = IntPtr.Zero;

                do
                {
                    try
                    {

                        if (mVersionControl == null)
                            break;

                        (mVersionControl as IVersionControlInner).getXMLStringVersion(
                            out lPtrXMLstring);

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

        public bool getXMLStringVersion(out string aPtrPtrXMLstring)
        {
            bool lresult = false;

            aPtrPtrXMLstring = getXMLStringVersionTask(false).Result;

            lresult = !string.IsNullOrWhiteSpace(aPtrPtrXMLstring);

            return lresult;
        }


        public async Task<string> getXMLStringVersionAsync()
        {
            return await getXMLStringVersionTask(true);
        }
    }
}
