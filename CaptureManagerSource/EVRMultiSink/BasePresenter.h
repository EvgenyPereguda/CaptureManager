#pragma once

#include <mutex>
#include <condition_variable>
#include <memory>
#include <thread>

#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "IPresenter.h"
#include "IPresenterInit.h"


namespace EVRMultiSink
{
	namespace Sinks
	{
		namespace EVR
		{
			class CBasePresenter : 
				public CaptureManager::BaseUnknown<
				IPresenter,
				IMFGetService,
				IPresenterInit
				>
			{
			public:
				CBasePresenter();
				
			protected:

				std::mutex mAccessMutex;

				MFRatio mFrameRate;

				MFRatio mPixelRate;

				LONG mImageWidth;
				LONG mImageHeight;
				UINT mDeviceResetToken;

				HWND mHWNDVideo;

				INT64 mVideoFrameDuration;



				HRESULT init(
					UINT32 aImageWidth,
					UINT32 aImageHeight,
					MFRatio aFrameRate);
				
				HRESULT createUncompressedVideoType(
					DWORD                fccFormat,  // FOURCC or D3DFORMAT value.     
					UINT32               width,
					UINT32               height,
					MFVideoInterlaceMode interlaceMode,
					const MFRatio&       frameRate,
					const MFRatio&       par,
					IMFMediaType         **ppType
					);

				virtual ~CBasePresenter();

			private:

				bool mIsShutdown;

				bool mIsStarted;
			};
		}
	}
}