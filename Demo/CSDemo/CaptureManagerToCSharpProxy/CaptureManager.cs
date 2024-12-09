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
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using CaptureManagerToCSharpProxy.Interfaces;
using CaptureManagerToCSharpProxy.WrapClasses;

namespace CaptureManagerToCSharpProxy
{
    public class CaptureManager
    {

        private class CaptureManagerNative
        {
            CaptureManagerLibrary.ILogPrintOutControl mILogPrintOutControl;
            CaptureManagerLibrary.ILogPrintOutCallbackControl mILogPrintOutCallbackControl;
            CaptureManagerLibrary.ICaptureManagerControl mICaptureManagerControl;

            public CaptureManagerLibrary.ILogPrintOutControl ILogPrintOutControl
            {
                get
                {
                    return mILogPrintOutControl;
                }
            }

            public CaptureManagerLibrary.ICaptureManagerControl ICaptureManagerControl
            {
                get
                {
                    return mICaptureManagerControl;
                }
            }

            public CaptureManagerNative()
            {
                blockedMTAInit(() => {
                    try
                    {
                        mILogPrintOutControl = new CaptureManagerLibrary.CoLogPrintOut() as CaptureManagerLibrary.ILogPrintOutControl;

                        if (mILogPrintOutControl == null)
                            throw new NullReferenceException("mILogPrintOutControl is null");


                        mILogPrintOutControl.addPrintOutDestination(
                            (int)CaptureManagerLibrary.LogLevel.ERROR_LEVEL,
                            "Log.txt");

                        mILogPrintOutControl.addPrintOutDestination(
                            (int)CaptureManagerLibrary.LogLevel.INFO_LEVEL,
                            "Log.txt");

                        mILogPrintOutCallbackControl = mILogPrintOutControl as CaptureManagerLibrary.ILogPrintOutCallbackControl;

                        do
                        {
                            try
                            {

                                object lUnknown;

                                mILogPrintOutCallbackControl.getIConnectionPointContainer(
                                    typeof(IConnectionPointContainer).GUID,
                                    out lUnknown);

                                if (lUnknown == null)
                                    break;

                                var lConnectionPointContainer = lUnknown as IConnectionPointContainer;

                                if (lConnectionPointContainer == null)
                                    break;

                                IConnectionPoint lConnectionPoint = null;

                                lConnectionPointContainer.FindConnectionPoint(
                                    typeof(CaptureManagerLibrary.ILogPrintOutCallback).GUID,
                                    out lConnectionPoint);

                                if (lConnectionPoint == null)
                                    break;

                                int lId = 0;

                                lConnectionPoint.Advise(
                                    new LogPrintOutCallback((aLogLevel, aMessage) => {

                                        LogManager.getInstance().writeLog(aLogLevel, aMessage);

                                    }),
                                    out lId);

                            }
                            catch (Exception exc)
                            {
                                LogManager.getInstance().write(exc.Message);
                            }

                        } while (false);
                        

                        mICaptureManagerControl = new CaptureManagerLibrary.CoCaptureManager();

                        if (mICaptureManagerControl == null)
                            throw new NullReferenceException("mCaptureManagerNative.ICaptureManagerControl is null");

                    }
                    catch (Exception exc)
                    {
                        throw exc;
                    }
                });
            }

