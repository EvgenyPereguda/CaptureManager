#pragma once

#include <mutex>
#include <condition_variable>
#include <memory>
#include <thread>
#include <dxgi1_2.h>

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
			namespace Direct3D11
			{
				using namespace CaptureManager;

				class Direct3D11Presenter :
					public CBasePresenter
				{

				public:
					Direct3D11Presenter();

					HRESULT setVideoWindowHandle(
						HWND aVideoWindowHandle);


					static HRESULT getMaxInputStreamCount(DWORD* aPtrMaxInputStreamCount);
					
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

					enum TargetType
					{
						Handler,
						SwapChain,
						RenderTexture
					};



					INT64 mLastTime;

					RECT mDestRect;
					
					static UINT mUseDebugLayer;

					CComPtrCustom<IMFMediaType> mCurrentMediaType;

					CComPtrCustom<IDirect3DDevice9> mDevice9;

					CComPtrCustom<IDirect3DSurface9> mTargetDirect3DSurface9;

					CComPtrCustom<IDirect3DSurface9> mRenderDirect3DSurface9;

					CComPtrCustom<ID3D11Device> mD3D11Device;

					CComPtrCustom<IMFDXGIDeviceManager> mMFDXGIDeviceManager;

					CComPtrCustom<ID3D11DeviceContext> mImmediateContext;

					CComPtrCustom<IDXGIFactory2> mDXGIFactory2;

					CComPtrCustom<IDXGIOutput1> mDXGIOutput1;

					CComPtrCustom<IDXGISwapChain1> mSwapChain1;

					CComQIPtrCustom<ID3D11Texture2D> mID3D11Texture2D;
					
					CComPtrCustom<IMFTransform> mMixer;

					CComPtrCustom<IMFSample> mSample;






					virtual ~Direct3D11Presenter();

				
					HRESULT createManagerAndDevice();

					HRESULT createDirectX9ManagerAndDevice(IDirect3DDeviceManager9** a_DeviceManager);

					void releaseResources();

					static HRESULT createDevice(ID3D11Device** aPtrPtrDevice);

					HRESULT createSample(TargetType aTargetType = TargetType::Handler);
				};
			}
		}
	}
}