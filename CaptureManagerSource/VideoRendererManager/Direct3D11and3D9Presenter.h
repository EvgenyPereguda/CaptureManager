#pragma once

#include <mutex>
#include <condition_variable>
#include <memory>
#include <thread>
#include <dxgi1_2.h>


#include <D2d1.h>
#include <d2d1_2.h>
#include <dwrite.h>
#include <D2d1_1helper.h>

#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "BasePresenter.h"

namespace CaptureManager
{
	namespace Sinks
	{
		namespace EVR
		{
			namespace Direct3D11
			{
				class Direct3D11and3D9Presenter :
					public CBasePresenter
				{
													

				public:
					Direct3D11and3D9Presenter();

					HRESULT setVideoWindowHandle(
						HWND aVideoWindowHandle);

					HRESULT createDevice()override;

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

					virtual HRESULT processFrameInner()override;



					// IRenderingControl implements

					virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE renderToTarget(
						IUnknown* aPtrRenderTarget,
						BOOL aCopyMode) override;
					
				private:



					enum TargetType
					{
						Handler,
						SwapChain,
						RenderTexture
					};


					
					RECT mDestRect;

					RECT mPrevWindowRect;
					
					static UINT mUseDebugLayer;

					bool mSharedTexture;

					CComPtrCustom<IMFMediaType> mCurrentMediaType;

					CComPtrCustom<ID3D11Device> mD3D11Device;

					CComPtrCustom<IDirect3DDevice9> mDevice9;

					CComPtrCustom<IMFDXGIDeviceManager> mDeviceManager;

					CComPtrCustom<ID3D11DeviceContext> mImmediateContext;

					CComPtrCustom<IDXGIFactory2> mDXGIFactory2;

					CComPtrCustom<IDXGIOutput1> mDXGIOutput1;

					CComPtrCustom<IDXGISwapChain1> mSwapChain1;

					CComQIPtrCustom<ID3D11Texture2D> mID3D11Texture2D;

					CComPtrCustom<IDirect3DSurface9> mTargetDirect3DSurface9;

					CComPtrCustom<IDirect3DSurface9> mRenderDirect3DSurface9;

					CComQIPtrCustom<ID3D11Texture2D> mSharedID3D11Texture2D;
										
					CComPtrCustom<IMFTransform> mMixer;

					CComPtrCustom<IMFSample> mSample;



									   

					virtual ~Direct3D11and3D9Presenter();

				
					HRESULT createManagerAndDevice();

					void releaseResources();

					static HRESULT createDevice(ID3D11Device** aPtrPtrDevice);

					HRESULT createSample(TargetType aTargetType);

					HRESULT init_vb();

					void set_device_states();

					HRESULT resize();
				};
			}
		}
	}
}