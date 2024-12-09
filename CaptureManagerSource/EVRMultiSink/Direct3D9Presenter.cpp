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

#include "Direct3D9Presenter.h"
#include "../DirectXManager/Direct3D9Manager.h"
#include "../DirectXManager/Direct3D9ExManager.h"
#include "../DirectXManager/Direct3D11Manager.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/Singleton.h"
#include "../DwmManager/DwmManager.h"
#include <d3d9types.h>



namespace EVRMultiSink
{
	namespace Sinks
	{
		namespace EVR
		{
			namespace Direct3D9
			{
				using namespace CaptureManager;
				using namespace CaptureManager::Core;
				using namespace CaptureManager::Core::Direct3D9;				
				using namespace CaptureManager::Core::Direct3D11;
				using namespace CaptureManager::Core::Dwm;
				

				Direct3D9Presenter::Direct3D9Presenter():
					mLastTime(0)
				{
					ZeroMemory(&mPrevWindowRect, sizeof(mPrevWindowRect));
				}

				Direct3D9Presenter::~Direct3D9Presenter()
				{
				}

				static HRESULT createDevice(ID3D11Device** aPtrPtrDevice)
				{
					HRESULT lresult(E_FAIL);

					do
					{

						// Driver types supported
						D3D_DRIVER_TYPE gDriverTypes[] =
						{
							D3D_DRIVER_TYPE_HARDWARE
						};
						UINT gNumDriverTypes = ARRAYSIZE(gDriverTypes);

						// Feature levels supported
						D3D_FEATURE_LEVEL gFeatureLevels[] =
						{
							D3D_FEATURE_LEVEL_11_1,
							D3D_FEATURE_LEVEL_11_0
							//D3D_FEATURE_LEVEL_10_1,
							//D3D_FEATURE_LEVEL_10_0,
							//D3D_FEATURE_LEVEL_9_1
						};

						UINT gNumFeatureLevels = ARRAYSIZE(gFeatureLevels);


						UINT mUseDebugLayer(D3D11_CREATE_DEVICE_VIDEO_SUPPORT);

						LOG_CHECK_PTR_MEMORY(aPtrPtrDevice);

						LOG_INVOKE_FUNCTION(Singleton<Direct3D11Manager>::getInstance().getState);

						D3D_FEATURE_LEVEL lfeatureLevel;

						LOG_INVOKE_DX11_FUNCTION(D3D11CreateDevice,
							NULL,
							D3D_DRIVER_TYPE_HARDWARE,
							NULL,
							mUseDebugLayer,
							gFeatureLevels,
							gNumFeatureLevels,
							D3D11_SDK_VERSION,
							aPtrPtrDevice,
							&lfeatureLevel,
							NULL);

						LOG_CHECK_PTR_MEMORY(aPtrPtrDevice);

					} while (false);

					return lresult;
				}



				// IPresenterInit methods

