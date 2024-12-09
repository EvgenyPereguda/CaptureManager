#pragma once

#include "pch.h"
#include "IMediaType.h"

using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	public interface class IMediaTypeParser
	{
		String^ parse(IMediaType^ aPtrMediaType);
	};
}