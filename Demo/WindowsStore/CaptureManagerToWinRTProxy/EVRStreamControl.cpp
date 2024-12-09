#include "pch.h"
#include "EVRStreamControl.h"
#include "TopologyNode.h"

namespace CaptureManagerToWinRTProxy
{
	EVRStreamControl::EVRStreamControl(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mIEVRStreamControl));

		} while (false);
	}

	void EVRStreamControl::flush(ITopologyNode^ aPtrEVROutputNode)
	{

		do
		{
			if (!mIEVRStreamControl)
				break;

			if (aPtrEVROutputNode == nullptr)
				break;

			TopologyNode^ lTopologyNode = (TopologyNode^)aPtrEVROutputNode;

			auto lPtrUnkTopologyNode = reinterpret_cast<IUnknown*>((void*)lTopologyNode->getTopologyNode());

			if (lPtrUnkTopologyNode == nullptr)
				break;

			auto lhr = mIEVRStreamControl->flush(
				lPtrUnkTopologyNode);

			if (FAILED(lhr))
				break;

		} while (false);

	}

	void EVRStreamControl::setPosition(ITopologyNode^ aPtrEVROutputNode, float aLeft, float aRight, float aTop, float aBottom)
	{
		do
		{
			if (!mIEVRStreamControl)
				break;

			if (aPtrEVROutputNode == nullptr)
				break;

			TopologyNode^ lTopologyNode = (TopologyNode^)aPtrEVROutputNode;

			auto lPtrUnkTopologyNode = reinterpret_cast<IUnknown*>((void*)lTopologyNode->getTopologyNode());

			if (lPtrUnkTopologyNode == nullptr)
				break;

			auto lhr = mIEVRStreamControl->setPosition(
				lPtrUnkTopologyNode,
				aLeft, aRight, aTop, aBottom);

			if (FAILED(lhr))
				break;

		} while (false);

	}

	void EVRStreamControl::setSrcPosition(ITopologyNode^ aPtrEVROutputNode, float aLeft, float aRight, float aTop, float aBottom)
	{
		do
		{
			if (!mIEVRStreamControl)
				break;

			if (aPtrEVROutputNode == nullptr)
				break;

			TopologyNode^ lTopologyNode = (TopologyNode^)aPtrEVROutputNode;

			auto lPtrUnkTopologyNode = reinterpret_cast<IUnknown*>((void*)lTopologyNode->getTopologyNode());

			if (lPtrUnkTopologyNode == nullptr)
				break;

			auto lhr = mIEVRStreamControl->setSrcPosition(
				lPtrUnkTopologyNode,
				aLeft, aRight, aTop, aBottom);

			if (FAILED(lhr))
				break;

		} while (false);

	}

	void EVRStreamControl::setZOrder(ITopologyNode^ aPtrEVROutputNode, uint32 aZOrder)
	{
		do
		{
			if (!mIEVRStreamControl)
				break;

			if (aPtrEVROutputNode == nullptr)
				break;

			TopologyNode^ lTopologyNode = (TopologyNode^)aPtrEVROutputNode;

			auto lPtrUnkTopologyNode = reinterpret_cast<IUnknown*>((void*)lTopologyNode->getTopologyNode());

			if (lPtrUnkTopologyNode == nullptr)
				break;

			auto lhr = mIEVRStreamControl->setZOrder(
				lPtrUnkTopologyNode,
				aZOrder);

			if (FAILED(lhr))
				break;

		} while (false);

	}
}