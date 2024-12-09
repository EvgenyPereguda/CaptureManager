#include "pch.h"
#include "SampleGrabberCall.h"
#include "TopologyNode.h"


namespace CaptureManagerToWinRTProxy
{
	SampleGrabberCall::SampleGrabberCall(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mISampleGrabberCall));

		} while (false);
	}

	Boolean SampleGrabberCall::readData(
		Platform::WriteOnlyArray<uint8>^ aPtrData,
		uint32 *aByteSize)
	{
		Boolean lresult(false);

		do
		{
			if (!mISampleGrabberCall)
				break;

			if (aPtrData == nullptr)
				break;

			auto lhr = mISampleGrabberCall->readData(
				aPtrData->Data,
				(DWORD*)aByteSize);

			if (FAILED(lhr))
				break;

			lresult = true;

		} while (false);

		return lresult;
	}

	ITopologyNode^ SampleGrabberCall::getTopologyNode()
	{
		ITopologyNode^ lresult = nullptr;

		do
		{
			if (!mISampleGrabberCall)
				break;
			
			lresult = ref new TopologyNode(IntPtr(mISampleGrabberCall.get()));

		} while (false);

		return lresult;
	}
}