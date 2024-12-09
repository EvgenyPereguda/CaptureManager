#pragma once

#include "Interfaces/ISessionControl.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"

namespace CaptureManagerToWinRTProxy
{
	ref class SessionControl sealed :
		public ISessionControl
	{

		CComPtrCustom<::ISessionControl> mISessionControl;
	public:
		SessionControl(IntPtr aIntPtr);

		virtual ISession^ createSession(
			IVector<ITopologyNode^>^ aArrayPtrSourceNodesOfTopology);
	};
}