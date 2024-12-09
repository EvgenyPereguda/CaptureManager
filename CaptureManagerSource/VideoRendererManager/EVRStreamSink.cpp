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

#include "EVRStreamSink.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/GUIDs.h"
#include "../Common/Common.h"


namespace CaptureManager
{
	namespace Sinks
	{
		namespace EVR
		{
			using namespace Core;
						
			EVRStreamSink::EVRStreamSink(
				DWORD aStreamID, 
				DWORD aMixerStreamID) :
				mStreamID(aStreamID),
				mMixerStreamID(aMixerStreamID),
				mWorkQueueId(0),
				mIsShutdown(false)
			{
			}

			EVRStreamSink::~EVRStreamSink()
			{
			}

			HRESULT EVRStreamSink::initialize(
				IMFMediaSink* aPtrMediaSink,
				bool aIsSingleStream,
				IPresenter* aPtrPresenter)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrMediaSink);

					LOG_CHECK_PTR_MEMORY(aPtrPresenter);
					
					mMediaSink = aPtrMediaSink;

					mIsSingleStream = aIsSingleStream;

					mPresenter.Release();
										
					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrPresenter, &mPresenter);
					
					LOG_CHECK_PTR_MEMORY(mPresenter);
										
					if (!mMixer)
					{
						CComPtrCustom<IMFGetService> lGetService;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mMediaSink, &lGetService);

						LOG_CHECK_PTR_MEMORY(lGetService);

						LOG_INVOKE_POINTER_METHOD(lGetService, GetService,
							MR_VIDEO_MIXER_SERVICE,
							IID_PPV_ARGS(&mMixer));

