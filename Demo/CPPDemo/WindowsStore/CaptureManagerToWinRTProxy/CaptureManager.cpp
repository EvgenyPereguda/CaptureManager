// CaptureManager.cpp
#include "pch.h"
#include "CaptureManager.h"
#include "LogPrintOutControl.h"
#include "SourceControl.h"
#include "SinkControl.h"
#include "FileSinkFactory.h"
#include "EncoderControl.h"
#include "SessionControl.h"
#include "StreamControl.h"
#include "MediaTypeParser.h"
#include "StrideForBitmap.h"
#include "VersionControl.h"


#pragma comment(lib, "OleAut32.lib")

using namespace CaptureManagerToWinRTProxy;
using namespace Platform;

using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Core;

CaptureManager^ CaptureManager::mInstance = nullptr;

CaptureManager::CaptureManager()
{

	bool lresult = false;

	do
	{
		do
		{
			
			const GUID lCLSID_CoLogPrintOut = { 0x4563EE3E, 0xDA1E, 0x4911, { 0x9F, 0x40, 0x88, 0xA2, 0x84, 0xE2, 0xDD, 0x69 } };

			const GUID lCLSID_CoCaptureManager = { 0xD5F07FB8, 0xCE60, 0x4017, { 0xB2, 0x15, 0x95, 0xC8, 0xA0, 0xDD, 0xF4, 0x2A } };


			HRESULT hr;

			MULTI_QI arrMultiQI[1] = { &__uuidof(::ILogPrintOutControl), nullptr, S_OK };

			hr = CoCreateInstanceFromApp(lCLSID_CoLogPrintOut, nullptr, CLSCTX_ALL, nullptr, 1, arrMultiQI);

			if (FAILED(hr) || arrMultiQI[0].pItf == nullptr)
			{
				break;
			}
			
			arrMultiQI[0].pItf->QueryInterface(IID_PPV_ARGS(&mILogPrintOutControl));

			arrMultiQI[0].pItf->Release();

			arrMultiQI[0].pItf = nullptr;

			if (!mILogPrintOutControl)
				break;
			
			arrMultiQI[0] = { &__uuidof(ICaptureManagerControl), nullptr, S_OK };

			hr = CoCreateInstanceFromApp(lCLSID_CoCaptureManager, nullptr, CLSCTX_ALL, nullptr, 1, arrMultiQI);

			if (FAILED(hr) || arrMultiQI[0].pItf == nullptr)
			{
				break;
			}

			arrMultiQI[0].pItf->QueryInterface(IID_PPV_ARGS(&mICaptureManagerControl));

			arrMultiQI[0].pItf->Release();

			arrMultiQI[0].pItf = nullptr;
			
			if (!mICaptureManagerControl)
				break;

			
			lresult = true;

		} while (false);

	} while (false);

	mIsInitialized = lresult;
}

CaptureManager^ CaptureManager::getInstance()
{
	if (mInstance == nullptr)
		mInstance = ref new CaptureManager();

	return mInstance;
}

bool CaptureManager::isInitialized()
{
	return mIsInitialized;
}

CaptureManagerToWinRTProxy::ILogPrintOutControl^ CaptureManager::getILogPrintOutControl()
{
	CaptureManagerToWinRTProxy::ILogPrintOutControl^ lresult = nullptr;

	do
	{
		if (!mIsInitialized)
			break;

		if (!mILogPrintOutControl)
			break;
				
		lresult = ref new LogPrintOutControl(IntPtr(mILogPrintOutControl.get()));
		
	} while (false);

	return lresult;
}

CaptureManagerToWinRTProxy::ISourceControl^ CaptureManager::getISourceControl()
{
	CaptureManagerToWinRTProxy::ISourceControl^ lresult = nullptr;

	do
	{
		if (!mIsInitialized)
			break;

		if (!mICaptureManagerControl)
			break;

		CComPtrCustom<::ISourceControl> lISourceControl;

		CHK(mICaptureManagerControl->createControl(IID_PPV_IUNKNOWN_ARGS(&lISourceControl)));
		
		lresult = ref new SourceControl(IntPtr(lISourceControl.get()));
		
	} while (false);

	return lresult;
}

