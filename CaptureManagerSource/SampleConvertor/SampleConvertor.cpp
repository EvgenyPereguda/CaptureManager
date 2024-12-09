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

#include "SampleConvertor.h"
#include "DirectX11ToSystemConvertor.h"
#include "RgbToNV12.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/Common.h"
#include "../Common/GUIDs.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/MFHeaders.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"

namespace CaptureManager
{
	using namespace SampleConvertorInner;

	using namespace CaptureManager::Core;

	HRESULT SampleConvertor::createDirectX11Convertor(
		IUnknown* aPtrInputDeviceManager,
		IMFMediaType* aPtrInputMediaType,
		IUnknown* aPtrOutputDeviceManager,
		IMFMediaType* aPtrOutputMediaType,
		ISampleConvertor** aPtrPtrSampleConvertor)
	{
		HRESULT lresult = E_FAIL;

		do
		{
			if (aPtrInputDeviceManager != nullptr &&
				aPtrOutputDeviceManager == nullptr)
			{
				CComPtrCustom<DirectX11ToSystemConvertor> lDirectX11ToSystemConvertor(new DirectX11ToSystemConvertor());

				LOG_CHECK_PTR_MEMORY(lDirectX11ToSystemConvertor);

				LOG_INVOKE_POINTER_METHOD(lDirectX11ToSystemConvertor, init, aPtrInputDeviceManager, aPtrInputMediaType);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lDirectX11ToSystemConvertor, aPtrPtrSampleConvertor);
			}

		} while (false);

		return lresult;
	}

	HRESULT SampleConvertor::createRgbToNV12(
		IMFMediaType* aPtrInputMediaType,
		IMFMediaType* aPtrOutputMediaType,
		ISampleConvertor** aPtrPtrSampleConvertor)
	{
		HRESULT lresult = E_FAIL;

		do
		{
			LOG_CHECK_PTR_MEMORY(aPtrInputMediaType);

			LOG_CHECK_PTR_MEMORY(aPtrOutputMediaType);

			LOG_CHECK_PTR_MEMORY(aPtrPtrSampleConvertor);

			GUID lInputSubType;

			LOG_INVOKE_MF_METHOD(GetGUID, aPtrInputMediaType,
				MF_MT_SUBTYPE,
				&lInputSubType);


			GUID lOutputSubType;

			LOG_INVOKE_MF_METHOD(GetGUID, aPtrOutputMediaType,
				MF_MT_SUBTYPE,
				&lOutputSubType);
			
			CComPtrCustom<RgbToNV12> lRgbToNV12(new RgbToNV12());

			LOG_CHECK_PTR_MEMORY(lRgbToNV12);

			LOG_INVOKE_POINTER_METHOD(lRgbToNV12, init,
				aPtrInputMediaType,
				aPtrOutputMediaType);

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lRgbToNV12, aPtrPtrSampleConvertor);

		} while (false);

		return lresult;
	}
}
