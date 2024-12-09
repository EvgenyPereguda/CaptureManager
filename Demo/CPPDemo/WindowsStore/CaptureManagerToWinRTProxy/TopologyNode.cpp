#include "pch.h"
#include "TopologyNode.h"


namespace CaptureManagerToWinRTProxy
{
	TopologyNode::TopologyNode(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mPtrIUnkTopologyNode));

		} while (false);
	}

	IntPtr TopologyNode::getTopologyNode()
	{
		IntPtr lresult = nullptr;

		do
		{
			lresult = IntPtr(mPtrIUnkTopologyNode.get());
						
		} while (false);

		return lresult;
	}
}