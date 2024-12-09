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

#include "VideoSampleAllocatorTransform.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/Common.h"
#include "../LogPrintOut/LogPrintOut.h"


namespace CaptureManager
{
	namespace Transform
	{
		namespace VideoSampleAllocator
		{
			using namespace Core;
			
			VideoSampleAllocatorTransform::VideoSampleAllocatorTransform():
				mIsReady(false)
			{
			}
			
			VideoSampleAllocatorTransform::~VideoSampleAllocatorTransform()
			{
			}

			// IMFTransform implementation
			
			STDMETHODIMP VideoSampleAllocatorTransform::GetStreamLimits(
				DWORD* aPtrInputMinimum,
				DWORD* aPtrInputMaximum,
				DWORD* aPtrOutputMinimum,
				DWORD* aPtrOutputMaximum)
			{
				HRESULT lresult;

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

			STDMETHODIMP VideoSampleAllocatorTransform::GetStreamIDs(
				DWORD aInputIDArraySize,
				DWORD* aPtrInputIDs,
				DWORD aOutputIDArraySize,
				DWORD* aPtrOutputIDs)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP VideoSampleAllocatorTransform::GetStreamCount(
				DWORD* aPtrInputStreams,
				DWORD* aPtrOutputStreams)
			{
				HRESULT lresult;

				do
				{

					LOG_CHECK_STATE_DESCR(aPtrInputStreams == NULL || aPtrOutputStreams == NULL, E_POINTER);
					
					*aPtrInputStreams = 1;

					*aPtrOutputStreams = 1;


					lresult = S_OK;

				} while (false);
				
				return lresult;
			}

			STDMETHODIMP VideoSampleAllocatorTransform::GetInputStreamInfo(
				DWORD aInputStreamID,
				MFT_INPUT_STREAM_INFO* aPtrStreamInfo)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::unique_lock<std::mutex> lock(mMutex);

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

			STDMETHODIMP VideoSampleAllocatorTransform::GetOutputStreamInfo(
				DWORD aOutputStreamID,
				MFT_OUTPUT_STREAM_INFO* aPtrStreamInfo)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::unique_lock<std::mutex> lock(mMutex);

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

			STDMETHODIMP VideoSampleAllocatorTransform::GetInputStreamAttributes(
				DWORD aInputStreamID,
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP VideoSampleAllocatorTransform::GetOutputStreamAttributes(
				DWORD aOutputStreamID,
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP VideoSampleAllocatorTransform::DeleteInputStream(
				DWORD aStreamID)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP VideoSampleAllocatorTransform::AddInputStreams(
				DWORD aStreams,
				DWORD* aPtrStreamIDs)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP VideoSampleAllocatorTransform::GetInputAvailableType(
				DWORD aInputStreamID,
				DWORD aTypeIndex,
				IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;
				CComPtrCustom<IMFMediaType> lMediaType;

				do
				{
					std::unique_lock<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrPtrType);
					
					LOG_CHECK_STATE_DESCR(aInputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
					
					if (aTypeIndex == 0)
					{
						*aPtrPtrType = mInputMediaType.get();

						(*aPtrPtrType)->AddRef();
					}
					else
					{
						lresult = MF_E_NO_MORE_TYPES;
					}

				} while (false);

				if (FAILED(MF_E_INVALIDSTREAMNUMBER) && aPtrPtrType != NULL)
				{
					*aPtrPtrType = NULL;
				}
				
				return lresult;
			}

			STDMETHODIMP VideoSampleAllocatorTransform::GetOutputAvailableType(
				DWORD aOutputStreamID,
				DWORD aTypeIndex,
				IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;
				CComPtrCustom<IMFMediaType> lMediaType;

				do
				{
					std::unique_lock<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrPtrType);

					LOG_CHECK_STATE_DESCR(aOutputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
					
					*aPtrPtrType = mOutputMediaType.get();

					(*aPtrPtrType)->AddRef();

					lresult = MF_E_NO_MORE_TYPES;


				} while (false);
				
				return lresult;
			}

			STDMETHODIMP VideoSampleAllocatorTransform::SetInputType(
				DWORD aInputStreamID,
				IMFMediaType* aPtrType,
				DWORD aFlags)
			{
				HRESULT lresult = S_OK;

				do
				{

					LOG_CHECK_STATE_DESCR(aPtrType == nullptr, MF_E_INVALIDMEDIATYPE);
					
					std::unique_lock<std::mutex> lock(mMutex);
					
					LOG_CHECK_STATE_DESCR(aInputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
					
					BOOL lCompareResult = FALSE;

					LOG_INVOKE_MF_METHOD(Compare,
						mInputMediaType,
						aPtrType, 
						MF_ATTRIBUTES_MATCH_INTERSECTION, 
						&lCompareResult);
					
					LOG_CHECK_STATE_DESCR(lCompareResult == FALSE, MF_E_INVALIDMEDIATYPE);
					
					LOG_CHECK_STATE_DESCR(!(!mAllocator),
						MF_E_TRANSFORM_CANNOT_CHANGE_MEDIATYPE_WHILE_PROCESSING);
					
					if (aFlags != MFT_SET_TYPE_TEST_ONLY)
					{
						mInputMediaType = aPtrType;
					}

				} while (false);
				
				return lresult;
			}

			STDMETHODIMP VideoSampleAllocatorTransform::SetOutputType(
				DWORD aOutputStreamID,
				IMFMediaType* aPtrType,
				DWORD aFlags)
			{
				HRESULT lresult = S_OK;

				do
				{
					LOG_CHECK_STATE_DESCR(aPtrType == nullptr, MF_E_INVALIDMEDIATYPE);
					
					std::unique_lock<std::mutex> lock(mMutex);

					LOG_CHECK_STATE_DESCR(aOutputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
					
					BOOL lCompareResult = FALSE;

					LOG_INVOKE_MF_METHOD(Compare, mOutputMediaType,
						aPtrType, MF_ATTRIBUTES_MATCH_OUR_ITEMS, &lCompareResult);
					
					LOG_CHECK_STATE_DESCR(lCompareResult == FALSE, MF_E_INVALIDMEDIATYPE);
					
					LOG_CHECK_STATE_DESCR(!(!mAllocator),
						MF_E_TRANSFORM_CANNOT_CHANGE_MEDIATYPE_WHILE_PROCESSING);
					
					if (aFlags != MFT_SET_TYPE_TEST_ONLY)
					{
						mOutputMediaType = aPtrType;

						lresult = S_OK;
					}

				} while (false);
				
				return lresult;
			}

			STDMETHODIMP VideoSampleAllocatorTransform::GetInputCurrentType(
				DWORD aInputStreamID,
				IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;

				do
				{

					std::unique_lock<std::mutex> lock(mMutex);
					
					LOG_CHECK_PTR_MEMORY(aPtrPtrType);

					LOG_CHECK_STATE_DESCR(aInputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
					
					LOG_CHECK_STATE_DESCR(!mInputMediaType, MF_E_TRANSFORM_TYPE_NOT_SET);

					*aPtrPtrType = mInputMediaType;

					(*aPtrPtrType)->AddRef();

				} while (false);

				return lresult;
			}

			STDMETHODIMP VideoSampleAllocatorTransform::GetOutputCurrentType(
				DWORD aOutputStreamID,
				IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::unique_lock<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrPtrType);
					
					LOG_CHECK_STATE_DESCR(aOutputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
					
					LOG_CHECK_STATE_DESCR(!mOutputMediaType,
						MF_E_TRANSFORM_TYPE_NOT_SET);

					*aPtrPtrType = mOutputMediaType;
					
					(*aPtrPtrType)->AddRef();

				} while (false);

				return lresult;
			}

			STDMETHODIMP VideoSampleAllocatorTransform::GetInputStatus(
				DWORD aInputStreamID,
				DWORD* aPtrFlags)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::unique_lock<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrFlags);
					
					LOG_CHECK_STATE_DESCR(aInputStreamID != 0,
						MF_E_INVALIDSTREAMNUMBER);
					
					if (!mAllocator)
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

			STDMETHODIMP VideoSampleAllocatorTransform::GetOutputStatus(
				DWORD* aPtrFlags)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP VideoSampleAllocatorTransform::SetOutputBounds(
				LONGLONG aLowerBound,
				LONGLONG aUpperBound)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP VideoSampleAllocatorTransform::ProcessEvent(
				DWORD aInputStreamID,
				IMFMediaEvent* aPtrEvent)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP VideoSampleAllocatorTransform::GetAttributes(
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP VideoSampleAllocatorTransform::ProcessMessage(
				MFT_MESSAGE_TYPE aMessage,
				ULONG_PTR aParam)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::unique_lock<std::mutex> lock(mMutex);

					if (aMessage == MFT_MESSAGE_COMMAND_FLUSH)
					{
						if (mAllocator)
						{
							mAllocator->UninitializeSampleAllocator();

							mAllocator.Release();
						}
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

			STDMETHODIMP VideoSampleAllocatorTransform::ProcessInput(
				DWORD aInputStreamID,
				IMFSample* aPtrSample,
				DWORD aFlags)
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
							
							LOG_INVOKE_MF_METHOD(Lock,
								aPtrInputBuffer,
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
						HRESULT lresult;

						do
						{

							if (mInputBuffer)
							{
								LOG_INVOKE_MF_METHOD(Unlock,
									mInputBuffer);
							}

						} while (false);
					}

				private:
					
					CComPtrCustom<IMFMediaBuffer> mInputBuffer;
					
					MediaBufferLock(
						const MediaBufferLock&){}

					MediaBufferLock& operator=(
						const MediaBufferLock&){
						return *this;
					}

				};
				
				HRESULT lresult = S_OK;

				do
				{
					std::unique_lock<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrSample);
					
					LOG_CHECK_STATE(aInputStreamID != 0 || aFlags != 0);
					
					LOG_CHECK_STATE_DESCR(!mInputMediaType, MF_E_NOTACCEPTING);
										
					LOG_CHECK_STATE_DESCR(mIsReady, MF_E_NOTACCEPTING);
					
					LOG_CHECK_STATE_DESCR(!mAllocator, MF_E_NOTACCEPTING);

					mSample.Release();

					LOG_INVOKE_POINTER_METHOD(mAllocator, AllocateSample, &mSample);

					CComPtrCustom<IMFMediaBuffer> lBuffer;

					LOG_INVOKE_POINTER_METHOD(mSample, GetBufferByIndex,
						0,
						&lBuffer);
					
					CComPtrCustom<IMF2DBuffer> l2DBuffer;
					
					LOG_INVOKE_QUERY_INTERFACE_METHOD(lBuffer, &l2DBuffer);

					LOG_CHECK_PTR_MEMORY(l2DBuffer);
					
					LONGLONG lSampleTime;

					LOG_INVOKE_MF_METHOD(GetSampleTime,
						aPtrSample, &lSampleTime);

					LONGLONG lSampleDuration;

					LOG_INVOKE_MF_METHOD(GetSampleDuration,
						aPtrSample,
						&lSampleDuration);

					CComPtrCustom<IMFMediaBuffer> lInputBuffer;

					LOG_INVOKE_MF_METHOD(GetBufferByIndex,
						aPtrSample,
						0,
						&lInputBuffer);
					
					{

						DWORD lMaxLength;

						DWORD lCurrentLength;

						BYTE* lPtrInputBuffer;

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

						DWORD l;

						LOG_INVOKE_MF_METHOD(GetContiguousLength,
							l2DBuffer,
							&l);

						LOG_INVOKE_MF_METHOD(ContiguousCopyFrom,
							l2DBuffer,
							lPtrInputBuffer,
							lCurrentLength);
					}
					
					LOG_INVOKE_MF_METHOD(SetSampleTime,
						mSample,
						lSampleTime);
					
					LOG_INVOKE_MF_METHOD(SetSampleDuration,
						mSample,
						lSampleDuration);
					
					mIsReady = true;
					

				} while (false);
				
				return lresult;
			}

			STDMETHODIMP VideoSampleAllocatorTransform::ProcessOutput(
				DWORD aFlags,
				DWORD aOutputBufferCount,
				MFT_OUTPUT_DATA_BUFFER* aPtrOutputSamples,
				DWORD* aPtrStatus)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::unique_lock<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrOutputSamples);

					LOG_CHECK_PTR_MEMORY(aPtrStatus);

					LOG_CHECK_STATE(aOutputBufferCount != 1 || aFlags != 0);
					
					if (!mIsReady)
					{
						lresult = MF_E_TRANSFORM_NEED_MORE_INPUT;

						break;
					}
					
					aPtrOutputSamples[0].pSample = mSample;

					mSample->AddRef();

					aPtrOutputSamples[0].dwStatus = 0;

					*aPtrStatus = 0;

					mIsReady = false;

				} while (false);
				
				return lresult;
			}

			HRESULT VideoSampleAllocatorTransform::init(
				IMFVideoSampleAllocator*  aPtrIMFVideoSampleAllocator,
				IMFMediaType* aPtrMediaType)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrIMFVideoSampleAllocator);

					LOG_CHECK_PTR_MEMORY(aPtrMediaType);
										
					mAllocator = aPtrIMFVideoSampleAllocator;

					LOG_CHECK_PTR_MEMORY(mAllocator);
					
					mInputMediaType = aPtrMediaType;
					
					lresult = S_OK;

				} while (false);

				return lresult;
			}
		}
	}
}