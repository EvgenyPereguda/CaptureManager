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

#include "Direct3DSurface9Transform.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../DirectXManager/Direct3D9Manager.h"
#include "../Common/Common.h"
#include "../LogPrintOut/LogPrintOut.h"


namespace CaptureManager
{
	namespace Transform
	{
		namespace Direct3DSurface9
		{
			using namespace Core;

			using namespace Core::Direct3D9;

			Direct3DSurface9Transform::Direct3DSurface9Transform():
				mIsReady(false)
			{
			}
			
			Direct3DSurface9Transform::~Direct3DSurface9Transform()
			{
			}

			// IMFTransform implementation
			
			STDMETHODIMP Direct3DSurface9Transform::GetStreamLimits(
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

			STDMETHODIMP Direct3DSurface9Transform::GetStreamIDs(
				DWORD aInputIDArraySize,
				DWORD* aPtrInputIDs,
				DWORD aOutputIDArraySize,
				DWORD* aPtrOutputIDs)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP Direct3DSurface9Transform::GetStreamCount(
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

			STDMETHODIMP Direct3DSurface9Transform::GetInputStreamInfo(
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

			STDMETHODIMP Direct3DSurface9Transform::GetOutputStreamInfo(
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

			STDMETHODIMP Direct3DSurface9Transform::GetInputStreamAttributes(
				DWORD aInputStreamID,
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP Direct3DSurface9Transform::GetOutputStreamAttributes(
				DWORD aOutputStreamID,
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP Direct3DSurface9Transform::DeleteInputStream(
				DWORD aStreamID)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP Direct3DSurface9Transform::AddInputStreams(
				DWORD aStreams,
				DWORD* aPtrStreamIDs)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP Direct3DSurface9Transform::GetInputAvailableType(
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

			STDMETHODIMP Direct3DSurface9Transform::GetOutputAvailableType(
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

			STDMETHODIMP Direct3DSurface9Transform::SetInputType(
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
					
					LOG_CHECK_STATE_DESCR(!(!mSample),
						MF_E_TRANSFORM_CANNOT_CHANGE_MEDIATYPE_WHILE_PROCESSING);
					
					if (aFlags != MFT_SET_TYPE_TEST_ONLY)
					{
						mInputMediaType = aPtrType;
					}

				} while (false);
				
				return lresult;
			}

			STDMETHODIMP Direct3DSurface9Transform::SetOutputType(
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
					
					LOG_CHECK_STATE_DESCR(!(!mSample),
						MF_E_TRANSFORM_CANNOT_CHANGE_MEDIATYPE_WHILE_PROCESSING);
					
					if (aFlags != MFT_SET_TYPE_TEST_ONLY)
					{
						mOutputMediaType = aPtrType;

						lresult = S_OK;
					}

				} while (false);
				
				return lresult;
			}

			STDMETHODIMP Direct3DSurface9Transform::GetInputCurrentType(
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

			STDMETHODIMP Direct3DSurface9Transform::GetOutputCurrentType(
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

			STDMETHODIMP Direct3DSurface9Transform::GetInputStatus(
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

			STDMETHODIMP Direct3DSurface9Transform::GetOutputStatus(
				DWORD* aPtrFlags)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP Direct3DSurface9Transform::SetOutputBounds(
				LONGLONG aLowerBound,
				LONGLONG aUpperBound)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP Direct3DSurface9Transform::ProcessEvent(
				DWORD aInputStreamID,
				IMFMediaEvent* aPtrEvent)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP Direct3DSurface9Transform::GetAttributes(
				IMFAttributes** aPtrPtrAttributes)
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP Direct3DSurface9Transform::ProcessMessage(
				MFT_MESSAGE_TYPE aMessage,
				ULONG_PTR aParam)
			{
				HRESULT lresult = S_OK;

				do
				{
					std::unique_lock<std::mutex> lock(mMutex);

					if (aMessage == MFT_MESSAGE_COMMAND_FLUSH)
					{
						mSample = NULL;
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

			STDMETHODIMP Direct3DSurface9Transform::ProcessInput(
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
					
					LOG_CHECK_STATE_DESCR(!m2DBuffer, MF_E_NOTACCEPTING);
					
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
							m2DBuffer,
							&l);

						LOG_INVOKE_MF_METHOD(ContiguousCopyFrom,
							m2DBuffer,
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

			STDMETHODIMP Direct3DSurface9Transform::ProcessOutput(
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

			HRESULT Direct3DSurface9Transform::init(
				IDirect3DDeviceManager9*  aPtrIDirect3DDeviceManager9,
				IMFMediaType* aPtrMediaType)
			{
				class LockDevice
				{
				public:
					LockDevice(
						IDirect3DDeviceManager9*  aPtrIDirect3DDeviceManager9,
						IDirect3DDevice9** aPtrPtrDevice9)
					{
						HRESULT lresult;

						do
						{
							LOG_CHECK_PTR_MEMORY(aPtrIDirect3DDeviceManager9);

							LOG_CHECK_PTR_MEMORY(aPtrPtrDevice9);

							mDirect3DDeviceManager9 = aPtrIDirect3DDeviceManager9;

							*aPtrPtrDevice9 = nullptr;

							LOG_INVOKE_DX9_METHOD(OpenDeviceHandle, mDirect3DDeviceManager9,
								&mHANDLEDevice);
							
							LOG_INVOKE_DX9_METHOD(LockDevice, mDirect3DDeviceManager9,
								mHANDLEDevice,
								aPtrPtrDevice9,
								TRUE);
							
						} while (false);
					}

					~LockDevice()
					{
						HRESULT lresult(E_FAIL);

						do
						{

							if (!mDirect3DDeviceManager9)
							{
								LOG_INVOKE_POINTER_METHOD(mDirect3DDeviceManager9, UnlockDevice,
									mHANDLEDevice, TRUE);
							}

							if (mHANDLEDevice != nullptr)
								LOG_INVOKE_POINTER_METHOD(mDirect3DDeviceManager9, CloseDeviceHandle,
								mHANDLEDevice);

						} while (false);
					}		

				private:

					HANDLE mHANDLEDevice;

					CComPtrCustom<IDirect3DDeviceManager9> mDirect3DDeviceManager9;


					LockDevice(
						const LockDevice&){}

					LockDevice& operator=(
						const LockDevice&){
						return *this;
					}

				};

				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrIDirect3DDeviceManager9);

					LOG_CHECK_PTR_MEMORY(aPtrMediaType);
										
					CComPtrCustom<IDirect3DDevice9> lDevice9;

					LockDevice(
						aPtrIDirect3DDeviceManager9,
						&lDevice9);

					LOG_CHECK_PTR_MEMORY(lDevice9);
					
					mInputMediaType = aPtrMediaType;

					mOutputMediaType = aPtrMediaType;
										
					CComPtrCustom<IDirect3DSwapChain9> lSwapChain;
										
					LOG_INVOKE_POINTER_METHOD(lDevice9, GetSwapChain,
						0, &lSwapChain);
					
					LOG_CHECK_PTR_MEMORY(lSwapChain);

					D3DPRESENT_PARAMETERS lPresentationParameters;

					LOG_INVOKE_POINTER_METHOD(lSwapChain, GetPresentParameters,
						&lPresentationParameters);

					lSwapChain.Release();

					UINT32 lwidth, lheight;

					LOG_INVOKE_FUNCTION(MediaFoundation::MediaFoundationManager::GetAttributeSize,
						mInputMediaType,
						MF_MT_FRAME_SIZE,
						lwidth,
						lheight);				
						
					lPresentationParameters.BackBufferHeight = lheight;

					lPresentationParameters.BackBufferWidth = lwidth;

					lPresentationParameters.Windowed = TRUE;

					lPresentationParameters.SwapEffect = D3DSWAPEFFECT_COPY;// D3DSWAPEFFECT_FLIP;

					lPresentationParameters.Flags =
						D3DPRESENTFLAG_VIDEO | D3DPRESENTFLAG_DEVICECLIP |
						D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

					lPresentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

					lPresentationParameters.BackBufferCount = 1;

					LOG_INVOKE_DX9_METHOD(Reset, lDevice9,
						&lPresentationParameters);
															
					LOG_INVOKE_DX9_METHOD(GetSwapChain, lDevice9,
						0, &lSwapChain);
					
					LOG_CHECK_PTR_MEMORY(lSwapChain);
										
					D3DCOLOR clrBlack = D3DCOLOR_ARGB(0xFF, 0x00, 0x00, 0x00);

					CComPtrCustom<IDirect3DSurface9> lSurface;
										
					LOG_INVOKE_DX9_METHOD(GetBackBuffer, lSwapChain, 
						0, D3DBACKBUFFER_TYPE_MONO, &lSurface);
					
					LOG_INVOKE_DX9_METHOD(ColorFill, lDevice9, lSurface, NULL, clrBlack);
					
					mSample.Release();

					LOG_INVOKE_MF_FUNCTION(MFCreateVideoSampleFromSurface,
						NULL,
						&mSample);
					
					LOG_CHECK_PTR_MEMORY(mSample);

					BOOL lFLIP = FALSE;

					do
					{
						LONG lStride = 0;

						LOG_INVOKE_MF_METHOD(GetUINT32,
							mInputMediaType,
							MF_MT_DEFAULT_STRIDE,
							(UINT32*)&lStride);

						if (lStride < 0)
							lFLIP = TRUE;

					} while (false);			
										
					CComPtrCustom<IMFMediaBuffer> lBuffer;

					LOG_INVOKE_MF_FUNCTION(MFCreateDXSurfaceBuffer,
						__uuidof(IDirect3DSurface9), 
						lSurface, 
						lFLIP,
						&lBuffer);
					
					LOG_CHECK_PTR_MEMORY(lBuffer);
					
					m2DBuffer.Release();
					
					LOG_INVOKE_QUERY_INTERFACE_METHOD(lBuffer, &m2DBuffer);

					LOG_CHECK_PTR_MEMORY(m2DBuffer);
					
					LOG_INVOKE_MF_METHOD(AddBuffer, mSample, 
						lBuffer);
									
					lresult = S_OK;

				} while (false);

				return lresult;
			}
		}
	}
}