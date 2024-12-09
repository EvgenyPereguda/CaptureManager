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
    class SourceControl : ISourceControl, ISourceControlAsync
    {
        CaptureManagerLibrary.ISourceControl mSourceControl;
        
        public SourceControl(
            CaptureManagerLibrary.ISourceControl aSourceControl)
        {
            mSourceControl = aSourceControl;
        }



        private async Task<object> getSourceOutputMediaTypeTask(string aSymbolicLink, uint aIndexStream, uint aIndexMediaType, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                object lresult = null;

                do
                {
                    if (mSourceControl == null)
                        break;

                    try
                    {

                        mSourceControl.getSourceOutputMediaType(
                            aSymbolicLink,
                            aIndexStream,
                            aIndexMediaType,
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

        public async Task<object> getSourceOutputMediaTypeAsync(string aSymbolicLink, uint aIndexStream, uint aIndexMediaType)
        {
            return await getSourceOutputMediaTypeTask(aSymbolicLink, aIndexStream, aIndexMediaType, true);
        }

        public bool getSourceOutputMediaType(
            string aSymbolicLink, 
            uint aIndexStream, 
            uint aIndexMediaType, 
            out object aPtrPtrOutputMediaType)
        {
            bool lresult = false;

            aPtrPtrOutputMediaType = getSourceOutputMediaTypeTask(aSymbolicLink, aIndexStream, aIndexMediaType, false).Result;

            lresult = aPtrPtrOutputMediaType != null;

            return lresult;
        }

        private async Task<object> createSourceNodeWithDownStreamConnectionTask(
            string aSymbolicLink,
            uint aIndexStream,
            uint aIndexMediaType,
           object aPtrDownStreamTopologyNode,
           bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                object lresult = null;

                do
                {
                    if (mSourceControl == null)
                        break;

                    try
                    {

                        mSourceControl.createSourceNodeWithDownStreamConnection(
                            aSymbolicLink,
                            aIndexStream,
                            aIndexMediaType,
                            aPtrDownStreamTopologyNode,
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

        public async Task<object> createSourceNodeWithDownStreamConnectionAsync(
            string aSymbolicLink,
            uint aIndexStream,
            uint aIndexMediaType,
           object aPtrDownStreamTopologyNode)
        {
            return await createSourceNodeWithDownStreamConnectionTask(
                    aSymbolicLink,
                    aIndexStream,
                    aIndexMediaType,
                    aPtrDownStreamTopologyNode, true);
        }

        public bool createSourceNode(
           string aSymbolicLink, 
           uint aIndexStream,
           uint aIndexMediaType,
           object aPtrDownStreamTopologyNode,
           out object aPtrPtrTopologyNode)
        {
            bool lresult = false;

            aPtrPtrTopologyNode = createSourceNodeWithDownStreamConnectionTask(
                    aSymbolicLink,
                    aIndexStream,
                    aIndexMediaType,
                    aPtrDownStreamTopologyNode, false).Result;

            lresult = aPtrPtrTopologyNode != null;

            return lresult;
        }

        private async Task<WebCamControl> createWebCamControlTask(string aSymbolicLink, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                WebCamControl lresult = null;

                do
                {

                    try
                    {
                        if (mSourceControl == null)
                            break;

                        object lUnknown;

                        mSourceControl.createSourceControl(
                            aSymbolicLink,
                            typeof(CaptureManagerLibrary.IWebCamControl).GUID,
                            out lUnknown);

                        var lWebCamControl = lUnknown as CaptureManagerLibrary.IWebCamControl;

                        if (lWebCamControl == null)
                            break;

                        lresult = new WebCamControl(lWebCamControl);

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<IWebCamControlAsync> createWebCamControlAsync(string aSymbolicLink)
        {
            return await createWebCamControlTask(aSymbolicLink, true);
        }

        public IWebCamControl createWebCamControl(string aSymbolicLink)
        {
            return createWebCamControlTask(aSymbolicLink, false).Result;
        }
        
        private async Task<object> createSourceTask(
            string aSymbolicLink,
            bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                object lresult = null;

                do
                {
                    if (mSourceControl == null)
                        break;

                    try
                    {

                        mSourceControl.createSource(
                            aSymbolicLink,
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

        public async Task<object> createSourceAsync(
            string aSymbolicLink)
        {
            return await createSourceTask(aSymbolicLink, true);
        }

        public bool createSource(
            string aSymbolicLink, 
            out object aPtrPtrMediaSource)
        {
            bool lresult = false;

            aPtrPtrMediaSource = createSourceTask(aSymbolicLink, false).Result;

            lresult = aPtrPtrMediaSource != null;

            return lresult;
        }

        private async Task<object> createSourceFromCaptureProcessorTask(
            object aPtrCaptureProcessor,
            bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                object lresult = null;

                do
                {
                    if (mSourceControl == null)
                        break;

                    try
                    {
                        ICaptureProcessor lICaptureProcessor = aPtrCaptureProcessor as ICaptureProcessor;

                        if (lICaptureProcessor != null)
                        {
                            CaptureProcessor lCaptureProcessor = new CaptureProcessor(lICaptureProcessor);

                            mSourceControl.createSourceFromCaptureProcessor(
                                lCaptureProcessor,
                                out lresult);
                        }
                        else
                        {
                            var lNativeCaptureProcessor = aPtrCaptureProcessor as CaptureManagerLibrary.ICaptureProcessor;

                            if (lNativeCaptureProcessor == null)
                                break;

                            mSourceControl.createSourceFromCaptureProcessor(
                                lNativeCaptureProcessor,
                                out lresult);
                        }
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<object> createSourceFromCaptureProcessorAsync(object aPtrCaptureProcessor)
        {
            return await createSourceFromCaptureProcessorTask(aPtrCaptureProcessor, true);
        }

        public bool createSourceFromCaptureProcessor(
            object aPtrCaptureProcessor, 
            out object aPtrPtrMediaSource)
        {
            bool lresult = false;

            aPtrPtrMediaSource = createSourceFromCaptureProcessorTask(aPtrCaptureProcessor, false).Result;

            lresult = aPtrPtrMediaSource != null;

            return lresult;
        }

        private async Task<object> createSourceNodeTask(
            string aSymbolicLink,
            uint aIndexStream,
            uint aIndexMediaType,
            bool aIsAwait
            )
        {
            return await Task.Run(() =>
            {

                object lresult = null;
                
                do
                {
                    if (mSourceControl == null)
                        break;

                    try
                    {

                        mSourceControl.createSourceNode(
                            aSymbolicLink,
                            aIndexStream,
                            aIndexMediaType,
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

        public async Task<object> createSourceNodeAsync(
            string aSymbolicLink,
            uint aIndexStream,
            uint aIndexMediaType)
        {
            return await createSourceNodeTask(
                    aSymbolicLink,
                    aIndexStream,
                    aIndexMediaType,
                    true);
        }

        public bool createSourceNode(
            string aSymbolicLink, 
            uint aIndexStream, 
            uint aIndexMediaType, 
            out object aPtrPtrTopologyNode)
        {
            bool lresult = false;

            aPtrPtrTopologyNode = createSourceNodeTask(
                    aSymbolicLink,
                    aIndexStream,
                    aIndexMediaType,
                    false).Result;

            lresult = aPtrPtrTopologyNode != null;

            return lresult;
        }

        private async Task<object> createSourceNodeFromExternalSourceTask(
            object aPtrMediaSource,
            uint aIndexStream,
            uint aIndexMediaType,
            bool aIsAwait
            )
        {
            return await Task.Run(() =>
            {

                object lresult = null;

                do
                {
                    if (mSourceControl == null)
                        break;

                    try
                    {
                        mSourceControl.createSourceNodeFromExternalSource(
                            aPtrMediaSource,
                            aIndexStream,
                            aIndexMediaType,
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

        public async Task<object> createSourceNodeFromExternalSourceAsync(
            object aPtrMediaSource,
            uint aIndexStream,
            uint aIndexMediaType)
        {
            return await createSourceNodeFromExternalSourceTask(
                aPtrMediaSource,
                aIndexStream,
                aIndexMediaType,
                true);
        }

        public bool createSourceNodeFromExternalSource(
            object aPtrMediaSource, 
            uint aIndexStream, 
            uint aIndexMediaType, 
            out object aPtrPtrTopologyNode)
        {
            bool lresult = false;

            aPtrPtrTopologyNode = createSourceNodeFromExternalSourceTask(
                aPtrMediaSource,
                aIndexStream,
                aIndexMediaType,
                false).Result;

            lresult = aPtrPtrTopologyNode != null;

            return lresult;
        }


        private async Task<object> createSourceNodeFromExternalSourceWithDownStreamConnectionTask(
            object aPtrMediaSource,
            uint aIndexStream,
            uint aIndexMediaType,
            object aPtrDownStreamTopologyNode,
            bool aIsAwait
            )
        {
            return await Task.Run(() =>
            {

                object lresult = null;

                do
                {
                    if (mSourceControl == null)
                        break;

                    try
                    {

                        mSourceControl.createSourceNodeFromExternalSourceWithDownStreamConnection(
                            aPtrMediaSource,
                            aIndexStream,
                            aIndexMediaType,
                            aPtrDownStreamTopologyNode,
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

        public async Task<object> createSourceNodeFromExternalSourceWithDownStreamConnectionAsync(
            object aPtrMediaSource,
            uint aIndexStream,
            uint aIndexMediaType,
            object aPtrDownStreamTopologyNode)
        {
            return await createSourceNodeFromExternalSourceWithDownStreamConnectionTask(
                aPtrMediaSource,
                aIndexStream,
                aIndexMediaType,
                aPtrDownStreamTopologyNode,
                true);
        }

        public bool createSourceNodeFromExternalSourceWithDownStreamConnection(
            object aPtrMediaSource, 
            uint aIndexStream, 
            uint aIndexMediaType, 
            object aPtrDownStreamTopologyNode, 
            out object aPtrPtrTopologyNode)
        {
            bool lresult = false;

            aPtrPtrTopologyNode = createSourceNodeFromExternalSourceWithDownStreamConnectionTask(
                aPtrMediaSource,
                aIndexStream,
                aIndexMediaType,
                aPtrDownStreamTopologyNode,
                false).Result;

            lresult = aPtrPtrTopologyNode != null;

            return lresult;
        }
                
        private async Task<string> getCollectionOfSourcesTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                string lresult = "";

                IntPtr lPtrXMLstring = IntPtr.Zero;

                do
                {
                    if (mSourceControl == null)
                        break;

                    try
                    {

                        (mSourceControl as ISourceControlInner).getCollectionOfSources(out lPtrXMLstring);

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

        public async Task<string> getCollectionOfSourcesAsync()
        {
            return await getCollectionOfSourcesTask(true);
        }

        public bool getCollectionOfSources(
            ref string aPtrPtrXMLstring)
        {
            bool lresult = false;

            aPtrPtrXMLstring = getCollectionOfSourcesTask(false).Result;

            lresult = !string.IsNullOrWhiteSpace(aPtrPtrXMLstring);

            return lresult;
        }

        private async Task<object> getSourceOutputMediaTypeFromMediaSourceTask(
            object aPtrMediaSource,
            uint aIndexStream,
            uint aIndexMediaType,
            bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                object lresult = null;
                
                do
                {
                    if (mSourceControl == null)
                        break;

                    try
                    {
                        mSourceControl.getSourceOutputMediaTypeFromMediaSource(
                            aPtrMediaSource,
                            aIndexStream,
                            aIndexMediaType,
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

        public async Task<object> getSourceOutputMediaTypeFromMediaSourceAsync(
            object aPtrMediaSource,
            uint aIndexStream,
            uint aIndexMediaType)
        {
            return await getSourceOutputMediaTypeFromMediaSourceTask(
                aPtrMediaSource,
                aIndexStream,
                aIndexMediaType,
                true);
        }

        public bool getSourceOutputMediaTypeFromMediaSource(
            object aPtrMediaSource, 
            uint aIndexStream, 
            uint aIndexMediaType, 
            out object aPtrPtrOutputMediaType)
        {
            bool lresult = false;

            aPtrPtrOutputMediaType = getSourceOutputMediaTypeFromMediaSourceTask(
                aPtrMediaSource,
                aIndexStream,
                aIndexMediaType,
                false).Result;

            lresult = aPtrPtrOutputMediaType != null;

            return lresult;
        }

    }
}
