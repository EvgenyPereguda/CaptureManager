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

#include "DirectShowVideoInputCaptureProcessor.h"
#include <Strmif.h>
#include "DirectShowCommon.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../DataParser/DataParser.h"
#include "../Common/Common.h"
#include "../Common/GUIDs.h"
#include "../Common/MFHeaders.h"
#include "../Common/IMediaEvent.h"
#include "../Common/Constants.h"


namespace CaptureManager
{
	namespace Sources
	{
		namespace DirectShow
		{
			using namespace Core;			

			using namespace CaptureManager::Core;

			DirectShowVideoInputCaptureProcessor::DirectShowVideoInputCaptureProcessor(IMoniker* aPtrIMoniker, 
				OUTPUT_INPUT_PIN_MAP aOutputInputPinMap) :
				mState(SourceState::SourceStateUninitialized),
				mDiscontinuity(FALSE),
				mSelectedStreamIndex(0),
				mStreamAmount(aOutputInputPinMap.size())
			{
				mMoniker = aPtrIMoniker;

				mOutputInputPinMap = aOutputInputPinMap;	
			}

			DirectShowVideoInputCaptureProcessor::~DirectShowVideoInputCaptureProcessor()
			{
			}

			
			//IInnerCaptureProcessor implementation

			HRESULT DirectShowVideoInputCaptureProcessor::getFrendlyName(
				BSTR* aPtrString)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrString);
					
					LOG_CHECK_PTR_MEMORY(mMoniker);
					
					CComPtrCustom<IPropertyBag> lPropBag;

					CComPtrCustom<IBindCtx> pbc;

					LOG_INVOKE_FUNCTION(CreateBindCtx, NULL, &pbc);

					LOG_INVOKE_POINTER_METHOD(mMoniker, BindToStorage,
						pbc,
						nullptr,
						IID_PPV_ARGS(&lPropBag));
					
					VARIANT lVariant;

					VariantInit(&lVariant);

					lresult = lPropBag->Read(L"FriendlyName", &lVariant, 0);

					if (FAILED(lresult))
					{
						LOG_INVOKE_POINTER_METHOD(lPropBag, Read,
							L"Description",
							&lVariant, 
							nullptr);
						
					}

					if (SUCCEEDED(lresult))
					{
						*aPtrString = SysAllocString(lVariant.bstrVal);
					}
					else
					{
						*aPtrString = SysAllocString(L"UnknownDevice");
					}

					VariantClear(&lVariant);

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT DirectShowVideoInputCaptureProcessor::getSymbolicLink(
				BSTR* aPtrString)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrString);

					LOG_CHECK_PTR_MEMORY(mMoniker);

					CComPtrCustom<IPropertyBag> lPropBag;

					CComPtrCustom<IBindCtx> pbc;

					LOG_INVOKE_FUNCTION(CreateBindCtx, NULL, &pbc);

					LOG_INVOKE_POINTER_METHOD(mMoniker, BindToStorage,
						pbc,
						nullptr,
						IID_PPV_ARGS(&lPropBag));

					VARIANT lVariant;

					VariantInit(&lVariant);

					do
					{
						LPOLESTR lDisplayName = nullptr;

						LOG_INVOKE_POINTER_METHOD(mMoniker, GetDisplayName,
							0, 0, &lDisplayName);

						if (lDisplayName == nullptr)
						{
							lresult = E_FAIL;
							break;
						}

						BSTR bstrString = SysAllocString(lDisplayName);

						lVariant.vt = VT_BSTR;

						lVariant.bstrVal = bstrString;

						CoTaskMemFree(lDisplayName);

					} while (false);

					if (FAILED(lresult))
					{
						do
						{
							LOG_INVOKE_POINTER_METHOD(lPropBag, Read,
								L"DevicePath",
								&lVariant,
								nullptr);

						} while (false);
					}

					if (FAILED(lresult))
					{
						do
						{
							LOG_INVOKE_POINTER_METHOD(lPropBag, Read,
								L"Description", // L"DevicePath",
								&lVariant,
								nullptr);

						} while (false);
					}

					if (FAILED(lresult))
					{
						do
						{
							LOG_INVOKE_POINTER_METHOD(lPropBag, Read,
								L"FriendlyName",
								&lVariant,
								nullptr);

						} while (false);
					}

					std::wstring lsymbolicLinkString(L"UnknownDevice");

					if (SUCCEEDED(lresult))
					{
						lsymbolicLinkString = std::wstring(lVariant.bstrVal);
					}

					lsymbolicLinkString = L"CaptureManager///Software///Sources///DirectShowCapture///" + lsymbolicLinkString;
										
					*aPtrString = SysAllocString(lsymbolicLinkString.c_str());

