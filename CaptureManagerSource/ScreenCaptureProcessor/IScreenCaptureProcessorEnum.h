#pragma once
#include <Unknwnbase.h>

struct IInnerCaptureProcessor;

namespace CaptureManager
{
	namespace Sources
	{
		namespace ScreenCapture
		{
			MIDL_INTERFACE("B1B394DA-20C3-4E8D-9D65-A2B4D66A7780")
			IScreenCaptureProcessorEnum : public IUnknown
			{
				virtual HRESULT STDMETHODCALLTYPE enumIInnerCaptureProcessor(
				UINT aIndex,
				UINT aOrientation,
				IInnerCaptureProcessor** aPtrPtrIInnerCaptureProcessor) = 0;
			};
		}
	}
}