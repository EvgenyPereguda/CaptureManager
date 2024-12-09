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

#include "CustomisedAudioResampler.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "BitsAudioResampler.h"
#include "../Common/Singleton.h"
#include "../CPUDetection/Cpu.h"


namespace CaptureManager
{
	namespace Transform
	{
		namespace Audio
		{
			using namespace Core;


			STDMETHODIMP CustomisedAudioResampler::GetStreamLimits(DWORD* aPtrInputMinimum, DWORD* aPtrInputMaximum,
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

			STDMETHODIMP CustomisedAudioResampler::GetStreamIDs(DWORD aInputIDArraySize, DWORD* aPtrInputIDs,
				DWORD aOutputIDArraySize, DWORD* aPtrOutputIDs)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedAudioResampler::GetStreamCount(DWORD* aPtrInputStreams, DWORD* aPtrOutputStreams)
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

			STDMETHODIMP CustomisedAudioResampler::GetInputStreamInfo(DWORD aInputStreamID,
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

			STDMETHODIMP CustomisedAudioResampler::GetOutputStreamInfo(DWORD aOutputStreamID,
				MFT_OUTPUT_STREAM_INFO* aPtrStreamInfo)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_INVOKE_MF_METHOD(GetOutputStreamInfo, mAudioResamplerTransform,
						aOutputStreamID,
						aPtrStreamInfo);

				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedAudioResampler::GetInputStreamAttributes(DWORD aInputStreamID,
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedAudioResampler::GetOutputStreamAttributes(DWORD aOutputStreamID,
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedAudioResampler::DeleteInputStream(DWORD aStreamID)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedAudioResampler::AddInputStreams(DWORD aStreams, DWORD* aPtrStreamIDs)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedAudioResampler::GetInputAvailableType(DWORD aInputStreamID, DWORD aTypeIndex,
				IMFMediaType** aPtrPtrType)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedAudioResampler::GetOutputAvailableType(DWORD aOutputStreamID, DWORD aTypeIndex,
				IMFMediaType** aPtrPtrType)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedAudioResampler::SetInputType(DWORD aInputStreamID, IMFMediaType* aPtrType,
				DWORD aFlags)
			{
				HRESULT lresult = S_OK;
				CComPtrCustom<IMFAttributes> lTypeAttributes;

				do
				{
					lTypeAttributes = aPtrType;

					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_STATE_DESCR(aInputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);

					LOG_INVOKE_FUNCTION(checkMediaType, aPtrType);

					LOG_CHECK_STATE_DESCR(!(!mSample), MF_E_TRANSFORM_CANNOT_CHANGE_MEDIATYPE_WHILE_PROCESSING);

					if (aFlags != MFT_SET_TYPE_TEST_ONLY)
					{
						mInputMediaType = aPtrType;

						if (mInputMediaType && mOutputMediaType)
							LOG_INVOKE_FUNCTION(createProcessor);
					}

				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedAudioResampler::SetOutputType(DWORD aOutputStreamID, IMFMediaType* aPtrType,
				DWORD aFlags)
			{
				HRESULT lresult = S_OK;

				CComPtrCustom<IMFMediaType> lType;

				do
				{
					lType = aPtrType;

					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_STATE_DESCR(aOutputStreamID != 0, MF_E_INVALIDSTREAMNUMBER);

					LOG_INVOKE_FUNCTION(checkMediaType, lType);

					LOG_CHECK_STATE_DESCR(!(!mSample), MF_E_TRANSFORM_CANNOT_CHANGE_MEDIATYPE_WHILE_PROCESSING);

					if (aFlags != MFT_SET_TYPE_TEST_ONLY)
					{
						mOutputMediaType = lType.Detach();

						if (mInputMediaType && mOutputMediaType)
							LOG_INVOKE_FUNCTION(createProcessor);
					}

				} while (false);


				return lresult;
			}

			STDMETHODIMP CustomisedAudioResampler::GetInputCurrentType(DWORD aInputStreamID, IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;

				do
				{

					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrPtrType);

					if (aInputStreamID != 0)
					{
						lresult = MF_E_INVALIDSTREAMNUMBER;
					}
					else if (!mInputMediaType)
					{
						lresult = MF_E_TRANSFORM_TYPE_NOT_SET;
					}
					else
					{
						LOG_INVOKE_QUERY_INTERFACE_METHOD(mInputMediaType, aPtrPtrType);
					}

				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedAudioResampler::GetOutputCurrentType(DWORD aOutputStreamID, IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrPtrType);

					if (aOutputStreamID != 0)
					{
						lresult = MF_E_INVALIDSTREAMNUMBER;
					}
					else if (!mOutputMediaType)
					{
						lresult = MF_E_TRANSFORM_TYPE_NOT_SET;
					}
					else
					{
						LOG_INVOKE_QUERY_INTERFACE_METHOD(mOutputMediaType, aPtrPtrType);
					}

				} while (false);

				return lresult;
			}

			STDMETHODIMP CustomisedAudioResampler::GetInputStatus(DWORD aInputStreamID, DWORD* aPtrFlags)
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

			STDMETHODIMP CustomisedAudioResampler::GetOutputStatus(DWORD* aPtrFlags)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedAudioResampler::SetOutputBounds(LONGLONG aLowerBound, LONGLONG aUpperBound)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedAudioResampler::ProcessEvent(DWORD aInputStreamID, IMFMediaEvent* aPtrEvent)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedAudioResampler::GetAttributes(IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP CustomisedAudioResampler::ProcessMessage(MFT_MESSAGE_TYPE aMessage, ULONG_PTR aParam)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					if (aMessage == MFT_MESSAGE_COMMAND_FLUSH)
					{
						mSample = nullptr;
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

			STDMETHODIMP CustomisedAudioResampler::ProcessInput(DWORD aInputStreamID, IMFSample* aPtrSample,
				DWORD aFlags)
			{
				HRESULT lresult = S_OK;
				DWORD dwBufferCount = 0;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrSample);

					LOG_CHECK_STATE(aInputStreamID != 0 || aFlags != 0);

					LOG_CHECK_STATE_DESCR(!mInputMediaType, MF_E_TRANSFORM_NOT_POSSIBLE_FOR_CURRENT_INPUT_MEDIATYPE);

					LOG_CHECK_STATE_DESCR(!(!mSample), MF_E_NOTACCEPTING);

					LOG_INVOKE_MF_METHOD(ProcessInput, mAudioResamplerTransform,
						aFlags,
						aPtrSample,
						aFlags);

					mSample = aPtrSample;

				} while (false);

				if (FAILED(lresult) && lresult == 0xc00d36b5)
					lresult = S_OK;

				return lresult;
			}

			STDMETHODIMP CustomisedAudioResampler::ProcessOutput(DWORD aFlags, DWORD aOutputBufferCount,
				MFT_OUTPUT_DATA_BUFFER* aPtrOutputSamples, DWORD* aPtrStatus)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrOutputSamples);

					LOG_CHECK_PTR_MEMORY(aPtrStatus);

					LOG_CHECK_STATE(aOutputBufferCount != 1 || aFlags != 0);

					if (!mSample)
					{
						lresult = MF_E_TRANSFORM_NEED_MORE_INPUT;

						break;
					}

					if (mCustomizedResampler == FALSE)
					{
						UINT32 lAvg_ByteRate;

						LOG_INVOKE_MF_METHOD(GetUINT32, mOutputMediaType,
							MF_MT_AUDIO_AVG_BYTES_PER_SECOND,
							&lAvg_ByteRate);

						CComPtrCustom<IMFMediaBuffer> lOutputMediaBuffer;

						LOG_INVOKE_MF_FUNCTION(MFCreateMemoryBuffer, lAvg_ByteRate, &lOutputMediaBuffer);

						BYTE* lOutputScanline0;

						LOG_INVOKE_MF_METHOD(Lock, lOutputMediaBuffer, &lOutputScanline0, NULL, NULL);

						DWORD lOutputCurrentLength = 0;


						for (;;)
						{
							lresult = mAudioResamplerTransform->ProcessOutput(
								aFlags, aOutputBufferCount,
								aPtrOutputSamples, aPtrStatus);

							if (MF_E_TRANSFORM_NEED_MORE_INPUT == lresult)
							{
								break;
							}
							if (FAILED(lresult))
							{
								break;
							}

							if (aPtrOutputSamples->pSample != nullptr)
							{
								DWORD lTotalLength;

								CComPtrCustom<IMFMediaBuffer> lInputMediaBuffer;

								LOG_INVOKE_MF_METHOD(GetTotalLength, aPtrOutputSamples->pSample, &lTotalLength);

								aPtrOutputSamples->pSample->GetBufferByIndex(0, &lInputMediaBuffer);

								BYTE* lInputScanline0;

								LOG_INVOKE_MF_METHOD(Lock, lInputMediaBuffer, &lInputScanline0, NULL, NULL);

								for (decltype(lTotalLength) i = 0; i < lTotalLength; i++)
								{
									*lOutputScanline0++ = *lInputScanline0++;
								}

								lInputMediaBuffer->Unlock();

								lOutputCurrentLength += lTotalLength;
							}
						}

						if (aPtrOutputSamples->pSample != nullptr)
						{
							aPtrOutputSamples->pSample->Release();

							aPtrOutputSamples->pSample = nullptr;
						}

						lresult = lOutputMediaBuffer->Unlock();

						lOutputMediaBuffer->SetCurrentLength(lOutputCurrentLength);

						mSample->RemoveAllBuffers();

						mSample->AddBuffer(lOutputMediaBuffer);

						aPtrOutputSamples[0].pSample = mSample.Detach();

						aPtrOutputSamples[0].dwStatus = 0;

						*aPtrStatus = 0;
					}
					else
					{
						lresult = mAudioResamplerTransform->ProcessOutput(
							aFlags, aOutputBufferCount,
							aPtrOutputSamples, aPtrStatus);

						if (MF_E_TRANSFORM_NEED_MORE_INPUT == lresult)
						{
							break;
						}
						if (FAILED(lresult))
						{
							break;
						}

						mSample.Release();

						*aPtrStatus = 0;
					}

				} while (false);

				return lresult;
			}

			HRESULT CustomisedAudioResampler::checkMediaType(
				IMFMediaType* aPtrMediaType)
			{
				GUID majorType = GUID_NULL;
				GUID subtype = GUID_NULL;
				HRESULT lresult = S_OK;

				CComPtrCustom<IMFMediaType> lType;

				do
				{
					lType = aPtrMediaType;

					LOG_CHECK_PTR_MEMORY(lType);

					LOG_INVOKE_MF_METHOD(GetGUID, lType, MF_MT_MAJOR_TYPE, &majorType);

					LOG_CHECK_STATE_DESCR(majorType != MFMediaType_Audio, MF_E_INVALIDMEDIATYPE);

					LOG_INVOKE_MF_METHOD(GetGUID, lType, MF_MT_SUBTYPE, &subtype);

					LOG_CHECK_STATE_DESCR(subtype != MFAudioFormat_PCM &&
						subtype != MFAudioFormat_Float, MF_E_INVALIDMEDIATYPE);

				} while (false);

				return lresult;
			}

			HRESULT CustomisedAudioResampler::createProcessor()
			{
				using namespace Core;

				HRESULT lresult = S_OK;

				do
				{
					LOG_CHECK_STATE_DESCR(!mInputMediaType || !mOutputMediaType, MF_E_INVALIDTYPE);

					GUID lInputSubType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						mInputMediaType,
						MF_MT_SUBTYPE,
						&lInputSubType);

					GUID lOutputSubType;

					LOG_INVOKE_MF_METHOD(GetGUID,
						mOutputMediaType,
						MF_MT_SUBTYPE,
						&lOutputSubType);

					UINT32 lInputBitsPerSample;

					LOG_INVOKE_MF_METHOD(GetUINT32,
						mInputMediaType,
						MF_MT_AUDIO_BITS_PER_SAMPLE,
						&lInputBitsPerSample);

					UINT32 lOutputBitsPerSample;

					LOG_INVOKE_MF_METHOD(GetUINT32,
						mOutputMediaType,
						MF_MT_AUDIO_BITS_PER_SAMPLE,
						&lOutputBitsPerSample);




					UINT32 lInputSamplesPerSecond;

					LOG_INVOKE_MF_METHOD(GetUINT32,
						mInputMediaType,
						MF_MT_AUDIO_SAMPLES_PER_SECOND,
						&lInputSamplesPerSecond);


					UINT32 lOutputSamplesPerSecond;

					LOG_INVOKE_MF_METHOD(GetUINT32,
						mOutputMediaType,
						MF_MT_AUDIO_SAMPLES_PER_SECOND,
						&lOutputSamplesPerSecond);




					UINT32 lInputNumChannels;

					LOG_INVOKE_MF_METHOD(GetUINT32,
						mInputMediaType,
						MF_MT_AUDIO_NUM_CHANNELS,
						&lInputNumChannels);


					UINT32 lOutputNumChannels;

					LOG_INVOKE_MF_METHOD(GetUINT32,
						mOutputMediaType,
						MF_MT_AUDIO_NUM_CHANNELS,
						&lOutputNumChannels);

					if (
						(lInputBitsPerSample > lOutputBitsPerSample)
						&& (lOutputSamplesPerSecond == lInputSamplesPerSecond)
						&& (lOutputNumChannels == lInputNumChannels)
						)
					{
						if (
							(lInputBitsPerSample == 32)
							&& (lOutputBitsPerSample == 16)
							&& (lInputSubType == MFAudioFormat_Float)
							&& (lOutputSubType == MFAudioFormat_PCM)
							&& Singleton<Core::CPUDetection::Cpu>::getInstance().is_SSE2()
							)
						{
							CComPtrCustom<BitsAudioResampler> mBitsAudioResamplerTransform(new BitsAudioResampler());

							mAudioResamplerTransform = mBitsAudioResamplerTransform;

							mCustomizedResampler = TRUE;

							break;
						}
					}

					mCustomizedResampler = FALSE;

					LOG_INVOKE_OBJECT_METHOD(mAudioResamplerTransform,
						CoCreateInstance,
						__uuidof(CResamplerMediaObject));

					LOG_INVOKE_MF_METHOD(SetInputType, mAudioResamplerTransform,
						0, mInputMediaType, 0);

					LOG_INVOKE_MF_METHOD(SetOutputType, mAudioResamplerTransform,
						0, mOutputMediaType, 0);

					LOG_INVOKE_MF_METHOD(ProcessMessage, mAudioResamplerTransform,
						MFT_MESSAGE_COMMAND_FLUSH, NULL);

					LOG_INVOKE_MF_METHOD(ProcessMessage, mAudioResamplerTransform,
						MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, NULL);

					LOG_INVOKE_MF_METHOD(ProcessMessage, mAudioResamplerTransform,
						MFT_MESSAGE_NOTIFY_START_OF_STREAM, NULL);

				} while (false);

				return lresult;
			}

		}
	}
}