#pragma once

#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"
#include "ILogPrintOutControl.h"
#include "ISourceControl.h"
#include "ISinkControl.h"
#include "IFileSinkFactory.h"
#include "IEncoderControl.h"
#include "ISessionControl.h"
#include "IStreamControl.h"
#include "IMediaTypeParser.h"
#include "IStrideForBitmap.h"
#include "IVersionControl.h"


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

		

	private:

		static CaptureManager^ mInstance;

		bool mIsInitialized;

		CComPtrCustom<ICaptureManagerControl> mICaptureManagerControl;

		CComPtrCustom<::ILogPrintOutControl> mILogPrintOutControl;
		

        CaptureManager();
    };
}