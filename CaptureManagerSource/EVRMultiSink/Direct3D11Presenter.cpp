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

#include "Direct3D11Presenter.h"
#include "../DirectXManager/DXGIManager.h"
#include "../DirectXManager/Direct3D9Manager.h"
#include "../DirectXManager/Direct3D9ExManager.h"
#include "../DirectXManager/Direct3D11Manager.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/Singleton.h"
#include "../Common/GUIDs.h"
#include <Windows.ui.xaml.media.dxinterop.h>

extern void OutputLog(const char *szFormat, ...);

namespace EVRMultiSink
{
	namespace Sinks
	{
		namespace EVR
		{
			namespace Direct3D11
			{
				using namespace CaptureManager;
				using namespace CaptureManager::Core;
				using namespace Core::DXGI;
				using namespace Core::Direct3D9;
				using namespace Core::Direct3D11;
				

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


				UINT Direct3D11Presenter::mUseDebugLayer(
					D3D11_CREATE_DEVICE_VIDEO_SUPPORT | 
					D3D11_CREATE_DEVICE_BGRA_SUPPORT);

				Direct3D11Presenter::Direct3D11Presenter():
					mLastTime(0)
				{
				}
				
				Direct3D11Presenter::~Direct3D11Presenter()
				{
					releaseResources();
				}


				// IPresenterInit methods