				HRESULT Direct3D9Presenter::initializeSharedTarget(
					HANDLE aHandle,
					IUnknown* aPtrUnkTarget)
				{

					HRESULT lresult(E_FAIL);

					do
					{

						LOG_INVOKE_FUNCTION(Singleton<Direct3D9ExManager>::getInstance().getState);

						LOG_CHECK_PTR_MEMORY(mDeviceManager);

						mD3D9.Release();

						CComPtrCustom<IDirect3D9Ex> lD3D9;

						LOG_INVOKE_DX9EX_FUNCTION(Direct3DCreate9Ex, D3D_SDK_VERSION, &lD3D9);

						LOG_CHECK_PTR_MEMORY(lD3D9);

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lD3D9, &mD3D9);

						LOG_CHECK_PTR_MEMORY(mD3D9);

						CComQIPtrCustom<IDirect3DTexture9> lDirect3DTexture9 = aPtrUnkTarget;

						CComPtrCustom<IDirect3DSurface9> lSurface;

						D3DSURFACE_DESC lSurfaceDesc;

						ZeroMemory(&lSurfaceDesc, sizeof(lSurfaceDesc));

						if (lDirect3DTexture9)
						{
							lresult = E_NOTIMPL;

							break;
						}
						else
						{
							CComQIPtrCustom<IDirect3DSurface9> lDirect3DSurface9 = aPtrUnkTarget;

							if (!lDirect3DSurface9)
							{
								if (!mDevice9)
								{

									CComPtrCustom<IDirect3DDevice9Ex> lDevice9;

									UINT lAdapter = D3DADAPTER_DEFAULT;

									D3DDISPLAYMODE dm;

									D3DPRESENT_PARAMETERS pp;


									LOG_INVOKE_DX9_METHOD(GetAdapterDisplayMode,
										mD3D9,
										lAdapter,
										&dm);

									ZeroMemory(&pp, sizeof(pp));

									pp.Windowed = TRUE;
									pp.hDeviceWindow = nullptr;
									pp.SwapEffect = D3DSWAPEFFECT_COPY;
									pp.BackBufferFormat = dm.Format;
									pp.BackBufferWidth = mImageWidth,
										pp.BackBufferHeight = mImageHeight;
									pp.Flags =
										D3DPRESENTFLAG_VIDEO |
										D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
									pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
									pp.BackBufferCount = 1;

									lresult = mD3D9->CreateDeviceEx(
										lAdapter,
										D3DDEVTYPE_HAL,
										nullptr,
										//D3DCREATE_HARDWARE_VERTEXPROCESSING |
										D3DCREATE_SOFTWARE_VERTEXPROCESSING |
										D3DCREATE_MULTITHREADED |
										D3DCREATE_FPU_PRESERVE,
										//D3DCREATE_HARDWARE_VERTEXPROCESSING |
										//D3DCREATE_NOWINDOWCHANGES |
										//D3DCREATE_MULTITHREADED |
										//D3DCREATE_FPU_PRESERVE,
										&pp,
										nullptr,
										&lDevice9);

									LOG_CHECK_PTR_MEMORY(lDevice9);

									CComPtrCustom<ID3D11Device> lID3D11Device;

									createDevice(&lID3D11Device);

									UINT l_Width = 1280;

									UINT l_Height = 720;

									D3DFORMAT l_Format = D3DFORMAT::D3DFMT_X8R8G8B8;

									if (lID3D11Device)
									{

										CComPtrCustom<ID3D11Resource> l_Resource;

										CComPtrCustom<ID3D11Texture2D> l_SharedTexture;

										lresult = lID3D11Device->OpenSharedResource(aHandle, IID_PPV_ARGS(&l_Resource));

										if (SUCCEEDED(lresult))
										{
											lresult = l_Resource->QueryInterface(IID_PPV_ARGS(&l_SharedTexture));
										}

										if (SUCCEEDED(lresult) && l_SharedTexture)
										{
											D3D11_TEXTURE2D_DESC l_Desc;

											l_SharedTexture->GetDesc(&l_Desc);

											l_Width = l_Desc.Width;

											l_Height = l_Desc.Height;

											if (l_Desc.Format == DXGI_FORMAT::DXGI_FORMAT_B8G8R8X8_UNORM)
												l_Format = D3DFORMAT::D3DFMT_X8R8G8B8;
										}
									}

									lresult = lDevice9->CreateRenderTarget(l_Width, l_Height, l_Format, D3DMULTISAMPLE_TYPE::D3DMULTISAMPLE_NONE, 0, FALSE, &lDirect3DSurface9, &aHandle);

								}
							}

							LOG_CHECK_PTR_MEMORY(lDirect3DSurface9);

							LOG_INVOKE_POINTER_METHOD(lDirect3DSurface9, GetDesc,
								&lSurfaceDesc);

							mDevice9.Release();

							LOG_INVOKE_POINTER_METHOD(lDirect3DSurface9, GetDevice,
								&mDevice9);

							lSurface = lDirect3DSurface9;
						}

						LOG_CHECK_STATE_DESCR(lSurfaceDesc.Width == 0, E_INVALIDARG);

						LOG_CHECK_PTR_MEMORY(mDevice9);

						LOG_INVOKE_DX9_METHOD(ResetDevice, mDeviceManager,
							mDevice9,
							mDeviceResetToken);

						mSample.Release();

						LOG_INVOKE_MF_FUNCTION(MFCreateSample,
							&mSample);

						LOG_CHECK_PTR_MEMORY(mSample);

						CComPtrCustom<IMFMediaBuffer> lBuffer;

						LOG_INVOKE_MF_FUNCTION(MFCreateDXSurfaceBuffer,
							__uuidof(IDirect3DSurface9),
							lSurface,
							FALSE,
							&lBuffer);

						LOG_CHECK_PTR_MEMORY(lBuffer);

						LOG_INVOKE_MF_METHOD(AddBuffer, mSample,
							lBuffer);


						CComPtrCustom<IMFMediaType> lCurrentMediaType;

						LOG_INVOKE_FUNCTION(createUncompressedVideoType,
							lSurfaceDesc.Format,
							lSurfaceDesc.Width,
							lSurfaceDesc.Height,
							MFVideoInterlaceMode::MFVideoInterlace_Progressive,
							mFrameRate,
							mPixelRate,
							&lCurrentMediaType);

						mCurrentMediaType = lCurrentMediaType;

						if (mMixer)
						{
							LOG_INVOKE_MF_METHOD(SetOutputType, mMixer,
								0,
								mCurrentMediaType,
								MFT_SET_TYPE_TEST_ONLY);

							LOG_INVOKE_MF_METHOD(SetOutputType, mMixer,
								0,
								mCurrentMediaType,
								0);
						}

					} while (false);

