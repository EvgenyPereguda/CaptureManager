#pragma once

#include "Interfaces/ISampleGrabberCallSinkFactory.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"

namespace CaptureManagerToWinRTProxy
{
	ref class SampleGrabberCallSinkFactory sealed :
		public ISampleGrabberCallSinkFactory
	{

		CComPtrCustom<::ISampleGrabberCallSinkFactory> mISampleGrabberCallSinkFactory;

	public:
		SampleGrabberCallSinkFactory(IntPtr aIntPtr);

		virtual ISampleGrabberCall^ createSampleGrabberCall(
			Guid aRefMajorType,
			Guid aRefSubType,
			uint32 aSampleByteSize);
	};
}