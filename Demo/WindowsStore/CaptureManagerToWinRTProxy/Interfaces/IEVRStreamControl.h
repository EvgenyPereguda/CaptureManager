#pragma once

#include "pch.h"
#include "ITopologyNode.h"

using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	public interface class IEVRStreamControl
	{
		void flush(ITopologyNode^ aPtrEVROutputNode);
	
		//void getPosition(ITopologyNode^ aPtrEVROutputNode, out float aPtrLeft, out float aPtrRight, out float aPtrTop, out float aPtrBottom);
	
		//void getSrcPosition(ITopologyNode^ aPtrEVROutputNode, out float aPtrLeft, out float aPtrRight, out float aPtrTop, out float aPtrBottom);
	
		//void getZOrder(ITopologyNode^ aPtrEVROutputNode, out uint aPtrZOrder);
	
		void setPosition(ITopologyNode^ aPtrEVROutputNode, float aLeft, float aRight, float aTop, float aBottom);

		void setSrcPosition(ITopologyNode^ aPtrEVROutputNode, float aLeft, float aRight, float aTop, float aBottom);
	
		void setZOrder(ITopologyNode^ aPtrEVROutputNode, uint32 aZOrder);
	};
}