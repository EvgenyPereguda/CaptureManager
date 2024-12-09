#pragma once

#include "pch.h"
#include "ISampleGrabberCallback.h"
#include "ITopologyNode.h"
#include "IWriteableBitmapCallback.h"

using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	public interface class ISampleGrabberCallbackSinkFactory
	{
		ITopologyNode^ createOutputNode(
			Guid aRefMajorType,
			Guid aRefSubType,
			ISampleGrabberCallback^ aPtrISampleGrabberCallback);

		ITopologyNode^ createOutputNode(
			Guid aRefMajorType,
			Guid aRefSubType,
			int32 aWidth,
			int32 aHeight,
			IWriteableBitmapCallback^ aPtrIWriteableBitmapCallback);
	};
}