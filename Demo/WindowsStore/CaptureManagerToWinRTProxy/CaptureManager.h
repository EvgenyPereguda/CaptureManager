#pragma once

#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"
#include "Interfaces/ILogPrintOutControl.h"
#include "Interfaces/ISourceControl.h"
#include "Interfaces/ISinkControl.h"
#include "Interfaces/IFileSinkFactory.h"
#include "Interfaces/IEncoderControl.h"
#include "Interfaces/ISessionControl.h"
#include "Interfaces/IStreamControl.h"
#include "Interfaces/IMediaTypeParser.h"
#include "Interfaces/IStrideForBitmap.h"
#include "Interfaces/IVersionControl.h"
#include "Interfaces/IIntPtr.h"
#include "Interfaces/IEVRStreamControl.h"




namespace CaptureManagerToWinRTProxy
{
    public ref class CaptureManager sealed
    {
	public:

		static CaptureManager^ getInstance();

		bool isInitialized();

		CaptureManagerToWinRTProxy::ILogPrintOutControl^ getILogPrintOutControl();

		CaptureManagerToWinRTProxy::ISourceControl^ getISourceControl();

		CaptureManagerToWinRTProxy::ISinkControl^ getISinkControl();
		
		CaptureManagerToWinRTProxy::IEncoderControl^ getIEncoderControl();

		CaptureManagerToWinRTProxy::ISessionControl^ getISessionControl();

		CaptureManagerToWinRTProxy::IStreamControl^ getIStreamControl();

	


		CaptureManagerToWinRTProxy::IMediaTypeParser^ getIMediaTypeParser();

		CaptureManagerToWinRTProxy::IStrideForBitmap^ getIStrideForBitmap();

		CaptureManagerToWinRTProxy::IVersionControl^ getIVersionControl();

		CaptureManagerToWinRTProxy::IEVRStreamControl^ getIEVRStreamControl();


		

		
		CaptureManagerToWinRTProxy::IIntPtr^ getIntPtr(Platform::IntPtr aIntPtr);


	private:

		static CaptureManager^ mInstance;

		bool mIsInitialized;

		CComPtrCustom<ICaptureManagerControl> mICaptureManagerControl;

		CComPtrCustom<::ILogPrintOutControl> mILogPrintOutControl;
		

        CaptureManager();
    };
}