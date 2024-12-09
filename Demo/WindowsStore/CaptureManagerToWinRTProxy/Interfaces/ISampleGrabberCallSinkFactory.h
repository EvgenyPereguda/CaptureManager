#pragma once

#include "pch.h"
#include "ISampleGrabberCall.h"

using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	public interface class ISampleGrabberCallSinkFactory
	{
		ISampleGrabberCall^ createSampleGrabberCall(
			Guid aRefMajorType,
			Guid aRefSubType,
			uint32 aSampleByteSize);
	};
}