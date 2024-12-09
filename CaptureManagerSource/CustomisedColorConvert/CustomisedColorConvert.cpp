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

#include "CustomisedColorConvert.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/Common.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../SampleConvertor/SampleConvertor.h"

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			using namespace CaptureManager::Core;

			CustomisedColorConvert::CustomisedColorConvert(){}

			CustomisedColorConvert::~CustomisedColorConvert(){}
			
			HRESULT CustomisedColorConvert::create(
				IMFMediaType* aPtrInputType,
				IMFMediaType* aPtrOutputType,
				IMFTransform** aPtrPtrTransform)
			{
				HRESULT lresult = E_FAIL;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrInputType);

					LOG_CHECK_PTR_MEMORY(aPtrOutputType);

					LOG_CHECK_PTR_MEMORY(aPtrPtrTransform);



					GUID lInputSubType;

					LOG_INVOKE_MF_METHOD(GetGUID, aPtrInputType,
						MF_MT_SUBTYPE,
						&lInputSubType);



					GUID lOutputSubType;

					LOG_INVOKE_MF_METHOD(GetGUID, aPtrOutputType,
						MF_MT_SUBTYPE,
						&lOutputSubType);

					LOG_INVOKE_FUNCTION(checkMediaType,
						aPtrInputType,
						aPtrOutputType);

					CComPtrCustom<CustomisedColorConvert> lCustomisedColorConvert(new CustomisedColorConvert());

					if (lInputSubType == MFVideoFormat_RGB24 &&
						lOutputSubType == MFVideoFormat_NV12)
					{
						LOG_INVOKE_FUNCTION(SampleConvertor::createRgbToNV12,
							aPtrInputType,
							aPtrOutputType,
							&lCustomisedColorConvert->mSampleConvertor);

						LOG_CHECK_PTR_MEMORY(lCustomisedColorConvert->mSampleConvertor);
											   
						lCustomisedColorConvert->mInputMediaType = aPtrInputType;

						lCustomisedColorConvert->mOutputMediaType = aPtrOutputType;
					}
					else
					{
						lresult = E_FAIL;

						break;
					}

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lCustomisedColorConvert, aPtrPtrTransform);

				} while (false);

				return lresult;
			}

			HRESULT CustomisedColorConvert::checkMediaType(
				IMFMediaType* aPtrInputType,
				IMFMediaType* aPtrOutputType)
			{
				HRESULT lresult = E_FAIL;

				do
				{
					PROPVARIANT lInputFrameSizeVarItem;

					LOG_INVOKE_MF_METHOD(GetItem, aPtrInputType,
						MF_MT_FRAME_SIZE,
						&lInputFrameSizeVarItem);


					PROPVARIANT lOutputFrameSizeVarItem;

					LOG_INVOKE_MF_METHOD(GetItem, aPtrOutputType,
						MF_MT_FRAME_SIZE,
						&lOutputFrameSizeVarItem);
															
					LOG_CHECK_STATE_DESCR(lInputFrameSizeVarItem.uhVal.QuadPart != lOutputFrameSizeVarItem.uhVal.QuadPart, MF_E_INVALIDMEDIATYPE);
					
				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedColorConvert::GetStreamLimits(DWORD* aPtrInputMinimum, DWORD* aPtrInputMaximum,
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

			STDMETHODIMP CustomisedColorConvert::GetStreamIDs(DWORD aInputIDArraySize, DWORD* aPtrInputIDs,
				DWORD aOutputIDArraySize, DWORD* aPtrOutputIDs)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedColorConvert::GetStreamCount(DWORD* aPtrInputStreams, DWORD* aPtrOutputStreams)
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

			STDMETHODIMP CustomisedColorConvert::GetInputStreamInfo(DWORD aInputStreamID,
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

			STDMETHODIMP CustomisedColorConvert::GetOutputStreamInfo(DWORD aOutputStreamID,
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

			STDMETHODIMP CustomisedColorConvert::GetInputStreamAttributes(DWORD aInputStreamID,
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedColorConvert::GetOutputStreamAttributes(DWORD aOutputStreamID,
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedColorConvert::DeleteInputStream(DWORD aStreamID)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedColorConvert::AddInputStreams(DWORD aStreams, DWORD* aPtrStreamIDs)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedColorConvert::GetInputAvailableType(DWORD aInputStreamID, DWORD aTypeIndex,
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

				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedColorConvert::GetOutputAvailableType(DWORD aOutputStreamID, DWORD aTypeIndex,
				IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;
				CComPtrCustom<IMFMediaType> lMediaType;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrPtrType);

					LOG_CHECK_STATE_DESCR(aOutputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
					
				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedColorConvert::SetInputType(DWORD aInputStreamID, IMFMediaType* aPtrType,
				DWORD aFlags)
			{
				HRESULT lresult = S_OK;
				CComPtrCustom<IMFAttributes> lTypeAttributes;

				do
				{
					lTypeAttributes = aPtrType;

					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_STATE_DESCR(aInputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);
					
				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedColorConvert::SetOutputType(DWORD aOutputStreamID, IMFMediaType* aPtrType,
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

			STDMETHODIMP CustomisedColorConvert::GetInputCurrentType(DWORD aInputStreamID, IMFMediaType** aPtrPtrType)
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

			STDMETHODIMP CustomisedColorConvert::GetOutputCurrentType(DWORD aOutputStreamID, IMFMediaType** aPtrPtrType)
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

			STDMETHODIMP CustomisedColorConvert::GetInputStatus(DWORD aInputStreamID, DWORD* aPtrFlags)
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

			STDMETHODIMP CustomisedColorConvert::GetOutputStatus(DWORD* aPtrFlags)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedColorConvert::SetOutputBounds(LONGLONG aLowerBound, LONGLONG aUpperBound)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedColorConvert::ProcessEvent(DWORD aInputStreamID, IMFMediaEvent* aPtrEvent)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedColorConvert::GetAttributes(IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedColorConvert::ProcessMessage(MFT_MESSAGE_TYPE aMessage, ULONG_PTR aParam)
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

			STDMETHODIMP CustomisedColorConvert::ProcessInput(DWORD aInputStreamID, IMFSample* aPtrSample,
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

					LOG_CHECK_STATE_DESCR(!mSampleConvertor, MF_E_NOTACCEPTING);

					mSample.Release();

					mSampleConvertor->Convert(aPtrSample, &mSample);

				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedColorConvert::ProcessOutput(DWORD aFlags, DWORD aOutputBufferCount,
				MFT_OUTPUT_DATA_BUFFER* aPtrOutputSamples, DWORD* aPtrStatus)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrOutputSamples);
					
					LOG_CHECK_PTR_MEMORY(aPtrStatus);

					LOG_CHECK_STATE_DESCR(aOutputBufferCount != 1 || aFlags != 0, E_INVALIDARG);

					if (!mSample)
					{
						lresult = MF_E_TRANSFORM_NEED_MORE_INPUT;

						break;
					}
														
					aPtrOutputSamples[0].pSample = mSample.Detach();

					aPtrOutputSamples[0].dwStatus = 0;

					*aPtrStatus = 0;

				} while (false);

				return lresult;
			}
		}
	}
}