				HRESULT Direct3D11Presenter::initializeSharedTarget(
					HANDLE aHandle,
					IUnknown* aPtrTarget)
				{
					HRESULT lresult(E_NOTIMPL);

					do
					{

						LOG_CHECK_PTR_MEMORY(mMFDXGIDeviceManager);

						LOG_CHECK_PTR_MEMORY(mD3D11Device);

						CComQIPtrCustom<IDirect3DSurface9> lDirect3DSurface9 = aPtrTarget;

						if (lDirect3DSurface9) {

							D3DSURFACE_DESC lDesc;

							LOG_INVOKE_POINTER_METHOD(lDirect3DSurface9, GetDesc, &lDesc);

							LOG_CHECK_STATE(lDesc.Usage != D3DUSAGE_RENDERTARGET);

							mDevice9.Release();

							LOG_INVOKE_POINTER_METHOD(lDirect3DSurface9, GetDevice, &mDevice9);

							LOG_CHECK_PTR_MEMORY(mDevice9);


							HANDLE lSharedHandle = NULL;

							mRenderDirect3DSurface9.Release();

							LOG_INVOKE_POINTER_METHOD(mDevice9, CreateRenderTarget,
								lDesc.Width,
								lDesc.Height,
								lDesc.Format,
								D3DMULTISAMPLE_NONE,
								0,
								0,
								&mRenderDirect3DSurface9,
								&lSharedHandle);

							LOG_CHECK_PTR_MEMORY(mRenderDirect3DSurface9);

							mTargetDirect3DSurface9 = lDirect3DSurface9;

							mID3D11Texture2D.Release();

							CComPtrCustom<ID3D11Resource> lID3D11Resource;

							mD3D11Device->OpenSharedResource(lSharedHandle, IID_PPV_ARGS(&lID3D11Resource));

							LOG_CHECK_PTR_MEMORY(lID3D11Resource);

							LOG_INVOKE_QUERY_INTERFACE_METHOD(lID3D11Resource, &mID3D11Texture2D);

							LOG_CHECK_PTR_MEMORY(mID3D11Texture2D);

							LOG_INVOKE_POINTER_METHOD(mDevice9, StretchRect,
								mRenderDirect3DSurface9,
								NULL,
								mTargetDirect3DSurface9,
								NULL,
								D3DTEXTUREFILTERTYPE::D3DTEXF_NONE);

							LOG_INVOKE_FUNCTION(createSample, Direct3D11Presenter::RenderTexture);

							mImmediateContext.Release();

							mD3D11Device->GetImmediateContext(&mImmediateContext);

							break;
						}

						CComQIPtrCustom<ISwapChainPanelNative> lSwapChainPanelNative = aPtrTarget;

						if (lSwapChainPanelNative)
						{

							CComQIPtrCustom<IDXGIDevice> ldxgiDevice = mD3D11Device;

							LOG_CHECK_PTR_MEMORY(ldxgiDevice);

							CComQIPtrCustom<IDXGIAdapter> ldxgiAdapter;
							
							LOG_INVOKE_POINTER_METHOD(ldxgiDevice, GetAdapter,
								&ldxgiAdapter);

							CComQIPtrCustom<IDXGIFactory2> ldxgiFactory;

							LOG_INVOKE_POINTER_METHOD(ldxgiAdapter, GetParent,
								IID_PPV_ARGS(&ldxgiFactory));

							// Get the DXGISwapChain1
							DXGI_SWAP_CHAIN_DESC1 scd;
							ZeroMemory(&scd, sizeof(scd));
							scd.SampleDesc.Count = 1;
							scd.SampleDesc.Quality = 0;
							scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
							scd.Scaling = DXGI_SCALING_STRETCH;
							scd.Width = mImageWidth;
							scd.Height = mImageHeight;
							scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
							scd.Stereo = FALSE;
							scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
							scd.Flags = 0;// m_bStereoEnabled ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0; //opt in to do direct flip;
							scd.BufferCount = 4;
							//scd.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_STRAIGHT;

							mSwapChain1.Release();

							// create swap chain by calling CreateSwapChainForComposition
							ldxgiFactory->CreateSwapChainForComposition(
								mD3D11Device,
								&scd,
								nullptr,		// allow on any display 
								&mSwapChain1
								);

							LOG_CHECK_PTR_MEMORY(mSwapChain1);

							LOG_INVOKE_POINTER_METHOD(lSwapChainPanelNative, SetSwapChain,
								mSwapChain1);
														
							LOG_INVOKE_FUNCTION(createSample);

							break;
						}

						CComQIPtrCustom<IDXGISwapChain1> lIDXGISwapChain1 = aPtrTarget;

						if (lIDXGISwapChain1)
						{
							mSwapChain1.Release();

							mID3D11Texture2D.Release();

							mSwapChain1 = lIDXGISwapChain1;

							LOG_CHECK_PTR_MEMORY(mSwapChain1);

							mD3D11Device.Release();

							LOG_INVOKE_POINTER_METHOD(mSwapChain1, GetDevice, IID_PPV_ARGS(&mD3D11Device));

							CComPtrCustom<ID3D11VideoDevice> lDX11VideoDevice;

							LOG_INVOKE_QUERY_INTERFACE_METHOD(mD3D11Device, &lDX11VideoDevice);

							LOG_INVOKE_FUNCTION(createSample, Direct3D11Presenter::SwapChain);

							LOG_CHECK_PTR_MEMORY(lDX11VideoDevice);

							LOG_CHECK_PTR_MEMORY(mMFDXGIDeviceManager);

							LOG_INVOKE_POINTER_METHOD(mMFDXGIDeviceManager, ResetDevice,
								mD3D11Device,
								mDeviceResetToken);

							mImmediateContext.Release();

							mD3D11Device->GetImmediateContext(&mImmediateContext);

							LOG_CHECK_PTR_MEMORY(mImmediateContext);

							CComPtrCustom<ID3D10Multithread> lMultiThread;

							// Need to explitly set the multithreaded mode for this device
							LOG_INVOKE_QUERY_INTERFACE_METHOD(mImmediateContext, &lMultiThread);

							LOG_CHECK_PTR_MEMORY(lMultiThread);

							BOOL lbRrsult = lMultiThread->SetMultithreadProtected(TRUE);

							break;
						}



						CComQIPtrCustom<ID3D11Texture2D> lID3D11Texture2D = aPtrTarget;
						
						if (lID3D11Texture2D)
						{
							mSwapChain1.Release();

							mID3D11Texture2D.Release();

							mID3D11Texture2D = lID3D11Texture2D;

							LOG_CHECK_PTR_MEMORY(mID3D11Texture2D);

							mD3D11Device.Release();
							
							mID3D11Texture2D->GetDevice(&mD3D11Device);

							LOG_CHECK_PTR_MEMORY(mD3D11Device);

							CComPtrCustom<ID3D11VideoDevice> lDX11VideoDevice;

							LOG_INVOKE_QUERY_INTERFACE_METHOD(mD3D11Device, &lDX11VideoDevice);

							LOG_INVOKE_FUNCTION(createSample, Direct3D11Presenter::RenderTexture);

							LOG_CHECK_PTR_MEMORY(lDX11VideoDevice);

							LOG_CHECK_PTR_MEMORY(mMFDXGIDeviceManager);

							LOG_INVOKE_POINTER_METHOD(mMFDXGIDeviceManager, ResetDevice,
								mD3D11Device,
								mDeviceResetToken);

							mImmediateContext.Release();

							mD3D11Device->GetImmediateContext(&mImmediateContext);

							LOG_CHECK_PTR_MEMORY(mImmediateContext);

							CComPtrCustom<ID3D10Multithread> lMultiThread;

							// Need to explitly set the multithreaded mode for this device
							LOG_INVOKE_QUERY_INTERFACE_METHOD(mImmediateContext, &lMultiThread);

							LOG_CHECK_PTR_MEMORY(lMultiThread);

							BOOL lbRrsult = lMultiThread->SetMultithreadProtected(TRUE);

							break;
						}

						CComPtrCustom<ID3D11Resource> l_Resource;

						CComPtrCustom<ID3D11Texture2D> l_SharedTexture;

						lresult = mD3D11Device->OpenSharedResource(aHandle, IID_PPV_ARGS(&l_Resource));

						if (SUCCEEDED(lresult))
						{
							lresult = l_Resource->QueryInterface(IID_PPV_ARGS(&l_SharedTexture));
						}

						lID3D11Texture2D = l_SharedTexture;

						if (lID3D11Texture2D)
						{
							mSwapChain1.Release();

							mID3D11Texture2D.Release();

							mID3D11Texture2D = lID3D11Texture2D;

							LOG_CHECK_PTR_MEMORY(mID3D11Texture2D);

							mD3D11Device.Release();

							mID3D11Texture2D->GetDevice(&mD3D11Device);

							LOG_CHECK_PTR_MEMORY(mD3D11Device);

							CComPtrCustom<ID3D11VideoDevice> lDX11VideoDevice;

							LOG_INVOKE_QUERY_INTERFACE_METHOD(mD3D11Device, &lDX11VideoDevice);

							LOG_INVOKE_FUNCTION(createSample, Direct3D11Presenter::RenderTexture);

							LOG_CHECK_PTR_MEMORY(lDX11VideoDevice);

							LOG_CHECK_PTR_MEMORY(mMFDXGIDeviceManager);

							LOG_INVOKE_POINTER_METHOD(mMFDXGIDeviceManager, ResetDevice,
								mD3D11Device,
								mDeviceResetToken);

							mImmediateContext.Release();

							mD3D11Device->GetImmediateContext(&mImmediateContext);

							LOG_CHECK_PTR_MEMORY(mImmediateContext);

							CComPtrCustom<ID3D10Multithread> lMultiThread;

							// Need to explitly set the multithreaded mode for this device
							LOG_INVOKE_QUERY_INTERFACE_METHOD(mImmediateContext, &lMultiThread);

							LOG_CHECK_PTR_MEMORY(lMultiThread);

							BOOL lbRrsult = lMultiThread->SetMultithreadProtected(TRUE);

							break;
						}

					} while (false);

					return lresult;
				}

				
				HRESULT Direct3D11Presenter::setVideoWindowHandle(
					HWND aVideoWindowHandle)
				{
					HRESULT lresult(E_NOTIMPL);

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



				// IMFGetService methods
				STDMETHODIMP Direct3D11Presenter::GetService(
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
							if (aRefIID == __uuidof(IMFDXGIDeviceManager))
							{
								lresult = E_NOINTERFACE;
							}
							else if (aRefIID == __uuidof(IDirect3DDeviceManager9))
							{
								CComPtrCustom<IDirect3DDeviceManager9> lDeviceManager9;

								LOG_INVOKE_FUNCTION(createDirectX9ManagerAndDevice, &lDeviceManager9);

								LOG_INVOKE_WIDE_QUERY_INTERFACE_METHOD(lDeviceManager9, aRefIID, aPtrPtrObject)
							}
						}
						else if (aRefGUIDService == CM_VIDEO_ACCELERATION_SERVICE) {
							if (aRefIID == __uuidof(IMFDXGIDeviceManager))
							{
								LOG_INVOKE_WIDE_QUERY_INTERFACE_METHOD(mMFDXGIDeviceManager,
									__uuidof(IUnknown),
									aPtrPtrObject);
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
										LOG_INVOKE_POINTER_METHOD(lVideoSampleAllocator, SetDirectXManager, mMFDXGIDeviceManager);

										LOG_INVOKE_WIDE_QUERY_INTERFACE_METHOD(lVideoSampleAllocator, aRefIID, aPtrPtrObject);
									}

								} while (false);
							}
							else if (aRefIID == __uuidof(IMFVideoSampleAllocatorEx))
							{
								do
								{

									CComPtrCustom<IMFVideoSampleAllocatorEx> lVideoSampleAllocator;

									LOG_INVOKE_MF_FUNCTION(MFCreateVideoSampleAllocatorEx,
										IID_PPV_ARGS(&lVideoSampleAllocator));

									if (lVideoSampleAllocator)
									{
										LOG_INVOKE_POINTER_METHOD(lVideoSampleAllocator, SetDirectXManager, mMFDXGIDeviceManager);

										LOG_INVOKE_WIDE_QUERY_INTERFACE_METHOD(lVideoSampleAllocator, aRefIID, aPtrPtrObject);
									}

								} while (false);
							}
							else
							{
								lresult = E_NOINTERFACE;
							}
						}

					} while (false);

