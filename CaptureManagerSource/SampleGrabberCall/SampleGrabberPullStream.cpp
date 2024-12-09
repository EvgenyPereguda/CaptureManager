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

#include "SampleGrabberPullStream.h"
#include "SampleGrabberPullSink.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/MFHeaders.h"
#include "../Common/Common.h"


namespace CaptureManager
{
	namespace Sinks
	{
		namespace SampleGrabberPull
		{
			using namespace Core;

			SampleGrabberPullStream::SampleGrabberPullStream() :
				//SampleGrabberCall::RegularSampleGrabberCall::ReadWriteBufferRegularSync(aImageByteSize),
				mPtrSink(nullptr)
			{
			}

			SampleGrabberPullStream::~SampleGrabberPullStream()
			{
				if (mEventQueue)
				{
					mEventQueue->Shutdown();
				}
			}

			// IMFShutdown interface implmenentation

			HRESULT STDMETHODCALLTYPE SampleGrabberPullStream::Shutdown()
			{
				HRESULT lresult;

				do
				{
					if (mEventQueue)
					{
						mEventQueue->Shutdown();
					}

					mEventQueue.Release();

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT STDMETHODCALLTYPE SampleGrabberPullStream::GetShutdownStatus(MFSHUTDOWN_STATUS * aPtrStatus)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrStatus);

					*aPtrStatus = MFSHUTDOWN_STATUS::MFSHUTDOWN_COMPLETED;

					lresult = S_OK;

				} while (false);

				return lresult;
			}
						
