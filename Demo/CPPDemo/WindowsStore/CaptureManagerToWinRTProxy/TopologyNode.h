#pragma once

#include "pch.h"
#include "ComPtrCustom.h"
#include "ITopologyNode.h"


using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	
	ref class TopologyNode sealed:
		public ITopologyNode
	{
		CComPtrCustom<IUnknown> mPtrIUnkTopologyNode;
				
	public:
		TopologyNode(IntPtr aIntPtr);

		IntPtr getTopologyNode();
	};
}