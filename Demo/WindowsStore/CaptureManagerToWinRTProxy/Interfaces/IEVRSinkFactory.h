#pragma once

#include "pch.h"
#include "ITopologyNode.h"

using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	public interface class IEVRSinkFactory
	{
		ITopologyNode^ createOutputNode(IntPtr aHWND);
	};
}