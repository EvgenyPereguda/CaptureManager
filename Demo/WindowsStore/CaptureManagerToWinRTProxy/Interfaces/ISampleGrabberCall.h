#pragma once

#include "pch.h"
#include "ITopologyNode.h"

using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	public interface class ISampleGrabberCall
	{
		Boolean readData(			
			Platform::WriteOnlyArray<uint8>^ aPtrData,
			uint32 *aByteSize);

		ITopologyNode^ getTopologyNode();
	};
}