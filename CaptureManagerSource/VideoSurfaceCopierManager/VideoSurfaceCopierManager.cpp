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

#include "VideoSurfaceCopierManager.h"
#include "Direct3DSurface9Transform.h"
#include "VideoSampleAllocatorTransform.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/MFHeaders.h"


namespace CaptureManager
{
	namespace Transform
	{
		VideoSurfaceCopierManager::VideoSurfaceCopierManager()
		{
		}
		
		VideoSurfaceCopierManager::~VideoSurfaceCopierManager()
		{
		}

		HRESULT VideoSurfaceCopierManager::createVideoSurfaceCopier(
			IUnknown* aPtrDeviceManager,
			IUnknown* aPtrMediaType,
			IUnknown** aPtrPtrTransform)
		{
			using namespace Direct3DSurface9;

			using namespace VideoSampleAllocator;

			HRESULT lresult;

			do
			{
				CComPtrCustom<IMFMediaType> lMediaType;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrMediaType, &lMediaType);
				
				CComQIPtrCustom<IDirect3DDeviceManager9> lDirect3DDeviceManager9;

				lDirect3DDeviceManager9 = aPtrDeviceManager;
				
				if (lDirect3DDeviceManager9)
				{
					CComPtrCustom<Direct3DSurface9Transform> lDirect3DSurface9Transform(
						new (std::nothrow) Direct3DSurface9Transform);

					LOG_INVOKE_POINTER_METHOD(lDirect3DSurface9Transform, init,
						lDirect3DDeviceManager9,
						lMediaType);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lDirect3DSurface9Transform, aPtrPtrTransform);
				}
				else
				{
					CComQIPtrCustom<IMFVideoSampleAllocator>  lIMFVideoSampleAllocator;

					lIMFVideoSampleAllocator = aPtrDeviceManager;

					if (lIMFVideoSampleAllocator)
					{
						CComPtrCustom<VideoSampleAllocatorTransform> lVideoSampleAllocatorTransform(
							new (std::nothrow) VideoSampleAllocatorTransform);

						LOG_INVOKE_POINTER_METHOD(lVideoSampleAllocatorTransform, init,
							lIMFVideoSampleAllocator,
							lMediaType);

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lVideoSampleAllocatorTransform, aPtrPtrTransform);
					}
					else
					{
						lresult = E_FAIL;
					}
				}
				
			} while (false);

			return lresult;
		}
	}
}