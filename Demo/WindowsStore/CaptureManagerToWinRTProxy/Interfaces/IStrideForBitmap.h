#pragma once

#include "pch.h"

using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	public interface class IStrideForBitmap
	{
		bool getStrideForBitmap(
			Guid aMFVideoFormat,
			uint32 aWidthInPixels,
			int32 *aPtrStride);
	};
}