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

#include "AudioCaptureProcessorProxy.h"
#include "../Common/Common.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../MemoryManager/MemoryManager.h"
#include "CurrentMediaType.h"
#include "../Common/GUIDs.h"
#include <chrono>
#include <thread>



namespace CaptureManager
{
	namespace COMServer
	{
		using namespace Core;

		using namespace Sources;		

		AudioCaptureProcessorProxy::AudioCaptureProcessorProxy() :
			CaptureInvoker(AVRT_PRIORITY_AvrtManager::AVRT_PRIORITY_CRITICAL_AvrtManager, L"Audio"),
			mCycleOfCapture(4),
			mState(SourceState::SourceStateUninitialized),
			mStreamIndex(0),
			mDeltaTimeDuration(0),
			mCurrentSampleTime(0),
			mReleaseAudioClientLock(false),
			mSampleDuration(400000),
			mBufferOffset(0),
			mIsCallBack(FALSE),
			mPrevSampleTime(0)
		{
		}

		// ISourceRequestResult

		HRESULT STDMETHODCALLTYPE AudioCaptureProcessorProxy::setData(
			/* [in] */ LPVOID aPtrData,
			/* [in] */ DWORD aByteSize,
			/* [in] */ BOOL aIsKeyFrame)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrData);

				writeAudioBuffer((BYTE*)aPtrData,
					aByteSize);

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		HRESULT STDMETHODCALLTYPE AudioCaptureProcessorProxy::getStreamIndex(
			/* [out] */ DWORD *aPtrStreamIndex)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrStreamIndex);
				
				*aPtrStreamIndex = mStreamIndex;

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		// CaptureInvoker implementation

		HRESULT __stdcall AudioCaptureProcessorProxy::starting()
		{
			mPrevTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE AudioCaptureProcessorProxy::invoke()
		{
			HRESULT lresult(E_NOTIMPL);

			do
			{

				if (mState != SourceState::SourceStateStarted)
				{
					Sleep(10);

					lresult = S_OK;

					break;
				}

				mReleaseAudioClientLock = true;

				if (mIsCallBack == FALSE)
				{
					auto lCurrentTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

					auto ldif = lCurrentTime - mPrevTime;

					if ((ldif + mDeltaTimeDuration) >= mSampleDuration)
					{
						mPrevTime = lCurrentTime;

						mDeltaTimeDuration = (ldif + mDeltaTimeDuration) - mSampleDuration;

						mCurrentSampleTime += mSampleDuration;
						
						mNewSampleCondition.notify_all();


						CComPtrCustom<ISourceRequestResult> lISourceRequestResult;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(this, &lISourceRequestResult);

						mICaptureProcessor->sourceRequest(lISourceRequestResult);
					}


					Sleep(mMillTickTime);
				}
				else
				{
					CComPtrCustom<ISourceRequestResult> lISourceRequestResult;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(this, &lISourceRequestResult);

					mICaptureProcessor->sourceRequest(lISourceRequestResult);
				}

			SHUTDOWN_LABLE:
				lresult = S_OK;

			} while (false);

			mReleaseAudioClientLock = false;

			mReleaseAudioClientCondition.notify_one();

			return lresult;
		}


		//IInnerCaptureProcessor implementation

		HRESULT AudioCaptureProcessorProxy::getFrendlyName(
			BSTR* aPtrString)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrString);

				std::lock_guard<std::mutex> lLock(mAccessMutex);

				*aPtrString = SysAllocString(mFriendlyName.c_str());

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		HRESULT AudioCaptureProcessorProxy::getSymbolicLink(
			BSTR* aPtrString)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrString);

				std::lock_guard<std::mutex> lLock(mAccessMutex);

				auto SymbolicLink = std::wstring(L"CaptureManager///Software///Sources///AudioCaptureProcessor///") + mSymbolicLink;

				*aPtrString = SysAllocString(SymbolicLink.c_str());

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		HRESULT AudioCaptureProcessorProxy::getStreamAmount(
			UINT32* aPtrStreamAmount)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrStreamAmount);

				std::lock_guard<std::mutex> lLock(mAccessMutex);

				*aPtrStreamAmount = 1;

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		HRESULT AudioCaptureProcessorProxy::getMediaTypes(
			UINT32 aStreamIndex,
			BSTR* aPtrStreamNameString,
			IUnknown*** aPtrPtrPtrMediaType,
			UINT32* aPtrMediaTypeCount)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrStreamNameString);

				LOG_CHECK_PTR_MEMORY(aPtrPtrPtrMediaType);

				LOG_CHECK_PTR_MEMORY(aPtrMediaTypeCount);

				auto lFindItr = mInitilaizeCaptureSource->mStreams.find(aStreamIndex);
				
				*aPtrStreamNameString = SysAllocString(mInitilaizeCaptureSource->mStreamName.c_str());
				
				LOG_CHECK_STATE(lFindItr == mInitilaizeCaptureSource->mStreams.end());

				std::vector<CComPtrCustom<IMFMediaType>> lMediaTypes;

				for (auto& lItem : (*lFindItr).second)
				{
					if (lItem.second)
					{
						CComPtrCustom<IMFMediaType> lMediaType;

						LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,
							&lMediaType);

						LOG_INVOKE_MF_METHOD(CopyAllItems, lItem.second, lMediaType);
						
						lMediaTypes.push_back(lMediaType);
					}
				}

				auto lMediaTypeSize = lMediaTypes.size();

				LOG_CHECK_STATE_DESCR(lMediaTypeSize == 0, E_FAIL);

				decltype(lMediaTypeSize) lMediaTypeCount = 0;

				auto lPtrVoid = CoTaskMemAlloc(sizeof(IUnknown*)* lMediaTypeSize);

				LOG_CHECK_PTR_MEMORY(lPtrVoid);

				IUnknown** lPtrPtrMediaTypes = (IUnknown **)(lPtrVoid);

				LOG_CHECK_PTR_MEMORY(lPtrPtrMediaTypes);

				for (decltype(lMediaTypeSize) lMediaTypeIndex = 0; lMediaTypeIndex < lMediaTypeSize; lMediaTypeIndex++)
				{
					CComPtrCustom<IMFMediaType> lMediaType;

					lMediaType = lMediaTypes[lMediaTypeIndex];

					lPtrPtrMediaTypes[lMediaTypeIndex] = lMediaType.detach();

					lMediaTypeCount++;
				}
				
				*aPtrPtrPtrMediaType = lPtrPtrMediaTypes;

				*aPtrMediaTypeCount = lMediaTypeCount;



				lresult = S_OK;

			} while (false);

			return lresult;
		}

		HRESULT AudioCaptureProcessorProxy::setCurrentMediaType(
			UINT32 aStreamIndex,
			IUnknown* aPtrMediaType)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrMediaType);

				LOG_CHECK_PTR_MEMORY(mICaptureProcessor);

				LOG_CHECK_PTR_MEMORY(mInitilaizeCaptureSource);

				CComQIPtrCustom<IMFMediaType> lMediaType = aPtrMediaType;

				LOG_CHECK_PTR_MEMORY(lMediaType);

				auto lFindItr = mInitilaizeCaptureSource->mStreams.find(aStreamIndex);

				LOG_CHECK_STATE(lFindItr == mInitilaizeCaptureSource->mStreams.end());

				DWORD lMediaTypeIndex = 0;

				BOOL l_bResult = FALSE;

				for (auto& litem : (*lFindItr).second)
				{

					LOG_INVOKE_MF_METHOD(Compare, litem.second,
						lMediaType,
						MF_ATTRIBUTES_MATCH_TYPE::MF_ATTRIBUTES_MATCH_INTERSECTION,
						&l_bResult);

					if (l_bResult == TRUE)
					{
						lMediaTypeIndex = litem.first;
						
						break;
					}
				}

				LOG_CHECK_STATE_DESCR(l_bResult == FALSE, MF_E_INVALIDMEDIATYPE);
												
				LOG_INVOKE_MF_METHOD(GetUINT32, lMediaType,
					MF_MT_AUDIO_BLOCK_ALIGNMENT,
					&mBlockAlign);

				LOG_INVOKE_MF_METHOD(GetUINT32, lMediaType,
					MF_MT_AUDIO_SAMPLES_PER_SECOND,
					&mSamplesPerSec);



				UINT64 lvalue = 100000;

				lresult = lMediaType->GetUINT64(
					CM_DURATION_PERIOD,
					&lvalue);

				if (SUCCEEDED(lresult))
				{
					mIsCallBack = TRUE;

					mSampleDuration = lvalue;
				}
				
				
				


				CComPtrCustom<CurrentMediaType> lCurrentMediaType(new (std::nothrow)CurrentMediaType);

				LOG_CHECK_PTR_MEMORY(lCurrentMediaType);

				lCurrentMediaType->mStreamIndex = aStreamIndex;
				
				mStreamIndex = mStreamIndex;

				lCurrentMediaType->mCurrentMediaType = aPtrMediaType;

				lCurrentMediaType->mMediaTypeIndex = lMediaTypeIndex;
				
				LOG_INVOKE_POINTER_METHOD(mICaptureProcessor, setCurrentMediaType, lCurrentMediaType);

				mState = SourceState::SourceStateInitialized;
				
			} while (false);

			return lresult;
		}

		HRESULT AudioCaptureProcessorProxy::getNewSample(
			DWORD aStreamIdentifier,
			IUnknown** aPtrPtrSample)
		{
			HRESULT lresult(E_FAIL);

			do
			{

				LOG_CHECK_PTR_MEMORY(aPtrPtrSample);

				LOG_CHECK_STATE(aStreamIdentifier > 0);

				CComPtrCustom<IMFSample> lSample;

				LOG_INVOKE_MF_FUNCTION(MFCreateSample, &lSample);

				LONGLONG lSampleTime = 0;

				LONGLONG lSampleDuration = mSampleDuration;

				CComPtrCustom<IMFMediaBuffer> lMediaBuffer;

				{
					std::unique_lock<std::mutex> lLock(mAccessBufferQueueMutex);

					auto lconditionResult = mNewSampleCondition.wait_for(lLock, std::chrono::seconds(4));

					if (lconditionResult == std::cv_status::timeout)
					{
						break;
					}

					if (!popMediaBuffer(
						&lMediaBuffer))
					{
						lMediaBuffer.Release();

						LOG_INVOKE_MF_FUNCTION(MFCreateMemoryBuffer, mExpectedBufferSize, &lMediaBuffer);

						LOG_CHECK_PTR_MEMORY(lMediaBuffer);

						DWORD lMaxLength = 0;

						DWORD lCurrentLength = 0;

						BYTE* lPtrBuffer = nullptr;

						LOG_INVOKE_MF_METHOD(Lock, lMediaBuffer, &lPtrBuffer, &lMaxLength, &lCurrentLength);

						LOG_CHECK_PTR_MEMORY(lPtrBuffer);

						ZeroMemory(lPtrBuffer, lMaxLength);

						LOG_INVOKE_MF_METHOD(Unlock, lMediaBuffer);

						LOG_INVOKE_MF_METHOD(SetCurrentLength, lMediaBuffer, mExpectedBufferSize);
					}

					lSampleTime = mCurrentSampleTime;

					lSampleTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

					//if (mPrevSampleTime > 0)
					//{
					//	lSampleDuration = lSampleTime - mPrevSampleTime;
					//}

					mPrevSampleTime = lSampleTime;
				}

				LOG_INVOKE_MF_METHOD(AddBuffer, lSample, lMediaBuffer);

				LOG_INVOKE_MF_METHOD(SetSampleDuration, lSample, lSampleDuration);

				LOG_INVOKE_MF_METHOD(SetSampleTime, lSample, lSampleTime);

				LOG_INVOKE_MF_METHOD(SetUINT64, lSample, MFSampleExtension_DeviceTimestamp, lSampleTime);

				LOG_CHECK_STATE(mState == SourceState::SourceStateStopped);

				*aPtrPtrSample = lSample.Detach();

			} while (false);

			return lresult;
		}

		HRESULT AudioCaptureProcessorProxy::start()
		{
			HRESULT lresult(E_FAIL);

			do
			{
				std::lock_guard<std::mutex> lLock(mAccessMutex);

				LOG_CHECK_STATE_DESCR(isUninitialized(), MF_E_AUDIO_RECORDING_DEVICE_INVALIDATED);

				LOG_INVOKE_FUNCTION(checkShutdown);

				if (mState == SourceState::SourceStateStarted)
				{
					lresult = S_OK;

					break;
				}

				mDeltaTimeDuration = 0;

				mPrevSampleTime = 0;

				clearMediaBuffer();

				if (mState == SourceState::SourceStatePaused)
				{

					LOG_INVOKE_POINTER_METHOD(mICaptureProcessor, start, mCurrentSampleTime,
						GUID_NULL);

					CaptureInvoker::start();

					mState = SourceState::SourceStateStarted;

					lresult = S_OK;

					break;
				}
												
				mExpectedBufferSize = (UINT32)((((LONGLONG)mSamplesPerSec) * ((LONGLONG)mBlockAlign)* mSampleDuration) / 10000000LL);

				mSleepDuration = (mSampleDuration >> 1) / 10000LL;
				
				mMillTickTime = (DWORD)(mSampleDuration / 40000);
				
				mCurrentSampleTime = 0;

				LOG_INVOKE_FUNCTION(allocateBuffer);

				LOG_INVOKE_POINTER_METHOD(mICaptureProcessor, start, mCurrentSampleTime,
					GUID_NULL);

				CaptureInvoker::start();
				
				mState = SourceState::SourceStateStarted;

			} while (false);

			return lresult;
		}

		HRESULT AudioCaptureProcessorProxy::stop()
		{
			HRESULT lresult;

			do
			{
				std::lock_guard<std::mutex> lLock(mAccessMutex);

				CaptureInvoker::stop();

				LOG_CHECK_STATE_DESCR(isUninitialized(), MF_E_AUDIO_RECORDING_DEVICE_INVALIDATED);

				LOG_INVOKE_FUNCTION(checkShutdown);
				
				if (mState != SourceState::SourceStateStarted &&
					mState != SourceState::SourceStatePaused)
				{
					break;
				}

				mState = SourceState::SourceStateStopped;
				
				lresult = S_OK;

			} while (false);

			return lresult;
		}

		HRESULT AudioCaptureProcessorProxy::pause()
		{
			HRESULT lresult;

			do
			{
				std::lock_guard<std::mutex> lLock(mAccessMutex);

				LOG_CHECK_STATE_DESCR(isUninitialized(), MF_E_AUDIO_RECORDING_DEVICE_INVALIDATED);

				LOG_INVOKE_FUNCTION(checkShutdown);
				
				if (mState != SourceState::SourceStateStarted)
				{
					break;
				}

				CaptureInvoker::stop();

				mState = SourceState::SourceStatePaused;

			} while (false);


			return lresult;
		}

		HRESULT AudioCaptureProcessorProxy::restart()
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_STATE_DESCR(isUninitialized(), MF_E_AUDIO_RECORDING_DEVICE_INVALIDATED);

				LOG_INVOKE_FUNCTION(checkShutdown);
				
				if (mState != SourceState::SourceStatePaused)
				{
					break;
				}

				LOG_INVOKE_FUNCTION(start);

			} while (false);

			return lresult;
		}

		HRESULT AudioCaptureProcessorProxy::shutdown()
		{
			HRESULT lresult;

			do
			{
				std::lock_guard<std::mutex> lLock(mAccessMutex);

				LOG_CHECK_STATE_DESCR(isUninitialized(), MF_E_AUDIO_RECORDING_DEVICE_INVALIDATED);

				LOG_INVOKE_FUNCTION(checkShutdown);

				CaptureInvoker::stop();
				
				mState = SourceState::SourceStateShutdown;

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		HRESULT AudioCaptureProcessorProxy::registerMediaEvent(IUnknown* pUnkIMediaEvent)
		{
			return E_NOTIMPL;
		}

		HRESULT AudioCaptureProcessorProxy::init(ICaptureProcessor* aPtrICaptureProcessor, InitilaizeCaptureSource* aPtrInitilaizeCaptureSource)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrICaptureProcessor);

				LOG_CHECK_PTR_MEMORY(aPtrInitilaizeCaptureSource);

				mICaptureProcessor = aPtrICaptureProcessor;

				LOG_CHECK_PTR_MEMORY(mICaptureProcessor);

				mInitilaizeCaptureSource = aPtrInitilaizeCaptureSource;

				LOG_CHECK_PTR_MEMORY(mInitilaizeCaptureSource);

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		HRESULT AudioCaptureProcessorProxy::checkShutdown() const
		{
			HRESULT lresult = S_OK;   

			if (mState == SourceState::SourceStateShutdown)
			{
				lresult = MF_E_SHUTDOWN;
			}

			return lresult;
		}

		bool AudioCaptureProcessorProxy::isUninitialized()
		{
			return mState == SourceState::SourceStateUninitialized;
		}

		void AudioCaptureProcessorProxy::clearMediaBuffer()
		{
			std::lock_guard<std::mutex> lLock(mAccessBufferQueueMutex);

			while (!mBufferQueue.empty())
			{
				mBufferQueue.front().mPtrMediaBuffer->Release();

				mBufferQueue.pop();
			}
		}

		HRESULT AudioCaptureProcessorProxy::allocateBuffer()
		{
			HRESULT lresult;

			do
			{
				mCurrentMediaBuffer.Release();

				LOG_INVOKE_MF_FUNCTION(MFCreateMemoryBuffer, mExpectedBufferSize, &mCurrentMediaBuffer);

				LOG_CHECK_PTR_MEMORY(mCurrentMediaBuffer);

				LOG_INVOKE_MF_METHOD(SetCurrentLength, mCurrentMediaBuffer, mExpectedBufferSize);

				mBufferOffset = 0;

			} while (false);

			return lresult;
		}

		void AudioCaptureProcessorProxy::writeAudioBuffer(
			BYTE* aPtrData,
			UINT32 aBufferByteLength)
		{
			HRESULT lresult;

			do
			{
				BYTE* lPtrBuffer = nullptr;

				LOG_CHECK_PTR_MEMORY(mCurrentMediaBuffer);

				LOG_INVOKE_MF_METHOD(Lock, mCurrentMediaBuffer, &lPtrBuffer, nullptr, nullptr);

				LOG_CHECK_PTR_MEMORY(lPtrBuffer);

				auto lavalableSize = mExpectedBufferSize - mBufferOffset;

				auto lmaxCount = lavalableSize >= aBufferByteLength ? aBufferByteLength : lavalableSize;

				if (aPtrData != nullptr)
					MemoryManager::memcpy(lPtrBuffer + mBufferOffset, aPtrData, lmaxCount);

				LOG_INVOKE_MF_METHOD(Unlock, mCurrentMediaBuffer);

				mBufferOffset += lmaxCount;

				if (mBufferOffset >= mExpectedBufferSize)
				{
					pushMediaBuffer(mCurrentMediaBuffer.detach());

					allocateBuffer();

					if (aBufferByteLength > lmaxCount)
					{
						//LogPrintOut::getInstance().printOutln(
						//	LogPrintOut::INFO_LEVEL,
						//	L" !aBufferByteLength: ",
						//	aBufferByteLength,
						//	L" !lmaxCount: ",
						//	lmaxCount);

						writeAudioBuffer(aPtrData + lmaxCount, aBufferByteLength - lmaxCount);
					}
				}

			} while (false);
		}

		void AudioCaptureProcessorProxy::pushMediaBuffer(
			IMFMediaBuffer* aPtrMediaBuffer)
		{
			std::lock_guard<std::mutex> lLock(mAccessBufferQueueMutex);

			if (mBufferQueue.size() > 4)
			{
				mBufferQueue.front().mPtrMediaBuffer->Release();

				mBufferQueue.pop();
			}

			BufferContainer lBufferContainer;

			lBufferContainer.mPtrMediaBuffer = aPtrMediaBuffer;

			mBufferQueue.push(lBufferContainer);

			if (mIsCallBack != FALSE)
				mNewSampleCondition.notify_all();
		}

		bool AudioCaptureProcessorProxy::popMediaBuffer(
			IMFMediaBuffer** aPtrPtrMediaBuffer)
		{
			bool lresult = false;

			do
			{
				if (mBufferQueue.empty())
				{
					break;
				}

				auto lbuff = mBufferQueue.front();

				*aPtrPtrMediaBuffer = lbuff.mPtrMediaBuffer;

				mBufferQueue.pop();

				lresult = true;

			} while (false);

			return lresult;
		}

		AudioCaptureProcessorProxy::~AudioCaptureProcessorProxy()
		{
		}
	}
}