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

#include "SampleAccumulator.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/Common.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../MemoryManager/MemoryManager.h"
#include "../Common/Singleton.h"

namespace CaptureManager
{
	namespace Transform
	{
		namespace Accumulator
		{
			using namespace CaptureManager::Core;

			SampleAccumulator::SampleAccumulator(
				UINT32 aAccumulatorSize) :
				mAccumulatorSize(aAccumulatorSize),
				mPtrOutputSampleAccumulator(nullptr),
				mEndOfStream(false),
				mCurrentLength(0)
			{				
				mPtrInputSampleAccumulator = &mFirstSampleAccumulator;

				mPtrOutputSampleAccumulator = &mSecondSampleAccumulator;

				Singleton<MemoryManager>::getInstance().initialize();
			}
						
			SampleAccumulator::~SampleAccumulator()
			{
			}

			STDMETHODIMP SampleAccumulator::GetStreamLimits(DWORD* aPtrInputMinimum, DWORD* aPtrInputMaximum,
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

			STDMETHODIMP SampleAccumulator::GetStreamIDs(DWORD aInputIDArraySize, DWORD* aPtrInputIDs,
				DWORD aOutputIDArraySize, DWORD* aPtrOutputIDs)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP SampleAccumulator::GetStreamCount(DWORD* aPtrInputStreams, DWORD* aPtrOutputStreams)
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

			STDMETHODIMP SampleAccumulator::GetInputStreamInfo(DWORD aInputStreamID,
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

			STDMETHODIMP SampleAccumulator::GetOutputStreamInfo(DWORD aOutputStreamID,
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

			STDMETHODIMP SampleAccumulator::GetInputStreamAttributes(DWORD aInputStreamID,
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP SampleAccumulator::GetOutputStreamAttributes(DWORD aOutputStreamID,
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP SampleAccumulator::DeleteInputStream(DWORD aStreamID)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP SampleAccumulator::AddInputStreams(DWORD aStreams, DWORD* aPtrStreamIDs)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP SampleAccumulator::GetInputAvailableType(DWORD aInputStreamID, DWORD aTypeIndex,
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
						*aPtrPtrType = lMediaType.Detach();
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

			STDMETHODIMP SampleAccumulator::GetOutputAvailableType(DWORD aOutputStreamID, DWORD aTypeIndex,
				IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;
				CComPtrCustom<IMFMediaType> lMediaType;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrPtrType);

					LOG_CHECK_STATE_DESCR(aOutputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
					
					if (!mOutputMediaType)
					{
						*aPtrPtrType = lMediaType.get();
						(*aPtrPtrType)->AddRef();
					}
					else
					{
						*aPtrPtrType = mOutputMediaType.get();

						(*aPtrPtrType)->AddRef();
					}

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleAccumulator::SetInputType(DWORD aInputStreamID, IMFMediaType* aPtrType,
				DWORD aFlags)
			{
				HRESULT lresult = S_OK;
				CComPtrCustom<IMFAttributes> lTypeAttributes;

				do
				{
					lTypeAttributes = aPtrType;

					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_STATE_DESCR(aInputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
										
					LOG_CHECK_STATE_DESCR(!mFirstSampleAccumulator.empty() || !mSecondSampleAccumulator.empty(),
						MF_E_TRANSFORM_CANNOT_CHANGE_MEDIATYPE_WHILE_PROCESSING);
					
					if (aPtrType != nullptr && !(!mInputMediaType))
					{
						BOOL lBoolResult = FALSE;

						LOG_INVOKE_MF_METHOD(Compare,
							aPtrType,
							lTypeAttributes, 
							MF_ATTRIBUTES_MATCH_INTERSECTION, 
							&lBoolResult);

						if (lBoolResult == FALSE)
						{
							lresult = MF_E_INVALIDMEDIATYPE;

							break;
						}
					}


					if (aFlags != MFT_SET_TYPE_TEST_ONLY)
					{
						mInputMediaType = aPtrType;	

						PROPVARIANT lVarItem;

						LOG_INVOKE_MF_METHOD(GetItem,
							mInputMediaType,
							MF_MT_FRAME_SIZE,
							&lVarItem);

						UINT32 lHigh = 0, lLow = 0;

						DataParser::unpack2UINT32AsUINT64(lVarItem, lHigh, lLow);

						LONG lstride = 0;

						do
						{
							LOG_INVOKE_MF_METHOD(GetUINT32,
								mInputMediaType,
								MF_MT_DEFAULT_STRIDE,
								((UINT32*)&lstride));

						} while (false);

						//if (FAILED(lresult))
						//{
						//	GUID lSubType;

						//	LOG_INVOKE_MF_METHOD(GetGUID,
						//		mInputMediaType,
						//		MF_MT_SUBTYPE,
						//		&lSubType);
						//	
						//	lresult = LOG_INVOKE_MF_FUNCTION(MFGetStrideForBitmapInfoHeader,
						//		lSubType.Data1,
						//		lHigh,
						//		&lstride);

						//	LOG_CHECK_STATE(lstride == 0);

						//	LOG_INVOKE_MF_METHOD(SetUINT32,
						//		mInputMediaType,
						//		MF_MT_DEFAULT_STRIDE,
						//		*((UINT32*)&lstride));
						//}

						if (SUCCEEDED(lresult))
							mCurrentLength = lLow * ::abs(lstride);

						lresult = S_OK;

						mOutputMediaType = aPtrType;
					}

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleAccumulator::SetOutputType(DWORD aOutputStreamID, IMFMediaType* aPtrType,
				DWORD aFlags)
			{
				HRESULT lresult = S_OK;

				CComPtrCustom<IMFMediaType> lType;

				do
				{
					lType = aPtrType;

					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_STATE_DESCR(aOutputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
										
					LOG_CHECK_STATE_DESCR(!mFirstSampleAccumulator.empty() || !mSecondSampleAccumulator.empty(), 
						MF_E_TRANSFORM_CANNOT_CHANGE_MEDIATYPE_WHILE_PROCESSING);
					
					if (!(!lType) && !(!mInputMediaType))
					{
						DWORD flags = 0;

						LOG_INVOKE_MF_METHOD(IsEqual, 
							lType,
							mInputMediaType, 
							&flags);
					}

					if (aFlags != MFT_SET_TYPE_TEST_ONLY)
					{
						mOutputMediaType = lType.Detach();
					}

				} while (false);


				return lresult;
			}

			STDMETHODIMP SampleAccumulator::GetInputCurrentType(DWORD aInputStreamID, IMFMediaType** aPtrPtrType)
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

			STDMETHODIMP SampleAccumulator::GetOutputCurrentType(DWORD aOutputStreamID, IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrPtrType);
					
					LOG_CHECK_STATE_DESCR(aOutputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
					
					LOG_CHECK_STATE_DESCR(!mOutputMediaType, MF_E_TRANSFORM_TYPE_NOT_SET);
					
					*aPtrPtrType = mOutputMediaType;
					(*aPtrPtrType)->AddRef();

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleAccumulator::GetInputStatus(DWORD aInputStreamID, DWORD* aPtrFlags)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrFlags);

					LOG_CHECK_STATE_DESCR(aInputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
									
					*aPtrFlags = MFT_INPUT_STATUS_ACCEPT_DATA;

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleAccumulator::GetOutputStatus(DWORD* aPtrFlags)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP SampleAccumulator::SetOutputBounds(LONGLONG aLowerBound, LONGLONG aUpperBound)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP SampleAccumulator::ProcessEvent(DWORD aInputStreamID, IMFMediaEvent* aPtrEvent)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP SampleAccumulator::GetAttributes(IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP SampleAccumulator::ProcessMessage(MFT_MESSAGE_TYPE aMessage, ULONG_PTR aParam)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					if (aMessage == MFT_MESSAGE_COMMAND_FLUSH)
					{
						while (!mFirstSampleAccumulator.empty())
						{
							mFirstSampleAccumulator.pop();
						}

						while (!mSecondSampleAccumulator.empty())
						{
							mSecondSampleAccumulator.pop();
						}
					}
					else if (aMessage == MFT_MESSAGE_COMMAND_DRAIN)
					{
						while (!mFirstSampleAccumulator.empty())
						{
							mFirstSampleAccumulator.pop();
						}

						while (!mSecondSampleAccumulator.empty())
						{
							mSecondSampleAccumulator.pop();
						}
					}
					else if (aMessage == MFT_MESSAGE_NOTIFY_BEGIN_STREAMING)
					{
					}
					else if (aMessage == MFT_MESSAGE_NOTIFY_END_STREAMING)
					{
					}
					else if (aMessage == MFT_MESSAGE_NOTIFY_END_OF_STREAM)
					{
						mEndOfStream = true;
					}
					else if (aMessage == MFT_MESSAGE_NOTIFY_START_OF_STREAM)
					{
					}

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleAccumulator::ProcessInput(DWORD aInputStreamID, IMFSample* aPtrSample,
				DWORD aFlags)
			{
				HRESULT lresult = S_OK;
				DWORD dwBufferCount = 0;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrSample);
					
					LOG_CHECK_STATE(aInputStreamID != 0 || aFlags != 0);
					
					LOG_CHECK_STATE_DESCR(!mInputMediaType, MF_E_NOTACCEPTING);

					LOG_CHECK_STATE_DESCR(!mOutputMediaType, MF_E_NOTACCEPTING);


					CComPtrCustom<IMFSample> lUnk;
					
					LOG_INVOKE_FUNCTION(copySample,
						aPtrSample, &lUnk);
					
					if (mPtrInputSampleAccumulator->size() >= mAccumulatorSize )
					{
						mPtrInputSampleAccumulator->front().Release();

						mPtrInputSampleAccumulator->pop();
					}
					
					mPtrInputSampleAccumulator->push(lUnk);
										
					lresult = S_FALSE;// MF_E_TRANSFORM_NEED_MORE_INPUT;

				} while (false);

				return lresult;
			}

			HRESULT SampleAccumulator::copySample(
				IMFSample* aPtrOriginalSample,
				IMFSample** aPtrPtrCopySample)
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

							LOG_INVOKE_POINTER_METHOD(aPtrInputBuffer, Lock,
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
						const MediaBufferLock&){
						return *this;
					}

				};

				HRESULT lresult;

				CComPtrCustom<IMFSample> lOutputSample;

				CComPtrCustom<IMFMediaBuffer> lMediaBuffer;

				CComPtrCustom<IMFMediaBuffer> lOriginalMediaBuffer;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrOriginalSample);

					LOG_CHECK_PTR_MEMORY(aPtrPtrCopySample);

					LOG_INVOKE_MF_METHOD(GetBufferByIndex, aPtrOriginalSample, 
						0, 
						&lOriginalMediaBuffer);

					DWORD lCurrentLength;

					LOG_INVOKE_MF_METHOD(GetCurrentLength, lOriginalMediaBuffer, 
						&lCurrentLength);

					LOG_INVOKE_MF_FUNCTION(MFCreateSample,
						&lOutputSample);

					LOG_INVOKE_MF_FUNCTION(MFCreateMemoryBuffer,
						lCurrentLength,
						&lMediaBuffer);

					LOG_INVOKE_MF_FUNCTION(SetCurrentLength, lMediaBuffer,
						lCurrentLength);

					LOG_INVOKE_MF_METHOD(AddBuffer,
						lOutputSample,
						lMediaBuffer);

					LOG_INVOKE_MF_METHOD(CopyAllItems, aPtrOriginalSample, 
						lOutputSample);

					MFTIME lTime;

					LOG_INVOKE_MF_METHOD(GetSampleDuration, aPtrOriginalSample, 
						&lTime);

					LOG_INVOKE_MF_METHOD(SetSampleDuration, lOutputSample, lTime);

					LOG_INVOKE_MF_METHOD(GetSampleTime, aPtrOriginalSample, &lTime);

					LOG_INVOKE_MF_METHOD(SetSampleTime, lOutputSample, lTime);


					DWORD lMaxDestLength;

					DWORD lCurrentDestLength;

					BYTE* lPtrDestBuffer;


					MediaBufferLock lMediaBufferLock(
						lMediaBuffer,
						lMaxDestLength,
						lCurrentDestLength,
						&lPtrDestBuffer,
						lresult);

					if (FAILED(lresult))
					{
						break;
					}


					DWORD lMaxScrLength;

					DWORD lCurrentScrLength;

					BYTE* lPtrScrBuffer;


					MediaBufferLock lScrMediaBufferLock(
						lOriginalMediaBuffer,
						lMaxScrLength,
						lCurrentScrLength,
						&lPtrScrBuffer,
						lresult);

					if (FAILED(lresult))
					{
						break;
					}
					
					MemoryManager::memcpy(lPtrDestBuffer, lPtrScrBuffer, lCurrentLength > lCurrentScrLength ? lCurrentScrLength : lCurrentLength);
										
					LOG_INVOKE_QUERY_INTERFACE_METHOD(lOutputSample, aPtrPtrCopySample);

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleAccumulator::ProcessOutput(DWORD aFlags, DWORD aOutputBufferCount,
				MFT_OUTPUT_DATA_BUFFER* aPtrOutputSamples, DWORD* aPtrStatus)
			{
				HRESULT lresult = S_OK;

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrOutputSamples);
					
					LOG_CHECK_PTR_MEMORY(aPtrStatus);

					LOG_CHECK_STATE_DESCR(aOutputBufferCount != 1 || aFlags != 0, E_INVALIDARG);
					
					//LOG_CHECK_STATE_DESCR(!mSample, MF_E_TRANSFORM_NEED_MORE_INPUT);

					CComPtrCustom<IMFSample> lOutputSample;

					{

						std::lock_guard<std::mutex> lock(mMutex);

						if (mEndOfStream)
						{							
							aPtrOutputSamples[0].pSample = lOutputSample.Detach();

							aPtrOutputSamples[0].dwStatus = 0;

							*aPtrStatus = 0;

							lresult = MF_E_TRANSFORM_NEED_MORE_INPUT;

							break;
						}

						if (mFirstSampleAccumulator.empty() && mSecondSampleAccumulator.empty())
						{
							CComPtrCustom<IMFMediaBuffer> lMediaBuffer;

							LOG_INVOKE_MF_FUNCTION(MFCreateSample,
								&lOutputSample);

							LOG_INVOKE_MF_FUNCTION(MFCreateMemoryBuffer,
								mCurrentLength,
								&lMediaBuffer);

							LOG_CHECK_PTR_MEMORY(lMediaBuffer);

							LOG_INVOKE_MF_METHOD(SetCurrentLength,
								lMediaBuffer,
								mCurrentLength);

							LOG_INVOKE_MF_METHOD(AddBuffer,
								lOutputSample,
								lMediaBuffer);
							
							aPtrOutputSamples[0].pSample = lOutputSample.Detach();

							aPtrOutputSamples[0].dwStatus = 0;

							*aPtrStatus = 0;

							break;
						}
						else if (mPtrOutputSampleAccumulator->empty())
						{
							auto ltempPtr = mPtrOutputSampleAccumulator;

							mPtrOutputSampleAccumulator = mPtrInputSampleAccumulator;

							mPtrInputSampleAccumulator = ltempPtr;
							
							//CComPtrCustom<IMFMediaBuffer> lMediaBuffer;

							//LOG_INVOKE_MF_FUNCTION(MFCreateSample,
							//	&lOutputSample);

							//LOG_INVOKE_MF_FUNCTION(MFCreateMemoryBuffer,
							//	1,
							//	&lMediaBuffer);

							//LOG_INVOKE_MF_METHOD(AddBuffer,
							//	lOutputSample,
							//	lMediaBuffer);

							//lMediaBuffer->SetCurrentLength(1);

							//aPtrOutputSamples[0].pSample = lOutputSample.Detach();

							//aPtrOutputSamples[0].dwStatus = 0;

							//*aPtrStatus = 0;

							//break;
						}

					}


					aPtrOutputSamples[0].pSample = mPtrOutputSampleAccumulator->front().Detach();

					mPtrOutputSampleAccumulator->pop();

					aPtrOutputSamples[0].dwStatus = 0;

					*aPtrStatus = 0;

				} while (false);

				return lresult;
			}

		}
	}
}