					return lresult;
				}
											

				HRESULT Direct3D9Presenter::createManagerAndDevice()
				{
					HRESULT lresult(E_FAIL);

					UINT lAdapter = D3DADAPTER_DEFAULT;

					D3DDISPLAYMODE dm;

					D3DPRESENT_PARAMETERS pp;

					do
					{
						LOG_CHECK_PTR_MEMORY(mDeviceManager);

						LOG_CHECK_PTR_MEMORY(mD3D9);
																	
						LOG_CHECK_STATE_DESCR(mHWNDVideo == nullptr, E_UNEXPECTED);
												

						LOG_INVOKE_DX9_METHOD(GetAdapterDisplayMode, 
							mD3D9,
							lAdapter, 
							&dm);
												
						ZeroMemory(&pp, sizeof(pp));

						pp.Windowed = TRUE;
						pp.hDeviceWindow = mHWNDVideo;
						pp.SwapEffect = D3DSWAPEFFECT_COPY;
						pp.BackBufferFormat = dm.Format;
						pp.BackBufferWidth = mImageWidth,
						pp.BackBufferHeight = mImageHeight;
						pp.Flags = 
							D3DPRESENTFLAG_VIDEO
							//| D3DPRESENTFLAG_DEVICECLIP
							| D3DPRESENTFLAG_LOCKABLE_BACKBUFFER
							;
						pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
						pp.BackBufferCount = 1;

						mDevice9.Release();

						LOG_INVOKE_DX9_METHOD(CreateDevice,
							mD3D9,
							lAdapter,
							D3DDEVTYPE_HAL,
							mHWNDVideo,
							//D3DCREATE_HARDWARE_VERTEXPROCESSING |
							D3DCREATE_SOFTWARE_VERTEXPROCESSING |
							D3DCREATE_NOWINDOWCHANGES |
							D3DCREATE_MULTITHREADED |
							D3DCREATE_FPU_PRESERVE,
							&pp,
							&mDevice9);
							
						LOG_CHECK_PTR_MEMORY(mDevice9);

						LOG_INVOKE_DX9_METHOD(ResetDevice, mDeviceManager,
							mDevice9,
							mDeviceResetToken);
					
						CComPtrCustom<IDirect3DSwapChain9> lSwapChain;

						LOG_INVOKE_DX9_METHOD(GetSwapChain, mDevice9,
							0,
							&lSwapChain);

						LOG_CHECK_PTR_MEMORY(lSwapChain);
						
						CComPtrCustom<IDirect3DSurface9> lSurface;

						LOG_INVOKE_DX9_METHOD(GetBackBuffer, lSwapChain,
							0,
							D3DBACKBUFFER_TYPE_MONO,
							&lSurface);

						mSample.Release();

						LOG_INVOKE_MF_FUNCTION(MFCreateSample,
							&mSample);

						LOG_CHECK_PTR_MEMORY(mSample);
						
						CComPtrCustom<IMFMediaBuffer> lBuffer;

						LOG_INVOKE_MF_FUNCTION(MFCreateDXSurfaceBuffer,
							__uuidof(IDirect3DSurface9),
							lSurface,
							FALSE,
							&lBuffer);

						LOG_CHECK_PTR_MEMORY(lBuffer);

						LOG_INVOKE_MF_METHOD(AddBuffer, mSample,
							lBuffer);
						

						D3DSURFACE_DESC lSurfaceDesc;

						ZeroMemory(&lSurfaceDesc, sizeof(lSurfaceDesc));
						
						LOG_INVOKE_POINTER_METHOD(lSurface, GetDesc,
							&lSurfaceDesc);

						CComPtrCustom<IMFMediaType> lCurrentMediaType;

						LOG_INVOKE_FUNCTION(createUncompressedVideoType,
							lSurfaceDesc.Format,
							lSurfaceDesc.Width,
							lSurfaceDesc.Height,
							MFVideoInterlaceMode::MFVideoInterlace_Progressive,
							mFrameRate,
							mPixelRate,
							&lCurrentMediaType);

						mCurrentMediaType = lCurrentMediaType;

						if (mMixer)
						{
							LOG_INVOKE_MF_METHOD(SetOutputType, mMixer,
								0,
								mCurrentMediaType,
								MFT_SET_TYPE_TEST_ONLY);

							LOG_INVOKE_MF_METHOD(SetOutputType, mMixer,
								0,
								mCurrentMediaType,
								0);
						}
																		
					} while (false);

					return lresult;
				}