			// IMFStreamSink interface implementation
			STDMETHODIMP SampleGrabberPullStream::GetMediaSink(
				IMFMediaSink** aPtrPtrMediaSink)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(mPtrSink);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mPtrSink, aPtrPtrMediaSink);

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullStream::GetIdentifier(
				DWORD* aPtrIdentifier)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrIdentifier);

					*aPtrIdentifier = 0;

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullStream::GetMediaTypeHandler(
				IMFMediaTypeHandler** aPtrPtrHandler)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrHandler);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(this, aPtrPtrHandler);

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullStream::ProcessSample(
				IMFSample* aPtrSample)
			{
				class MediaBufferLock
				{
				public:
					MediaBufferLock(
						IMFMediaBuffer* aPtrInputBuffer,
						DWORD& aRefMaxLength,
						DWORD& aRefCurrentLength,
						BYTE** aPtrPtrInputBuffer,
						HRESULT& aRefResult)
					{
						HRESULT lresult;

						do
						{
							LOG_CHECK_PTR_MEMORY(aPtrInputBuffer);

							LOG_CHECK_PTR_MEMORY(aPtrPtrInputBuffer);

							LOG_INVOKE_POINTER_METHOD(aPtrInputBuffer,Lock,
								aPtrPtrInputBuffer,
								&aRefMaxLength,
								&aRefCurrentLength);
							
							LOG_CHECK_PTR_MEMORY(aPtrPtrInputBuffer);

							mInputBuffer = aPtrInputBuffer;

						} while (false);

						aRefResult = lresult;
					}

					~MediaBufferLock()
					{
						if (mInputBuffer)
						{
							mInputBuffer->Unlock();
						}
					}

				private:

					CComPtrCustom<IMFMediaBuffer> mInputBuffer;

					MediaBufferLock(
						const MediaBufferLock&){}

					MediaBufferLock& operator=(
						const MediaBufferLock&){return *this;}

				};
				
				class MediaBufferLock2D
				{
				public:
					MediaBufferLock2D(
						IMF2DBuffer* aPtrInputBuffer,
						LONG* aPtrStride,
						BYTE** aPtrPtrInputBuffer,
						HRESULT& aRefResult)
					{
						HRESULT lresult;

						do
						{
							LOG_CHECK_PTR_MEMORY(aPtrInputBuffer);

							LOG_CHECK_PTR_MEMORY(aPtrPtrInputBuffer);

							LOG_INVOKE_POINTER_METHOD(aPtrInputBuffer, Lock2D,
								aPtrPtrInputBuffer,
								aPtrStride);

							LOG_CHECK_PTR_MEMORY(aPtrPtrInputBuffer);

							mInputBuffer = aPtrInputBuffer;
														
						} while (false);

						aRefResult = lresult;
					}

					~MediaBufferLock2D()
					{
						if (mInputBuffer)
						{
							mInputBuffer->Unlock2D();
						}
					}

				private:

					CComPtrCustom<IMF2DBuffer> mInputBuffer;

					MediaBufferLock2D(
						const MediaBufferLock2D&){}

					MediaBufferLock2D& operator=(
						const MediaBufferLock2D&){
						return *this;
					}

				};

				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrSample);

					LOG_CHECK_PTR_MEMORY(mMediaType);

					GUID lMajorTypeValue;

					LOG_INVOKE_MF_METHOD(GetGUID, 
						mMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorTypeValue);
					
					DWORD lSampleFlags;

					LOG_INVOKE_POINTER_METHOD(aPtrSample, GetSampleFlags, &lSampleFlags);
					
					LONGLONG lSampleTime;

					LOG_INVOKE_POINTER_METHOD(aPtrSample, GetSampleTime, &lSampleTime);
					
					LONGLONG lSampleDuration;

					LOG_INVOKE_POINTER_METHOD(aPtrSample, GetSampleDuration, &lSampleDuration);
					
					DWORD lBufferCount;

					LOG_INVOKE_POINTER_METHOD(aPtrSample, GetBufferCount,
						&lBufferCount);
					
					LOG_CHECK_STATE_DESCR(lBufferCount != 1, E_BOUNDS);

					CComPtrCustom<IMFMediaBuffer> lInputBuffer;

					LOG_INVOKE_POINTER_METHOD(aPtrSample, GetBufferByIndex,
						0,
						&lInputBuffer);
					

					DWORD lMaxLength;

					DWORD lCurrentLength = 0;

					BYTE* lPtrInputBuffer;

					CComPtrCustom<IMF2DBuffer> l2DBuffer;

					lresult = lInputBuffer->QueryInterface(IID_PPV_ARGS(&l2DBuffer));

					if (FAILED(lresult))
					{
						MediaBufferLock lMediaBufferLock(
							lInputBuffer,
							lMaxLength,
							lCurrentLength,
							&lPtrInputBuffer,
							lresult);

						if (FAILED(lresult))
						{
							break;
						}
					}
					else
					{

						LONG lPtrStride;

						MediaBufferLock2D lMediaBufferLock(
							l2DBuffer,
							&lPtrStride,
							&lPtrInputBuffer,
							lresult);

						if (FAILED(lresult))
						{
							break;
						}
					}

					std::lock_guard<std::mutex> lLock(mReadLockMutex);

					writeData(
						lPtrInputBuffer,
						lMaxLength);

					mReadLockCondition.notify_all();

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullStream::PlaceMarker(
				MFSTREAMSINK_MARKER_TYPE aMarkerType,
				const PROPVARIANT* aPtrVarMarkerValue,
				const PROPVARIANT* aPtrVarContextValue)
			{
				HRESULT lresult;

				do
				{
					lresult = E_NOTIMPL;

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullStream::Flush()
			{
				HRESULT lresult;

				do
				{
					lresult = S_OK;

				} while (false);

				return lresult;
			}

			// IMFMediaEventGenerator interface implementation
			STDMETHODIMP SampleGrabberPullStream::BeginGetEvent(
				IMFAsyncCallback* aPtrAsyncCallback,
				IUnknown* aPtrUnkState)
			{
				HRESULT lresult;

				std::lock_guard<std::mutex> lLock(mMutex);

				do
				{
					LOG_CHECK_PTR_MEMORY(mEventQueue);

					LOG_CHECK_PTR_MEMORY(aPtrAsyncCallback);

					LOG_CHECK_PTR_MEMORY(aPtrUnkState);
					
					LOG_INVOKE_MF_METHOD(BeginGetEvent, mEventQueue,
						aPtrAsyncCallback,
						aPtrUnkState);
					
				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullStream::EndGetEvent(
				IMFAsyncResult* aPtrAsyncResult,
				IMFMediaEvent** aPtrPtrEvent)
			{
				HRESULT lresult;

				std::lock_guard<std::mutex> lLock(mMutex);

				do
				{
					LOG_CHECK_PTR_MEMORY(mEventQueue);

					LOG_CHECK_PTR_MEMORY(aPtrAsyncResult);

					LOG_CHECK_PTR_MEMORY(aPtrPtrEvent);

					LOG_INVOKE_MF_METHOD(EndGetEvent, mEventQueue,
						aPtrAsyncResult,
						aPtrPtrEvent);
					
				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullStream::GetEvent(
				DWORD aFlags,
				IMFMediaEvent** aPtrPtrEvent)
			{
				HRESULT lresult;

				std::lock_guard<std::mutex> lLock(mMutex);

				do
				{
					LOG_CHECK_PTR_MEMORY(mEventQueue);

					LOG_CHECK_PTR_MEMORY(aPtrPtrEvent);

					LOG_INVOKE_MF_METHOD(GetEvent, mEventQueue,
						aFlags,
						aPtrPtrEvent);
					
				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullStream::QueueEvent(
				MediaEventType aMediaEventType,
				REFGUID aRefGUIDExtendedType,
				HRESULT aStatus,
				const PROPVARIANT* aPtrVarValue)
			{
				HRESULT lresult;

				std::lock_guard<std::mutex> lLock(mMutex);

				do
				{
					LOG_CHECK_PTR_MEMORY(mEventQueue);
					
					LOG_INVOKE_MF_METHOD(QueueEventParamVar, mEventQueue,
						aMediaEventType, 
						aRefGUIDExtendedType, 
						aStatus, 
						aPtrVarValue);
					
				} while (false);

				return lresult;
			}

			// IMFMediaTypeHandler interface implementation
			STDMETHODIMP SampleGrabberPullStream::IsMediaTypeSupported(
				IMFMediaType* aPtrMediaType,
				IMFMediaType** aPtrPtrMediaType)
			{
				HRESULT lresult;

				do
				{
					lresult = E_NOTIMPL;

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullStream::GetMediaTypeCount(
				DWORD* aPtrTypeCount)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_STATE_DESCR(!mMediaType, E_NOTIMPL);

					*aPtrTypeCount = 1;

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullStream::GetMediaTypeByIndex(
				DWORD aIndex,
				IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(mMediaType);

					LOG_CHECK_STATE_DESCR(aIndex != 0, E_BOUNDS);
					
					LOG_CHECK_PTR_MEMORY(aPtrPtrType);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mMediaType, aPtrPtrType);
					
				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullStream::SetCurrentMediaType(
				IMFMediaType* aPtrMediaType)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrMediaType);

					LOG_CHECK_PTR_MEMORY(mMediaType);
					
					GUID lMajorTypeValue;

					LOG_INVOKE_MF_METHOD(GetGUID, 
						mMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorTypeValue);
					
					GUID lSecondMajorTypeValue;

					LOG_INVOKE_MF_METHOD(GetGUID, 
						aPtrMediaType,
						MF_MT_MAJOR_TYPE,
						&lSecondMajorTypeValue);
					
					LOG_CHECK_STATE_DESCR(lSecondMajorTypeValue != lMajorTypeValue, E_NOT_VALID_STATE);
					
					GUID lSubTypeValue;

					LOG_INVOKE_MF_METHOD(GetGUID, 
						mMediaType,
						MF_MT_SUBTYPE,
						&lSubTypeValue);
					
					GUID lSecondSubTypeValue;

					LOG_INVOKE_MF_METHOD(GetGUID, 
						aPtrMediaType,
						MF_MT_SUBTYPE,
						&lSecondSubTypeValue);
					
					LOG_CHECK_STATE_DESCR(lSecondSubTypeValue != lSubTypeValue, E_NOT_VALID_STATE);

					mMediaType = aPtrMediaType;

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullStream::GetCurrentMediaType(
				IMFMediaType** aPtrPtrMediaType)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrMediaType);

					LOG_CHECK_PTR_MEMORY(mMediaType);
					
					LOG_INVOKE_QUERY_INTERFACE_METHOD(mMediaType, aPtrPtrMediaType);
					
				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullStream::GetMajorType(
				GUID* aPtrGUIDMajorType)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(mMediaType);

					GUID lMajorTypeValue;

					LOG_INVOKE_MF_METHOD(GetGUID, 
						mMediaType,
						MF_MT_MAJOR_TYPE,
						&lMajorTypeValue);
					
					*aPtrGUIDMajorType = lMajorTypeValue;

				} while (false);

				return lresult;
			}

			// ReadWriteBufferRegularSync interface implementation 

			HRESULT SampleGrabberPullStream::readData(
				unsigned char* aPtrData,
				DWORD* aPtrSampleSize)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrData);

					LOG_CHECK_PTR_MEMORY(aPtrSampleSize);

					std::unique_lock<std::mutex> lLock(mReadLockMutex);
					
					LOG_INVOKE_FUNCTION(QueueEvent, 
						MEStreamSinkRequestSample, 
						GUID_NULL, 
						S_OK, 
						nullptr);
					
					auto lconditionResult = mReadLockCondition.wait_for(
						lLock,
						std::chrono::milliseconds(1000));

					if (lconditionResult == std::cv_status::timeout)
					{
						lresult = CONTEXT_E_SYNCH_TIMEOUT;
					}
					else
					{
						LOG_INVOKE_FUNCTION(SampleGrabberCall::RegularSampleGrabberCall::ReadWriteBufferRegularSync::readData,
							aPtrData,
							aPtrSampleSize);
					}
					
				} while (false);

				return lresult;
			}
			
			// SampleGrabberPullStream interface implementation

			HRESULT SampleGrabberPullStream::init(
				SampleGrabberPullSink* aPtrSink,
				IMFMediaType* aPtrMediaType)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrSink);

					LOG_CHECK_PTR_MEMORY(aPtrMediaType);

					mMediaType = aPtrMediaType;

					mPtrSink = aPtrSink;

					LOG_INVOKE_MF_FUNCTION(MFCreateEventQueue,
						&mEventQueue);
					
					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT SampleGrabberPullStream::start()
			{
				HRESULT lresult;

				do
				{
					LOG_INVOKE_FUNCTION(QueueEvent,
						MEStreamSinkStarted, 
						GUID_NULL, 
						S_OK, 
						nullptr);

				} while (false);

				return lresult;
			}

			HRESULT SampleGrabberPullStream::stop()
			{
				HRESULT lresult;

				do
				{
					LOG_INVOKE_FUNCTION(QueueEvent,
						MEStreamSinkStopped, 
						GUID_NULL, 
						S_OK, 
						nullptr);

				} while (false);

				return lresult;
			}
		}
	}
}