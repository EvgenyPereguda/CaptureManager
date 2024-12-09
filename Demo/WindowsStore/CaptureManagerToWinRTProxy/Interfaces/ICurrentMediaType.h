#pragma once

#include "pch.h"
#include "IMediaType.h"

using namespace Platform;
using namespace Windows::Foundation::Collections;

namespace CaptureManagerToWinRTProxy
{
	public interface class ICurrentMediaType
	{
		IMediaType^ getMediaType();

		uint32 getMediaTypeIndex();

		uint32 getStreamIndex();
	};

}