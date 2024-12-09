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

#include "BitsAudioResampler.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/Common.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../MemoryManager/MemoryBufferManager.h"

#define _M_SSE

#ifdef _M_SSE
#include <emmintrin.h>
#endif


namespace CaptureManager
{
	namespace Transform
	{
		namespace Audio
		{
			using namespace CaptureManager::Core;

			static const __m128 scale = _mm_set_ps1(32767.0f);

			static void ConvertF32ToS16(short *out, const float *in, size_t size) {

#ifdef _M_SSE			

				while (size >= 16) {

					__m128i c1 = _mm_cvttps_epi32(_mm_mul_ps(_mm_loadu_ps(in), scale));

					__m128i c2 = _mm_cvttps_epi32(_mm_mul_ps(_mm_loadu_ps(in + 4), scale));

					__m128i d1 = _mm_packs_epi32(c1, c1);

					__m128i d2 = _mm_packs_epi32(c2, c2);

					_mm_storel_epi64((__m128i*)out, d1);

					_mm_storel_epi64((__m128i*)(out + 4), d2);



					c1 = _mm_cvttps_epi32(_mm_mul_ps(_mm_loadu_ps(in + 8), scale));

					c2 = _mm_cvttps_epi32(_mm_mul_ps(_mm_loadu_ps(in + 12), scale));

					d1 = _mm_packs_epi32(c1, c1);

					d2 = _mm_packs_epi32(c2, c2);

					_mm_storel_epi64((__m128i*)(out + 8), d1);

					_mm_storel_epi64((__m128i*)(out + 12), d2);


					in += 16;
					out += 16;
					size -= 16;
				}
#endif
				for (size_t i = 0; i < size; i++) {
					out[i] = (SHORT)(in[i] * 32768.0f);
				}
			}


			BitsAudioResampler::BitsAudioResampler()
			{
			}

			BitsAudioResampler::~BitsAudioResampler()
			{
			}

			STDMETHODIMP BitsAudioResampler::GetStreamLimits(DWORD* aPtrInputMinimum, DWORD* aPtrInputMaximum,
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

			STDMETHODIMP BitsAudioResampler::GetStreamIDs(DWORD aInputIDArraySize, DWORD* aPtrInputIDs,
				DWORD aOutputIDArraySize, DWORD* aPtrOutputIDs)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP BitsAudioResampler::GetStreamCount(DWORD* aPtrInputStreams, DWORD* aPtrOutputStreams)
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

			STDMETHODIMP BitsAudioResampler::GetInputStreamInfo(DWORD aInputStreamID,
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

			STDMETHODIMP BitsAudioResampler::GetOutputStreamInfo(DWORD aOutputStreamID,
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

			STDMETHODIMP BitsAudioResampler::GetInputStreamAttributes(DWORD aInputStreamID,
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP BitsAudioResampler::GetOutputStreamAttributes(DWORD aOutputStreamID,
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP BitsAudioResampler::DeleteInputStream(DWORD aStreamID)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP BitsAudioResampler::AddInputStreams(DWORD aStreams, DWORD* aPtrStreamIDs)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP BitsAudioResampler::GetInputAvailableType(DWORD aInputStreamID, DWORD aTypeIndex,
				IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

				} while (false);

				return lresult;
			}

			STDMETHODIMP BitsAudioResampler::GetOutputAvailableType(DWORD aOutputStreamID, DWORD aTypeIndex,
				IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;
				CComPtrCustom<IMFMediaType> lMediaType;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

				} while (false);

				return lresult;
			}

			STDMETHODIMP BitsAudioResampler::SetInputType(DWORD aInputStreamID, IMFMediaType* aPtrType,
				DWORD aFlags)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

				} while (false);

				return lresult;
			}

			STDMETHODIMP BitsAudioResampler::SetOutputType(DWORD aOutputStreamID, IMFMediaType* aPtrType,
				DWORD aFlags)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

				} while (false);

				return lresult;
			}

			STDMETHODIMP BitsAudioResampler::GetInputCurrentType(DWORD aInputStreamID, IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

				} while (false);