					return lresult;
				}

				HRESULT Direct3D11Presenter::createDevice(ID3D11Device** aPtrPtrDevice)
				{
					HRESULT lresult(E_FAIL);

					do
					{
						LOG_INVOKE_FUNCTION(Singleton<DXGIManager>::getInstance().getState);

						CComPtrCustom<IDXGIFactory1> lFactory;
						CComPtrCustom<IDXGIAdapter> dxgiAdapter;
						CComPtrCustom<ID3D11DeviceContext> cp_d3dContext;

						LOG_INVOKE_DXGI_FUNCTION(CreateDXGIFactory1,
							IID_PPV_ARGS(&lFactory));

						LOG_CHECK_PTR_MEMORY(lFactory);

						CComPtrCustom<IDXGIFactory2> lDxgiAdapter1;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lFactory, &lDxgiAdapter1);


						LOG_CHECK_PTR_MEMORY(aPtrPtrDevice);

						LOG_INVOKE_FUNCTION(Singleton<Direct3D11Manager>::getInstance().getState);

						CComPtrCustom<ID3D11Device> cp_d3dDevice = nullptr;
						UINT i_adapter = 0;
						while (cp_d3dDevice == nullptr)
						{
							lresult = lDxgiAdapter1->EnumAdapters(i_adapter++, &dxgiAdapter);
							if (FAILED(lresult))
							{
								if (mUseDebugLayer & D3D11_CREATE_DEVICE_VIDEO_SUPPORT)
								{
									/* try again without this flag */
									i_adapter = 0;
									mUseDebugLayer &= ~D3D11_CREATE_DEVICE_VIDEO_SUPPORT;
									continue; //Try again with the new flags
								}
								else
									break; /* no more flags to remove */
							}

							lresult = Direct3D11::Direct3D11Manager::D3D11CreateDevice(
								dxgiAdapter.get(),
								D3D_DRIVER_TYPE_UNKNOWN,
								nullptr,
								mUseDebugLayer,
								NULL,
								0,
								D3D11_SDK_VERSION,
								&cp_d3dDevice,
								nullptr,
								&cp_d3dContext
							);

							if (FAILED(lresult))
								cp_d3dDevice = nullptr;
						}

						LOG_CHECK_PTR_MEMORY(cp_d3dDevice);

						LOG_INVOKE_QUERY_INTERFACE_METHOD(cp_d3dDevice, aPtrPtrDevice);
						
						//D3D_FEATURE_LEVEL lfeatureLevel;

						//LOG_INVOKE_DX11_FUNCTION(D3D11CreateDevice,
						//	NULL,
						//	D3D_DRIVER_TYPE_UNKNOWN,
						//	//D3D_DRIVER_TYPE_HARDWARE,
						//	NULL,
						//	mUseDebugLayer,
						//	NULL,
						//	0,
						//	D3D11_SDK_VERSION,
						//	aPtrPtrDevice,
						//	//&lfeatureLevel,
						//	NULL,
						//	NULL);

						LOG_CHECK_PTR_MEMORY(aPtrPtrDevice);
						
					} while (false);