CaptureManagerToWinRTProxy::ISinkControl^ CaptureManager::getISinkControl()
{
	CaptureManagerToWinRTProxy::ISinkControl^ lresult = nullptr;

	do
	{
		if (!mIsInitialized)
			break;

		if (!mICaptureManagerControl)
			break;

		CComPtrCustom<::ISinkControl> lISinkControl;

		CHK(mICaptureManagerControl->createControl(IID_PPV_IUNKNOWN_ARGS(&lISinkControl)));
		
		lresult = ref new SinkControl(IntPtr(lISinkControl.get()));
		
	} while (false);

	return lresult;
}

CaptureManagerToWinRTProxy::IEncoderControl^ CaptureManager::getIEncoderControl()
{
	CaptureManagerToWinRTProxy::IEncoderControl^ lresult = nullptr;

	do
	{
		if (!mIsInitialized)
			break;

		if (!mICaptureManagerControl)
			break;

		CComPtrCustom<::IEncoderControl> lIEncoderControl;

		CHK(mICaptureManagerControl->createControl(IID_PPV_IUNKNOWN_ARGS(&lIEncoderControl)));
		
		lresult = ref new EncoderControl(IntPtr(lIEncoderControl.get()));

	} while (false);

	return lresult;
}

CaptureManagerToWinRTProxy::ISessionControl^ CaptureManager::getISessionControl()
{
	CaptureManagerToWinRTProxy::ISessionControl^ lresult = nullptr;

	do
	{
		if (!mIsInitialized)
			break;

		if (!mICaptureManagerControl)
			break;

		CComPtrCustom<::ISessionControl> lISessionControl;

		CHK(mICaptureManagerControl->createControl(IID_PPV_IUNKNOWN_ARGS(&lISessionControl)));
		
		lresult = ref new SessionControl(IntPtr(lISessionControl.get()));

	} while (false);

	return lresult;
}

CaptureManagerToWinRTProxy::IStreamControl^ CaptureManager::getIStreamControl()
{
	CaptureManagerToWinRTProxy::IStreamControl^ lresult = nullptr;

	do
	{
		if (!mIsInitialized)
			break;

		if (!mICaptureManagerControl)
			break;

		CComPtrCustom<::IStreamControl> lIStreamControl;

		CHK(mICaptureManagerControl->createControl(IID_PPV_IUNKNOWN_ARGS(&lIStreamControl)));
		
		lresult = ref new StreamControl(IntPtr(lIStreamControl.get()));

	} while (false);

	return lresult;
}


CaptureManagerToWinRTProxy::IMediaTypeParser^ CaptureManager::getIMediaTypeParser()
{
	CaptureManagerToWinRTProxy::IMediaTypeParser^ lresult = nullptr;

	do
	{
		if (!mIsInitialized)
			break;

		if (!mICaptureManagerControl)
			break;

		CComPtrCustom<::IMediaTypeParser> lIMediaTypeParser;

		CHK(mICaptureManagerControl->createMisc(IID_PPV_IUNKNOWN_ARGS(&lIMediaTypeParser)));
		
		lresult = ref new MediaTypeParser(IntPtr(lIMediaTypeParser.get()));

	} while (false);

	return lresult;
}

CaptureManagerToWinRTProxy::IStrideForBitmap^ CaptureManager::getIStrideForBitmap()
{
	CaptureManagerToWinRTProxy::IStrideForBitmap^ lresult = nullptr;

	do
	{
		if (!mIsInitialized)
			break;

		if (!mICaptureManagerControl)
			break;

		CComPtrCustom<::IStrideForBitmap> lIStrideForBitmap;

		CHK(mICaptureManagerControl->createMisc(IID_PPV_IUNKNOWN_ARGS(&lIStrideForBitmap)));
		
		lresult = ref new StrideForBitmap(IntPtr(lIStrideForBitmap.get()));

	} while (false);

	return lresult;
}

CaptureManagerToWinRTProxy::IVersionControl^ CaptureManager::getIVersionControl()
{
	CaptureManagerToWinRTProxy::IVersionControl^ lresult = nullptr;

	do
	{
		if (!mIsInitialized)
			break;

		if (!mICaptureManagerControl)
			break;

		CComPtrCustom<::IVersionControl> lIVersionControl;

		CHK(mICaptureManagerControl->createMisc(IID_PPV_IUNKNOWN_ARGS(&lIVersionControl)));
		
		lresult = ref new VersionControl(IntPtr(lIVersionControl.get()));

	} while (false);

	return lresult;
}