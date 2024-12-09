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

#include "DirectShowEVRMultiFilter.h"
#include "IMixerStreamPositionControl.h"
#include "IStreamFilterControl.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/Singleton.h"
#include "../Common/GUIDs.h"
#include "../VideoSurfaceCopierManager/VideoSurfaceCopierManager.h"
#include "../MemoryManager/MemoryManager.h"


namespace EVRMultiSink
{
	namespace Filters
	{
		namespace EVR
		{
			using namespace CaptureManager;

			using namespace CaptureManager::Core;
		
			using namespace CaptureManager::Core::MediaFoundation;

			class DECLSPEC_UUID("F6BAC3B0-DA40-4F77-AB08-23DE4CF055C1")
				CLSID_DirectShowEVRMultiFilter;

#define FILTER_NAME L"Capture Manager DirectShow Video Renderer"

			DirectShowEVRMultiFilter::DirectShowEVRMultiFilter(DWORD aMixerStreamID) :
				CBaseVideoRenderer(__uuidof(CLSID_DirectShowEVRMultiFilter), FILTER_NAME, nullptr, &mOLECode),
				mMixerStreamID(aMixerStreamID)
			{
				// Store the video input pin
				m_pInputPin = new CVideoInputPin(NAME("Video Pin"), this, &m_InterfaceLock, nullptr, L"Input");
				
				AddRef();

				mInnerAllocatorState = TRUE;
			}

			DirectShowEVRMultiFilter::~DirectShowEVRMultiFilter()
			{
			}


			HRESULT DirectShowEVRMultiFilter::createFilter(
				IPresenter* aPtrPresenter,
				IMFTransform* aPtrMixer,
				DWORD aMixerStreamID,
				IBaseFilter** aPtrPrtBaseFilter)
			{
				HRESULT lresult(E_FAIL);

				do
				{


					LOG_CHECK_PTR_MEMORY(aPtrPresenter);

					LOG_CHECK_PTR_MEMORY(aPtrMixer);

					LOG_CHECK_PTR_MEMORY(aPtrPrtBaseFilter);
					
					CComPtrCustom<DirectShowEVRMultiFilter> lDirectShowEVRMultiFilter(
						new (std::nothrow)DirectShowEVRMultiFilter(aMixerStreamID));

					LOG_CHECK_PTR_MEMORY(lDirectShowEVRMultiFilter);
					
					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrMixer, &lDirectShowEVRMultiFilter->mMixer);

					LOG_CHECK_PTR_MEMORY(lDirectShowEVRMultiFilter->mMixer);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrPresenter, &lDirectShowEVRMultiFilter->mPresenter);

					LOG_CHECK_PTR_MEMORY(lDirectShowEVRMultiFilter->mPresenter);				
					
					LOG_INVOKE_QUERY_INTERFACE_METHOD(lDirectShowEVRMultiFilter, aPtrPrtBaseFilter);
					
				} while (false);

				return lresult;
			}

			HRESULT DirectShowEVRMultiFilter::createVideoAllocator(
				IMFVideoSampleAllocator** aPtrPtrVideoSampleAllocator)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(mPresenter);

					LOG_CHECK_PTR_MEMORY(aPtrPtrVideoSampleAllocator);

					CComQIPtrCustom<IMFGetService> lGetService;

					lGetService = mPresenter;

					LOG_CHECK_PTR_MEMORY(lGetService);