					return lresult;
				}

				HRESULT Direct3D11Presenter::createSample(TargetType aTargetType)
				{
					HRESULT lresult(E_NOTIMPL);

					do
					{

						mSample.Release();

						LOG_INVOKE_MF_FUNCTION(MFCreateSample,
							&mSample);

						LOG_CHECK_PTR_MEMORY(mSample);
						
						D3D11_TEXTURE2D_DESC lSurfaceDesc;

						ZeroMemory(&lSurfaceDesc, sizeof(lSurfaceDesc));

						switch (aTargetType)
						{
						case EVRMultiSink::Sinks::EVR::Direct3D11::Direct3D11Presenter::Handler:
						{

							CComPtrCustom<ID3D11Texture2D> lSurface;

							// Get Backbuffer
							LOG_INVOKE_POINTER_METHOD(mSwapChain1, GetBuffer,
								0, IID_PPV_ARGS(&lSurface));

							LOG_CHECK_PTR_MEMORY(lSurface);

							lSurface->GetDesc(
								&lSurfaceDesc);


							CComPtrCustom<IMFMediaBuffer> lBuffer;

							LOG_INVOKE_MF_FUNCTION(MFCreateDXGISurfaceBuffer,
								__uuidof(ID3D11Texture2D),
								lSurface,
								0,
								FALSE,
								&lBuffer);

							LOG_CHECK_PTR_MEMORY(lBuffer);

							LOG_INVOKE_MF_METHOD(AddBuffer, mSample,
								lBuffer);

						}
							break;
						case EVRMultiSink::Sinks::EVR::Direct3D11::Direct3D11Presenter::SwapChain:
						{

							LOG_CHECK_PTR_MEMORY(mSwapChain1);

							CComPtrCustom<ID3D11Texture2D> lSurface;

							// Get Backbuffer
							LOG_INVOKE_POINTER_METHOD(mSwapChain1, GetBuffer,
								0, IID_PPV_ARGS(&lSurface));

							LOG_CHECK_PTR_MEMORY(lSurface);

							lSurface->GetDesc(
								&lSurfaceDesc);

							LOG_INVOKE_MF_METHOD(SetUnknown, mSample,
								CM_SwapChain,
								mSwapChain1);
						}
							break;
						case EVRMultiSink::Sinks::EVR::Direct3D11::Direct3D11Presenter::RenderTexture:
						{
							LOG_CHECK_PTR_MEMORY(mID3D11Texture2D);

							mID3D11Texture2D->GetDesc(
								&lSurfaceDesc);

							LOG_INVOKE_MF_METHOD(SetUnknown, mSample,
								CM_RenderTexture,
								mID3D11Texture2D);
						}
							break;
						default:
							break;
						}


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

				HRESULT Direct3D11Presenter::getMaxInputStreamCount(DWORD* aPtrMaxInputStreamCount)
				{
					HRESULT lresult(E_FAIL);

					do
					{
						LOG_CHECK_PTR_MEMORY(aPtrMaxInputStreamCount);

						LOG_INVOKE_FUNCTION(Singleton<Direct3D11Manager>::getInstance().getState);

						CComPtrCustom<ID3D11Device> lD3D11Device;
						
						LOG_INVOKE_FUNCTION(createDevice, &lD3D11Device);
						
						LOG_CHECK_PTR_MEMORY(lD3D11Device);

						CComPtrCustom<ID3D11VideoDevice> lVideoDevice;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lD3D11Device, &lVideoDevice);

						LOG_CHECK_PTR_MEMORY(lVideoDevice);

						CComPtrCustom<ID3D11VideoProcessorEnumerator> lVideoProcessorEnum;

						D3D11_VIDEO_PROCESSOR_CONTENT_DESC ContentDesc;
						ZeroMemory(&ContentDesc, sizeof(ContentDesc));
						ContentDesc.InputFrameFormat = D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE;// D3D11_VIDEO_FRAME_FORMAT_INTERLACED_TOP_FIELD_FIRST;
						ContentDesc.InputWidth = 1920;
						ContentDesc.InputHeight = 1080;
						ContentDesc.OutputWidth = 1920;
						ContentDesc.OutputHeight = 1080;
						ContentDesc.Usage = D3D11_VIDEO_USAGE_PLAYBACK_NORMAL;

						LOG_INVOKE_POINTER_METHOD(lVideoDevice, CreateVideoProcessorEnumerator,
							&ContentDesc,
							&lVideoProcessorEnum);

						LOG_CHECK_PTR_MEMORY(lVideoProcessorEnum);

						D3D11_VIDEO_PROCESSOR_CAPS lCAPS;

						LOG_INVOKE_POINTER_METHOD(lVideoProcessorEnum, GetVideoProcessorCaps,
							&lCAPS);

						*aPtrMaxInputStreamCount = lCAPS.MaxInputStreams;

					} while (false);

					return lresult;
				}

				// IPresenter methods
				HRESULT Direct3D11Presenter::initialize(
					UINT32 aImageWidth,
					UINT32 aImageHeight,
					DWORD aNumerator,
					DWORD aDenominator,
					IMFTransform* aPtrMixer)
				{
					HRESULT lresult(E_NOTIMPL);

					do
					{
						LOG_CHECK_PTR_MEMORY(aPtrMixer);

						LOG_INVOKE_FUNCTION(Singleton<Direct3D11Manager>::getInstance().getState);

						mD3D11Device.Release();

						LOG_INVOKE_FUNCTION(createDevice, &mD3D11Device);

						LOG_CHECK_PTR_MEMORY(mD3D11Device);

						CComPtrCustom<ID3D11VideoDevice> lDX11VideoDevice;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mD3D11Device, &lDX11VideoDevice);
						
						LOG_CHECK_PTR_MEMORY(lDX11VideoDevice);

						mMFDXGIDeviceManager.Release();
												
						LOG_INVOKE_MF_FUNCTION(MFCreateDXGIDeviceManager,
							&mDeviceResetToken,
							&mMFDXGIDeviceManager);
						
						LOG_CHECK_PTR_MEMORY(mMFDXGIDeviceManager);
						
						LOG_INVOKE_POINTER_METHOD(mMFDXGIDeviceManager, ResetDevice,
							mD3D11Device,
							mDeviceResetToken);

						MFRatio lFrameRate;

						lFrameRate.Numerator = aNumerator;

						lFrameRate.Denominator = aDenominator;

						LOG_INVOKE_FUNCTION(init,
							aImageWidth,
							aImageHeight,
							lFrameRate);

						mMixer = aPtrMixer;

						LOG_CHECK_PTR_MEMORY(mMixer);
												
						LOG_INVOKE_MF_METHOD(ProcessMessage, mMixer, MFT_MESSAGE_SET_D3D_MANAGER, 0);

						LOG_INVOKE_MF_METHOD(ProcessMessage, mMixer, MFT_MESSAGE_SET_D3D_MANAGER, (ULONG_PTR)mMFDXGIDeviceManager.get());
						
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

				MFTIME mLastTime1 = 0;

				HRESULT Direct3D11Presenter::ProcessFrame(BOOL aImmediate)
				{
					HRESULT lresult(E_FAIL);

					do
					{
						//if (aImmediate == FALSE)
						//{
						//	auto lCurrentTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

						//	if ((lCurrentTime - mLastTime) < mVideoFrameDuration)
						//	{
						//		lresult = S_OK;

						//		break;
						//	}

						//	mLastTime = lCurrentTime;
						//}

						//std::lock_guard<std::mutex> lLock(mAccessMutex);

						auto ltry_lock = mAccessMutex.try_lock();

						MFT_OUTPUT_DATA_BUFFER lBuffer;

						ZeroMemory(&lBuffer, sizeof(lBuffer));

						lBuffer.dwStreamID = 0;

						lBuffer.pSample = mSample;

						if (mHWNDVideo != nullptr)
						{
							RECT lWindowRect;

							GetClientRect(mHWNDVideo, &lWindowRect);

							auto lNativeClientWidth = lWindowRect.right - lWindowRect.left;

							auto lNativeClientHeight = lWindowRect.bottom - lWindowRect.top;

							MFSetAttributeSize(
								mSample,
								MF_MT_FRAME_SIZE,
								lNativeClientWidth,
								lNativeClientHeight);

						}
						else
						{
							
						}

						DWORD lState(0);

						LOG_INVOKE_MF_METHOD(ProcessOutput, mMixer,
							0,
							1,
							&lBuffer,
							&lState);

						if (mSwapChain1)
							lresult = mSwapChain1->Present(0, 0);
						else if (mImmediateContext)
						{
							mImmediateContext->Flush();

							if (mDevice9)
								lresult = mDevice9->StretchRect(
									mRenderDirect3DSurface9,
									NULL,
									mTargetDirect3DSurface9,
									NULL,
									D3DTEXTUREFILTERTYPE::D3DTEXF_NONE);
						}																			

						if (ltry_lock)
							mAccessMutex.unlock();

					} while (false);

					return lresult;
				}
								
				void Direct3D11Presenter::releaseResources()
				{
					mImmediateContext.Release();

					mDXGIFactory2.Release();

					mDXGIOutput1.Release();

					mSwapChain1.Release();
					
					mSample.Release();
				}	

				HRESULT Direct3D11Presenter::createManagerAndDevice()
				{
					HRESULT lresult(E_FAIL);
										
					do
					{

						LOG_CHECK_PTR_MEMORY(mMFDXGIDeviceManager);

						LOG_CHECK_PTR_MEMORY(mD3D11Device);

						LOG_CHECK_STATE_DESCR(mHWNDVideo == nullptr, E_UNEXPECTED);

						releaseResources();

						mImmediateContext.Release();

						mD3D11Device->GetImmediateContext(&mImmediateContext);

						LOG_CHECK_PTR_MEMORY(mImmediateContext);

						CComPtrCustom<ID3D10Multithread> lMultiThread;

						// Need to explitly set the multithreaded mode for this device
						LOG_INVOKE_QUERY_INTERFACE_METHOD(mImmediateContext, &lMultiThread);

						LOG_CHECK_PTR_MEMORY(lMultiThread);
												
						BOOL lbRrsult = lMultiThread->SetMultithreadProtected(TRUE);
						
						CComPtrCustom<IDXGIDevice1> lDXGIDev;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mD3D11Device, &lDXGIDev);

						LOG_CHECK_PTR_MEMORY(lDXGIDev);

						CComPtrCustom<IDXGIAdapter> lTempAdapter;

						LOG_INVOKE_POINTER_METHOD(lDXGIDev, GetAdapter, &lTempAdapter);

						LOG_CHECK_PTR_MEMORY(lTempAdapter);

						CComPtrCustom<IDXGIAdapter1> lAdapter;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lTempAdapter, &lAdapter);

						LOG_CHECK_PTR_MEMORY(lAdapter);

						mDXGIFactory2.Release();

						LOG_INVOKE_POINTER_METHOD(lAdapter, GetParent,
							IID_PPV_ARGS(&mDXGIFactory2));

						LOG_CHECK_PTR_MEMORY(mDXGIFactory2);

						CComPtrCustom<IDXGIOutput> lDXGIOutput;

						LOG_INVOKE_POINTER_METHOD(lAdapter, EnumOutputs,
							0, &lDXGIOutput);

						LOG_CHECK_PTR_MEMORY(lDXGIOutput);
						
						mDXGIOutput1.Release();

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lDXGIOutput, &mDXGIOutput1);

						LOG_CHECK_PTR_MEMORY(mDXGIOutput1);

						// Get the DXGISwapChain1
						DXGI_SWAP_CHAIN_DESC1 scd;
						ZeroMemory(&scd, sizeof(scd));
						scd.SampleDesc.Count = 1;
						scd.SampleDesc.Quality = 0;
						scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
						scd.Scaling = DXGI_SCALING_STRETCH;
						scd.Width = mImageWidth;
						scd.Height = mImageHeight;
						scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
						scd.Stereo = FALSE;
						scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
						scd.Flags = 0;// m_bStereoEnabled ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0; //opt in to do direct flip;
						scd.BufferCount = 4;
						
						mSwapChain1.Release();

						LOG_INVOKE_POINTER_METHOD(mDXGIFactory2, CreateSwapChainForHwnd,
							mD3D11Device,
							mHWNDVideo, 
							&scd, 
							NULL,
							NULL,
							&mSwapChain1);

						LOG_CHECK_PTR_MEMORY(mSwapChain1);

						LOG_INVOKE_FUNCTION(createSample);

					} while (false);

					return lresult;
				}

				HRESULT Direct3D11Presenter::createDirectX9ManagerAndDevice(IDirect3DDeviceManager9** a_DeviceManager)
				{
					HRESULT lresult(E_FAIL);

					UINT lAdapter = D3DADAPTER_DEFAULT;

					D3DDISPLAYMODE dm;

					D3DPRESENT_PARAMETERS pp;

					do
					{
						LOG_CHECK_PTR_MEMORY(a_DeviceManager);

						LOG_INVOKE_FUNCTION(Singleton<Direct3D9ExManager>::getInstance().getState);

						CComPtrCustom<IDirect3DDevice9> lDevice9;

						CComPtrCustom<IDirect3D9Ex> lD3D9;

						UINT lDirectX9DeviceResetToken;

						CComPtrCustom<IDirect3DDeviceManager9> lDirect3DDeviceManager;

						LOG_INVOKE_DX9EX_FUNCTION(Direct3DCreate9Ex, D3D_SDK_VERSION, &lD3D9);

						LOG_CHECK_PTR_MEMORY(lD3D9);

						LOG_INVOKE_DX9_METHOD(GetAdapterDisplayMode,
							lD3D9,
							lAdapter,
							&dm);

						ZeroMemory(&pp, sizeof(pp));

						pp.Windowed = TRUE;
						pp.hDeviceWindow = GetDesktopWindow();
						pp.SwapEffect = D3DSWAPEFFECT_COPY;
						pp.BackBufferFormat = dm.Format;
						pp.BackBufferWidth = 800,
							pp.BackBufferHeight = 600;
						pp.Flags =
							D3DPRESENTFLAG_VIDEO
							//| D3DPRESENTFLAG_DEVICECLIP
							| D3DPRESENTFLAG_LOCKABLE_BACKBUFFER
							;
						pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
						pp.BackBufferCount = 1;

						LOG_INVOKE_DX9_METHOD(CreateDevice,
							lD3D9,
							lAdapter,
							D3DDEVTYPE_HAL,
							GetDesktopWindow(),
							//D3DCREATE_HARDWARE_VERTEXPROCESSING |
							D3DCREATE_SOFTWARE_VERTEXPROCESSING |
							D3DCREATE_NOWINDOWCHANGES |
							D3DCREATE_MULTITHREADED |
							D3DCREATE_FPU_PRESERVE,
							&pp,
							&lDevice9);

						LOG_CHECK_PTR_MEMORY(lDevice9);

						LOG_INVOKE_DX9_FUNCTION(DXVA2CreateDirect3DDeviceManager9,
								&lDirectX9DeviceResetToken,
								&lDirect3DDeviceManager);

						LOG_CHECK_PTR_MEMORY(lDirect3DDeviceManager);

						LOG_INVOKE_DX9_METHOD(ResetDevice, lDirect3DDeviceManager,
							lDevice9,
							lDirectX9DeviceResetToken);

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lDirect3DDeviceManager, a_DeviceManager);						

					} while (false);

					return lresult;
				}

			}
		}
	}
}