            public CaptureManagerNative(string aFileName)
            {
                blockedMTAInit(()=> {
                    try
                    {

                        var lDLLModuleAddr = Win32NativeMethods.LoadLibrary(aFileName);

                        if (lDLLModuleAddr == null)
                            throw new NullReferenceException("lDLLModuleAddr is null");

                        var lEnterProcAddr = Win32NativeMethods.GetProcAddress(lDLLModuleAddr, "DllGetClassObject");

                        if (lEnterProcAddr == null)
                            throw new NullReferenceException("lEnterProcAddr is null");

                        var lGetClassObject = Marshal.GetDelegateForFunctionPointer(lEnterProcAddr,
                            typeof(Win32NativeMethods.DllGetClassObjectDelegate))
                            as Win32NativeMethods.DllGetClassObjectDelegate;

                        if (lGetClassObject == null)
                            throw new NullReferenceException("lGetClassObject is null");

                        var CLSID_CoLogPrintOut = new Guid("4563EE3E-DA1E-4911-9F40-88A284E2DD69");

                        var CLSID_CoCaptureManager = new Guid("D5F07FB8-CE60-4017-B215-95C8A0DDF42A");

                        object lUnknown;

                        IClassFactory lFactory;

                        lGetClassObject(
                            CLSID_CoLogPrintOut,
                            typeof(IClassFactory).GUID,
                            out lUnknown);

                        lFactory = lUnknown as IClassFactory;

                        if (lFactory == null)
                            throw new NullReferenceException("lFactory of CoLogPrintOut is null");

                        lFactory.CreateInstance(
                            null,
                            typeof(CaptureManagerLibrary.ILogPrintOutControl).GUID,
                            out lUnknown);

                        lFactory.LockServer(true);

                        mILogPrintOutControl = lUnknown as CaptureManagerLibrary.ILogPrintOutControl;

                        if (mILogPrintOutControl == null)
                            throw new NullReferenceException("mILogPrintOutControl is null");

                        lGetClassObject(
                            CLSID_CoCaptureManager,
                            typeof(IClassFactory).GUID,
                            out lUnknown);

                        lFactory = lUnknown as IClassFactory;

                        if (lFactory == null)
                            throw new NullReferenceException("lFactory of CoCaptureManager is null");

                        lFactory.CreateInstance(
                            null,
                            typeof(CaptureManagerLibrary.ICaptureManagerControl).GUID,
                            out lUnknown);

                        lFactory.LockServer(true);

                        mICaptureManagerControl = lUnknown as CaptureManagerLibrary.ICaptureManagerControl;

                        if (mICaptureManagerControl == null)
                            throw new NullReferenceException("mCaptureManagerNative.ICaptureManagerControl is null");
                    }
                    catch (Exception exc)
                    {
                        throw exc;
                    }
                });
            }

            ~CaptureManagerNative()
            {
            }
                        
            private void blockedMTAInit(Action aInitAction)
            {

                Thread lInnerMTAThread = new Thread(() =>
                {
                    try
                    {
                        aInitAction();
                    }
                    catch (Exception exc)
                    {
                        throw exc;
                    }
                });

                lInnerMTAThread.TrySetApartmentState(ApartmentState.MTA);

                lInnerMTAThread.Start();

                lInnerMTAThread.Join();
            }
        }

        private static CaptureManagerNative mCaptureManagerNative = null;

        private string getFullFilePath(string aFileName)
        {
            return System.IO.Path.GetDirectoryName(Assembly.GetExecutingAssembly().GetName().CodeBase).Replace("file:\\", "") + "\\" + aFileName;
        }

        public CaptureManager()
        {

            try
            {
                do
                {
                    if (mCaptureManagerNative != null)
                        break;

                    mCaptureManagerNative = new CaptureManagerNative();

                    if (mCaptureManagerNative == null)
                        throw new NullReferenceException("mCaptureManagerNative is null");

                } while (false);

            }
            catch (Exception exc)
            {
                LogManager.getInstance().write(exc.Message);

                throw exc;
            }
        }

        public CaptureManager(string aFileName)
        {
            try
            {
                string lFullFilePath = aFileName;

                if (!File.Exists(lFullFilePath))
                {
                    lFullFilePath = getFullFilePath(aFileName);

                    if (!File.Exists(lFullFilePath))
                    {
                        throw new Exception("File " + aFileName + " is not accessseble!!!");
                    }
                }

                do
                {
                    if (mCaptureManagerNative != null)
                        break;

                    mCaptureManagerNative = new CaptureManagerNative(lFullFilePath);

                    if (mCaptureManagerNative == null)
                        throw new NullReferenceException("mCaptureManagerNative is null");

                } while (false);
            }
            catch (Exception exc)
            {
                LogManager.getInstance().write(exc.Message);

                throw exc;
            }

        }
        
        private bool checkICaptureManagerControl()
        {
            return mCaptureManagerNative == null;
        }
        
        private async Task<string> getCollectionOfSourcesTask(bool aIsAwait)
        {            
            return await Task.Run(() =>
            {
                string lInfoString = "";

                IntPtr lPtrXMLstring = IntPtr.Zero;

                do
                {
                    if (checkICaptureManagerControl())
                        break;

                    object lUnknown;

                    mCaptureManagerNative.ICaptureManagerControl.createControl(
                        typeof(CaptureManagerLibrary.ISourceControl).GUID,
                        out lUnknown);

                    if (lUnknown == null)
                        break;

                    (lUnknown as ISourceControlInner).getCollectionOfSources(out lPtrXMLstring);

                    if (lPtrXMLstring != IntPtr.Zero)
                        lInfoString = Marshal.PtrToStringBSTR(lPtrXMLstring);

                } while (false);

                if (lPtrXMLstring != IntPtr.Zero)
                    Marshal.FreeBSTR(lPtrXMLstring);

                return lInfoString;

            }).ConfigureAwait(aIsAwait);            
        }

