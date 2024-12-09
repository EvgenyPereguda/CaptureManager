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
using System.Runtime.InteropServices.ComTypes;
using System.Text;
using System.Threading.Tasks;
using CaptureManagerToCSharpProxy.Interfaces;
using DISPPARAMS = System.Runtime.InteropServices.ComTypes.DISPPARAMS;
using EXCEPINFO = System.Runtime.InteropServices.ComTypes.EXCEPINFO;

namespace CaptureManagerToCSharpProxy.WrapClasses
{

    class EVRMultiSinkFactory: IEVRMultiSinkFactory, IEVRMultiSinkFactoryAsync
    {
        public static bool GetComMethod<T, U>(T comObj, int slot, out U method) where U : class
        {
            IntPtr objectAddress = Marshal.GetComInterfaceForObject(comObj, typeof(T));
            if (objectAddress == IntPtr.Zero)
            {
                method = null;
                return false;
            }

            try
            {
                IntPtr vTable = Marshal.ReadIntPtr(objectAddress, 0);
                IntPtr methodAddress = Marshal.ReadIntPtr(vTable, slot * IntPtr.Size);

                // We can't have a Delegate constraint, so we have to cast to
                // object then to our desired delegate
                method = (U)((object)Marshal.GetDelegateForFunctionPointer(methodAddress, typeof(U)));
                return true;
            }
            finally
            {
                Marshal.Release(objectAddress); // Prevent memory leak
            }
        }

        private CaptureManagerLibrary.IEVRMultiSinkFactory mIEVRMultiSinkFactory;

        private object mIUnknown;

        public EVRMultiSinkFactory(object aIUnknown)
        {
            mIEVRMultiSinkFactory = aIUnknown as CaptureManagerLibrary.IEVRMultiSinkFactory;

            mIUnknown = aIUnknown;
        }

        private async Task<List<object>> createOutputNodesTask(IntPtr aHandle, object aPtrUnkSharedResource, uint aOutputNodeAmount, bool aIsAwait)
        {

            IntPtr lptr = IntPtr.Zero;

            if (aPtrUnkSharedResource != null)
                lptr = Marshal.GetIUnknownForObject(aPtrUnkSharedResource);

            object[] largs = new object[] { aHandle.ToInt64(), aPtrUnkSharedResource, aOutputNodeAmount };

            object[] largs1 = new object[] { aHandle.ToInt64(), lptr.ToInt64(), aHandle.ToInt64(), aPtrUnkSharedResource, aOutputNodeAmount };

            object[] largs2 = new object[] { aHandle.ToInt64(), lptr.ToInt64(), aOutputNodeAmount };

            return await Task.Run(() =>
            {
                List<object> lresult = new List<object>();

                do
                {
                    if (mIEVRMultiSinkFactory == null)
                        break;
                    
                    try
                    {
                        object lArrayMediaNodes = new Object();

                        mIEVRMultiSinkFactory.createOutputNodes(
                            aHandle,
                            aPtrUnkSharedResource,
                            aOutputNodeAmount,
                            out lArrayMediaNodes);

                        if (lArrayMediaNodes == null)
                            break;

                        object[] lArray = lArrayMediaNodes as object[];

                        if (lArray == null)
                            break;

                        lresult.AddRange(lArray);
                    }
                    catch (Exception exc)
                    {
                        if (mIUnknown != null)
                        {                            
                            try
                            {

                                object lArrayMediaNodes = Win32NativeMethods.Invoke<object>(mIUnknown, Win32NativeMethods.InvokeFlags.DISPATCH_METHOD, "createOutputNodes", largs);

                                if (lArrayMediaNodes == null)
                                    break;

                                object[] lArray = lArrayMediaNodes as object[];

                                if (lArray == null)
                                    break;

                                lresult.AddRange(lArray);
                            }
                            catch (Exception)
                            {
                                try
                                {
                                    
                                    object lArrayMediaNodes = Win32NativeMethods.Invoke<object>(mIUnknown, Win32NativeMethods.InvokeFlags.DISPATCH_METHOD, "createOutputNodes", largs1);

                                    if (lArrayMediaNodes == null)
                                        break;

                                    object[] lArray = lArrayMediaNodes as object[];

                                    if (lArray == null)
                                        break;

                                    lresult.AddRange(lArray);
                                }
                                catch (Exception)
                                {
                                    try
                                    {

                                        object lArrayMediaNodes = Win32NativeMethods.Invoke<object>(mIUnknown, Win32NativeMethods.InvokeFlags.DISPATCH_METHOD, "createOutputNodes", largs2);

                                        if (lArrayMediaNodes == null)
                                            break;

                                        object[] lArray = lArrayMediaNodes as object[];

                                        if (lArray == null)
                                            break;

                                        lresult.AddRange(lArray);
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
        
        public async Task<List<object>> createOutputNodesAsync(IntPtr aHandle, object aPtrUnkSharedResource, uint aOutputNodeAmount)
        {                        
            return await createOutputNodesTask(aHandle, aPtrUnkSharedResource, aOutputNodeAmount, true);
        }
        
        public bool createOutputNodes(
            IntPtr aHWND,
            uint aOutputNodeAmount,
            out List<object> aTopologyOutputNodesList)
        {
            bool lresult = false;

            aTopologyOutputNodesList = createOutputNodesTask(aHWND, null, aOutputNodeAmount, false).Result;

            lresult = aTopologyOutputNodesList.Count != 0;

            return lresult;
        }

        public bool createOutputNodes(IntPtr aHandle, object aPtrUnkSharedResource, uint aOutputNodeAmount, out List<object> aTopologyOutputNodesList)
        {
            bool lresult = false;

            aTopologyOutputNodesList = createOutputNodesTask(aHandle, aPtrUnkSharedResource, aOutputNodeAmount, false).Result;

            lresult = aTopologyOutputNodesList.Count != 0;

            return lresult;
        }

        public async Task<List<object>> createOutputNodesAsync(IntPtr aHWND, uint aOutputNodeAmount)
        {
            return await createOutputNodesTask(aHWND, null, aOutputNodeAmount, true);
        }
    }
}
