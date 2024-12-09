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

#include "DirectX11ConvertorTransform.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/Common.h"
#include "../Common/GUIDs.h"
#include "../LogPrintOut/LogPrintOut.h"

#include <thread>


namespace CaptureManager
{
	namespace Transform
	{
		namespace VideoSampleAllocator
		{
			using namespace Core;
			
			
			DirectX11ConvertorTransform::DirectX11ConvertorTransform():
				mIsReady(false)
			{}
			
			DirectX11ConvertorTransform::~DirectX11ConvertorTransform()
			{}

			// IMFTransform implementation
			
			STDMETHODIMP DirectX11ConvertorTransform::GetStreamLimits(
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

			STDMETHODIMP DirectX11ConvertorTransform::GetStreamIDs(
				DWORD aInputIDArraySize,
				DWORD* aPtrInputIDs,
				DWORD aOutputIDArraySize,
				DWORD* aPtrOutputIDs)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP DirectX11ConvertorTransform::GetStreamCount(
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

			STDMETHODIMP DirectX11ConvertorTransform::GetInputStreamInfo(
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

			STDMETHODIMP DirectX11ConvertorTransform::GetOutputStreamInfo(
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

			STDMETHODIMP DirectX11ConvertorTransform::GetInputStreamAttributes(
				DWORD aInputStreamID,
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP DirectX11ConvertorTransform::GetOutputStreamAttributes(
				DWORD aOutputStreamID,
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP DirectX11ConvertorTransform::DeleteInputStream(
				DWORD aStreamID)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP DirectX11ConvertorTransform::AddInputStreams(
				DWORD aStreams,
				DWORD* aPtrStreamIDs)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP DirectX11ConvertorTransform::GetInputAvailableType(
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
						LOG_INVOKE_QUERY_INTERFACE_METHOD(mInputMediaType, aPtrPtrType);
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

			STDMETHODIMP DirectX11ConvertorTransform::GetOutputAvailableType(
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

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mOutputMediaType, aPtrPtrType);
					
					lresult = MF_E_NO_MORE_TYPES;


				} while (false);
				
				return lresult;
			}

			STDMETHODIMP DirectX11ConvertorTransform::SetInputType(
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
															
					if (aFlags != MFT_SET_TYPE_TEST_ONLY)
					{
						mInputMediaType = aPtrType;
						
						CComPtrCustom<IMFDXGIDeviceManager> lInputStreamDeviceManager;

						do
						{
							LOG_INVOKE_MF_METHOD(GetUnknown, aPtrType,
								CM_DeviceManager,
								IID_PPV_ARGS(&lInputStreamDeviceManager));

							LOG_CHECK_PTR_MEMORY(lInputStreamDeviceManager);

							mInputStreamDeviceManager = lInputStreamDeviceManager;
							
						} while (false);

						lresult = S_OK;
					}

				} while (false);
				
				return lresult;
			}

			STDMETHODIMP DirectX11ConvertorTransform::SetOutputType(
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
										
					if (aFlags != MFT_SET_TYPE_TEST_ONLY)
					{
						mOutputMediaType = aPtrType;

						CComPtrCustom<IMFDXGIDeviceManager> lOutputStreamDeviceManager;

						do
						{
							LOG_INVOKE_MF_METHOD(GetUnknown, aPtrType,
								CM_DeviceManager,
								IID_PPV_ARGS(&lOutputStreamDeviceManager));

							LOG_CHECK_PTR_MEMORY(lOutputStreamDeviceManager);

							mOutputStreamDeviceManager = lOutputStreamDeviceManager;

						} while (false);

						lresult = S_OK;
					}

				} while (false);
				
				return lresult;
			}

			STDMETHODIMP DirectX11ConvertorTransform::GetInputCurrentType(
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

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mInputMediaType, aPtrPtrType);
					
				} while (false);

				return lresult;
			}

			STDMETHODIMP DirectX11ConvertorTransform::GetOutputCurrentType(
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

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mOutputMediaType, aPtrPtrType);

				} while (false);

				return lresult;
			}

			STDMETHODIMP DirectX11ConvertorTransform::GetInputStatus(
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
					
					if (!mConvertor)
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

			STDMETHODIMP DirectX11ConvertorTransform::GetOutputStatus(
				DWORD* aPtrFlags)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP DirectX11ConvertorTransform::SetOutputBounds(
				LONGLONG aLowerBound,
				LONGLONG aUpperBound)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP DirectX11ConvertorTransform::ProcessEvent(
				DWORD aInputStreamID,
				IMFMediaEvent* aPtrEvent)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP DirectX11ConvertorTransform::GetAttributes(
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP DirectX11ConvertorTransform::ProcessMessage(
				MFT_MESSAGE_TYPE aMessage,
				ULONG_PTR aParam)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::unique_lock<std::mutex> lock(mMutex);

					if (aMessage == MFT_MESSAGE_COMMAND_FLUSH)
					{
						mConvertor.Release();
					}
					else if (aMessage == MFT_MESSAGE_COMMAND_DRAIN)
					{
						mConvertor.Release();
					}
					else if (aMessage == MFT_MESSAGE_NOTIFY_BEGIN_STREAMING)
					{
						if (!mConvertor)
							SampleConvertor::createDirectX11Convertor(
								mInputStreamDeviceManager,
								mInputMediaType,
								mOutputStreamDeviceManager,
								mOutputMediaType,
								&mConvertor);
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

			
			STDMETHODIMP DirectX11ConvertorTransform::ProcessInput(
				DWORD aInputStreamID,
				IMFSample* aPtrSample,
				DWORD aFlags)
			{				
				
				HRESULT lresult = S_OK;

				do
				{
					std::lock_guard<std::mutex> lock(mMutex);
					
					LOG_CHECK_PTR_MEMORY(aPtrSample);
					
					LOG_CHECK_STATE(aInputStreamID != 0 || aFlags != 0);
					
					LOG_CHECK_STATE_DESCR(!mInputMediaType, MF_E_NOTACCEPTING);
										
					LOG_CHECK_STATE_DESCR(mIsReady, MF_E_NOTACCEPTING);
					
					LOG_CHECK_STATE_DESCR(!mConvertor, MF_E_NOTACCEPTING);

					mSample.Release();
										
					LOG_INVOKE_POINTER_METHOD(mConvertor, Convert, aPtrSample, &mSample);
															
					mIsReady = true;					

				} while (false);
				
				return lresult;
			}

			STDMETHODIMP DirectX11ConvertorTransform::ProcessOutput(
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
		}
	}
}