        public async Task<string> getCollectionOfSourcesAsync()
        {
            return await getCollectionOfSourcesTask(true);
        }

        public bool getCollectionOfSources(ref string aInfoString)
        {
            bool lresult = false;

            string lInfoString = "";

            try
            {
                lInfoString = getCollectionOfSourcesTask(false).Result;

                if (string.IsNullOrWhiteSpace(lInfoString))
                    throw new NullReferenceException("lInfoString is null");

                aInfoString = lInfoString;

                lresult = true;

            }
            catch (Exception exc)
            {
                LogManager.getInstance().write(exc.Message);
            }

            return lresult;
        }

        private async Task<string> getCollectionOfSinksTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                string lInfoString = "";
                
                IntPtr lPtrXMLstring = IntPtr.Zero;

                do
                {
                    if (checkICaptureManagerControl())
                        break;

                    object lUnknown;

                    mCaptureManagerNative.ICaptureManagerControl.createControl(
                        typeof(CaptureManagerLibrary.ISinkControl).GUID,
                        out lUnknown);

                    if (lUnknown == null)
                        break;

                    (lUnknown as ISinkControlInner).getCollectionOfSinks(out lPtrXMLstring);

                    if (lPtrXMLstring != IntPtr.Zero)
                        lInfoString = Marshal.PtrToStringBSTR(lPtrXMLstring);

                } while (false);

                if (lPtrXMLstring != IntPtr.Zero)
                    Marshal.FreeBSTR(lPtrXMLstring);

