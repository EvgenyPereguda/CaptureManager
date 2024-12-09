#pragma once

#include "../Common/BaseUnknown.h"

namespace CaptureManager
{
	MIDL_INTERFACE("A50EF532-560E-4750-8093-1206A5D0A313")
	ISampleGrabberCallbackInner : public IUnknown
	{
	public:

		virtual void operator()(
			REFGUID aGUIDMajorMediaType,
			DWORD aSampleFlags,
			LONGLONG aSampleTime,
			LONGLONG aSampleDuration,
			const BYTE* aPtrSampleBuffer,
			DWORD aSampleSize) = 0;
	};

	typedef ISampleGrabberCallbackInner* PtrISampleGrabberCallbackInner;
}