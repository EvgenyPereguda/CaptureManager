#pragma once

#include "pch.h"
#include "ComPtrCustom.h"
#include "Interfaces/ITopologyNode.h"


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