                return lInfoString;

            }).ConfigureAwait(aIsAwait);
        }

        public async Task<string> getCollectionOfSinksAsync()
        {
            return await getCollectionOfSinksTask(true);
        }

        public bool getCollectionOfSinks(ref string aInfoString)
        {
            bool lresult = false;
            
            try
            {
                var lInfoString = getCollectionOfSinksTask(false).Result;

                if (string.IsNullOrWhiteSpace(lInfoString))
                    throw new NullReferenceException("lInfoString is null");

                aInfoString = lInfoString;

                lresult = true;

            }
            catch (Exception exc)
            {
                LogManager.getInstance().write(exc.Message);
            }

            return lresult;
        }

        private async Task<string> getCollectionOfEncodersTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {

                string lInfoString = "";

                IntPtr lPtrXMLstring = IntPtr.Zero;


                do
                {
                    if (checkICaptureManagerControl())
                        break;

                    object lUnknown;

                    mCaptureManagerNative.ICaptureManagerControl.createControl(
                        typeof(CaptureManagerLibrary.IEncoderControl).GUID,
                        out lUnknown);

                    if (lUnknown == null)
                        break;

                    (lUnknown as IEncoderControlInner).getCollectionOfEncoders(out lPtrXMLstring);

                    if (lPtrXMLstring != IntPtr.Zero)
                        lInfoString = Marshal.PtrToStringBSTR(lPtrXMLstring);

                } while (false);

                if (lPtrXMLstring != IntPtr.Zero)
                    Marshal.FreeBSTR(lPtrXMLstring);

                return lInfoString;

            }).ConfigureAwait(aIsAwait);
        }

        public async Task<string> getCollectionOfEncodersAsync()
        {
            return await getCollectionOfEncodersTask(true);
        }

        public bool getCollectionOfEncoders(ref string aInfoString)
        {
            bool lresult = false;


            try
            {

                var lInfoString = getCollectionOfEncodersTask(false).Result;

                if (string.IsNullOrWhiteSpace(lInfoString))
                    throw new NullReferenceException("lInfoString is null");

                aInfoString = lInfoString;

                lresult = true;

            }
            catch (Exception exc)
            {
                LogManager.getInstance().write(exc.Message);
            }

            return lresult;
        }

        private async Task<SinkControl> createSinkControlTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                SinkControl lresult = null;

                do
                {

                    object lUnknown;

                    mCaptureManagerNative.ICaptureManagerControl.createControl(
                        typeof(CaptureManagerLibrary.ISinkControl).GUID,
                        out lUnknown);

                    if (lUnknown == null)
                        break;

                    var lSinkControl = lUnknown as CaptureManagerLibrary.ISinkControl;

                    if (lSinkControl == null)
                        break;

                    lresult = new SinkControl(lSinkControl);

                } while (false);

                return lresult;

            }).ConfigureAwait(aIsAwait);
        }

        public async Task<ISinkControlAsync> createSinkControlAsync()
        {
            return await createSinkControlTask(true);
        }

        public ISinkControl createSinkControl()
        {
            return createSinkControlTask(false).Result;
        }

        private async Task<SourceControl> createSourceControlTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                SourceControl lresult = null;
                
                do
                {

                    object lUnknown;

                    mCaptureManagerNative.ICaptureManagerControl.createControl(
                        typeof(CaptureManagerLibrary.ISourceControl).GUID,
                        out lUnknown);

                    if (lUnknown == null)
                        break;

                    var lSourceControl = lUnknown as CaptureManagerLibrary.ISourceControl;

                    if (lSourceControl == null)
                        break;

                    lresult = new SourceControl(lSourceControl);

                } while (false);
         
                return lresult;

            }).ConfigureAwait(aIsAwait);
        }

        public async Task<ISourceControlAsync> createSourceControlAsync()
        {
            return await createSourceControlTask(true);
        }

        public ISourceControl createSourceControl()
        {
            return createSourceControlTask(false).Result;
        }

        private async Task<SessionControl> createSessionControlTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {

                SessionControl lresult = null;

                do
                {
                    object lUnknown;

                    mCaptureManagerNative.ICaptureManagerControl.createControl(
                        typeof(CaptureManagerLibrary.ISessionControl).GUID,
                        out lUnknown);

                    if (lUnknown == null)
                        break;

                    var lSessionControl = lUnknown as CaptureManagerLibrary.ISessionControl;

                    if (lSessionControl == null)
                        break;

                    lresult = new SessionControl(lSessionControl);

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<ISessionControlAsync> createSessionControlAsync()
        {
            return await createSessionControlTask(true);
        }

        public ISessionControl createSessionControl()
        {
            return createSessionControlTask(false).Result;
        }

        private async Task<StreamControl> createStreamControlTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                StreamControl lresult = null;
                
                do
                {
                    object lUnknown;

                    mCaptureManagerNative.ICaptureManagerControl.createControl(
                        typeof(CaptureManagerLibrary.IStreamControl).GUID,
                        out lUnknown);

                    if (lUnknown == null)
                        break;

                    var lSessionControl = lUnknown as CaptureManagerLibrary.IStreamControl;

                    if (lSessionControl == null)
                        break;

                    lresult = new StreamControl(lSessionControl);

                } while (false);
    
                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<IStreamControlAsync> createStreamControlAsync()
        {
            return await createStreamControlTask(true);
        }

        public IStreamControl createStreamControl()
        {
            return createStreamControlTask(false).Result;
        }

        private async Task<EncoderControl> createEncoderControlTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                EncoderControl lresult = null;
                
                do
                {
                    object lUnknown;

                    mCaptureManagerNative.ICaptureManagerControl.createControl(
                        typeof(CaptureManagerLibrary.IEncoderControl).GUID,
                        out lUnknown);

                    if (lUnknown == null)
                        break;

                    var lEncoderControl = lUnknown as CaptureManagerLibrary.IEncoderControl;

                    if (lEncoderControl == null)
                        break;

                    lresult = new EncoderControl(lEncoderControl);

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<IEncoderControlAsync> createEncoderControlAsync()
        {
            return await createEncoderControlTask(true);
        }

        public IEncoderControl createEncoderControl()
        {
            return createEncoderControlTask(false).Result;
        }

        private async Task<string> parseMediaTypeTask(object aMediaType, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                string lresult = "";
                
                do
                {
                    IntPtr lPtrXMLstring = IntPtr.Zero;

                    do
                    {
                        try
                        {


                            if (checkICaptureManagerControl())
                                break;

                            if (aMediaType == null)
                                break;

                            object lUnknown;

                            mCaptureManagerNative.ICaptureManagerControl.createMisc(
                                typeof(CaptureManagerLibrary.IMediaTypeParser).GUID,
                                out lUnknown);

                            if (lUnknown == null)
                                break;

                            (lUnknown as IMediaTypeParserInner).parse(Marshal.GetIUnknownForObject(aMediaType), out lPtrXMLstring);

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

                } while (false);
           
                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<string> parseMediaTypeAsync(object aMediaType)
        {
            return await parseMediaTypeTask(aMediaType, true);
        }

        public bool parseMediaType(
            object aMediaType,
            out string aInfoString)
        {
            bool lresult = false;

            aInfoString = parseMediaTypeTask(aMediaType, false).Result;

            lresult = !string.IsNullOrWhiteSpace(aInfoString);

            return lresult;
        }

        private async Task<int> getStrideForBitmapInfoHeaderTask(
                    Guid aMFVideoFormat,
                    uint aWidthInPixels, 
                    bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                int lresult = 0;
                                
                do
                {
                    try
                    {


                        if (checkICaptureManagerControl())
                            break;

                        object lUnknown;

                        mCaptureManagerNative.ICaptureManagerControl.createMisc(
                            typeof(CaptureManagerLibrary.IStrideForBitmap).GUID,
                            out lUnknown);

                        if (lUnknown == null)
                            break;

                        var lStrideForBitmap = lUnknown as CaptureManagerLibrary.IStrideForBitmap;

                        if (lStrideForBitmap == null)
                            break;

                        lStrideForBitmap.getStrideForBitmap(
                                        aMFVideoFormat,
                                        aWidthInPixels,
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

        public async Task<int> getStrideForBitmapInfoHeaderAsync(
                    Guid aMFVideoFormat,
                    uint aWidthInPixels)
        {
            return await getStrideForBitmapInfoHeaderTask(aMFVideoFormat, aWidthInPixels, true);
        }

        public bool getStrideForBitmapInfoHeader(
                    Guid aMFVideoFormat,
                    uint aWidthInPixels,
                    out int aPtrStride)
        {
            bool lresult = false;

            aPtrStride = getStrideForBitmapInfoHeaderTask(aMFVideoFormat, aWidthInPixels, false).Result;

            if (aPtrStride != 0)
                lresult = true;

            return lresult;
        }

        private async Task<VersionControl> getVersionControlTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                VersionControl lresult = null;
                
                do
                {
                    try
                    {

                        if (checkICaptureManagerControl())
                            break;

                        object lUnknown;

                        mCaptureManagerNative.ICaptureManagerControl.createMisc(
                            typeof(CaptureManagerLibrary.IVersionControl).GUID,
                            out lUnknown);

                        if (lUnknown == null)
                            break;

                        var lVersionControl = lUnknown as CaptureManagerLibrary.IVersionControl;

                        if (lVersionControl == null)
                            break;

                        lresult = new VersionControl(lVersionControl);

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);
           
                return lresult;
            }).ConfigureAwait(aIsAwait);
        }
        
        public async Task<IVersionControlAsync> getVersionControlAsync()
        {
            return await getVersionControlTask(true);
        }

        public IVersionControl getVersionControl()
        {
            return getVersionControlTask(false).Result;
        }

        private async Task<EVRStreamControl> createEVRStreamControlTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                EVRStreamControl lresult = null;
                
                do
                {
                    try
                    {

                        if (checkICaptureManagerControl())
                            break;

                        object lUnknown;

                        mCaptureManagerNative.ICaptureManagerControl.createMisc(
                            typeof(CaptureManagerLibrary.IEVRStreamControl).GUID,
                            out lUnknown);

                        if (lUnknown == null)
                            break;

                        var lEVRStreamControl = lUnknown as CaptureManagerLibrary.IEVRStreamControl;

                        if (lEVRStreamControl == null)
                            break;

                        lresult = new EVRStreamControl(lEVRStreamControl);

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);
            
                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<IEVRStreamControlAsync> createEVRStreamControlAsync()
        {
            return await createEVRStreamControlTask(true);
        }

        public IEVRStreamControl createEVRStreamControl()
        {
            return createEVRStreamControlTask(false).Result;
        }

        private async Task<RenderingControl> createRenderingControlTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                RenderingControl lresult = null;
                
                do
                {
                    try
                    {

                        if (checkICaptureManagerControl())
                            break;

                        object lUnknown;

                        mCaptureManagerNative.ICaptureManagerControl.createMisc(
                            typeof(CaptureManagerLibrary.IRenderingControl).GUID,
                            out lUnknown);

                        if (lUnknown == null)
                            break;

                        var lRenderingControl = lUnknown as CaptureManagerLibrary.IRenderingControl;

                        if (lRenderingControl == null)
                            break;

                        lresult = new RenderingControl(lRenderingControl);

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);
       
                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<IRenderingControlAsync> createRenderingControlAsync()
        {
            return await createRenderingControlTask(true);
        }

        public IRenderingControl createRenderingControl()
        {
            return createRenderingControlTask(false).Result;
        }

        private async Task<SwitcherControl> createSwitcherControlTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                SwitcherControl lresult = null;
                
                do
                {
                    try
                    {

                        if (checkICaptureManagerControl())
                            break;

                        object lUnknown;

                        mCaptureManagerNative.ICaptureManagerControl.createMisc(
                            typeof(CaptureManagerLibrary.ISwitcherControl).GUID,
                            out lUnknown);

                        if (lUnknown == null)
                            break;

                        var lSwitcherControl = lUnknown as CaptureManagerLibrary.ISwitcherControl;

                        if (lSwitcherControl == null)
                            break;

                        lresult = new SwitcherControl(lSwitcherControl);

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);
          
                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<ISwitcherControlAsync> createSwitcherControlAsync()
        {
            return await createSwitcherControlTask(true);
        }

        public ISwitcherControl createSwitcherControl()
        {
            return createSwitcherControlTask(false).Result;
        }

        private async Task<VideoMixerControl> createVideoMixerControlTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                VideoMixerControl lresult = null;
                
                do
                {
                    try
                    {

                        if (checkICaptureManagerControl())
                            break;

                        object lUnknown;

                        mCaptureManagerNative.ICaptureManagerControl.createMisc(
                            typeof(CaptureManagerLibrary.IVideoMixerControl).GUID,
                            out lUnknown);

                        if (lUnknown == null)
                            break;

                        var lVideoMixerControl = lUnknown as CaptureManagerLibrary.IVideoMixerControl;

                        if (lVideoMixerControl == null)
                            break;

                        lresult = new VideoMixerControl(lVideoMixerControl);

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);
          
                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<IVideoMixerControlAsync> createVideoMixerControlAsync()
        {
            return await createVideoMixerControlTask(true);
        }

        public IVideoMixerControl createVideoMixerControl()
        {
            return createVideoMixerControlTask(false).Result;
        }

        private async Task<AudioMixerControl> createAudioMixerControlTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                AudioMixerControl lresult = null;
                
                do
                {
                    try
                    {

                        if (checkICaptureManagerControl())
                            break;

                        object lUnknown;

                        mCaptureManagerNative.ICaptureManagerControl.createMisc(
                            typeof(CaptureManagerLibrary.IAudioMixerControl).GUID,
                            out lUnknown);

                        if (lUnknown == null)
                            break;

                        var lAudioMixerControl = lUnknown as CaptureManagerLibrary.IAudioMixerControl;

                        if (lAudioMixerControl == null)
                            break;

                        lresult = new AudioMixerControl(lAudioMixerControl);

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);
       
                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<IAudioMixerControlAsync> createAudioMixerControlAsync()
        {
            return await createAudioMixerControlTask(true);
        }

        public IAudioMixerControl createAudioMixerControl()
        {
            return createAudioMixerControlTask(false).Result;
        }

        private async Task<SARVolumeControl> createSARVolumeControlTask(bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                SARVolumeControl lresult = null;
                
                do
                {
                    try
                    {

                        if (checkICaptureManagerControl())
                            break;

                        object lUnknown;

                        mCaptureManagerNative.ICaptureManagerControl.createMisc(
                            typeof(CaptureManagerLibrary.ISARVolumeControl).GUID,
                            out lUnknown);

                        if (lUnknown == null)
                            break;

                        var lSARVolumeControl = lUnknown as CaptureManagerLibrary.ISARVolumeControl;

                        if (lSARVolumeControl == null)
                            break;

                        lresult = new SARVolumeControl(lSARVolumeControl);

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);
           
                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<ISARVolumeControlAsync> createSARVolumeControlAsync()
        {
            return await createSARVolumeControlTask(true);
        }

        public ISARVolumeControl createSARVolumeControl()
        {
            return createSARVolumeControlTask(false).Result;
        }
    }
}
