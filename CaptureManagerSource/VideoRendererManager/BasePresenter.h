#pragma once

#include <mutex>
#include <condition_variable>
#include <memory>
#include <thread>
#include <map>

#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "IPresenter.h"
#include "IPresenterInit.h"
#include "IRenderingControl.h"


namespace CaptureManager
{
	namespace Sinks
	{
		namespace EVR
		{
			class CBasePresenter : 
				public BaseUnknown<
				IPresenter,
				IMFGetService,
				IPresenterInit,
				IRenderingControl
				>
			{
			public:
				CBasePresenter();

				virtual void refresh();
				
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

			// IPresenter implements

				virtual HRESULT ProcessFrame() override;

			// IRenderingControl implements

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE enableInnerRendering(
					BOOL aMTProtect) override;

				virtual ~CBasePresenter();

			protected:

				virtual HRESULT processFrameInner()=0;

				void InitializeHook(HWND handler);

				void ShutdownHook();

			private:

				INT64 mLastTime;

				bool mIsShutdown;

				bool mIsStarted;

				bool isMultithreadProtected;

				HHOOK mHook;

				HWND mHandler;


				static LRESULT WINAPI CallWndProc(int uMsg, WPARAM wParam, LPARAM lParam);

				static std::map<HWND, CBasePresenter*> mPresenters;
			};
		}
	}
}