#pragma once

#include "ISampleConvertor.h"

struct IMFMediaType;

namespace CaptureManager
{
	class SampleConvertor
	{
	public:

		static HRESULT createDirectX11Convertor(
			IUnknown* aPtrInputDeviceManager,
			IMFMediaType* aPtrInputMediaType,
			IUnknown* aPtrOutputDeviceManager,
			IMFMediaType* aPtrOutputMediaType,
			ISampleConvertor** aPtrPtrSampleConvertor);

		static HRESULT createRgbToNV12(
			IMFMediaType* aPtrInputMediaType,
			IMFMediaType* aPtrOutputMediaType,
			ISampleConvertor** aPtrPtrSampleConvertor);

	private:
		SampleConvertor() = delete;
		~SampleConvertor() = delete;
	};
}