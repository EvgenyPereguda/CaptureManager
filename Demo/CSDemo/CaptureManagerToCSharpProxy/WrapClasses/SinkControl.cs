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
    internal class SinkControl : ISinkControl, ISinkControlAsync
    {
        CaptureManagerLibrary.ISinkControl mSinkControl;

        public SinkControl(CaptureManagerLibrary.ISinkControl aSinkControl)
        {
            mSinkControl = aSinkControl;
        }
        
        private async Task<FileSinkFactory> createSinkFactoryTask(
            Guid aContainerTypeGUID,
            bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                FileSinkFactory lresult = null;
                
                do
                {
                    if (mSinkControl == null)
                        break;


                    try
                    {
                        object lIUnknown;

                        mSinkControl.createSinkFactory(
                            aContainerTypeGUID,
                            typeof(CaptureManagerLibrary.IFileSinkFactory).GUID,
                            out lIUnknown);

                        if (lIUnknown == null)
                            break;

                        var lFileSinkFactory = lIUnknown as CaptureManagerLibrary.IFileSinkFactory;

                        if (lFileSinkFactory == null)
                            break;

                        lresult = new FileSinkFactory(lFileSinkFactory);
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }


        public async Task<IFileSinkFactoryAsync> createFileSinkFactoryAsync(Guid aContainerTypeGUID)
        {
            return await createSinkFactoryTask(aContainerTypeGUID, true);
        }

        public bool createSinkFactory(
            Guid aContainerTypeGUID, 
            out IFileSinkFactory aSinkFactory)
        {
            bool lresult = false;

            aSinkFactory = createSinkFactoryTask(aContainerTypeGUID, false).Result;

            lresult = aSinkFactory != null;

            return lresult;
        }

        private async Task<SampleGrabberCallSinkFactory> createSampleGrabberCallSinkFactoryTask(Guid aContainerTypeGUID, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                SampleGrabberCallSinkFactory lresult = null;
                
                do
                {
                    if (mSinkControl == null)
                        break;

                    try
                    {
                        object lIUnknown;

                        mSinkControl.createSinkFactory(
                            aContainerTypeGUID,
                            typeof(CaptureManagerLibrary.ISampleGrabberCallSinkFactory).GUID,
                            out lIUnknown);

                        if (lIUnknown == null)
                            break;

                        var lFileSinkFactory = lIUnknown as CaptureManagerLibrary.ISampleGrabberCallSinkFactory;

                        if (lFileSinkFactory == null)
                            break;

                        lresult = new SampleGrabberCallSinkFactory(lFileSinkFactory);

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);
   
                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<ISampleGrabberCallSinkFactoryAsync> createSampleGrabberCallSinkFactoryAsync(Guid aContainerTypeGUID)
        {
            return await createSampleGrabberCallSinkFactoryTask(aContainerTypeGUID, true);
        }

        public bool createSinkFactory(
            Guid aContainerTypeGUID,
            out ISampleGrabberCallSinkFactory aSinkFactory)
        {
            bool lresult = false;

            aSinkFactory = null;

            do
	        {
                if(mSinkControl == null)
                    break;

                aSinkFactory = createSampleGrabberCallSinkFactoryTask(aContainerTypeGUID, false).Result;

                if (aSinkFactory == null)
                    break;

                lresult = true;

            } while (false);

            return lresult;
        }

        private async Task<SampleGrabberCallbackSinkFactory> createSampleGrabberCallbackSinkFactoryTask(Guid aContainerTypeGUID, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                SampleGrabberCallbackSinkFactory lresult = null;

                do
                {
                    if (mSinkControl == null)
                        break;

                    try
                    {
                        object lIUnknown;

                        mSinkControl.createSinkFactory(
                            aContainerTypeGUID,
                            typeof(CaptureManagerLibrary.ISampleGrabberCallbackSinkFactory).GUID,
                            out lIUnknown);

                        if (lIUnknown == null)
                            break;

                        var lFileSinkFactory = lIUnknown as CaptureManagerLibrary.ISampleGrabberCallbackSinkFactory;

                        if (lFileSinkFactory == null)
                            break;

                        lresult = new SampleGrabberCallbackSinkFactory(lFileSinkFactory);

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<ISampleGrabberCallbackSinkFactoryAsync> createSampleGrabberCallbackSinkFactoryAsync(Guid aContainerTypeGUID)
        {
            return await createSampleGrabberCallbackSinkFactoryTask(aContainerTypeGUID, true);
        }

        public bool createSinkFactory(
            Guid aContainerTypeGUID, 
            out ISampleGrabberCallbackSinkFactory aSinkFactory)
        {
            bool lresult = false;

            aSinkFactory = createSampleGrabberCallbackSinkFactoryTask(aContainerTypeGUID, false).Result;

            lresult = aSinkFactory != null;

            return lresult;
        }
               
        private async Task<EVRSinkFactory> createEVRSinkFactoryTask(Guid aContainerTypeGUID, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                EVRSinkFactory lresult = null;

                do
                {
                    if (mSinkControl == null)
                        break;


                    try
                    {
                        object lIUnknown;

                        mSinkControl.createSinkFactory(
                            aContainerTypeGUID,
                            typeof(CaptureManagerLibrary.IEVRSinkFactory).GUID,
                            out lIUnknown);

                        if (lIUnknown == null)
                            break;

                        var lEVRSinkFactory = lIUnknown as CaptureManagerLibrary.IEVRSinkFactory;

                        if (lEVRSinkFactory == null)
                            break;

                        lresult = new EVRSinkFactory(lEVRSinkFactory);
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }
        
        public async Task<IEVRSinkFactoryAsync> createEVRSinkFactoryAsync(Guid aContainerTypeGUID)
        {
            return await createEVRSinkFactoryTask(aContainerTypeGUID, true);
        }

        public bool createSinkFactory(
            Guid aContainerTypeGUID, 
            out IEVRSinkFactory aSinkFactory)
        {
            bool lresult = false;

            aSinkFactory = createEVRSinkFactoryTask(aContainerTypeGUID, false).Result;

            lresult = aSinkFactory != null;

            return lresult;
        }

        private async Task<ByteStreamSinkFactory> createByteStreamSinkFactoryTask(Guid aContainerTypeGUID, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                ByteStreamSinkFactory lresult = null;

                do
                {
                    if (mSinkControl == null)
                        break;


                    try
                    {
                        object lIUnknown;

                        mSinkControl.createSinkFactory(
                            aContainerTypeGUID,
                            typeof(CaptureManagerLibrary.IByteStreamSinkFactory).GUID,
                            out lIUnknown);

                        if (lIUnknown == null)
                            break;

                        var lByteStreamSinkFactory = lIUnknown as CaptureManagerLibrary.IByteStreamSinkFactory;

                        if (lByteStreamSinkFactory == null)
                            break;

                        lresult = new ByteStreamSinkFactory(lByteStreamSinkFactory);
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<IByteStreamSinkFactoryAsync> createByteStreamSinkFactoryAsync(Guid aContainerTypeGUID)
        {
            return await createByteStreamSinkFactoryTask(aContainerTypeGUID, true);
        }

        public bool createSinkFactory(
            Guid aContainerTypeGUID, 
            out IByteStreamSinkFactory aSinkFactory)
        {
            bool lresult = false;

            aSinkFactory = createByteStreamSinkFactoryTask(aContainerTypeGUID, false).Result;

            lresult = aSinkFactory != null;

            return lresult;
        }

        private async Task<EVRMultiSinkFactory> createEVRMultiSinkFactoryTask(Guid aContainerTypeGUID, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                EVRMultiSinkFactory lresult = null;

                do
                {
                    if (mSinkControl == null)
                        break;

                    try
                    {
                        object lIUnknown;

                        mSinkControl.createSinkFactory(
                            aContainerTypeGUID,
                            typeof(CaptureManagerLibrary.IEVRMultiSinkFactory).GUID,
                            out lIUnknown);

                        if (lIUnknown == null)
                            break;

                        var lEVRSinkFactory = lIUnknown as CaptureManagerLibrary.IEVRMultiSinkFactory;

                        if (lEVRSinkFactory == null)
                            break;

                        lresult = new EVRMultiSinkFactory(lIUnknown);
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }
        
        public async Task<IEVRMultiSinkFactoryAsync> createEVRMultiSinkFactoryAsync(Guid aContainerTypeGUID)
        {
            return await createEVRMultiSinkFactoryTask(aContainerTypeGUID, true);
        }

        public bool createSinkFactory(Guid aContainerTypeGUID, out IEVRMultiSinkFactory aSinkFactory)
        {
            bool lresult = false;

            aSinkFactory = createEVRMultiSinkFactoryTask(aContainerTypeGUID, false).Result;

            lresult = aSinkFactory != null;

            return lresult;
        }

        private async Task<CompatibleEVRMultiSinkFactory> createCompatibleEVRMultiSinkFactoryTask(Guid aContainerTypeGUID, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                CompatibleEVRMultiSinkFactory lresult = null;

                do
                {
                    if (mSinkControl == null)
                        break;

                    try
                    {
                        object lIUnknown;

                        mSinkControl.createSinkFactory(
                            aContainerTypeGUID,
                            new Guid("{A2224D8D-C3C1-4593-8AC9-C0FCF318FF05}"),// typeof(CaptureManagerLibrary.IEVRMultiSinkFactory).GUID,
                            out lIUnknown);

                        if (lIUnknown == null)
                            break;

                        var lEVRSinkFactory = lIUnknown as CaptureManagerLibrary.IEVRMultiSinkFactory;

                        if (lEVRSinkFactory == null)
                            break;

                        lresult = new CompatibleEVRMultiSinkFactory(lIUnknown);
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<IEVRMultiSinkFactoryAsync> createCompatibleEVRMultiSinkFactoryAsync(Guid aContainerTypeGUID)
        {
            return await createCompatibleEVRMultiSinkFactoryTask(aContainerTypeGUID, true);
        }

        public bool createCompatibleEVRMultiSinkFactory(Guid aContainerTypeGUID, out IEVRMultiSinkFactory aSinkFactory)
        {
            bool lresult = false;

            aSinkFactory = createCompatibleEVRMultiSinkFactoryTask(aContainerTypeGUID, false).Result;

            lresult = aSinkFactory != null;

            return lresult;
        }

        private async Task<SARSinkFactory> createSARSinkFactoryTask(Guid aContainerTypeGUID, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                SARSinkFactory lresult = null;


                do
                {
                    if (mSinkControl == null)
                        break;


                    try
                    {
                        object lIUnknown;

                        mSinkControl.createSinkFactory(
                            aContainerTypeGUID,
                            typeof(CaptureManagerLibrary.ISARSinkFactory).GUID,
                            out lIUnknown);

                        if (lIUnknown == null)
                            break;

                        var lSinkFactory = lIUnknown as CaptureManagerLibrary.ISARSinkFactory;

                        if (lSinkFactory == null)
                            break;

                        lresult = new SARSinkFactory(lIUnknown);
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public bool createSinkFactory(Guid aContainerTypeGUID, out ISARSinkFactory aSinkFactory)
        {
            bool lresult = false;

            aSinkFactory = createSARSinkFactoryTask(aContainerTypeGUID, false).Result;

            lresult = aSinkFactory != null;

            return lresult;
        }
        
        public async Task<ISARSinkFactoryAsync> createSARSinkFactoryAsync(Guid aContainerTypeGUID)
        {
            return await createSARSinkFactoryTask(aContainerTypeGUID, true);
        }

    }
}
