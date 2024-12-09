#pragma once

#include "pch.h"

using namespace Platform;

namespace CaptureManagerToWinRTProxy
{
	public interface class ISourceRequestResult
	{
		uint32 getStreamIndex();

		void setData(
			const Platform::Array<uint8>^ aData,
			bool aIsKeyFrame);
	};

}