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
    
    class EVRStreamControl: IEVRStreamControl, IEVRStreamControlAsync
    {
        private CaptureManagerLibrary.IEVRStreamControl mIEVRStreamControl = null;

        private object mIUnknown;

        public EVRStreamControl(object aIUnknown)
        {
            mIEVRStreamControl = aIUnknown as CaptureManagerLibrary.IEVRStreamControl;

            mIUnknown = aIUnknown;
        }

    private async Task<bool> flushTask(object aPtrEVROutputNode, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                bool lresult = false;

                do
                {
                    if (mIEVRStreamControl == null)
                        break;

                    if (aPtrEVROutputNode == null)
                        break;

                    try
                    {
                        mIEVRStreamControl.flush(aPtrEVROutputNode);

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

        public async Task<bool> flushAsync(object aPtrEVROutputNode)
        {
            return await flushTask(aPtrEVROutputNode, true);
        }

        public bool flush(object aPtrEVROutputNode)
        {
            return flushTask(aPtrEVROutputNode, false).Result;
        }

        private async Task<EVRStreamControlPosition> getPositionTask(object aPtrEVROutputNode, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                EVRStreamControlPosition lresult = new EVRStreamControlPosition();


                do
                {
                    if (mIEVRStreamControl == null)
                        break;

                    if (aPtrEVROutputNode == null)
                        break;

                    try
                    {
                        mIEVRStreamControl.getPosition(
                            aPtrEVROutputNode,
                            out lresult.aPtrLeft,
                            out lresult.aPtrRight,
                            out lresult.aPtrTop,
                            out lresult.aPtrBottom);
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<EVRStreamControlPosition> getPositionAsync(object aPtrEVROutputNode)
        {
            return await getPositionTask(aPtrEVROutputNode, true);
        }

        public bool getPosition(
            object aPtrEVROutputNode,
            out float aPtrLeft,
            out float aPtrRight,
            out float aPtrTop,
            out float aPtrBottom)
        {
            bool lresult = false;

            aPtrLeft = 0.0f;

            aPtrRight = 0.0f;

            aPtrTop = 0.0f;

            aPtrBottom = 0.0f;

            do
            {
                if (mIEVRStreamControl == null)
                    break;

                if (aPtrEVROutputNode == null)
                    break;

                try
                {
                    var lposition = getPositionTask(aPtrEVROutputNode, false).Result;

                    aPtrLeft = lposition.aPtrLeft;

                    aPtrRight = lposition.aPtrRight;

                    aPtrTop = lposition.aPtrTop;

                    aPtrBottom = lposition.aPtrBottom;

                    lresult = true;
                }
                catch (Exception exc)
                {
                    LogManager.getInstance().write(exc.Message);
                }

            } while (false);

            return lresult;
        }


        private async Task<uint> getZOrderTask(object aPtrEVROutputNode, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                uint lresult = 0;


                do
                {
                    if (mIEVRStreamControl == null)
                        break;

                    if (aPtrEVROutputNode == null)
                        break;

                    try
                    {
                        mIEVRStreamControl.getZOrder(
                            aPtrEVROutputNode,
                            out lresult);
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);


                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<uint> getZOrderAsync(object aPtrEVROutputNode)
        {
            return await getZOrderTask(aPtrEVROutputNode, true);
        }

        public bool getZOrder(object aPtrEVROutputNode, out uint aPtrZOrder)
        {
            bool lresult = true;

            aPtrZOrder = getZOrderTask(aPtrEVROutputNode, false).Result;
            
            return lresult;
        }


        private async Task<bool> setPositionTask(object aPtrEVROutputNode, float aLeft, float aRight, float aTop, float aBottom, bool aIsAwait)
        {
            IntPtr lptr = IntPtr.Zero;

            if (aPtrEVROutputNode != null)
                lptr = Marshal.GetIUnknownForObject(aPtrEVROutputNode);

            long unkPtr = lptr.ToInt64();

            object[] largs = new object[] { unkPtr, aLeft, aRight, aTop, aBottom };

            object[] largs1 = new object[] { IntPtr.Zero, unkPtr, aLeft, aRight, aTop, aBottom };

            object[] largs2 = new object[] { IntPtr.Zero, lptr.ToInt64(), unkPtr, aLeft, aRight, aTop, aBottom };

            return await Task.Run(() =>
            {
                bool lresult = false;

                do
                {
                    if (mIEVRStreamControl == null)
                        break;

                    if (aPtrEVROutputNode == null)
                        break;
                    
                    try
                    {
                        mIEVRStreamControl.setPosition(
                            aPtrEVROutputNode,
                            aLeft,
                            aRight,
                            aTop,
                            aBottom);

                        lresult = true;
                    }
                    catch (Exception exc)
                    {
                        if (mIUnknown != null)
                        {
                            try
                            {

                                Win32NativeMethods.Invoke<object>(mIUnknown, Win32NativeMethods.InvokeFlags.DISPATCH_METHOD, "setPosition", largs);

                                lresult = true;
                            }
                            catch (Exception)
                            {
                                try
                                {
                                    Win32NativeMethods.Invoke<object>(mIUnknown, Win32NativeMethods.InvokeFlags.DISPATCH_METHOD, "setPosition", largs1);

                                    lresult = true;
                                }
                                catch (Exception)
                                {
                                    try
                                    {
                                        Win32NativeMethods.Invoke<object>(mIUnknown, Win32NativeMethods.InvokeFlags.DISPATCH_METHOD, "setPosition", largs2);

                                        lresult = true;
                                    }
                                    catch (Exception exc1)
                                    {
                                        LogManager.getInstance().write(exc1.Message);
                                    }
                                }
                            }
                        }
                        else
                            LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<bool> setPositionAsync(object aPtrEVROutputNode, float aLeft, float aRight, float aTop, float aBottom)
        {
            return await setPositionTask(aPtrEVROutputNode, aLeft, aRight, aTop, aBottom, true);
        }

        public bool setPosition(object aPtrEVROutputNode, float aLeft, float aRight, float aTop, float aBottom)
        {
            return setPositionTask(aPtrEVROutputNode, aLeft, aRight, aTop, aBottom, false).Result;
        }

        private async Task<bool> setZOrderTask(object aPtrEVROutputNode, uint aZOrder, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                bool lresult = false;

                do
                {
                    if (mIEVRStreamControl == null)
                        break;

                    if (aPtrEVROutputNode == null)
                        break;

                    try
                    {
                        mIEVRStreamControl.setZOrder(
                            aPtrEVROutputNode,
                            aZOrder);

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

        public async Task<bool> setZOrderAsync(object aPtrEVROutputNode, uint aZOrder)
        {
            return await setZOrderTask(aPtrEVROutputNode, aZOrder, true);
        }

        public bool setZOrder(object aPtrEVROutputNode, uint aZOrder)
        {
            return setZOrderTask(aPtrEVROutputNode, aZOrder, false).Result;
        }
        
        private async Task<bool> setSrcPositionTask(object aPtrEVROutputNode, float aLeft, float aRight, float aTop, float aBottom, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                bool lresult = false;

                do
                {
                    if (mIEVRStreamControl == null)
                        break;

                    if (aPtrEVROutputNode == null)
                        break;

                    try
                    {
                        mIEVRStreamControl.setSrcPosition(
                            aPtrEVROutputNode,
                            aLeft,
                            aRight,
                            aTop,
                            aBottom);

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

        public async Task<bool> setSrcPositionAsync(object aPtrEVROutputNode, float aLeft, float aRight, float aTop, float aBottom)
        {
            return await setSrcPositionTask(aPtrEVROutputNode, aLeft, aRight, aTop, aBottom, true);
        }

        public bool setSrcPosition(object aPtrEVROutputNode, float aLeft, float aRight, float aTop, float aBottom)
        {
            return setSrcPositionTask(aPtrEVROutputNode, aLeft, aRight, aTop, aBottom, false).Result;
        }


        private async Task<EVRStreamControlPosition> getSrcPositionTask(object aPtrEVROutputNode, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                EVRStreamControlPosition lresult = new EVRStreamControlPosition();


                do
                {
                    if (mIEVRStreamControl == null)
                        break;

                    if (aPtrEVROutputNode == null)
                        break;

                    try
                    {
                        mIEVRStreamControl.getSrcPosition(
                            aPtrEVROutputNode,
                            out lresult.aPtrLeft,
                            out lresult.aPtrRight,
                            out lresult.aPtrTop,
                            out lresult.aPtrBottom);
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<EVRStreamControlPosition> getSrcPositionAsync(object aPtrEVROutputNode)
        {
            return await getSrcPositionTask(aPtrEVROutputNode, true);
        }

        public bool getSrcPosition(object aPtrEVROutputNode, out float aPtrLeft, out float aPtrRight, out float aPtrTop, out float aPtrBottom)
        {
            bool lresult = false;

            aPtrLeft = 0.0f;

            aPtrRight = 0.0f;

            aPtrTop = 0.0f;

            aPtrBottom = 0.0f;

            do
            {
                if (mIEVRStreamControl == null)
                    break;

                if (aPtrEVROutputNode == null)
                    break;

                try
                {
                    var lposition = getSrcPositionTask(aPtrEVROutputNode, false).Result;

                    aPtrLeft = lposition.aPtrLeft;

                    aPtrRight = lposition.aPtrRight;

                    aPtrTop = lposition.aPtrTop;

                    aPtrBottom = lposition.aPtrBottom;

                    lresult = true;
                }
                catch (Exception exc)
                {
                    LogManager.getInstance().write(exc.Message);
                }

            } while (false);

            return lresult;
        }
               
        private async Task<string> getCollectionOfFiltersTask(object aPtrEVROutputNode, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                string lresult = "";

                IntPtr lPtrXMLstring = IntPtr.Zero;

                do
                {
                    if (mIEVRStreamControl == null)
                        break;

                    if (aPtrEVROutputNode == null)
                        break;

                    try
                    {
                        (mIEVRStreamControl as IEVRStreamControlInner).getCollectionOfFilters(
                            Marshal.GetIUnknownForObject(aPtrEVROutputNode),
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

        public async Task<string> getCollectionOfFiltersAsync(object aPtrEVROutputNode)
        {
            return await getCollectionOfFiltersTask(aPtrEVROutputNode, true);
        }

        public void getCollectionOfFilters(object aPtrEVROutputNode, out string aPtrPtrXMLstring)
        {
            aPtrPtrXMLstring = getCollectionOfFiltersTask(aPtrEVROutputNode, false).Result;
        }

        private async Task<string> getCollectionOfOutputFeaturesTask(object aPtrEVROutputNode, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                string lresult = "";

                IntPtr lPtrXMLstring = IntPtr.Zero;

                do
                {
                    if (mIEVRStreamControl == null)
                        break;

                    if (aPtrEVROutputNode == null)
                        break;

                    try
                    {
                        (mIEVRStreamControl as IEVRStreamControlInner).getCollectionOfOutputFeatures(
                            Marshal.GetIUnknownForObject(aPtrEVROutputNode),
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

        public async Task<string> getCollectionOfOutputFeaturesAsync(object aPtrEVROutputNode)
        {
            return await getCollectionOfOutputFeaturesTask(aPtrEVROutputNode, true);
        }

        public void getCollectionOfOutputFeatures(object aPtrEVROutputNode, out string aPtrPtrXMLstring)
        {
            aPtrPtrXMLstring = getCollectionOfOutputFeaturesTask(aPtrEVROutputNode, false).Result;
        }

        private async Task setFilterParametrTask(object aPtrEVROutputNode, uint aParametrIndex, int aNewValue, bool aIsEnabled, bool aIsAwait)
        {
            await Task.Run(() =>
            {
                do
                {
                    if (mIEVRStreamControl == null)
                        break;

                    if (aPtrEVROutputNode == null)
                        break;

                    try
                    {
                        mIEVRStreamControl.setFilterParametr(
                            aPtrEVROutputNode,
                            aParametrIndex,
                            aNewValue,
                            aIsEnabled ? 1 : 0);

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);
            }).ConfigureAwait(aIsAwait);
        }

        public async Task setFilterParametrAsync(object aPtrEVROutputNode, uint aParametrIndex, int aNewValue, bool aIsEnabled)
        {
            await setFilterParametrTask(aPtrEVROutputNode, aParametrIndex, aNewValue, aIsEnabled, true);
        }

        public void setFilterParametr(object aPtrEVROutputNode, uint aParametrIndex, int aNewValue, bool aIsEnabled)
        {
            setFilterParametrTask(aPtrEVROutputNode, aParametrIndex, aNewValue, aIsEnabled, false);
        }

        private async Task setOutputFeatureParametrTask(object aPtrEVROutputNode, uint aParametrIndex, int aNewValue, bool aIsAwait)
        {
            await Task.Run(() =>
            {
                do
                {
                    if (mIEVRStreamControl == null)
                        break;

                    if (aPtrEVROutputNode == null)
                        break;

                    try
                    {
                        mIEVRStreamControl.setOutputFeatureParametr(
                            aPtrEVROutputNode,
                            aParametrIndex,
                            aNewValue);

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);
            }).ConfigureAwait(aIsAwait);
        }

        public async Task setOutputFeatureParametrAsync(object aPtrEVROutputNode, uint aParametrIndex, int aNewValue)
        {
            await setOutputFeatureParametrTask(aPtrEVROutputNode, aParametrIndex, aNewValue, true);
        }

        public void setOutputFeatureParametr(object aPtrEVROutputNode, uint aParametrIndex, int aNewValue)
        {
            setOutputFeatureParametrTask(aPtrEVROutputNode, aParametrIndex, aNewValue, false);
        }
                 
    }
}
