#pragma once

#include "pch.h"

using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	public interface class ISampleGrabberCallback
	{
		void invoke(
			Guid aGUIDMajorMediaType,
			uint32 aSampleFlags,
			int64 aSampleTime,
			int64 aSampleDuration,
			uint8* aPtrSampleBuffer,
			uint32 aSampleSize);
	};
}