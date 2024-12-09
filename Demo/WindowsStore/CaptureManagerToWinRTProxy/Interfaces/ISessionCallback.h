#pragma once

#include "pch.h"

using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	public interface class ISessionCallback
	{
		bool invoke(
			uint32 aCallbackEventCode,
			uint32 aSessionDescriptor);
	};
}