				// IMFGetService methods
				STDMETHODIMP Direct3D9Presenter::GetService(
					REFGUID aRefGUIDService,
					REFIID aRefIID,
					LPVOID* aPtrPtrObject)
				{
					HRESULT lresult(MF_E_UNSUPPORTED_SERVICE);

					do
					{
						LOG_CHECK_PTR_MEMORY(aPtrPtrObject);

						if (aRefGUIDService == MR_VIDEO_ACCELERATION_SERVICE)
						{
							if (aRefIID == __uuidof(IDirect3DDeviceManager9))
							{
								if (mDeviceManager)
								{
									LOG_INVOKE_WIDE_QUERY_INTERFACE_METHOD(mDeviceManager,
										__uuidof(IUnknown),
										aPtrPtrObject);
								}
								else
								{
									lresult = E_NOINTERFACE;
								}
							}
							else if (aRefIID == __uuidof(IMFVideoSampleAllocator))
							{
								do
								{

									CComPtrCustom<IMFVideoSampleAllocator> lVideoSampleAllocator;

									LOG_INVOKE_MF_FUNCTION(MFCreateVideoSampleAllocator,
										IID_PPV_ARGS(&lVideoSampleAllocator));

									if (lVideoSampleAllocator)
									{
										if (mDeviceManager)
											lVideoSampleAllocator->SetDirectXManager(mDeviceManager);

										LOG_INVOKE_WIDE_QUERY_INTERFACE_METHOD(lVideoSampleAllocator, aRefIID, aPtrPtrObject);
									}

								} while (false);
							}
						}
						
					} while (false);

					return lresult;
				}



				// IPresenter methods
				

				HRESULT Direct3D9Presenter::initialize(
					UINT32 aImageWidth,
					UINT32 aImageHeight,
					DWORD aNumerator,
					DWORD aDenominator,
					IMFTransform* aPtrMixer)
				{
					HRESULT lresult(E_FAIL);

					do
					{

						LOG_CHECK_PTR_MEMORY(aPtrMixer);

						LOG_INVOKE_FUNCTION(Singleton<Direct3D9ExManager>::getInstance().getState);

						LOG_INVOKE_FUNCTION(Singleton<Direct3D9Manager>::getInstance().getState);


						CComPtrCustom<IDirect3D9Ex> lD3D9;

						LOG_INVOKE_DX9EX_FUNCTION(Direct3DCreate9Ex, D3D_SDK_VERSION, &lD3D9);

						LOG_CHECK_PTR_MEMORY(lD3D9);

						if (mD3D9 == nullptr)
							LOG_INVOKE_QUERY_INTERFACE_METHOD(lD3D9, &mD3D9);

						LOG_CHECK_PTR_MEMORY(mD3D9);

						if (mDeviceManager == nullptr)
							LOG_INVOKE_DX9_FUNCTION(DXVA2CreateDirect3DDeviceManager9,
								&mDeviceResetToken,
								&mDeviceManager);

						LOG_CHECK_PTR_MEMORY(mDeviceManager);

						MFRatio lFrameRate;

						lFrameRate.Numerator = aNumerator;

						lFrameRate.Denominator = aDenominator;

						LOG_INVOKE_FUNCTION(init,
							aImageWidth,
							aImageHeight,
							lFrameRate);

						mMixer = aPtrMixer;
						
						if (mCurrentMediaType)
						{
							LOG_INVOKE_MF_METHOD(SetOutputType, mMixer,
								0,
								mCurrentMediaType,
								MFT_SET_TYPE_TEST_ONLY);

							LOG_INVOKE_MF_METHOD(SetOutputType, mMixer,
								0,
								mCurrentMediaType,
								0);
						}

					} while (false);

					return lresult;
				}
				
