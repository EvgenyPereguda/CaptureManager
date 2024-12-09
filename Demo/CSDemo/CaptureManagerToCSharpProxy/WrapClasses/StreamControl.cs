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
using CaptureManagerToCSharpProxy.Interfaces;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace CaptureManagerToCSharpProxy.WrapClasses
{

    class StreamControl : IStreamControl, IStreamControlAsync
    {
        CaptureManagerLibrary.IStreamControl mIStreamControl = null;

        public StreamControl(CaptureManagerLibrary.IStreamControl aIStreamControl)
        {
            mIStreamControl = aIStreamControl;
        }

        private async Task<string> getCollectionOfStreamControlNodeFactoriesTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                string lresult = "";
                
                IntPtr lPtrXMLstring = IntPtr.Zero;

                do
                {
                    try
                    {

                        if (mIStreamControl == null)
                            break;

                        (mIStreamControl as IStreamControlInner).getCollectionOfStreamControlNodeFactories(out lPtrXMLstring);

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

        public async Task<string> getCollectionOfStreamControlNodeFactoriesAsync()
        {
            return await getCollectionOfStreamControlNodeFactoriesTask(true);
        }

        public bool getCollectionOfStreamControlNodeFactories(ref string aInfoString)
        {
            bool lresult = false;

            aInfoString = getCollectionOfStreamControlNodeFactoriesTask(false).Result;

            lresult = !string.IsNullOrWhiteSpace(aInfoString);

            return lresult;
        }
        
        private async Task<SpreaderNodeFactory> createSpreaderNodeFactoryTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                SpreaderNodeFactory lresult = null;

                do
                {
                    try
                    {

                        if (mIStreamControl == null)
                            break;

                        object lIUnknown;

                        mIStreamControl.createStreamControlNodeFactory(
                            typeof(CaptureManagerLibrary.ISpreaderNodeFactory).GUID,
                            out lIUnknown);

                        if (lIUnknown == null)
                            break;

                        var lSpreaderNodeFactory = lIUnknown as CaptureManagerLibrary.ISpreaderNodeFactory;

                        if (lSpreaderNodeFactory == null)
                            break;

                        lresult = new SpreaderNodeFactory(lSpreaderNodeFactory);
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<ISpreaderNodeFactoryAsync> createSpreaderNodeFactoryAsync()
        {
            return await createSpreaderNodeFactoryTask(true);
        }

        public bool createStreamControlNodeFactory(ref ISpreaderNodeFactory aISpreaderNodeFactory)
        {
            bool lresult = false;

            aISpreaderNodeFactory = createSpreaderNodeFactoryTask(false).Result;

            lresult = aISpreaderNodeFactory != null;

            return lresult;
        }

        private async Task<SpreaderNodeFactory> createSpreaderNodeFactoryTask(Guid aIID, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                SpreaderNodeFactory lresult = null;

                do
                {
                    try
                    {

                        if (mIStreamControl == null)
                            break;

                        object lIUnknown;

                        mIStreamControl.createStreamControlNodeFactory(
                            aIID,
                            out lIUnknown);

                        if (lIUnknown == null)
                            break;

                        var lSpreaderNodeFactory = lIUnknown as CaptureManagerLibrary.ISpreaderNodeFactory;

                        if (lSpreaderNodeFactory == null)
                            break;

                        lresult = new SpreaderNodeFactory(lSpreaderNodeFactory);
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<ISpreaderNodeFactoryAsync> createSpreaderNodeFactoryAsync(Guid aIID)
        {
            return await createSpreaderNodeFactoryTask(aIID, true);
        }

        public bool createStreamControlNodeFactory(Guid aIID, ref ISpreaderNodeFactory aISpreaderNodeFactory)
        {
            bool lresult = false;

            aISpreaderNodeFactory = createSpreaderNodeFactoryTask(aIID, false).Result;

            lresult = aISpreaderNodeFactory != null;

            return lresult;
        }

        private async Task<SwitcherNodeFactory> createSwitcherNodeFactoryTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                SwitcherNodeFactory lresult = null;

                do
                {
                    try
                    {

                        if (mIStreamControl == null)
                            break;

                        object lIUnknown;

                        mIStreamControl.createStreamControlNodeFactory(
                            typeof(CaptureManagerLibrary.ISwitcherNodeFactory).GUID,
                            out lIUnknown);

                        if (lIUnknown == null)
                            break;

                        var lSwitcherNodeFactory = lIUnknown as CaptureManagerLibrary.ISwitcherNodeFactory;

                        if (lSwitcherNodeFactory == null)
                            break;

                        lresult = new SwitcherNodeFactory(lSwitcherNodeFactory);
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<ISwitcherNodeFactoryAsync> createSwitcherNodeFactoryAsync()
        {
            return await createSwitcherNodeFactoryTask(true);
        }

        public bool createStreamControlNodeFactory(ref ISwitcherNodeFactory aISwitcherNodeFactory)
        {
            bool lresult = false;

            aISwitcherNodeFactory = createSwitcherNodeFactoryTask(false).Result;

            lresult = aISwitcherNodeFactory != null;

            return lresult;
        }

        private async Task<MixerNodeFactory> createMixerNodeFactoryTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                MixerNodeFactory lresult = null;

                do
                {
                    try
                    {

                        if (mIStreamControl == null)
                            break;

                        object lIUnknown;

                        mIStreamControl.createStreamControlNodeFactory(
                            typeof(CaptureManagerLibrary.IMixerNodeFactory).GUID,
                            out lIUnknown);

                        if (lIUnknown == null)
                            break;

                        var lMixerNodeFactory = lIUnknown as CaptureManagerLibrary.IMixerNodeFactory;

                        if (lMixerNodeFactory == null)
                            break;

                        lresult = new MixerNodeFactory(lMixerNodeFactory);
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<IMixerNodeFactoryAsync> createMixerNodeFactoryAsync()
        {
            return await createMixerNodeFactoryTask(true);
        }

        public bool createStreamControlNodeFactory(ref IMixerNodeFactory aIMixerNodeFactory)
        {
            bool lresult = false;

            aIMixerNodeFactory = createMixerNodeFactoryTask(false).Result;

            lresult = aIMixerNodeFactory != null;

            return lresult;
        }
    }
}