				return lresult;
			}

			STDMETHODIMP BitsAudioResampler::GetOutputCurrentType(DWORD aOutputStreamID, IMFMediaType** aPtrPtrType)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

				} while (false);

				return lresult;
			}

			STDMETHODIMP BitsAudioResampler::GetInputStatus(DWORD aInputStreamID, DWORD* aPtrFlags)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

				} while (false);

				return lresult;
			}

			STDMETHODIMP BitsAudioResampler::GetOutputStatus(DWORD* aPtrFlags)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP BitsAudioResampler::SetOutputBounds(LONGLONG aLowerBound, LONGLONG aUpperBound)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP BitsAudioResampler::ProcessEvent(DWORD aInputStreamID, IMFMediaEvent* aPtrEvent)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP BitsAudioResampler::GetAttributes(IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP BitsAudioResampler::ProcessMessage(MFT_MESSAGE_TYPE aMessage, ULONG_PTR aParam)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

				} while (false);

				return lresult;
			}

			STDMETHODIMP BitsAudioResampler::ProcessInput(DWORD aInputStreamID, IMFSample* aPtrSample,
				DWORD aFlags)
			{
				HRESULT lresult = S_OK;
				DWORD dwBufferCount = 0;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrSample);

					LOG_CHECK_STATE(aInputStreamID != 0 || aFlags != 0);

					mSample = aPtrSample;

				} while (false);

				return lresult;
			}

			STDMETHODIMP BitsAudioResampler::ProcessOutput(DWORD aFlags, DWORD aOutputBufferCount,
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

					CComPtrCustom<IMFMediaBuffer> lOutputMediaBuffer;

					DWORD lOutputCurrentLength = 0;

					mSample->GetTotalLength(&lOutputCurrentLength);

					DWORD lSampleCount = lOutputCurrentLength >> 2;

					LOG_INVOKE_MF_FUNCTION(MFCreateMemoryBuffer, lSampleCount << 1, &lOutputMediaBuffer);



					CComPtrCustom<IMFMediaBuffer> lInputMediaBuffer;

					mSample->GetBufferByIndex(0, &lInputMediaBuffer);

					LOG_CHECK_PTR_MEMORY(lInputMediaBuffer);




					BYTE* lInputScanline0;

					LOG_INVOKE_MF_METHOD(Lock, lInputMediaBuffer, &lInputScanline0, NULL, NULL);

					do
					{
						BYTE* lOutputScanline0;

						LOG_INVOKE_MF_METHOD(Lock, lOutputMediaBuffer, &lOutputScanline0, NULL, NULL);

						FLOAT* lFloatlInputScanline0 = (FLOAT*)lInputScanline0;

						SHORT* lShortlOutputScanline0 = (SHORT*)lOutputScanline0;

						ConvertF32ToS16(lShortlOutputScanline0, lFloatlInputScanline0, lSampleCount);

					} while (false);


					lInputMediaBuffer->Unlock();

					lresult = lOutputMediaBuffer->Unlock();



					lOutputMediaBuffer->SetCurrentLength(lSampleCount << 1);



					CComPtrCustom<IMFSample> lSample;

					LONGLONG lSampleTime;

					LONGLONG lSampleDuration;

					LOG_INVOKE_MF_FUNCTION(MFCreateSample, &lSample);

					LOG_INVOKE_MF_METHOD(AddBuffer, lSample, lOutputMediaBuffer);

					LOG_INVOKE_MF_METHOD(GetSampleDuration, mSample, &lSampleDuration);

					LOG_INVOKE_MF_METHOD(GetSampleTime, mSample, &lSampleTime);

					LOG_INVOKE_MF_METHOD(SetSampleDuration, lSample, lSampleDuration);

					LOG_INVOKE_MF_METHOD(SetSampleTime, lSample, lSampleTime);

					mSample.Release();

					aPtrOutputSamples[0].pSample = lSample.Detach();

					aPtrOutputSamples[0].dwStatus = 0;

					*aPtrStatus = 0;

				} while (false);

				return lresult;
			}
		}
	}
}