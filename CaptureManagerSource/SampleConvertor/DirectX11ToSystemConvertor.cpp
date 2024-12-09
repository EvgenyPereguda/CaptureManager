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

#include "DirectX11ToSystemConvertor.h"
#include "../Common/Common.h"
#include "../Common/GUIDs.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../DirectXManager/DXGIManager.h"
#include "../DirectXManager/Direct3D11Manager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "MediaBufferLock.h"
#include "../MemoryManager/MemoryBufferManager.h"
#include "../MemoryManager/MemoryManager.h"


namespace CaptureManager
{
	namespace SampleConvertorInner
	{
		using namespace Core;
		
		DirectX11ToSystemConvertor::DirectX11ToSystemConvertor()
		{
		}

		DirectX11ToSystemConvertor::~DirectX11ToSystemConvertor()
		{
			if (mMemoryBufferManager)
				mMemoryBufferManager->clearMemory();
		}

		HRESULT DirectX11ToSystemConvertor::videoMemoryCopy(IMFSample* aPtrInputSample, IMFMediaBuffer* aPtrDstBuffer)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrInputSample);

				LOG_CHECK_PTR_MEMORY(aPtrDstBuffer);

				DWORD lBufferCount(0);

				LOG_INVOKE_MF_METHOD(GetBufferCount, aPtrInputSample, &lBufferCount);

				LOG_CHECK_STATE(lBufferCount != 1);

				CComPtrCustom<IMFMediaBuffer> lInputBuffer;

				LOG_INVOKE_MF_METHOD(GetBufferByIndex, aPtrInputSample, 0, &lInputBuffer);


				CComPtrCustom<IMF2DBuffer> l2DBuffer;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lInputBuffer, &l2DBuffer);

				LOG_CHECK_PTR_MEMORY(l2DBuffer);
				
				{

					DWORD lMaxLength;

					DWORD lCurrentLength;

					BYTE* lPtrInputBuffer;

					MediaBufferLock lMediaBufferLock(
						aPtrDstBuffer,
						lMaxLength,
						lCurrentLength,
						&lPtrInputBuffer,
						lresult);

					if (FAILED(lresult))
					{
						break;
					}
					
					l2DBuffer->ContiguousCopyTo(lPtrInputBuffer, lCurrentLength);
				}
				
			} while (false);

			return lresult;
		}

		HRESULT DirectX11ToSystemConvertor::Convert(IMFSample* aPtrInputSample, IMFSample** aPtrPtrOutputSample)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrInputSample);

				LOG_CHECK_PTR_MEMORY(aPtrPtrOutputSample);

				DWORD lBufferCount(0);

				LOG_INVOKE_MF_METHOD(GetBufferCount, aPtrInputSample, &lBufferCount);

				LOG_CHECK_STATE(lBufferCount != 1);
				
				CComPtrCustom<IMFMediaBuffer> lInputBuffer;

				LOG_INVOKE_MF_METHOD(GetBufferByIndex, aPtrInputSample, 0, &lInputBuffer);
								

				CComPtrCustom<IMF2DBuffer> l2DBuffer;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lInputBuffer, &l2DBuffer);

				LOG_CHECK_PTR_MEMORY(l2DBuffer);


				DWORD lInputBufferLength(0);

				LOG_INVOKE_MF_METHOD(GetContiguousLength,
					l2DBuffer,
					&lInputBufferLength);

				CComPtrCustom<IMFMediaBuffer> lSampleBuffer;

				if (!mMemoryBufferManager)
				{
					MemoryBufferManager::createSystemMemoryBufferManager(lInputBufferLength, &mMemoryBufferManager);

					LOG_CHECK_PTR_MEMORY(mMemoryBufferManager);
				}

/*				lresult = aPtrInputSample->GetUnknown(CM_Sample_Buffer, IID_PPV_ARGS(&lSampleBuffer));

				if (FAILED(lresult) && !lSampleBuffer)
				{
					mMemoryBufferManager->getMediaBuffer(&lSampleBuffer);

					LOG_CHECK_PTR_MEMORY(lSampleBuffer);

					lSampleBuffer->SetCurrentLength(lInputBufferLength);

					aPtrInputSample->SetUnknown(CM_Sample_Buffer, lSampleBuffer);
				}

				UINT32 lBuffer_Is_Filled = FALSE;

				lresult = aPtrInputSample->GetUINT32(CM_Buffer_Is_Filled, &lBuffer_Is_Filled);

				if (FAILED(lresult) || lBuffer_Is_Filled == FALSE)
				{
					videoMemoryCopy(aPtrInputSample, lSampleBuffer);					

					lresult = aPtrInputSample->SetUINT32(CM_Buffer_Is_Filled, TRUE);
				}	*/						
								

				//LOG_INVOKE_MF_FUNCTION(MFCreateMemoryBuffer,
				//	lInputBufferLength,
				//	&lSampleBuffer);


				mMemoryBufferManager->getMediaBuffer(&lSampleBuffer);

				LOG_CHECK_PTR_MEMORY(lSampleBuffer);

				lSampleBuffer->SetCurrentLength(lInputBufferLength);

				videoMemoryCopy(aPtrInputSample, lSampleBuffer);

				LONGLONG lSampleTime;

				LOG_INVOKE_MF_METHOD(GetSampleTime,
					aPtrInputSample, &lSampleTime);

				LONGLONG lSampleDuration;

				LOG_INVOKE_MF_METHOD(GetSampleDuration,
					aPtrInputSample,
					&lSampleDuration);

				CComPtrCustom<IMFSample> lSample;

				LOG_INVOKE_MF_FUNCTION(MFCreateSample, &lSample);

				LOG_CHECK_PTR_MEMORY(lSample);

				LOG_INVOKE_MF_METHOD(AddBuffer, lSample, lSampleBuffer);

				LOG_INVOKE_MF_METHOD(SetSampleDuration, lSample, lSampleDuration);

				LOG_INVOKE_MF_METHOD(SetSampleTime, lSample, lSampleTime);				
				
				LOG_INVOKE_QUERY_INTERFACE_METHOD(lSample, aPtrPtrOutputSample);

			} while (false);

			return lresult;
		}

		HRESULT DirectX11ToSystemConvertor::init(
			IUnknown* aPtrInputDeviceManager,
			IMFMediaType* aPtrInputMediaType)
		{
			HRESULT lresult = S_OK;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrInputMediaType);

				UINT32 lState(FALSE);

				LOG_INVOKE_MF_METHOD(GetUINT32, aPtrInputMediaType,
					MF_MT_FIXED_SIZE_SAMPLES,
					&lState);

				LOG_CHECK_STATE(lState == FALSE);
								
			} while (false);

			return lresult;
		}
	}
}