					do
					{
						CComPtrCustom<IMFVideoSampleAllocator> lVideoSampleAllocator;

						LOG_INVOKE_POINTER_METHOD(lGetService, GetService,
							CM_VIDEO_ACCELERATION_SERVICE,
							IID_PPV_ARGS(&lVideoSampleAllocator));

						LOG_CHECK_PTR_MEMORY(lVideoSampleAllocator);

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lVideoSampleAllocator, aPtrPtrVideoSampleAllocator);

					} while (false);

					if (FAILED(lresult))
					{
						CComPtrCustom<IMFVideoSampleAllocatorEx> lVideoSampleAllocatorEx;

						LOG_INVOKE_POINTER_METHOD(lGetService, GetService,
							CM_VIDEO_ACCELERATION_SERVICE,
							IID_PPV_ARGS(&lVideoSampleAllocatorEx));

						LOG_CHECK_PTR_MEMORY(lVideoSampleAllocatorEx);

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lVideoSampleAllocatorEx, aPtrPtrVideoSampleAllocator);
					}

				} while (false);

				return lresult;
			}

			HRESULT DirectShowEVRMultiFilter::DoRenderSample(IMediaSample *pMediaSample)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(pMediaSample);

					LOG_CHECK_PTR_MEMORY(mCurrentSample);

					LOG_CHECK_PTR_MEMORY(mCurrentMediaBuffer);
					
					LONGLONG lSampleTime = MediaFoundationManager::MFGetSystemTime();

					LOG_INVOKE_MF_METHOD(SetSampleTime,
						mCurrentSample, lSampleTime);

					LONGLONG lSampleDuration = 330000;

					LOG_INVOKE_MF_METHOD(SetSampleDuration,
						mCurrentSample,
						lSampleDuration);

					auto lActualDataLength = pMediaSample->GetActualDataLength();


					BYTE* lPtrBuffer = nullptr;

					DWORD lMaxLength;

					DWORD lCurrentLength;

					LOG_INVOKE_MF_METHOD(Lock, mCurrentMediaBuffer,
						&lPtrBuffer,
						&lMaxLength,
						&lCurrentLength);
					
					PBYTE pbData = nullptr;

					LOG_INVOKE_POINTER_METHOD(pMediaSample, GetPointer, &pbData);

					LOG_CHECK_PTR_MEMORY(pbData);
					
					MemoryManager::memcpy(lPtrBuffer, pbData, lActualDataLength);
					
					LOG_INVOKE_MF_METHOD(Unlock, mCurrentMediaBuffer);
					
					LOG_INVOKE_MF_METHOD(SetCurrentLength, mCurrentMediaBuffer, lActualDataLength);
										
					LOG_INVOKE_MF_METHOD(ProcessInput, mMixer,
						mMixerStreamID,
						mCurrentSample,
						0);

					mPresenter->ProcessFrame(FALSE);

				} while (false);

				return lresult;
			}

			HRESULT DirectShowEVRMultiFilter::OnStartStreaming() 
			{ 
				return NOERROR; 
			};

			HRESULT DirectShowEVRMultiFilter::OnStopStreaming() 
			{ 
				return NOERROR; 
			};
						
			//IMFGetService implements

			STDMETHODIMP DirectShowEVRMultiFilter::NonDelegatingQueryInterface(REFIID riid, __deref_out void **ppv)
			{
				// Do we have this interface

				if (riid == __uuidof(IMFGetService)) {

					class MFVideoDisplayControlProxy :
						public BaseUnknown<IMFVideoDisplayControl>
					{
						const DWORD mMixerStreamID;

						CaptureManager::CComQIPtrCustom<EVRMultiSink::Sinks::EVR::IMixerStreamPositionControl> mMixerStreamPositionControl;
						
						CaptureManager::CComQIPtrCustom<EVRMultiSink::Sinks::EVR::IStreamFilterControl> mStreamFilterControl;

						CaptureManager::CComPtrCustom<IPresenter> mPresenter;

					public:

						MFVideoDisplayControlProxy(IMFTransform* aPtrMixer, DWORD aMixerStreamID, IPresenter* aPtrIPresenter) :
							mMixerStreamID(aMixerStreamID)
						{ 
							mMixerStreamPositionControl = aPtrMixer;

							mStreamFilterControl = aPtrMixer;

							mPresenter = aPtrIPresenter;
						}

						virtual ~MFVideoDisplayControlProxy(){}

						virtual HRESULT STDMETHODCALLTYPE GetNativeVideoSize(
							/* [unique][out][in] */ __RPC__inout_opt SIZE *pszVideo,
							/* [unique][out][in] */ __RPC__inout_opt SIZE *pszARVideo)override
						{
							HRESULT lresult(E_NOTIMPL);

							do
							{
								lresult = S_OK;

							} while (false);

							return lresult;
						}

						virtual HRESULT STDMETHODCALLTYPE GetIdealVideoSize(
							/* [unique][out][in] */ __RPC__inout_opt SIZE *pszMin,
							/* [unique][out][in] */ __RPC__inout_opt SIZE *pszMax)override
						{
							HRESULT lresult(E_NOTIMPL);

							do
							{
								lresult = S_OK;

							} while (false);

							return lresult;
						}

						virtual HRESULT STDMETHODCALLTYPE SetVideoPosition(
							/* [unique][in] */ __RPC__in_opt const MFVideoNormalizedRect *pnrcSource,
							/* [unique][in] */ __RPC__in_opt const LPRECT prcDest)override
						{
							HRESULT lresult(E_NOTIMPL);

							do
							{
								lresult = S_OK;

							} while (false);

							return lresult;
						}

						virtual HRESULT STDMETHODCALLTYPE GetVideoPosition(
							/* [out] */ __RPC__out MFVideoNormalizedRect *pnrcSource,
							/* [out] */ __RPC__out LPRECT prcDest)override
						{
							HRESULT lresult(E_NOTIMPL);

							do
							{
								lresult = S_OK;

							} while (false);

							return lresult;
						}

						virtual HRESULT STDMETHODCALLTYPE SetAspectRatioMode(
							/* [in] */ DWORD dwAspectRatioMode)override
						{
							HRESULT lresult(S_OK);

							do
							{

							} while (false);

							return lresult;
						}

						virtual HRESULT STDMETHODCALLTYPE GetAspectRatioMode(
							/* [out] */ __RPC__out DWORD *pdwAspectRatioMode)override
						{
							HRESULT lresult(E_NOTIMPL);

							do
							{
								lresult = S_OK;

							} while (false);

							return lresult;
						}

						virtual HRESULT STDMETHODCALLTYPE SetVideoWindow(
							/* [in] */ __RPC__in HWND hwndVideo)override
						{
							HRESULT lresult(E_NOTIMPL);

							do
							{

								lresult = S_OK;

							} while (false);

							return lresult;
						}

						virtual HRESULT STDMETHODCALLTYPE GetVideoWindow(
							/* [out] */ __RPC__deref_out_opt HWND *phwndVideo)override
						{
							HRESULT lresult(E_NOTIMPL);

							do
							{
								if (phwndVideo != nullptr)
									*phwndVideo = nullptr;

								lresult = S_OK;

							} while (false);

							return lresult;
						}

						virtual HRESULT STDMETHODCALLTYPE RepaintVideo(void)override
						{
							HRESULT lresult(E_NOTIMPL);

							do
							{
								if (mPresenter)
									mPresenter->ProcessFrame(TRUE);

								lresult = S_OK;

							} while (false);

							return lresult;
						}

						virtual HRESULT STDMETHODCALLTYPE GetCurrentImage(
							/* [out][in] */ __RPC__inout BITMAPINFOHEADER *pBih,
							/* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*pcbDib) BYTE **pDib,
							/* [out] */ __RPC__out DWORD *pcbDib,
							/* [unique][out][in] */ __RPC__inout_opt LONGLONG *pTimeStamp)override
						{
							HRESULT lresult(E_NOTIMPL);

							do
							{

								lresult = S_OK;
							} while (false);

							return lresult;
						}

						virtual HRESULT STDMETHODCALLTYPE SetBorderColor(
							/* [in] */ COLORREF Clr)override
						{
							HRESULT lresult(E_NOTIMPL);

							do
							{

								lresult = S_OK;
							} while (false);

							return lresult;
						}

						virtual HRESULT STDMETHODCALLTYPE GetBorderColor(
							/* [out] */ __RPC__out COLORREF *pClr)override
						{
							HRESULT lresult(E_NOTIMPL);

							do
							{

								lresult = S_OK;
							} while (false);

							return lresult;
						}

						virtual HRESULT STDMETHODCALLTYPE SetRenderingPrefs(
							/* [in] */ DWORD dwRenderFlags)override
						{
							HRESULT lresult(E_NOTIMPL);

							do
							{

								lresult = S_OK;
							} while (false);

							return lresult;
						}

						virtual HRESULT STDMETHODCALLTYPE GetRenderingPrefs(
							/* [out] */ __RPC__out DWORD *pdwRenderFlags)override
						{
							HRESULT lresult(E_NOTIMPL);

							do
							{

								lresult = S_OK;
							} while (false);

							return lresult;
						}

						virtual HRESULT STDMETHODCALLTYPE SetFullscreen(
							/* [in] */ BOOL fFullscreen)override
						{
							HRESULT lresult(E_NOTIMPL);

							do
							{

								lresult = S_OK;
							} while (false);

							return lresult;
						}

						virtual HRESULT STDMETHODCALLTYPE GetFullscreen(
							/* [out] */ __RPC__out BOOL *pfFullscreen)override
						{
							HRESULT lresult(E_NOTIMPL);

							do
							{
								if (pfFullscreen != nullptr)
									*pfFullscreen = FALSE;

								lresult = S_OK;
							} while (false);

							return lresult;
						}
					};

					class MFGetServiceProxy :
						public BaseUnknown<IMFGetService>
					{
						const DWORD mMixerStreamID;

						CaptureManager::CComPtrCustom<IMFTransform> mMixer;

						CaptureManager::CComPtrCustom<IPresenter> mPresenter;
					public:

						MFGetServiceProxy(IMFTransform* aPtrMixer, DWORD aMixerStreamID, IPresenter* aPtrIPresenter) :
							mMixerStreamID(aMixerStreamID){
							mMixer = aPtrMixer;
							mPresenter = aPtrIPresenter;
						}

						virtual ~MFGetServiceProxy(){}

						virtual HRESULT STDMETHODCALLTYPE GetService(
							/* [in] */ __RPC__in REFGUID guidService,
							/* [in] */ __RPC__in REFIID aRefIID,
							/* [iid_is][out] */ __RPC__deref_out_opt LPVOID *ppvObject)override
						{
							HRESULT lresult(MF_E_UNSUPPORTED_SERVICE);

							do
							{
								LOG_CHECK_PTR_MEMORY(ppvObject);

								if (guidService == MR_VIDEO_RENDER_SERVICE)
								{
									if (aRefIID == __uuidof(IMFVideoDisplayControl))
									{
										*ppvObject = new MFVideoDisplayControlProxy(mMixer, mMixerStreamID, mPresenter);

										if (*ppvObject == nullptr)
										{
											lresult = E_NOTIMPL;
										}
										else
										{
											lresult = S_OK;
										}
									}
								}

							} while (false);

							return lresult;
						}
					};
					
					*ppv = new MFGetServiceProxy(mMixer, mMixerStreamID, mPresenter);

					return S_OK;
				}
				else if (riid == __uuidof(IBaseFilter)) {

					CheckPointer(ppv, E_POINTER);

					*ppv = (IBaseFilter *)this;

					this->AddRef();

					return S_OK;
				}				
				else {
					return CBaseVideoRenderer::NonDelegatingQueryInterface(riid, ppv);
				}

			}

			HRESULT DirectShowEVRMultiFilter::CheckMediaType(const CMediaType* aPtrCMediaType)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrCMediaType);

					CComPtrCustom<IMFMediaType> lMFType;

					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lMFType);

					LOG_CHECK_PTR_MEMORY(lMFType);

					LOG_INVOKE_MF_FUNCTION(MFInitMediaTypeFromAMMediaType,
						lMFType,
						aPtrCMediaType);


					LOG_INVOKE_POINTER_METHOD(mMixer, SetInputType,
						mMixerStreamID,
						lMFType,
						MFT_SET_TYPE_TEST_ONLY);									
					
				} while (false);

				return lresult;
			}

			//######################################
			// GetPin
			// We only support one input pin and it is numbered zero
			//######################################
			CBasePin *DirectShowEVRMultiFilter::GetPin(int n) {
				ASSERT(n == 0);
				if (n != 0) return NULL;

				return m_pInputPin;
			}



			//######################################
			// BreakConnect
			// This is called when a connection or an attempted connection is terminated
			// and lets us to reset the connection flag held by the base class renderer
			// The filter object may be hanging onto an image to use for refreshing the
			// video window so that must be freed (the allocator decommit may be waiting
			// for that image to return before completing) then we must also uninstall
			// any palette we were using, reset anything set with the control interfaces
			// then set our overall state back to disconnected ready for the next time
			//######################################
			HRESULT DirectShowEVRMultiFilter::BreakConnect() {
				CAutoLock cInterfaceLock(&m_InterfaceLock);

				// Check we are in a valid state
				HRESULT hr = CBaseVideoRenderer::BreakConnect();
				if (FAILED(hr)) return hr;

				// The window is not used when disconnected
				if (m_pInputPin != nullptr)
				{
					IPin *pPin = m_pInputPin->GetConnected();
					if (pPin) SendNotifyWindow(pPin, NULL);
				}

				return NOERROR;
			}

			//######################################
			// CompleteConnect
			// When we complete connection we need to see if the video has changed sizes
			// If it has then we activate the window and reset the source and destination
			// rectangles. If the video is the same size then we bomb out early. By doing
			// this we make sure that temporary disconnections such as when we go into a
			// fullscreen mode do not cause unnecessary property changes. The basic ethos
			// is that all properties should be persistent across connections if possible
			//######################################
			HRESULT DirectShowEVRMultiFilter::CompleteConnect(IPin *pReceivePin) {

				CAutoLock cInterfaceLock(&m_InterfaceLock);

				return CBaseVideoRenderer::CompleteConnect(pReceivePin);
			}


			HRESULT DirectShowEVRMultiFilter::SetMediaType(const CMediaType *aPtrCMediaType)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrCMediaType);

					if (mAllocatorTransform)
						mAllocatorTransform->ProcessMessage(MFT_MESSAGE_TYPE::MFT_MESSAGE_COMMAND_FLUSH, NULL);

					CComPtrCustom<IMFMediaType> lMFType;

					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lMFType);

					LOG_CHECK_PTR_MEMORY(lMFType);

					LOG_INVOKE_MF_FUNCTION(MFInitMediaTypeFromAMMediaType,
						lMFType,
						aPtrCMediaType);
					
					LOG_INVOKE_POINTER_METHOD(mMixer, SetInputType,
						mMixerStreamID,
						lMFType,
						MFT_SET_TYPE_TEST_ONLY);

					LOG_INVOKE_POINTER_METHOD(mMixer, SetInputType,
						mMixerStreamID,
						lMFType,
						0);

					mCurrentSample.Release();

					LOG_INVOKE_MF_FUNCTION(MFCreateSample, &mCurrentSample);

					LOG_CHECK_PTR_MEMORY(mCurrentSample);

					UINT32 lSampleLength = 0;

					lresult = lMFType->GetUINT32(MF_MT_SAMPLE_SIZE, &lSampleLength);

					LOG_CHECK_STATE(lSampleLength == 0);

					mCurrentMediaBuffer.Release();

					LOG_INVOKE_MF_FUNCTION(MFCreateMemoryBuffer,
						lSampleLength,
						&mCurrentMediaBuffer);

					LOG_INVOKE_MF_METHOD(AddBuffer, mCurrentSample, mCurrentMediaBuffer);

					LOG_CHECK_PTR_MEMORY(mCurrentMediaBuffer);
										
					CComPtrCustom<IMFVideoSampleAllocator> lVideoSampleAllocator;

					LOG_INVOKE_FUNCTION(createVideoAllocator,
						&lVideoSampleAllocator);

					LOG_CHECK_PTR_MEMORY(lVideoSampleAllocator);

					LOG_INVOKE_POINTER_METHOD(lVideoSampleAllocator, InitializeSampleAllocator,
						5,
						lMFType);
										
					CComPtrCustom<IUnknown> lUnknown;

					LOG_INVOKE_FUNCTION(Singleton<CaptureManager::Transform::VideoSurfaceCopierManager>::getInstance().createVideoSurfaceCopier,
						lVideoSampleAllocator,
						lMFType,
						&lUnknown);

					LOG_CHECK_PTR_MEMORY(lUnknown);
					
					mAllocatorTransform.Release();

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnknown, &mAllocatorTransform);

					LOG_CHECK_PTR_MEMORY(mAllocatorTransform);
										
				} while (false);

				return lresult;
			}



			//######################################
			// Constructor
			//######################################
			CVideoInputPin::CVideoInputPin(TCHAR *pObjectName,
				DirectShowEVRMultiFilter *pRenderer,
				CCritSec *pInterfaceLock,
				HRESULT *phr,
				LPCWSTR pPinName) :
				CRendererInputPin(pRenderer, phr, pPinName),
				m_pRenderer(pRenderer),
				m_pInterfaceLock(pInterfaceLock)
			{
				ASSERT(m_pRenderer);
				ASSERT(pInterfaceLock);
			}
		}
	}
}