#include "pch.h"
#include "SampleGrabberCallSinkFactory.h"
#include "TopologyNode.h"
#include "SampleGrabberCall.h"


namespace CaptureManagerToWinRTProxy
{
	SampleGrabberCallSinkFactory::SampleGrabberCallSinkFactory(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mISampleGrabberCallSinkFactory));

		} while (false);
	}

	ISampleGrabberCall^ SampleGrabberCallSinkFactory::createSampleGrabberCall(
		Guid aRefMajorType,
		Guid aRefSubType,
		uint32 aSampleByteSize)
	{
		ISampleGrabberCall^ lresult = nullptr;

		do
		{
			if (!mISampleGrabberCallSinkFactory)
				break;

			CComPtrCustom<IUnknown> lISampleGrabberCall;

			auto lhr = mISampleGrabberCallSinkFactory->createOutputNode(
				aRefMajorType,
				aRefSubType,
				aSampleByteSize,
				__uuidof(::ISampleGrabberCall),
				&lISampleGrabberCall);

			if (FAILED(lhr))
				break;

			if (!lISampleGrabberCall)
				break;
			
			lresult = ref new SampleGrabberCall(IntPtr(lISampleGrabberCall.get()));

		} while (false);

		return lresult;
	}
}