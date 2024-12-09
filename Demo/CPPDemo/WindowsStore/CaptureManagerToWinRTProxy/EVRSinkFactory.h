#pragma once

#include "IEVRSinkFactory.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"

namespace CaptureManagerToWinRTProxy
{
	ref class EVRSinkFactory sealed
		:public IEVRSinkFactory
	{

		CComPtrCustom<::IEVRSinkFactory> mIEVRSinkFactory;

	public:
		EVRSinkFactory(IntPtr aIntPtr);
		
		virtual ITopologyNode^ createOutputNode(IntPtr aHWND);
	};
}