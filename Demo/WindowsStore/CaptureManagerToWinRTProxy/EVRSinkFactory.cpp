#include "pch.h"
#include "EVRSinkFactory.h"
#include "TopologyNode.h"


namespace CaptureManagerToWinRTProxy
{
	EVRSinkFactory::EVRSinkFactory(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mIEVRSinkFactory));

		} while (false);
	}

	ITopologyNode^ EVRSinkFactory::createOutputNode(IntPtr aHWND)
	{
		ITopologyNode^ lresult(nullptr);

		do
		{
			if (!mIEVRSinkFactory)
				break;

			CComPtrCustom<IUnknown> lOutputNode;

			auto lhr = mIEVRSinkFactory->createOutputNode(
				(LPVOID)aHWND,
				&lOutputNode);

			if (FAILED(lhr))
				break;

			if (!lOutputNode)
				break;

			lresult = ref new TopologyNode(IntPtr(lOutputNode.get()));

		} while (false);

		return lresult;
	}
}