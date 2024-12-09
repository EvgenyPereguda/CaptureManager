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

#include "MemorySampleManager.h"
#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../VideoRendererManager/Direct3D11VideoProcessor.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"


namespace CaptureManager
{
	namespace Core
	{

		using namespace MediaFoundation;

		namespace
		{

			template<typename MemoryAllocator>
			class MemorySampleManagerInner :
				public BaseUnknown<IMemorySampleManager>
			{
			public:
				MemorySampleManagerInner(D3D11_TEXTURE2D_DESC aTextureDesc):
					mTextureDesc(aTextureDesc){}
				
				virtual HRESULT getSample(
					IUnknown* aPtrDevice,
					IMFSample** aPtrPtrSample)override
				{
					HRESULT lresult(E_FAIL);

					do
					{
						CComQIPtrCustom<ID3D11Device> lDevice;

						lDevice = aPtrDevice;

						LOG_CHECK_PTR_MEMORY(lDevice);

						mMemoryAllocator(mTextureDesc, lDevice, aPtrPtrSample);

					} while (false);

					return lresult;
				}

				virtual void clearMemory()override
				{
				}

			private:

				virtual ~MemorySampleManagerInner() {}


				D3D11_TEXTURE2D_DESC mTextureDesc;

				MemoryAllocator mMemoryAllocator;


				//DWORD mMaxBufferLength;

				//std::stack<IMFMediaBuffer*> mBufferStack;

				//std::list<CComPtrCustom<IMFMediaBuffer>> mMemoryList;

				//std::mutex mAccessMutex;
			};

		}

		HRESULT MemorySampleManager::createVideoMemorySampleManager(
			IMFMediaType* aPtrMediaType,
			IMemorySampleManager** aPtrPtrMemorySampleManager)
		{
			using namespace CaptureManager::Sinks::EVR::Mixer;

			struct VideoMemoryAllocator
			{
				HRESULT operator()(
					D3D11_TEXTURE2D_DESC aTextureDesc,
					ID3D11Device* aPtrDevice,
					IMFSample** aPtrPtrSample) const {

					HRESULT lresult(E_FAIL);

					do
					{

						CComPtrCustom<IMFSample> lSample;

						LOG_INVOKE_MF_FUNCTION(MFCreateSample,
							&lSample);

						LOG_CHECK_PTR_MEMORY(lSample);
						

						CComPtrCustom<ID3D11Texture2D> lDestSurface;

						LOG_INVOKE_POINTER_METHOD(aPtrDevice, CreateTexture2D,
							&aTextureDesc, NULL, &lDestSurface);


						CComPtrCustom<IMFMediaBuffer> lBuffer;

						LOG_INVOKE_MF_FUNCTION(MFCreateDXGISurfaceBuffer,
							__uuidof(ID3D11Texture2D),
							lDestSurface,
							0,
							TRUE,
							&lBuffer);

						LOG_CHECK_PTR_MEMORY(lBuffer);

						LOG_INVOKE_MF_METHOD(AddBuffer, lSample,
							lBuffer);

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lSample, aPtrPtrSample);

					} while (false);

					return lresult;
				};
			};


			HRESULT lresult = E_FAIL;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrMediaType);

				D3D11_TEXTURE2D_DESC lTextureDesc;

				UINT32 lWidth = 0;
				UINT32 lHeight = 0;

				LOG_INVOKE_FUNCTION(MFGetAttributeSize,
					aPtrMediaType,
					MF_MT_FRAME_SIZE,
					&lWidth,
					&lHeight);

				GUID lSubType;

				LOG_INVOKE_MF_METHOD(GetGUID,
					aPtrMediaType,
					MF_MT_SUBTYPE,
					&lSubType);

				LOG_INVOKE_FUNCTION(Direct3D11VideoProcessor::convertSubTypeToDXGIFormat, lSubType, &lTextureDesc.Format);


				lTextureDesc.Width = lWidth;

				lTextureDesc.Height = lHeight;
				
				lTextureDesc.ArraySize = 1;

				lTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET;

				lTextureDesc.MiscFlags = 0;

				lTextureDesc.SampleDesc.Count = 1;

				lTextureDesc.SampleDesc.Quality = 0;

				lTextureDesc.MipLevels = 1;

				lTextureDesc.CPUAccessFlags = 0;

				lTextureDesc.Usage = D3D11_USAGE_DEFAULT;



				CComPtrCustom<IMemorySampleManager> lMemorySampleManager(new (std::nothrow) MemorySampleManagerInner<VideoMemoryAllocator>(lTextureDesc));

				LOG_CHECK_PTR_MEMORY(lMemorySampleManager);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lMemorySampleManager, aPtrPtrMemorySampleManager);

			} while (false);

			return lresult;
		}
	}
}