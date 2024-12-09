#pragma once

#include "Interfaces/ISampleGrabberCallbackSinkFactory.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"

namespace CaptureManagerToWinRTProxy
{
	ref class SampleGrabberCallbackSinkFactory sealed :
		public ISampleGrabberCallbackSinkFactory
	{

		CComPtrCustom<::ISampleGrabberCallbackSinkFactory> mISampleGrabberCallbackSinkFactory;
	public:
		SampleGrabberCallbackSinkFactory(IntPtr aIntPtr);

		virtual ITopologyNode^ createOutputNode(
			Guid aRefMajorType,
			Guid aRefSubType,
			ISampleGrabberCallback^ aPtrISampleGrabberCallback);

		virtual ITopologyNode^ createOutputNode(
			Guid aRefMajorType,
			Guid aRefSubType,
			int32 aWidth,
			int32 aHeight,
			IWriteableBitmapCallback^ aPtrIWriteableBitmapCallback);
	};
}