						LOG_CHECK_PTR_MEMORY(mMixer);
					}
					
					LOG_INVOKE_MF_FUNCTION(MFCreateEventQueue,
						&mEventQueue);

					LOG_INVOKE_MF_FUNCTION(MFAllocateWorkQueueEx,
						_MF_STANDARD_WORKQUEUE,
						&mWorkQueueId);
					
				} while (false);
				
				return lresult;
			}

			HRESULT EVRStreamSink::createEVRStreamSink(
				DWORD aStreamID,
				IMFMediaSink* aPtrMediaSink,
				IPresenter* aPtrPresenter,
				DWORD aMixerStreamID,
				bool aIsSingleStream,
				IStreamSink** aPtrPtrStreamSink)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrStreamSink);

					CComPtrCustom<EVRStreamSink> lEVRStreamSink = new (std::nothrow)EVRStreamSink(aStreamID, aMixerStreamID);

					LOG_CHECK_PTR_MEMORY(lEVRStreamSink);

					LOG_INVOKE_POINTER_METHOD(lEVRStreamSink, initialize,
						aPtrMediaSink,
						aIsSingleStream,
						aPtrPresenter);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lEVRStreamSink, aPtrPtrStreamSink);

				} while (false);

				return lresult;
			}

			HRESULT EVRStreamSink::createVideoAllocator(
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
							MR_VIDEO_ACCELERATION_SERVICE,
							IID_PPV_ARGS(&lVideoSampleAllocator));

						LOG_CHECK_PTR_MEMORY(lVideoSampleAllocator);

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lVideoSampleAllocator, aPtrPtrVideoSampleAllocator);

					} while (false);

					if (FAILED(lresult))
					{
						CComPtrCustom<IMFVideoSampleAllocatorEx> lVideoSampleAllocatorEx;

						LOG_INVOKE_POINTER_METHOD(lGetService, GetService,
							MR_VIDEO_ACCELERATION_SERVICE,
							IID_PPV_ARGS(&lVideoSampleAllocatorEx));

						LOG_CHECK_PTR_MEMORY(lVideoSampleAllocatorEx);

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lVideoSampleAllocatorEx, aPtrPtrVideoSampleAllocator);
					}
					
				} while (false);

				return lresult;
			}

			HRESULT EVRStreamSink::checkShutdown() const
			{
				if (mIsShutdown)
				{
					return MF_E_SHUTDOWN;
				}
				else
				{
					return S_OK;
				}
			}	
			


			// IMFClockStateSink methods
			HRESULT EVRStreamSink::OnClockStart(
				MFTIME aHNSSystemTime,
				LONGLONG aClockStartOffset)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					std::lock_guard<std::mutex> lLock(mClockStateAccessMutex);

					LOG_INVOKE_FUNCTION(checkShutdown);
					
					mStreamState = Started;
					
					LOG_INVOKE_MF_FUNCTION(MFPutWorkItem,
						mWorkQueueId,
						this,
						nullptr);

				} while (false);

				return lresult;
			}

			HRESULT EVRStreamSink::OnClockStop(
				MFTIME aHNSSystemTime)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					std::lock_guard<std::mutex> lLock(mClockStateAccessMutex);

					LOG_INVOKE_FUNCTION(checkShutdown);

					if (mStreamState != Started &&
						mStreamState != Paused)
					{
						lresult = S_OK;

						break;
					}
					
					mStreamState = Stoped;
					
					LOG_INVOKE_MF_METHOD(QueueEvent, this,
						MEStreamSinkStopped,
						GUID_NULL,
						lresult,
						NULL);

				} while (false);

				return lresult;
			}

			HRESULT EVRStreamSink::OnClockPause(
				MFTIME aHNSSystemTime)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					std::lock_guard<std::mutex> lLock(mClockStateAccessMutex);

					LOG_INVOKE_FUNCTION(checkShutdown);

					if (mStreamState != Started)
					{
						lresult = S_OK;

						break;
					}

					LOG_CHECK_PTR_MEMORY(this);

					mStreamState = Paused;

					LOG_INVOKE_MF_METHOD(QueueEvent, this,
						MEStreamSinkPaused,
						GUID_NULL,
						lresult,
						NULL);

				} while (false);

				return lresult;
			}

			HRESULT EVRStreamSink::OnClockRestart(
				MFTIME aHNSSystemTime)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					lresult = OnClockStart(aHNSSystemTime, 0);

				} while (false);

				return lresult;
			}

			HRESULT EVRStreamSink::OnClockSetRate(
				MFTIME aHNSSystemTime,
				float aRate)
			{
				HRESULT lresult(E_FAIL);

				do
				{

				} while (false);

				return lresult;
			}

			// IMFMediaTypeHandler implementation
			HRESULT EVRStreamSink::IsMediaTypeSupported(
				IMFMediaType* aPtrMediaType,
				IMFMediaType** aPtrPtrMediaType)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrMediaType);

					if (aPtrPtrMediaType != nullptr)
						*aPtrPtrMediaType = nullptr;

					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_INVOKE_FUNCTION(checkShutdown);

					GUID lMajorType = GUID_NULL;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					LOG_CHECK_STATE_DESCR(lMajorType != MFMediaType_Video, MF_E_INVALIDMEDIATYPE);
					
					if (!mMixer)
					{
						CComPtrCustom<IMFGetService> lGetService;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mMediaSink, &lGetService);

						LOG_CHECK_PTR_MEMORY(lGetService);

						LOG_INVOKE_POINTER_METHOD(lGetService, GetService,
							MR_VIDEO_MIXER_SERVICE,
							IID_PPV_ARGS(&mMixer));

						LOG_CHECK_PTR_MEMORY(mMixer);
					}

					LOG_INVOKE_FUNCTION(checkShutdown);

					LOG_INVOKE_POINTER_METHOD(mMixer, SetInputType,
						mMixerStreamID,
						aPtrMediaType,
						MFT_SET_TYPE_TEST_ONLY);
										
				} while (false);

				return lresult;
			}

			HRESULT EVRStreamSink::GetMediaTypeCount(
				DWORD* aPtrTypeCount)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrTypeCount);
					
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_INVOKE_FUNCTION(checkShutdown);

					if (mCurrentMediaType)
						*aPtrTypeCount = 1;
					else
						*aPtrTypeCount = 0;

				} while (false);

				return lresult;
			}

			HRESULT EVRStreamSink::GetMediaTypeByIndex(
				DWORD aIndex,
				IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrType);

					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_INVOKE_FUNCTION(checkShutdown);

					if (mCurrentMediaType && aIndex == 0)
					{
						LOG_INVOKE_QUERY_INTERFACE_METHOD(mCurrentMediaType, aPtrPtrType);
					}
					else
					{
						*aPtrPtrType = nullptr;

						lresult = MF_E_INVALIDINDEX;
					}		

				} while (false);

				return lresult;
			}

			HRESULT EVRStreamSink::SetCurrentMediaType(
				IMFMediaType* aPtrMediaType)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrMediaType);
										
					LOG_INVOKE_FUNCTION(IsMediaTypeSupported, aPtrMediaType, nullptr);

					std::lock_guard<std::mutex> lLock(mAccessMutex);

					if (!mMixer)
					{
						CComPtrCustom<IMFGetService> lGetService;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mMediaSink, &lGetService);

						LOG_CHECK_PTR_MEMORY(lGetService);

						LOG_INVOKE_POINTER_METHOD(lGetService, GetService,
							MR_VIDEO_MIXER_SERVICE,
							IID_PPV_ARGS(&mMixer));

						LOG_CHECK_PTR_MEMORY(mMixer);
					}

					LOG_INVOKE_FUNCTION(checkShutdown);

					LOG_INVOKE_POINTER_METHOD(mMixer,SetInputType,
						mMixerStreamID,
						aPtrMediaType,
						MFT_SET_TYPE_TEST_ONLY);
					
					if (mIsSingleStream)
					{

						UINT32 lWidth = 0;

						UINT32 lHeight = 0;

						MFGetAttributeSize(
							aPtrMediaType,
							MF_MT_FRAME_SIZE,
							&lWidth,
							&lHeight);

						MFRatio lframeRate;

						LOG_INVOKE_FUNCTION(MFGetAttributeRatio,
							aPtrMediaType,
							MF_MT_FRAME_RATE,
							(UINT32*)&lframeRate.Numerator,
							(UINT32*)&lframeRate.Denominator);

						mPresenter->initialize(
							lWidth,
							lHeight,
							lframeRate.Numerator,
							lframeRate.Denominator,
							mMixer);

						mPresenter->createDevice();

						mMixer->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, 0);
					}

					LOG_INVOKE_POINTER_METHOD(mMixer, SetInputType,
						mMixerStreamID,
						aPtrMediaType,
						0);

					mCurrentMediaType.Release();

					LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,
						&mCurrentMediaType);

					LOG_CHECK_PTR_MEMORY(mCurrentMediaType);
					
					LOG_INVOKE_MF_METHOD(CopyAllItems,
						aPtrMediaType,
						mCurrentMediaType);

					do
					{

						LOG_INVOKE_MF_METHOD(DeleteItem,
							mCurrentMediaType,
							CM_DeviceManager);

					} while (false);

					lresult = S_OK;
					
				} while (false);

				return lresult;
			}

			HRESULT EVRStreamSink::GetCurrentMediaType(
				IMFMediaType** aPtrPtrMediaType)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrMediaType);

					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_INVOKE_FUNCTION(checkShutdown);

					if (mCurrentMediaType)
					{
						LOG_INVOKE_QUERY_INTERFACE_METHOD(mCurrentMediaType, aPtrPtrMediaType);
					}
					else
					{
						aPtrPtrMediaType = nullptr;

						lresult = MF_E_INVALIDMEDIATYPE;
					}

				} while (false);

				return lresult;
			}

			HRESULT EVRStreamSink::GetMajorType(
				GUID* PtrGUIDMajorType)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(PtrGUIDMajorType);

					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_INVOKE_FUNCTION(checkShutdown);

					*PtrGUIDMajorType = MFMediaType_Video;

				} while (false);

				return lresult;
			}


			/// IMFStreamSink methods
			
			HRESULT EVRStreamSink::Flush()
			{
				HRESULT lresult(E_FAIL);

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_INVOKE_FUNCTION(checkShutdown);
					
				} while (false);

				return lresult;
			}
			
			HRESULT EVRStreamSink::GetIdentifier(
				DWORD* aPtrIdentifier)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrIdentifier);

					std::lock_guard<std::mutex> lLock(mAccessMutex);
					
					*aPtrIdentifier = mStreamID;

					lresult = S_OK;

				} while (false);

				return lresult;
			}
			
			HRESULT EVRStreamSink::GetMediaSink(IMFMediaSink** aPtrPtrMediaSink)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrMediaSink);

					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_INVOKE_FUNCTION(checkShutdown);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mMediaSink, aPtrPtrMediaSink);

				} while (false);

				return lresult;
			}
			
			HRESULT EVRStreamSink::GetMediaTypeHandler(IMFMediaTypeHandler** aPtrPtrHandler)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrHandler);

					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_INVOKE_FUNCTION(checkShutdown);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(this, aPtrPtrHandler);

				} while (false);

				return lresult;
			}
			
			HRESULT EVRStreamSink::PlaceMarker(
				MFSTREAMSINK_MARKER_TYPE aMarkerType,
				const PROPVARIANT* aPtrVarMarkerValue,
				const PROPVARIANT* aPtrVarContextValue)
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{
					
				} while (false);

				return lresult;
			}
			
			HRESULT EVRStreamSink::ProcessSample(
				IMFSample* aPtrSample)
			{

				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrSample);

					LOG_CHECK_PTR_MEMORY(mPresenter);

					LOG_CHECK_PTR_MEMORY(mMixer);

					LOG_INVOKE_FUNCTION(checkShutdown);
									   
					LOG_INVOKE_MF_FUNCTION(MFPutWorkItem,
						mWorkQueueId,
						this,
						aPtrSample);
															
					LOG_INVOKE_MF_METHOD(QueueEvent, this,
						MEStreamSinkRequestSample,
						GUID_NULL,
						lresult,
						NULL);

				} while (false);

				return lresult;
			}


			// IMFMediaEventGenerator methods.
			// Note: These methods call through to the event queue helper object.

			HRESULT EVRStreamSink::BeginGetEvent(
				IMFAsyncCallback* aPtrCallback,
				IUnknown* aPtrUnkState)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_INVOKE_FUNCTION(checkShutdown);

					LOG_CHECK_PTR_MEMORY(aPtrCallback);

					LOG_INVOKE_MF_METHOD(BeginGetEvent, mEventQueue,
						aPtrCallback,
						aPtrUnkState);
					
				} while (false);
				
				return lresult;
			}

			HRESULT EVRStreamSink::EndGetEvent(
				IMFAsyncResult* aPtrResult,
				IMFMediaEvent** aPtrPtrEvent)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_INVOKE_FUNCTION(checkShutdown);

					LOG_CHECK_PTR_MEMORY(aPtrResult);

					LOG_CHECK_PTR_MEMORY(aPtrPtrEvent);

					LOG_INVOKE_MF_METHOD(EndGetEvent, mEventQueue,
						aPtrResult,
						aPtrPtrEvent);

				} while (false);

				return lresult;
			}

			HRESULT EVRStreamSink::GetEvent(
				DWORD aFlags,
				IMFMediaEvent** aPtrPtrEvent)
			{

				HRESULT lresult(E_FAIL);

				do
				{

					CComPtrCustom<IMFMediaEventQueue> lEventQueue;
					
					{
						std::lock_guard<std::mutex> lLock(mAccessMutex);

						LOG_INVOKE_FUNCTION(checkShutdown);

						lEventQueue = mEventQueue;					
					}
					
					LOG_INVOKE_MF_METHOD(GetEvent, lEventQueue,
						aFlags,
						aPtrPtrEvent);

				} while (false);

				return lresult;
			}

			HRESULT EVRStreamSink::QueueEvent(
				MediaEventType aMediaEventType,
				REFGUID aRefGUIDExtendedType,
				HRESULT aHRStatus,
				const PROPVARIANT* aPtrValue)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_INVOKE_FUNCTION(checkShutdown);
					
					LOG_INVOKE_MF_METHOD(QueueEventParamVar, mEventQueue,
						aMediaEventType,
						aRefGUIDExtendedType,
						aHRStatus,
						aPtrValue);

				} while (false);

				return lresult;
			}

			// IStreamSink implementation
			HRESULT EVRStreamSink::getMaxRate(
				BOOL aThin,
				float* aPtrRate)
			{
				HRESULT lresult(E_NOTIMPL);
				
				return lresult;
			}

			HRESULT EVRStreamSink::preroll()
			{
				HRESULT lresult(E_FAIL);

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_INVOKE_FUNCTION(checkShutdown);
					
				} while (false);

				return lresult;
			}

			HRESULT EVRStreamSink::shutdown()
			{
				HRESULT lresult(E_FAIL);

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_CHECK_STATE_DESCR(mIsShutdown, MF_E_SHUTDOWN);						

					mIsShutdown = true;
					
					if (mEventQueue)
					{
						do
						{
							LOG_INVOKE_MF_METHOD(Shutdown, mEventQueue);

						} while (false);
					}

					LOG_INVOKE_MF_FUNCTION(MFUnlockWorkQueue,
						mWorkQueueId);
					
					mMediaSink.Release();

					//mPresenter->Release();

					//mMixer->Release();

				} while (false);

				return lresult;
			}

			// IMFGetService
			STDMETHODIMP EVRStreamSink::GetService(
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
						if (aRefIID == __uuidof(IMFVideoSampleAllocator))
						{
							CComPtrCustom<IMFVideoSampleAllocator> lVideoSampleAllocator;
							
							LOG_INVOKE_FUNCTION(createVideoAllocator,
								&lVideoSampleAllocator);

							LOG_CHECK_PTR_MEMORY(lVideoSampleAllocator);

							LOG_INVOKE_WIDE_QUERY_INTERFACE_METHOD(lVideoSampleAllocator,
								aRefIID,
								aPtrPtrObject);
						}
					}

				} while (false);

				return lresult;
			}

			// IMFAsyncCallback methods
			HRESULT EVRStreamSink::GetParameters(
				DWORD* aPtrFlags,
				DWORD* aPtrQueue)
			{
				return E_NOTIMPL;
			}

			HRESULT EVRStreamSink::Invoke(
				IMFAsyncResult* aPtrAsyncResult)
			{
				HRESULT lresult(S_OK);

				do
				{
					if (mStreamState == Stoped)
						break;

					LOG_CHECK_PTR_MEMORY(aPtrAsyncResult);

					CComPtrCustom<IUnknown> lunkState;

					aPtrAsyncResult->GetState(&lunkState);

					CComQIPtrCustom<IMFSample> lSample(lunkState);

					if (!lSample)
					{
						if (mStreamState == Started)
						{
							LOG_INVOKE_MF_METHOD(QueueEvent, this,
								MEStreamSinkStarted,
								GUID_NULL,
								lresult,
								NULL);

							Sleep(400);

							LOG_INVOKE_MF_METHOD(QueueEvent, this,
								MEStreamSinkRequestSample,
								GUID_NULL,
								lresult,
								NULL);
						}
					}
					else
					{
						if (mProcessMutex.try_lock())
						{
							lresult = mMixer->ProcessInput(
								mMixerStreamID,
								lSample,
								0);

							if(SUCCEEDED(lresult))
								mPresenter->ProcessFrame();

							lresult = S_OK;

							mProcessMutex.unlock();
						}
					}

					lresult = S_OK;

				} while (false);

				return lresult;
			}
		}
	}
}