				HRESULT Direct3D9Presenter::ProcessFrame(BOOL aImmediate)
				{
					HRESULT lresult(E_FAIL);

					do
					{
						if (aImmediate == FALSE)
						{
							auto lCurrentTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

							if ((lCurrentTime - mLastTime) < mVideoFrameDuration)
							{
								lresult = S_OK;

								break;
							}

							mLastTime = lCurrentTime;
						}

						std::lock_guard<std::mutex> lLock(mAccessMutex);

						if (mHWNDVideo != nullptr)
						{

							RECT lWindowRect;

							GetClientRect(mHWNDVideo, &lWindowRect);

							auto lNativeClientWidth = lWindowRect.right - lWindowRect.left;

							auto lNativeClientHeight = lWindowRect.bottom - lWindowRect.top;

							auto lnativeProportion = (float)lNativeClientHeight / (float)lNativeClientWidth;

							auto limageProportion = (float)mImageHeight / (float)mImageWidth;

							if (lnativeProportion >= limageProportion)
							{
								LONG lidealHeight = lNativeClientWidth * mImageHeight / mImageWidth;
								
								LONG lborder = (lNativeClientHeight - lidealHeight) >> 1;

								mDestRect.left = 0;

								mDestRect.right = lNativeClientWidth;

								mDestRect.top = lborder;

								mDestRect.bottom = lborder + lidealHeight;
							}
							else
							{
								LONG lidealWidth = lNativeClientHeight * mImageWidth / mImageHeight;
								
								LONG lborder = (lNativeClientWidth - lidealWidth) >> 1;

								mDestRect.left = lborder;

								mDestRect.right = lborder + lidealWidth;

								mDestRect.top = 0;

								mDestRect.bottom = lNativeClientHeight;
							}

							if (mPrevWindowRect != lWindowRect)
							{
								InvalidateRect(mHWNDVideo, NULL, TRUE);

								mPrevWindowRect = lWindowRect;
							}


							MFT_OUTPUT_DATA_BUFFER lBuffer;

							ZeroMemory(&lBuffer, sizeof(lBuffer));

							lBuffer.dwStreamID = 0;

							lBuffer.pSample = mSample;

							DWORD lState(0);

							LOG_INVOKE_MF_METHOD(ProcessOutput, mMixer,
								0,
								1,
								&lBuffer,
								&lState);

							lresult = mDevice9->Present(nullptr, &mDestRect, mHWNDVideo, nullptr);

						}
						else
						{							
							MFT_OUTPUT_DATA_BUFFER lBuffer;

							ZeroMemory(&lBuffer, sizeof(lBuffer));

							lBuffer.dwStreamID = 0;

							lBuffer.pSample = mSample;

							DWORD lState(0);

							LOG_INVOKE_MF_METHOD(ProcessOutput, mMixer,
								0,
								1,
								&lBuffer,
								&lState);							
						}

					} while (false);

					return lresult;
				}

				HRESULT Direct3D9Presenter::setVideoWindowHandle(
					HWND aVideoWindowHandle)
				{
					HRESULT lresult(E_FAIL);

					do
					{
						std::lock_guard<std::mutex> lLock(mAccessMutex);
						
						LOG_CHECK_STATE(!IsWindow(aVideoWindowHandle));

						if (mHWNDVideo != aVideoWindowHandle)
						{
							mHWNDVideo = aVideoWindowHandle;

							LOG_INVOKE_FUNCTION(createManagerAndDevice);
						}

					} while (false);

					return lresult;
				}
			}
		}
	}
}