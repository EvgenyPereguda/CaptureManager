#pragma once

#include "Interfaces/ISampleGrabberCall.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"

namespace CaptureManagerToWinRTProxy
{
	ref class SampleGrabberCall sealed :
		public ISampleGrabberCall
	{
		CComPtrCustom<::ISampleGrabberCall> mISampleGrabberCall;
	public:
		SampleGrabberCall(IntPtr aIntPtr);

		virtual Boolean readData(
			Platform::WriteOnlyArray<uint8>^ aPtrData,
			uint32 *aByteSize);

		virtual ITopologyNode^ getTopologyNode();
	};
}