#pragma once

#include <mutex>
#include <condition_variable>
#include <memory>
#include <thread>

#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "BasePresenter.h"

namespace EVRMultiSink
{
	namespace Sinks
	{
		namespace EVR
		{
			namespace Direct3D9
			{
				using namespace CaptureManager;

				class Direct3D9Presenter :
					public CBasePresenter
				{

				public:
					Direct3D9Presenter();

					HRESULT setVideoWindowHandle(
						HWND aVideoWindowHandle);

					// IPresenterInit methods
				 
					virtual HRESULT initializeSharedTarget(
					HANDLE aHandle,
					IUnknown* aPtrTarget)override;





					// IMFGetService methods
					STDMETHODIMP GetService(
						REFGUID aRefGUIDService,
						REFIID aRefIID,
						LPVOID* aPtrPtrObject);


					// IPresenter methods
					virtual HRESULT initialize(
						UINT32 aImageWidth,
						UINT32 aImageHeight,
						DWORD aNumerator,
						DWORD aDenominator,
						IMFTransform* aPtrMixer)override;
					virtual HRESULT ProcessFrame(BOOL aImmediate)override;

				private:



					INT64 mLastTime;

					RECT mDestRect;

					RECT mPrevWindowRect;

					CComPtrCustom<IMFMediaType> mCurrentMediaType;
					CComPtrCustom<IDirect3D9Ex> mD3D9;
					CComPtrCustom<IDirect3DDevice9> mDevice9;
					CComPtrCustom<IDirect3DDeviceManager9> mDeviceManager;
					CComPtrCustom<IMFTransform> mMixer;
					CComPtrCustom<IMFSample> mSample;

					
					virtual ~Direct3D9Presenter();
					HRESULT createManagerAndDevice();
				};
			}
		}
	}
}