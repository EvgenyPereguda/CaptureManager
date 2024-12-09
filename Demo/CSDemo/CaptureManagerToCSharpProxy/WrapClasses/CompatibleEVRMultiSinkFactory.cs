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
    [Guid("D4BB697C-E0C9-4ABE-B02F-F24557AF1DA8")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [System.Security.SuppressUnmanagedCodeSecurity]
    interface ICustomisedFilterTopologyNode
    {

    }

    class CompatibleEVRMultiSinkFactory : IEVRMultiSinkFactory, IEVRMultiSinkFactoryAsync
    {
        private CaptureManagerLibrary.IEVRMultiSinkFactory mIEVRMultiSinkFactory;

        private object mIUnknown;

        public CompatibleEVRMultiSinkFactory(object aIUnknown)
        {
            mIEVRMultiSinkFactory = aIUnknown as CaptureManagerLibrary.IEVRMultiSinkFactory;

            mIUnknown = aIUnknown;
        }

        private async Task<List<IntPtr>> createOutputNodesTask(IntPtr aHandle, object aPtrUnkSharedResource, uint aOutputNodeAmount, bool aIsAwait)
        {

            IntPtr lptr = IntPtr.Zero;

            if (aPtrUnkSharedResource != null)
                lptr = Marshal.GetIUnknownForObject(aPtrUnkSharedResource);

            object[] largs = new object[] { aHandle.ToInt64(), aPtrUnkSharedResource, aOutputNodeAmount };

            object[] largs1 = new object[] { aHandle.ToInt64(), lptr.ToInt64(), aHandle.ToInt64(), aPtrUnkSharedResource, aOutputNodeAmount };

            object[] largs2 = new object[] { aHandle.ToInt64(), lptr.ToInt64(), aOutputNodeAmount };

            return await Task.Run(() =>
            {
                List<IntPtr> lresult = new List<IntPtr>();

                List<object> ltempResult = new List<object>();

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

                        ltempResult.AddRange(lArray);
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

                                ltempResult.AddRange(lArray);
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

                                    ltempResult.AddRange(lArray);
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

                                        ltempResult.AddRange(lArray);
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

                foreach (var item in ltempResult)
                {
                    var lunk = Marshal.GetIUnknownForObject(item);

                    IntPtr lUnkQuery = IntPtr.Zero;

                    Marshal.QueryInterface(lunk, ref Win32NativeMethods.IID_IUnknown, out lUnkQuery);

                    lresult.Add(lUnkQuery);

                    Marshal.Release(lunk);

                    Marshal.Release(lUnkQuery);
                }

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<List<object>> createOutputNodesAsync(IntPtr aHandle, object aPtrUnkSharedResource, uint aOutputNodeAmount)
        {
            var lUnkList = await createOutputNodesTask(aHandle, aPtrUnkSharedResource, aOutputNodeAmount, true);

            return queryCustomisedFilterTopologyNode(lUnkList);
        }
        
        public bool createOutputNodes(IntPtr aHWND, uint aOutputNodeAmount, out List<object> aTopologyOutputNodesList)
        {
            bool lresult = false;

            aTopologyOutputNodesList = queryCustomisedFilterTopologyNode(
                createOutputNodesTask(aHWND, null, aOutputNodeAmount, false).Result);

            lresult = aTopologyOutputNodesList.Count != 0;

            return lresult;
        }

        public bool createOutputNodes(IntPtr aHandle, object aPtrUnkSharedResource, uint aOutputNodeAmount, out List<object> aTopologyOutputNodesList)
        {
            bool lresult = false;

            aTopologyOutputNodesList = queryCustomisedFilterTopologyNode(
                createOutputNodesTask(aHandle, aPtrUnkSharedResource, aOutputNodeAmount, false).Result);

            lresult = aTopologyOutputNodesList.Count != 0;

            return lresult;
        }

        private List<object> queryCustomisedFilterTopologyNode(List<IntPtr> aUnkList)
        {
            List<object> lCustomisedFilterTopologyNode = new List<object>();

            foreach (var item in aUnkList)
            {
                IntPtr lICustomisedFilterTopologyNode = IntPtr.Zero;

                Guid IID_ICustomisedFilterTopologyNode = typeof(ICustomisedFilterTopologyNode).GUID;

                System.Runtime.InteropServices.Marshal.QueryInterface(item, ref IID_ICustomisedFilterTopologyNode, out lICustomisedFilterTopologyNode);

                object lnode = Marshal.GetObjectForIUnknown(lICustomisedFilterTopologyNode);

                lCustomisedFilterTopologyNode.Add(lnode);

                Marshal.Release(item);

                Marshal.Release(item);

                Marshal.Release(lICustomisedFilterTopologyNode);
            }

            return lCustomisedFilterTopologyNode;
        }

        public async Task<List<object>> createOutputNodesAsync(IntPtr aHWND, uint aOutputNodeAmount)
        {
            var lUnkList = await createOutputNodesTask(aHWND, null, aOutputNodeAmount, true);

            return queryCustomisedFilterTopologyNode(lUnkList);
        }
    }
}
