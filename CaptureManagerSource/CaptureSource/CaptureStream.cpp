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

#include "CaptureStream.h"
#include "../Common/Common.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "CaptureSource.h"

namespace CaptureManager
{
	namespace Sources
	{
		using namespace CaptureManager::Core;

		CaptureStream::CaptureStream(CaptureSource* aPtrCaptureSource,
			IMFStreamDescriptor* aStreamDescriptor) :
			mPtrCaptureSource(aPtrCaptureSource),
			mIsActive(false)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				LOG_INVOKE_MF_FUNCTION(MFCreateEventQueue, &mEventQueue);

				mStreamDescriptor = aStreamDescriptor;

				LOG_INVOKE_MF_METHOD(GetStreamIdentifier,mStreamDescriptor, &mStreamIdentifier);

			} while (false);

			if (FAILED(lresult))
				throw;
		}
			
		CaptureStream::~CaptureStream()
		{
			mStreamDescriptor.Release();

			mEventQueue.Release();
		}
		
		STDMETHODIMP CaptureStream::GetMediaSource(IMFMediaSource** aPtrPtrMediaSource)
		{
			HRESULT lresult;

			do
			{
				std::lock_guard<std::mutex> llock(mMutex);

				LOG_CHECK_PTR_MEMORY(aPtrPtrMediaSource);

				LOG_CHECK_PTR_MEMORY(mPtrCaptureSource);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(mPtrCaptureSource,aPtrPtrMediaSource);
					
			} while (false);
				
			return lresult;
		}

		STDMETHODIMP CaptureStream::GetStreamDescriptor(IMFStreamDescriptor** aPtrPtrStreamDescriptor)
		{
			HRESULT lresult;

			do
			{
				std::lock_guard<std::mutex> llock(mMutex);

				LOG_CHECK_PTR_MEMORY(aPtrPtrStreamDescriptor);

				LOG_CHECK_PTR_MEMORY(mStreamDescriptor);

				LOG_INVOKE_OBJECT_METHOD(mStreamDescriptor, CopyTo, aPtrPtrStreamDescriptor);

			} while (false);
			
			return lresult;
		}

		HRESULT CaptureStream::queueRequestToken(
			IUnknown* aPtrToken)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				std::lock_guard<std::mutex> lLock(mOperationMutex);

				if (mState == SourceState::SourceStatePaused)
				{
					CComPtrCustom<RequestContainer> lPostponedRequestContainer(new (std::nothrow) RequestContainer(aPtrToken));

					mPostponedRequestContainer = lPostponedRequestContainer;

					lresult = S_OK;

					break;
				}

			} while (false);

			return lresult;
		}
			
		STDMETHODIMP CaptureStream::RequestSample(IUnknown* aPtrToken)
		{
			HRESULT lresult;

			do
			{


				std::lock_guard<std::mutex> llock(mMutex);

				LOG_INVOKE_POINTER_METHOD(mPtrCaptureSource, requestNewSample,
					mStreamIdentifier,
					aPtrToken);
								
			} while (false);

			return lresult;
		}

		STDMETHODIMP CaptureStream::BeginGetEvent(IMFAsyncCallback* aPtrCallback, IUnknown* aPtrUnkState)
		{
			HRESULT lresult;

			do
			{
				std::lock_guard<std::mutex> llock(mMutex);

				LOG_INVOKE_MF_METHOD(BeginGetEvent, mEventQueue, 
					aPtrCallback, aPtrUnkState);

			} while (false);

			return lresult;
		}
		STDMETHODIMP CaptureStream::EndGetEvent(IMFAsyncResult* aPtrResult, IMFMediaEvent** aPtrPtrEvent)
		{
			HRESULT lresult;

			do
			{
				std::lock_guard<std::mutex> llock(mMutex);

				LOG_INVOKE_MF_METHOD(EndGetEvent, mEventQueue,
					aPtrResult, aPtrPtrEvent);

			} while (false);

			return lresult;
		}
		STDMETHODIMP CaptureStream::GetEvent(DWORD aFlags, IMFMediaEvent** aPtrPtrEvent)
		{
			HRESULT lresult;

			do
			{
				std::lock_guard<std::mutex> llock(mMutex);

				LOG_INVOKE_MF_METHOD(GetEvent, mEventQueue,
					aFlags, aPtrPtrEvent);

			} while (false);

			return lresult;
		}
		STDMETHODIMP CaptureStream::QueueEvent(MediaEventType aMediaEventType, REFGUID aGUIDExtendedType,
			HRESULT aHRStatus, const PROPVARIANT* aPtrvValue)
		{
			HRESULT lresult;

			do
			{
				std::lock_guard<std::mutex> llock(mMutex);
				
				LOG_INVOKE_MF_METHOD(QueueEventParamVar, mEventQueue, aMediaEventType,
					aGUIDExtendedType, aHRStatus, aPtrvValue);

			} while (false);

			return lresult;
		}

		HRESULT CaptureStream::start()
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(mStreamDescriptor);

				std::lock_guard<std::mutex> lLock(mOperationMutex);

				LOG_INVOKE_FUNCTION(checkShutdown);

				LOG_INVOKE_FUNCTION(QueueEvent, MEStreamStarted, GUID_NULL, S_OK);

				auto lprevState = mState;

				mState = SourceState::SourceStateStarted;
				
				if (lprevState == SourceState::SourceStatePaused)
				{
					if (mPostponedRequestContainer)
					{
						CComPtrCustom<IUnknown> lToken;

						LOG_INVOKE_POINTER_METHOD(mPostponedRequestContainer, getToken, &lToken);

						LOG_INVOKE_POINTER_METHOD(mPtrCaptureSource, requestNewSample, 
							mStreamIdentifier,
							lToken);
					}
				}
				
			} while (false);

			return lresult;
		}

		HRESULT CaptureStream::pause()
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(mStreamDescriptor);

				std::lock_guard<std::mutex> lLock(mOperationMutex);
				
				LOG_INVOKE_FUNCTION(checkShutdown);

				LOG_INVOKE_FUNCTION(QueueEvent, MEStreamPaused, GUID_NULL, S_OK);
				
				mState = SourceState::SourceStatePaused;

			} while (false);

			return lresult;
		}

		HRESULT CaptureStream::stop()
		{
			HRESULT lresult = S_OK;

			do
			{

				LOG_CHECK_PTR_MEMORY(mStreamDescriptor);

				std::lock_guard<std::mutex> lLock(mOperationMutex);

				LOG_INVOKE_FUNCTION(checkShutdown);

				LOG_INVOKE_POINTER_METHOD(mPtrCaptureSource, stopCapture);
				
				mIsActive = false;

				mState = SourceState::SourceStateStopped;

				LOG_INVOKE_FUNCTION(QueueEvent, MEStreamStopped, GUID_NULL, S_OK);

			} while (false);
								
			return lresult;
		}

		HRESULT CaptureStream::shutdown()
		{
			HRESULT lresult = S_OK;

			do
			{

				LOG_CHECK_PTR_MEMORY(mStreamDescriptor);

				std::lock_guard<std::mutex> lLock(mOperationMutex);

				LOG_INVOKE_FUNCTION(checkShutdown);

				mState = SourceState::SourceStateShutdown;

				if (!(!mEventQueue))
				{
					mEventQueue->Shutdown();
				}

			} while (false);

			return lresult;

		}
			
		HRESULT CaptureStream::isActive(BOOL* aPtrState)
		{
			HRESULT lresult(S_OK);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrState);

				*aPtrState = this->mIsActive;

			} while (false);

			return lresult;
		}
			
		HRESULT CaptureStream::activate(BOOL aActivate)
		{
			this->mIsActive = aActivate == TRUE;

			return S_OK;
		}

		HRESULT CaptureStream::checkShutdown()
		{
			if (mState == SourceState::SourceStateShutdown)
			{
				return MF_E_SHUTDOWN;
			}
			else
			{
				return S_OK;
			}
		}
		
		HRESULT CaptureStream::QueueEvent(MediaEventType aMediaEventType, REFGUID aGUIDExtendedType,
			HRESULT aHRStatus, IUnknown* aPtrUnk)
		{
			HRESULT lresult;

			do
			{
				std::lock_guard<std::mutex> llock(mMutex);

				LOG_INVOKE_POINTER_METHOD(mEventQueue, QueueEventParamUnk,
					aMediaEventType,
					aGUIDExtendedType, 
					aHRStatus, 
					aPtrUnk);

			} while (false);

			return lresult;
		}		
	}
}