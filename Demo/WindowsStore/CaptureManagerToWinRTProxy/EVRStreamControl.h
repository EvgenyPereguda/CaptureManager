#pragma once

#include "Interfaces/IEVRStreamControl.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"

namespace CaptureManagerToWinRTProxy
{
	ref class EVRStreamControl sealed
		:public IEVRStreamControl
	{

		CComPtrCustom<::IEVRStreamControl> mIEVRStreamControl;

	public:
		EVRStreamControl(IntPtr aIntPtr);
		
		virtual void flush(ITopologyNode^ aPtrEVROutputNode);

		virtual void setPosition(ITopologyNode^ aPtrEVROutputNode, float aLeft, float aRight, float aTop, float aBottom);

		virtual void setSrcPosition(ITopologyNode^ aPtrEVROutputNode, float aLeft, float aRight, float aTop, float aBottom);

		virtual void setZOrder(ITopologyNode^ aPtrEVROutputNode, uint32 aZOrder);
	};
}