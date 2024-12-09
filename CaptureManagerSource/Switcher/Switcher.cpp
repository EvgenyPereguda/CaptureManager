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

#include "Switcher.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/Common.h"
#include "../LogPrintOut/LogPrintOut.h"

namespace CaptureManager
{
	namespace Transform
	{
		using namespace CaptureManager::Core;

		Switcher::Switcher():
			mIsPaused(false),
			mFirstSample(true),
			mPauseOffset(0),
			mLastTimeStamp(0)
		{
		}


		Switcher::~Switcher()
		{
		}

		STDMETHODIMP Switcher::GetStreamLimits(DWORD* aPtrInputMinimum, DWORD* aPtrInputMaximum,
			DWORD* aPtrOutputMinimum, DWORD* aPtrOutputMaximum)
		{
			HRESULT lresult = E_FAIL;

			do
			{

				LOG_CHECK_STATE_DESCR(aPtrInputMinimum == NULL ||
					aPtrInputMaximum == NULL ||
					aPtrOutputMinimum == NULL ||
					aPtrOutputMaximum == NULL, E_POINTER);

				*aPtrInputMinimum = 1;

				*aPtrInputMaximum = 1;

				*aPtrOutputMinimum = 1;

				*aPtrOutputMaximum = 1;

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		STDMETHODIMP Switcher::GetStreamIDs(DWORD aInputIDArraySize, DWORD* aPtrInputIDs,
			DWORD aOutputIDArraySize, DWORD* aPtrOutputIDs)
		{
			return E_NOTIMPL;
		}

		STDMETHODIMP Switcher::GetStreamCount(DWORD* aPtrInputStreams, DWORD* aPtrOutputStreams)
		{
			HRESULT lresult = E_FAIL;

			do
			{

				LOG_CHECK_STATE_DESCR(aPtrInputStreams == NULL || aPtrOutputStreams == NULL, E_POINTER);

				*aPtrInputStreams = 1;

				*aPtrOutputStreams = 1;

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		STDMETHODIMP Switcher::GetInputStreamInfo(DWORD aInputStreamID,
			MFT_INPUT_STREAM_INFO* aPtrStreamInfo)
		{
			HRESULT lresult = S_OK;

			do
			{
				std::lock_guard<std::mutex> lock(mMutex);

				LOG_CHECK_PTR_MEMORY(aPtrStreamInfo);

				LOG_CHECK_STATE_DESCR(aInputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);

				aPtrStreamInfo->dwFlags = MFT_INPUT_STREAM_WHOLE_SAMPLES |
					MFT_INPUT_STREAM_SINGLE_SAMPLE_PER_BUFFER;

				aPtrStreamInfo->cbMaxLookahead = 0;

				aPtrStreamInfo->cbAlignment = 0;

				aPtrStreamInfo->hnsMaxLatency = 0;

				aPtrStreamInfo->cbSize = 0;

			} while (false);

			return lresult;
		}

		STDMETHODIMP Switcher::GetOutputStreamInfo(DWORD aOutputStreamID,
			MFT_OUTPUT_STREAM_INFO* aPtrStreamInfo)
		{
			HRESULT lresult = S_OK;

			do
			{
				std::lock_guard<std::mutex> lock(mMutex);

				LOG_CHECK_PTR_MEMORY(aPtrStreamInfo);

				LOG_CHECK_STATE_DESCR(aOutputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);


				aPtrStreamInfo->dwFlags =
					MFT_OUTPUT_STREAM_WHOLE_SAMPLES |
					MFT_OUTPUT_STREAM_SINGLE_SAMPLE_PER_BUFFER |
					MFT_OUTPUT_STREAM_FIXED_SAMPLE_SIZE |
					MFT_OUTPUT_STREAM_PROVIDES_SAMPLES;


				aPtrStreamInfo->cbAlignment = 0;

				aPtrStreamInfo->cbSize = 0;

			} while (false);

			return lresult;
		}

		STDMETHODIMP Switcher::GetInputStreamAttributes(DWORD aInputStreamID,
			IMFAttributes** aPtrPtrAttributes)
		{
			return E_NOTIMPL;
		}

		STDMETHODIMP Switcher::GetOutputStreamAttributes(DWORD aOutputStreamID,
			IMFAttributes** aPtrPtrAttributes)
		{
			return E_NOTIMPL;
		}

		STDMETHODIMP Switcher::DeleteInputStream(DWORD aStreamID)
		{
			return E_NOTIMPL;
		}

		STDMETHODIMP Switcher::AddInputStreams(DWORD aStreams, DWORD* aPtrStreamIDs)
		{
			return E_NOTIMPL;
		}

		STDMETHODIMP Switcher::GetInputAvailableType(DWORD aInputStreamID, DWORD aTypeIndex,
			IMFMediaType** aPtrPtrType)
		{
			HRESULT lresult = S_OK;
			CComPtrCustom<IMFMediaType> lMediaType;

			do
			{
				std::lock_guard<std::mutex> lock(mMutex);

				LOG_CHECK_PTR_MEMORY(aPtrPtrType);

				LOG_CHECK_STATE_DESCR(aInputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);

				*aPtrPtrType = NULL;

				if (!mInputMediaType)
				{
					lresult = MF_E_NO_MORE_TYPES;
				}
				else if (aTypeIndex == 0)
				{
					*aPtrPtrType = mInputMediaType.get();

					(*aPtrPtrType)->AddRef();
				}
				else
				{
					lresult = MF_E_NO_MORE_TYPES;
				}

			} while (false);

			return lresult;
		}

		STDMETHODIMP Switcher::GetOutputAvailableType(DWORD aOutputStreamID, DWORD aTypeIndex,
			IMFMediaType** aPtrPtrType)
		{
			HRESULT lresult = S_OK;
			CComPtrCustom<IMFMediaType> lMediaType;

			do
			{
				std::lock_guard<std::mutex> lock(mMutex);

				LOG_CHECK_PTR_MEMORY(aPtrPtrType);

				LOG_CHECK_STATE_DESCR(aOutputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);


				LOG_CHECK_PTR_MEMORY(mInputMediaType);
				
				*aPtrPtrType = mInputMediaType.get();

				(*aPtrPtrType)->AddRef();

			} while (false);

			return lresult;
		}

		STDMETHODIMP Switcher::SetInputType(DWORD aInputStreamID, IMFMediaType* aPtrType,
			DWORD aFlags)
		{
			HRESULT lresult = S_OK;
			CComPtrCustom<IMFAttributes> lTypeAttributes;

			do
			{
				lTypeAttributes = aPtrType;

				std::lock_guard<std::mutex> lock(mMutex);

				LOG_CHECK_STATE_DESCR(aInputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
				
				if (aFlags != MFT_SET_TYPE_TEST_ONLY)
				{
					mInputMediaType = aPtrType;		

					mInputMediaType->GetMajorType(&mGUIDMajorType);
				}

			} while (false);

			return lresult;
		}

		STDMETHODIMP Switcher::SetOutputType(DWORD aOutputStreamID, IMFMediaType* aPtrType,
			DWORD aFlags)
		{
			HRESULT lresult = S_OK;

			CComPtrCustom<IMFMediaType> lType;

			do
			{
				lType = aPtrType;

				std::lock_guard<std::mutex> lock(mMutex);

				LOG_CHECK_STATE_DESCR(aOutputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
				
			} while (false);


			return lresult;
		}

		STDMETHODIMP Switcher::GetInputCurrentType(DWORD aInputStreamID, IMFMediaType** aPtrPtrType)
		{
			HRESULT lresult = S_OK;

			do
			{

				std::lock_guard<std::mutex> lock(mMutex);

				LOG_CHECK_PTR_MEMORY(aPtrPtrType);

				LOG_CHECK_STATE_DESCR(aInputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);

				LOG_CHECK_STATE_DESCR(!mInputMediaType, MF_E_TRANSFORM_TYPE_NOT_SET);

				*aPtrPtrType = mInputMediaType;

				(*aPtrPtrType)->AddRef();

			} while (false);

			return lresult;
		}

		STDMETHODIMP Switcher::GetOutputCurrentType(DWORD aOutputStreamID, IMFMediaType** aPtrPtrType)
		{
			HRESULT lresult = S_OK;

			do
			{
				std::lock_guard<std::mutex> lock(mMutex);

				LOG_CHECK_PTR_MEMORY(aPtrPtrType);

				LOG_CHECK_STATE_DESCR(aOutputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);

				LOG_CHECK_STATE_DESCR(!mInputMediaType, MF_E_TRANSFORM_TYPE_NOT_SET);

				*aPtrPtrType = mInputMediaType;
				(*aPtrPtrType)->AddRef();

			} while (false);

			return lresult;
		}

		STDMETHODIMP Switcher::GetInputStatus(DWORD aInputStreamID, DWORD* aPtrFlags)
		{
			HRESULT lresult = S_OK;

			do
			{
				std::lock_guard<std::mutex> lock(mMutex);

				LOG_CHECK_PTR_MEMORY(aPtrFlags);

				LOG_CHECK_STATE_DESCR(aInputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);

				if (!mSample)
				{
					*aPtrFlags = MFT_INPUT_STATUS_ACCEPT_DATA;
				}
				else
				{
					*aPtrFlags = 0;
				}

			} while (false);

			return lresult;
		}

		STDMETHODIMP Switcher::GetOutputStatus(DWORD* aPtrFlags)
		{
			return E_NOTIMPL;
		}

		STDMETHODIMP Switcher::SetOutputBounds(LONGLONG aLowerBound, LONGLONG aUpperBound)
		{
			return E_NOTIMPL;
		}

		STDMETHODIMP Switcher::ProcessEvent(DWORD aInputStreamID, IMFMediaEvent* aPtrEvent)
		{
			return E_NOTIMPL;
		}

		STDMETHODIMP Switcher::GetAttributes(IMFAttributes** aPtrPtrAttributes)
		{
			return E_NOTIMPL;
		}

		STDMETHODIMP Switcher::ProcessMessage(MFT_MESSAGE_TYPE aMessage, ULONG_PTR aParam)
		{
			HRESULT lresult = S_OK;

			do
			{
				std::lock_guard<std::mutex> lock(mMutex);

				if (aMessage == MFT_MESSAGE_COMMAND_FLUSH)
				{
					mSample.Release();
				}
				else if (aMessage == MFT_MESSAGE_COMMAND_DRAIN)
				{
				}
				else if (aMessage == MFT_MESSAGE_NOTIFY_BEGIN_STREAMING)
				{
				}
				else if (aMessage == MFT_MESSAGE_NOTIFY_END_STREAMING)
				{
				}
				else if (aMessage == MFT_MESSAGE_NOTIFY_END_OF_STREAM)
				{
				}
				else if (aMessage == MFT_MESSAGE_NOTIFY_START_OF_STREAM)
				{
				}

			} while (false);

			return lresult;
		}

		STDMETHODIMP Switcher::ProcessInput(DWORD aInputStreamID, IMFSample* aPtrSample,
			DWORD aFlags)
		{
			HRESULT lresult = S_OK;
			DWORD dwBufferCount = 0;

			do
			{
				std::lock_guard<std::mutex> lock(mMutex);

				LOG_CHECK_PTR_MEMORY(aPtrSample);

				LOG_CHECK_STATE(aInputStreamID != 0 || aFlags != 0);
				
				LOG_CHECK_STATE_DESCR(!(!mSample), MF_E_NOTACCEPTING);

				if (!mIsPaused)				
					mSample = aPtrSample;

			} while (false);

			return lresult;
		}

		STDMETHODIMP Switcher::ProcessOutput(DWORD aFlags, DWORD aOutputBufferCount,
			MFT_OUTPUT_DATA_BUFFER* aPtrOutputSamples, DWORD* aPtrStatus)
		{
			HRESULT lresult = S_OK;

			do
			{
				std::lock_guard<std::mutex> lock(mMutex);

				LOG_CHECK_PTR_MEMORY(aPtrOutputSamples);

				LOG_CHECK_PTR_MEMORY(aPtrStatus);

				LOG_CHECK_STATE_DESCR(aOutputBufferCount != 1 || aFlags != 0, E_INVALIDARG);

				LOG_CHECK_STATE_DESCR(!mSample, MF_E_TRANSFORM_NEED_MORE_INPUT);							

				if (mIsPaused)
				{
					mSample.Release();

					lresult = S_FALSE;

					if (mGUIDMajorType == MFMediaType_Audio)
					{
						lresult = MF_E_TRANSFORM_NEED_MORE_INPUT;
					}

					break;
				}
				
				LONGLONG lSampleDuration;

				LOG_INVOKE_MF_METHOD(GetSampleDuration, mSample, &lSampleDuration);

				LONGLONG lSampleTimeStamp;
				
				LOG_INVOKE_MF_METHOD(GetSampleTime, mSample, &lSampleTimeStamp);
				
				if (mFirstSample)
				{
					mPauseOffset += (lSampleTimeStamp - mLastTimeStamp);

					mFirstSample = false;
				}

				mLastTimeStamp = lSampleTimeStamp + lSampleDuration;
				
				LOG_INVOKE_MF_METHOD(SetSampleTime, mSample, lSampleTimeStamp - mPauseOffset);	
								
				aPtrOutputSamples[0].pSample = mSample.Detach();

				aPtrOutputSamples[0].dwStatus = 0;

				*aPtrStatus = 0;

			} while (false);

			return lresult;
		}



		// ISwitcher interface

		STDMETHODIMP Switcher::pause()
		{
			HRESULT lresult = S_OK;

			do
			{
				std::lock_guard<std::mutex> lock(mMutex);

				mIsPaused = true;
				
			} while (false);

			return lresult;
		}

		STDMETHODIMP Switcher::resume()
		{
			HRESULT lresult = S_OK;

			do
			{
				std::lock_guard<std::mutex> lock(mMutex);

				mSample.Release();

				mIsPaused = false;
				
				mFirstSample = true;

			} while (false);

			return lresult;
		}

		STDMETHODIMP Switcher::reset()
		{
			HRESULT lresult = S_OK;

			do
			{
				std::lock_guard<std::mutex> lock(mMutex);

				mSample.Release();

				mPauseOffset = 0;
				
				mLastTimeStamp = 0;

				mIsPaused = false;
				
				mFirstSample = true;

			} while (false);

			return lresult;
		}
	}
}