					VariantClear(&lVariant);

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT DirectShowVideoInputCaptureProcessor::getStreamAmount(
				UINT32* aPtrStreamAmount)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrStreamAmount);

					*aPtrStreamAmount = mStreamAmount;

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT DirectShowVideoInputCaptureProcessor::getMediaTypes(
				UINT32 aStreamIndex,
				BSTR* aPtrStreamNameString,
				IUnknown*** aPtrPtrPtrMediaType,
				UINT32* aPtrMediaTypeCount)
			{
				HRESULT lresult(E_FAIL);

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrMediaTypeCount);

					LOG_CHECK_PTR_MEMORY(aPtrPtrPtrMediaType);

					LOG_CHECK_STATE(aStreamIndex > 0);
										
					CComPtrCustom<ICaptureGraphBuilder2> lCaptureGraph;

					LOG_INVOKE_FUNCTION(lCaptureGraph.CoCreateInstance, __uuidof(CLSID_CaptureGraphBuilder2Proxy));

					LOG_CHECK_PTR_MEMORY(lCaptureGraph);

					CComPtrCustom<IGraphBuilder> lGraphBuilder;

					LOG_INVOKE_FUNCTION(lGraphBuilder.CoCreateInstance, __uuidof(CLSID_FilterGraphProxy));

					LOG_CHECK_PTR_MEMORY(lGraphBuilder);

					LOG_INVOKE_POINTER_METHOD(lCaptureGraph, SetFiltergraph,
						lGraphBuilder);								
					
					CComPtrCustom<IBaseFilter> lVideoCaptureFilter;

					CComPtrCustom<IBindCtx> pbc;

					LOG_INVOKE_FUNCTION(CreateBindCtx, NULL, &pbc);

					LOG_INVOKE_POINTER_METHOD(mMoniker, BindToObject,
						pbc,
						nullptr, 
						IID_PPV_ARGS(&lVideoCaptureFilter));

					LOG_CHECK_PTR_MEMORY(lVideoCaptureFilter);

					LOG_INVOKE_POINTER_METHOD(lGraphBuilder, AddFilter,
						lVideoCaptureFilter,
						L"Video Capture");

					std::vector<CComPtrCustom<IMFMediaType>> lMediaTypes;

					do
					{
						CComPtrCustom<IPin> lPin;

						LOG_INVOKE_POINTER_METHOD(lCaptureGraph, FindPin,
							lVideoCaptureFilter,
							PINDIR_OUTPUT,
							&PIN_CATEGORY_CAPTURE,
							&__uuidof(MEDIATYPE_VideoProxy),
							FALSE,
							aStreamIndex,
							&lPin);

						if (lPin != nullptr)
						{
							PIN_INFO lPIN_INFO;

							ZeroMemory(&lPIN_INFO, sizeof(lPIN_INFO));

							HRESULT lr = lPin->QueryPinInfo(&lPIN_INFO);

							if (SUCCEEDED(lr) && lPIN_INFO.achName[0] != L'\0')
							{
								if (aPtrStreamNameString != nullptr)
								{
									*aPtrStreamNameString = SysAllocString(lPIN_INFO.achName);
								}
							}

							getMediaTypes(
								lPin,
								lMediaTypes);
						}

					} while (false);

					do
					{
						CComPtrCustom<IAMCrossbar> lCrossbar;

						LOG_INVOKE_POINTER_METHOD(lCaptureGraph, FindInterface,
							&LOOK_UPSTREAM_ONLY,
							nullptr,//&__uuidof(MEDIATYPE_VideoProxy),
							lVideoCaptureFilter,
							IID_PPV_ARGS(&lCrossbar)
							);

						LOG_CHECK_PTR_MEMORY(lCrossbar);
						
						auto lfindItr = mOutputInputPinMap.find(aStreamIndex);

						LOG_CHECK_STATE(lfindItr == mOutputInputPinMap.end());


						std::vector<CComPtrCustom<IMFMediaType>> lPhysicalTypedMediaTypes;
																		
						for (auto& lPhysicalType : (*lfindItr).second)
						{
							for (auto& lItem : lMediaTypes)
							{
								CComPtrCustom<IMFMediaType> lMediaType;

								LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lMediaType);

								HRESULT lr = lItem->CopyAllItems(lMediaType);
								
								LOG_INVOKE_MF_METHOD(SetUINT32, lMediaType,
									DirectShowPhysicalType,
									lPhysicalType);

								if (SUCCEEDED(lr))
									lPhysicalTypedMediaTypes.push_back(lMediaType);
							}
						}

						lMediaTypes.clear();

						lMediaTypes = lPhysicalTypedMediaTypes;

					} while (false);

					lresult = S_OK;

					if (lMediaTypes.empty())
					{
						*aPtrMediaTypeCount = 0;
					}
					else
					{
						auto lPtrVoid = CoTaskMemAlloc(sizeof(IUnknown*)* lMediaTypes.size());

						LOG_CHECK_PTR_MEMORY(lPtrVoid);

						IUnknown** lPtrPtrMediaTypes = (IUnknown **)(lPtrVoid);

						LOG_CHECK_PTR_MEMORY(lPtrPtrMediaTypes);

						UINT32 lMediaTypeCount = 0;

						for (auto& lItem : lMediaTypes)
						{
							lPtrPtrMediaTypes[lMediaTypeCount++] = lItem.detach();
						}
						
						*aPtrPtrPtrMediaType = lPtrPtrMediaTypes;

						*aPtrMediaTypeCount = lMediaTypeCount;
					}


					CComPtrCustom<IEnumFilters> lEnumFilters;

					lGraphBuilder->EnumFilters(&lEnumFilters);

					if (lEnumFilters)
					{
						ULONG lFilterIndex = 0;

						CComPtrCustom<IBaseFilter> lBaseFilter;

						while (lEnumFilters->Next(lFilterIndex++, &lBaseFilter, nullptr) == S_OK)
						{
							lGraphBuilder->RemoveFilter(lBaseFilter);

							lBaseFilter.Release();
						}
					}

				} while (false);

				return lresult;
			}

			HRESULT DirectShowVideoInputCaptureProcessor::setCurrentMediaType(
				UINT32 aStreamIndex,
				IUnknown* aPtrMediaType)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					if (mState == SourceState::SourceStateStarted ||
						mState == SourceState::SourceStatePaused)
					{
						lresult = MF_E_MEDIA_SOURCE_WRONGSTATE;
					}

					LOG_CHECK_PTR_MEMORY(aPtrMediaType);

					LOG_CHECK_STATE(aStreamIndex > 0);

					CComPtrCustom<IMFMediaType> lCurrentMediaType;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrMediaType, &lCurrentMediaType);

					LOG_CHECK_PTR_MEMORY(lCurrentMediaType);


					BOOL lRes = FALSE;

					LOG_INVOKE_FUNCTION(checkMediaType,
						lCurrentMediaType, 
						&lRes);

					LOG_CHECK_STATE(lRes == FALSE);

					mCurrentMediaType = lCurrentMediaType;

					mSelectedStreamIndex = aStreamIndex;

				} while (false);

				return lresult;
			}


			HRESULT DirectShowVideoInputCaptureProcessor::getNewSample(
				DWORD aStreamIdentifier,
				IUnknown** aPtrPtrSample)
			{
				HRESULT lresult = E_FAIL;

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrPtrSample);

					LOG_CHECK_STATE(aStreamIdentifier > 0);

					CComPtrCustom<IMFSample> lSample;

					LOG_INVOKE_MF_FUNCTION(MFCreateSample, &lSample);


					LONGLONG lSampleTime;

					LONGLONG lSampleDuration;

					CComPtrCustom<IMFMediaBuffer> lMediaBuffer;

					{

						if (!mBufferContainer.getBuffer(
							&lMediaBuffer,
							&lSampleTime,
							&lSampleDuration))
						{
							std::unique_lock<std::mutex> lLock(mAccessBufferQueueMutex);

							auto lconditionResult = mNewFrameCondition.wait_for(
								lLock,
								std::chrono::seconds(5));

							LOG_CHECK_STATE_DESCR(lconditionResult == std::cv_status::timeout, CONTEXT_E_SYNCH_TIMEOUT);

							LOG_CHECK_STATE_DESCR(!mBufferContainer.getBuffer(&lMediaBuffer, &lSampleTime, &lSampleDuration), E_FAIL);

							LOG_CHECK_PTR_MEMORY(lMediaBuffer);
						}

					}

					LOG_INVOKE_MF_METHOD(AddBuffer, lSample, lMediaBuffer);

					LOG_INVOKE_MF_METHOD(SetSampleDuration, lSample, lSampleDuration);

					LOG_INVOKE_MF_METHOD(SetSampleTime, lSample, lSampleTime);

					LOG_INVOKE_MF_METHOD(SetUINT64, lSample, MFSampleExtension_DeviceTimestamp, lSampleTime);

					LOG_INVOKE_MF_METHOD(SetUINT32, lSample, MFSampleExtension_BottomFieldFirst, TRUE);

					LOG_INVOKE_MF_METHOD(SetUINT32, lSample, MFSampleExtension_CleanPoint, TRUE);

					LOG_INVOKE_MF_METHOD(SetUINT32, lSample, MFSampleExtension_Interlaced, TRUE);

					LOG_INVOKE_MF_METHOD(SetUINT32, lSample, MFSampleExtension_Discontinuity, mDiscontinuity);

					mDiscontinuity = FALSE;

					std::lock_guard<std::mutex> lLock(mAccessMutex);

					if (mState == SourceState::SourceStateStopped)
					{
						lresult = E_FAIL;

						break;
					}

					*aPtrPtrSample = lSample.Detach();

				} while (false);

				return lresult;
			}

			class SnapSampleGrabberCallback : public BaseUnknown<ISampleGrabberCB>
			{
				IMediaEvent* mIMediaEvent = nullptr;

			public:

				SnapSampleGrabberCallback(IMediaEvent* aIMediaEvent)
				:mIMediaEvent(aIMediaEvent)
				{}

				~SnapSampleGrabberCallback() {}

				STDMETHODIMP SampleCB(double Time, IMediaSample* pSample)
				{
					return S_OK;
				}

				STDMETHODIMP BufferCB(double Time, BYTE* pBuffer, long BufferLen)
				{
					if (mIMediaEvent != nullptr)
						mIMediaEvent->PostEvent(
							CM_SnapMediaEvent,
							GUID_NULL,
							S_OK,
							nullptr);

					return S_OK;
				}
			};

			HRESULT DirectShowVideoInputCaptureProcessor::start()
			{
				HRESULT lresult(E_FAIL);

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);
					
					if (mState == SourceState::SourceStateStarted)
					{
						lresult = S_OK;

						break;
					}

					if (mState == SourceState::SourceStateUninitialized ||
						mState == SourceState::SourceStateStopped)
					{
						LOG_INVOKE_FUNCTION(init);
					}

					LOG_CHECK_PTR_MEMORY(mCaptureGraph);

					LOG_CHECK_PTR_MEMORY(mMediaControl);

					mMediaControl->Run();

					mState = SourceState::SourceStateStarted;
					
				} while (false);

				return S_OK;
			}

			HRESULT DirectShowVideoInputCaptureProcessor::stop()
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					if (mState == SourceState::SourceStatePaused ||
						mState == SourceState::SourceStateStarted)
					{
						LOG_CHECK_PTR_MEMORY(mCaptureGraph);

						LOG_CHECK_PTR_MEMORY(mMediaControl);

						LOG_INVOKE_POINTER_METHOD(mMediaControl, 
							Stop);
						
						clearMediaBuffer();

						CComPtrCustom<IGraphBuilder> lGraphBuilder;

						mCaptureGraph->GetFiltergraph(&lGraphBuilder);																

						
						if (lGraphBuilder)
						{
							CComPtrCustom<IEnumFilters> lEnumFilters;

							lGraphBuilder->EnumFilters(&lEnumFilters);

							if (lEnumFilters)
							{
								ULONG lFilterIndex = 0;

								CComPtrCustom<IBaseFilter> lBaseFilter;

								while (lEnumFilters->Next(lFilterIndex++, &lBaseFilter, nullptr) == S_OK)
								{
									lGraphBuilder->RemoveFilter(lBaseFilter);

									lBaseFilter.Release();
								}
							}
						}

						mState = SourceState::SourceStateStopped;						
					}					

				} while (false);

				return lresult;
			}

			HRESULT DirectShowVideoInputCaptureProcessor::pause()
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					if (mState == SourceState::SourceStateStarted)
					{
						LOG_CHECK_PTR_MEMORY(mCaptureGraph);

						LOG_CHECK_PTR_MEMORY(mMediaControl);

						LOG_INVOKE_POINTER_METHOD(mMediaControl,
							Pause);

						clearMediaBuffer();

						mState = SourceState::SourceStatePaused;
					}

				} while (false);

				return lresult;
			}

			HRESULT DirectShowVideoInputCaptureProcessor::restart()
			{
				HRESULT lresult = S_OK;

				do
				{
					LOG_INVOKE_FUNCTION(stop);

					LOG_INVOKE_FUNCTION(start);

				} while (false);

				return lresult;
			}

			HRESULT DirectShowVideoInputCaptureProcessor::shutdown()
			{
				HRESULT lresult(S_OK);

				std::lock_guard<std::mutex> lLock(mAccessMutex);
				
				if (mCaptureGraph != nullptr)
					mCaptureGraph.Release();

				if (mMediaControl != nullptr)
					mMediaControl.Release();

				clearMediaBuffer();

				mState = SourceState::SourceStateUninitialized;

				return lresult;
			}

			HRESULT DirectShowVideoInputCaptureProcessor::registerMediaEvent(IUnknown* pUnkIMediaEvent)
			{
				if (pUnkIMediaEvent != nullptr)
				{

					mSnapSampleGrabberCB.Release();

					CComQIPtrCustom<IMediaEvent> mIMediaEvent(pUnkIMediaEvent);

					if (mIMediaEvent)
					{
						CComPtrCustom<ISampleGrabberCB> lSnapSampleGrabberCB(new SnapSampleGrabberCallback(mIMediaEvent));

						mSnapSampleGrabberCB = lSnapSampleGrabberCB;
					}
				}

				return S_OK;
			}


			//ISampleGrabberCB interface

			HRESULT STDMETHODCALLTYPE DirectShowVideoInputCaptureProcessor::SampleCB(
				double aSampleTime,
				IMediaSample* aPtrSample)
			{
				return E_NOTIMPL;
			}

			HRESULT STDMETHODCALLTYPE DirectShowVideoInputCaptureProcessor::BufferCB(
				double aSampleTime,
				BYTE* aPtrBuffer,
				long aBufferLen)
			{
				HRESULT lresult(E_FAIL);

				do
				{

					CComPtrCustom<IMFMediaBuffer> lMediaBuffer;

					LOG_INVOKE_MF_FUNCTION(MFCreateMemoryBuffer,
						aBufferLen,
						&lMediaBuffer);

					BYTE* lPtrBuffer = NULL;

					DWORD lMaxLength;

					DWORD lCurrentLength;

					MFTIME lCurrentTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

					LOG_INVOKE_MF_METHOD(Lock, lMediaBuffer,
						&lPtrBuffer,
						&lMaxLength,
						&lCurrentLength);

					memcpy(lPtrBuffer, aPtrBuffer, aBufferLen);

					//LOG_INVOKE_MF_FUNCTION(MFCopyImage,
					//	lPtrBuffer,
					//	mStride,
					//	mPtrBits,
					//	mStride,
					//	mStride,
					//	mHeight);


					LOG_INVOKE_MF_METHOD(Unlock, lMediaBuffer);

					LOG_INVOKE_MF_METHOD(SetCurrentLength, lMediaBuffer, aBufferLen);

					mBufferContainer.setBuffer(
						lMediaBuffer.detach(),
						lCurrentTime,
						mDuration);

					mNewFrameCondition.notify_all();

				} while (false);

				return lresult;
			}
			
			HRESULT DirectShowVideoInputCaptureProcessor::init()
			{
				HRESULT lresult(E_FAIL);

				AM_MEDIA_TYPE* lPtrAM_MEDIA_TYPE = nullptr;

				do
				{

					LOG_CHECK_PTR_MEMORY(mCurrentMediaType);

					LOG_CHECK_PTR_MEMORY(mMoniker);

					

					UINT64 luint64;

					LOG_INVOKE_MF_METHOD(GetUINT64, mCurrentMediaType,
						MF_MT_FRAME_RATE,
						&luint64);

					UINT32 lNumenator;

					UINT32 lDenomenator;

					Unpack2UINT32AsUINT64(luint64, &lNumenator, &lDenomenator);
					

					LOG_INVOKE_MF_FUNCTION(MFFrameRateToAverageTimePerFrame,
						lNumenator,
						lDenomenator,
						&mDuration);


					UINT32 lPhysicalType = 0;

					do
					{

						LOG_INVOKE_MF_METHOD(GetUINT32, mCurrentMediaType,
							DirectShowPhysicalType,
							&lPhysicalType);

					} while (false);

					bool luseCrossbar = SUCCEEDED(lresult);


					mCaptureGraph.Release();

					mMediaControl.Release();

					CComPtrCustom<ICaptureGraphBuilder2> lCaptureGraph;

					LOG_INVOKE_FUNCTION(lCaptureGraph.CoCreateInstance, __uuidof(CLSID_CaptureGraphBuilder2Proxy));

					LOG_CHECK_PTR_MEMORY(lCaptureGraph);

					CComPtrCustom<IGraphBuilder> lGraphBuilder;

					LOG_INVOKE_FUNCTION(lGraphBuilder.CoCreateInstance, __uuidof(CLSID_FilterGraphProxy));

					LOG_CHECK_PTR_MEMORY(lGraphBuilder);
					
					CComPtrCustom<IMediaControl> lMediaControl;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lGraphBuilder, &lMediaControl);

					LOG_CHECK_PTR_MEMORY(lMediaControl);

					LOG_INVOKE_POINTER_METHOD(lCaptureGraph, SetFiltergraph,
						lGraphBuilder);
					
					CComPtrCustom<IBaseFilter> lVideoCaptureFilter;

					CComPtrCustom<IBindCtx> pbc;

					LOG_INVOKE_FUNCTION(CreateBindCtx, NULL, &pbc);

					LOG_INVOKE_POINTER_METHOD(mMoniker, BindToObject,
						pbc,
						nullptr,
						IID_PPV_ARGS(&lVideoCaptureFilter));

					LOG_CHECK_PTR_MEMORY(lVideoCaptureFilter);
					
					LOG_INVOKE_POINTER_METHOD(lGraphBuilder, AddFilter,
						lVideoCaptureFilter, 
						L"Video Capture");
					
					CComPtrCustom<IPin> lPin;

					LOG_INVOKE_POINTER_METHOD(lCaptureGraph, FindPin,
						lVideoCaptureFilter,
						PINDIR_OUTPUT,
						&PIN_CATEGORY_CAPTURE,
						&__uuidof(MEDIATYPE_VideoProxy),
						FALSE,
						mSelectedStreamIndex,
						&lPin);

					if (lPin != nullptr)
					{
						lPin->EndOfStream();

						lPin->Disconnect();
					}


					if (luseCrossbar)
					{

						CComPtrCustom<IAMCrossbar> lCrossbar;

						LOG_INVOKE_POINTER_METHOD(lCaptureGraph, FindInterface,
							&LOOK_UPSTREAM_ONLY,
							&__uuidof(MEDIATYPE_VideoProxy),
							lVideoCaptureFilter,
							IID_PPV_ARGS(&lCrossbar)
							);

						LOG_CHECK_PTR_MEMORY(lCrossbar);

						route(lCrossbar, lPhysicalType);
					}
					
					CComPtrCustom<IBaseFilter> lSampleGrabberFilter;

					LOG_INVOKE_FUNCTION(lSampleGrabberFilter.CoCreateInstance, __uuidof(CLSID_SampleGrabberProxy));
					
					CComPtrCustom<ISampleGrabber> lSampleGrabber;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lSampleGrabberFilter, &lSampleGrabber);

					LOG_CHECK_PTR_MEMORY(lSampleGrabber);

					LOG_INVOKE_POINTER_METHOD(lSampleGrabber, SetBufferSamples,
						TRUE);

					LOG_INVOKE_POINTER_METHOD(lGraphBuilder, AddFilter,
						lSampleGrabberFilter, 
						L"Sample Grabber");



					CComPtrCustom<IBaseFilter> lSnapSampleGrabberFilter;

					LOG_INVOKE_FUNCTION(lSnapSampleGrabberFilter.CoCreateInstance, __uuidof(CLSID_SampleGrabberProxy));

					CComPtrCustom<ISampleGrabber> lSnapSampleGrabber;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lSnapSampleGrabberFilter, &lSnapSampleGrabber);

					LOG_CHECK_PTR_MEMORY(lSnapSampleGrabber);

					LOG_INVOKE_POINTER_METHOD(lSnapSampleGrabber, SetBufferSamples,
						TRUE);

					LOG_INVOKE_POINTER_METHOD(lSnapSampleGrabber, SetOneShot,
						FALSE);

					LOG_INVOKE_POINTER_METHOD(lGraphBuilder, AddFilter,
						lSnapSampleGrabberFilter,
						L"Snap Sample Grabber");
					
					
					CComPtrCustom<IBaseFilter> lNullRendererFilter;

					LOG_INVOKE_FUNCTION(lNullRendererFilter.CoCreateInstance, __uuidof(CLSID_NullRendererProxy));

					LOG_INVOKE_POINTER_METHOD(lGraphBuilder, AddFilter,
						lNullRendererFilter,
						L"NullRenderer");


					CComPtrCustom<IBaseFilter> lSnapNullRendererFilter;

					LOG_INVOKE_FUNCTION(lSnapNullRendererFilter.CoCreateInstance, __uuidof(CLSID_NullRendererProxy));

					LOG_INVOKE_POINTER_METHOD(lGraphBuilder, AddFilter,
						lSnapNullRendererFilter,
						L"SnapNullRenderer");


					BOOL lcheck = FALSE;
					
					LOG_INVOKE_FUNCTION(checkMediaType,
						mCurrentMediaType, 
						&lcheck, 
						&lPtrAM_MEDIA_TYPE);

					LOG_CHECK_STATE(lcheck == FALSE);

					LOG_CHECK_PTR_MEMORY(lPtrAM_MEDIA_TYPE);
					
					CComPtrCustom<IAMStreamConfig> lVideoCaptureStreamConfig;

					LOG_INVOKE_POINTER_METHOD(lCaptureGraph, FindInterface,
						&__uuidof(PIN_CATEGORY_CAPTUREProxy),
						&__uuidof(MEDIATYPE_VideoProxy),
						lVideoCaptureFilter,
						IID_PPV_ARGS(&lVideoCaptureStreamConfig));

					LOG_CHECK_PTR_MEMORY(lVideoCaptureStreamConfig);
					
					LOG_INVOKE_POINTER_METHOD(lVideoCaptureStreamConfig, SetFormat,
						lPtrAM_MEDIA_TYPE);				
																				
					lVideoCaptureStreamConfig.Release();
										
					LOG_INVOKE_POINTER_METHOD(lCaptureGraph, RenderStream,
						&__uuidof(PIN_CATEGORY_CAPTUREProxy), 
						&__uuidof(MEDIATYPE_VideoProxy),
						lVideoCaptureFilter, 
						lSampleGrabberFilter, 
						lNullRendererFilter);

					LOG_INVOKE_POINTER_METHOD(lSampleGrabber, SetCallback,
						this,
						1);

					do
					{
						if (mSnapSampleGrabberCB)
						{
							LOG_INVOKE_POINTER_METHOD(lCaptureGraph, RenderStream,
								&__uuidof(PIN_CATEGORY_STILLProxy),
								&__uuidof(MEDIATYPE_VideoProxy),
								lVideoCaptureFilter,
								lSnapSampleGrabberFilter,
								lSnapNullRendererFilter);

							LOG_INVOKE_POINTER_METHOD(lSnapSampleGrabber, SetCallback,
								mSnapSampleGrabberCB,
								1);
						}

					} while (false);

					lresult = S_OK;
															
					mCaptureGraph = lCaptureGraph; 
					
					mMediaControl = lMediaControl;


				} while (false);

				if (lPtrAM_MEDIA_TYPE != nullptr)
					CoTaskMemFree(lPtrAM_MEDIA_TYPE);

				return lresult;
			}

			HRESULT DirectShowVideoInputCaptureProcessor::checkMediaType(IMFMediaType* aPtrMediaType, BOOL *aPtrBool, AM_MEDIA_TYPE** aPtrPtrAM_MEDIA_TYPE)
			{

				HRESULT lresult(E_FAIL);

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrMediaType);

					LOG_CHECK_PTR_MEMORY(aPtrBool);


					CComPtrCustom<ICaptureGraphBuilder2> lCaptureGraph;

					LOG_INVOKE_FUNCTION(lCaptureGraph.CoCreateInstance, __uuidof(CLSID_CaptureGraphBuilder2Proxy));

					LOG_CHECK_PTR_MEMORY(lCaptureGraph);

					CComPtrCustom<IGraphBuilder> lGraphBuilder;

					LOG_INVOKE_FUNCTION(lGraphBuilder.CoCreateInstance, __uuidof(CLSID_FilterGraphProxy));

					LOG_CHECK_PTR_MEMORY(lGraphBuilder);

					LOG_INVOKE_POINTER_METHOD(lCaptureGraph, SetFiltergraph,
						lGraphBuilder);

					CComPtrCustom<IBaseFilter> lVideoCaptureFilter;

					CComPtrCustom<IBindCtx> pbc;

					LOG_INVOKE_FUNCTION(CreateBindCtx, NULL, &pbc);

					LOG_INVOKE_POINTER_METHOD(mMoniker, BindToObject,
						pbc,
						nullptr,
						IID_PPV_ARGS(&lVideoCaptureFilter));

					LOG_CHECK_PTR_MEMORY(lVideoCaptureFilter);

					CComPtrCustom<IAMStreamConfig> lVideoCaptureStreamConfig;

					LOG_INVOKE_POINTER_METHOD(lCaptureGraph, FindInterface,
						&__uuidof(PIN_CATEGORY_CAPTUREProxy),
						&__uuidof(MEDIATYPE_VideoProxy),
						lVideoCaptureFilter,
						IID_PPV_ARGS(&lVideoCaptureStreamConfig));

					LOG_CHECK_PTR_MEMORY(lVideoCaptureStreamConfig);

					int lStreamCapCount = 0;
					int lConfigCapSize = 0;

					LOG_INVOKE_POINTER_METHOD(lVideoCaptureStreamConfig, GetNumberOfCapabilities,
						&lStreamCapCount,
						&lConfigCapSize);

					LOG_CHECK_STATE_DESCR(lStreamCapCount == 0, E_FAIL);

					decltype(lStreamCapCount) lMediaTypeCount = 0;
										
					for (int lindex = 0; lindex < lStreamCapCount; lindex++)
					{
						AM_MEDIA_TYPE* lPtrMediaType = nullptr;

						std::unique_ptr<BYTE> lbyteBuffer(new BYTE[lConfigCapSize]);

						LOG_INVOKE_POINTER_METHOD(lVideoCaptureStreamConfig, GetStreamCaps,
							lindex,
							&lPtrMediaType,
							lbyteBuffer.get());

						if (lPtrMediaType != nullptr)
						{
							if (lPtrMediaType->majortype == __uuidof(MEDIATYPE_VideoProxy))
							{


								CComPtrCustom<IMFMediaType> lMediaType;

								LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,
									&lMediaType);

								LOG_INVOKE_MF_FUNCTION(MFInitMediaTypeFromAMMediaType,
									lMediaType,
									lPtrMediaType);
																
								BOOL lres = FALSE;

								LOG_INVOKE_MF_METHOD(Compare, lMediaType,
									aPtrMediaType,
									MF_ATTRIBUTES_MATCH_INTERSECTION,
									//MF_ATTRIBUTES_MATCH_ALL_ITEMS,
									&lres);

								if (lres == TRUE)
								{
									*aPtrBool = lres;

									if (aPtrPtrAM_MEDIA_TYPE != nullptr)
									{
										*aPtrPtrAM_MEDIA_TYPE = lPtrMediaType;

										lPtrMediaType = nullptr;
									}

									break;
								}
								
							}
							if (lPtrMediaType != nullptr)
								CoTaskMemFree(lPtrMediaType);
						}

					}
					
				} while (false);

				return lresult;
			}


			HRESULT DirectShowVideoInputCaptureProcessor::route(
				IAMCrossbar* aPtrIAMCrossbar,
				UINT32 aPhysicalType)
			{

				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrIAMCrossbar);
					LOG_CHECK_PTR_MEMORY(mCurrentMediaType);
					LOG_CHECK_STATE(mOutputInputPinMap.empty());



					LONG lInputPins = 0;

					LONG lOutputPins = 0;

					LOG_INVOKE_POINTER_METHOD(aPtrIAMCrossbar, get_PinCounts,
						&lOutputPins,
						&lInputPins);
					
					for (auto& lItem : mOutputInputPinMap)
					{

						for (LONG ilInputindex = 0; ilInputindex < lInputPins; ilInputindex++)
						{
							LONG lPinIndexRelated = 0;
							
							LONG lPhysicalType = 0;

							HRESULT lCheck = aPtrIAMCrossbar->get_CrossbarPinInfo(
								TRUE,
								ilInputindex, 
								&lPinIndexRelated,
								&lPhysicalType);
							
							if (SUCCEEDED(lCheck) && lPhysicalType == aPhysicalType)
							{
								aPtrIAMCrossbar->Route(lItem.first, ilInputindex);

								break;
							}
						}											
					}					
				} while (false);

				return lresult;
			}

			void DirectShowVideoInputCaptureProcessor::BufferContainer::setBuffer(IMFMediaBuffer* aPtrMediaBuffer,
				INT64 aCurrentTime,
				INT64 aCurrentDuration)
			{
				IMFMediaBuffer* lTempMediaBuffer = nullptr;

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					lTempMediaBuffer = mPtrMediaBuffer;

					mPtrMediaBuffer = aPtrMediaBuffer;

					mCurrentTime = aCurrentTime;

					mCurrentDuration = aCurrentDuration;

					IsBufferReady = true;

					if (lTempMediaBuffer != nullptr)
						lTempMediaBuffer->Release();

				} while (false);
			}

			bool DirectShowVideoInputCaptureProcessor::BufferContainer::getBuffer(IMFMediaBuffer** aPtrPtrMediaBuffer,
				INT64* aPtrCurrentTime,
				INT64* aPtrCurrentDuration)
			{
				if (!IsBufferReady)
					return false;

				std::lock_guard<std::mutex> lLock(mAccessMutex);

				*aPtrPtrMediaBuffer = mPtrMediaBuffer;

				mPtrMediaBuffer = nullptr;

				*aPtrCurrentTime = mCurrentTime;

				*aPtrCurrentDuration = mCurrentDuration;

				IsBufferReady = false;

				return true;
			}

			void DirectShowVideoInputCaptureProcessor::BufferContainer::clearBuffer()
			{

				std::lock_guard<std::mutex> lLock(mAccessMutex);

				if (mPtrMediaBuffer != nullptr)
					mPtrMediaBuffer->Release();

				mPtrMediaBuffer = nullptr;

				IsBufferReady = false;
			}

			void DirectShowVideoInputCaptureProcessor::clearMediaBuffer()
			{
				std::lock_guard<std::mutex> lLock(mAccessBufferQueueMutex);

				mBufferContainer.clearBuffer();
			}
						
			HRESULT DirectShowVideoInputCaptureProcessor::getMediaTypes(IPin* aPtrPin,
				std::vector<CComPtrCustom<IMFMediaType>>& aMediaTypes)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPin);

					CComPtrCustom<IEnumMediaTypes> lEnum;

					LOG_INVOKE_POINTER_METHOD(aPtrPin, EnumMediaTypes,
						&lEnum);

					AM_MEDIA_TYPE* lPtrMediaType = nullptr;

					aMediaTypes.clear();
					
					while (SUCCEEDED(lEnum->Next(1, &lPtrMediaType, 0)))
					{
						if (lPtrMediaType != nullptr)
						{
							if (lPtrMediaType->majortype == __uuidof(MEDIATYPE_VideoProxy))
							{

								CComPtrCustom<IMFMediaType> lMediaType;

								LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,
									&lMediaType);

								LOG_INVOKE_MF_FUNCTION(MFInitMediaTypeFromAMMediaType,
									lMediaType,
									lPtrMediaType);

								UINT32 lState = FALSE;

								LOG_INVOKE_MF_METHOD(GetUINT32, lMediaType,
									MF_MT_ALL_SAMPLES_INDEPENDENT,
									&lState);
								
								if (lState == TRUE)
								{
									BOOL lCompareState(FALSE);

									for (auto& lItem : aMediaTypes)
									{
										CComPtrCustom<IMFMediaType> lTempMediaType;

										if (lItem)
										{
											lItem->QueryInterface(IID_PPV_ARGS(&lTempMediaType));
										}

										if (lTempMediaType)
										{
											auto lcompRes = lTempMediaType->Compare(
												lMediaType,
												MF_ATTRIBUTES_MATCH_TYPE::MF_ATTRIBUTES_MATCH_ALL_ITEMS,
												&lCompareState);

											if (SUCCEEDED(lcompRes) && lCompareState == TRUE)
											{
												break;
											}
										}
									}

									if (lCompareState == FALSE)
									{
										aMediaTypes.push_back(lMediaType);
									}

								}
							}

							DirectShowCommon::_DeleteMediaType(lPtrMediaType);
						}
						else
						{
							break;
						}

						lPtrMediaType = nullptr;
					}

				} while (false);

				return lresult;
			}

		}
	}
}