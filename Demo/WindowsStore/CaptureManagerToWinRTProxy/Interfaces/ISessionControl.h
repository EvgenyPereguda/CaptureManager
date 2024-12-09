#pragma once

#include "pch.h"
#include "ISession.h"
#include "ITopologyNode.h"

using namespace Platform;
using namespace Windows::Foundation::Collections;


namespace CaptureManagerToWinRTProxy
{
	public interface class ISessionControl
	{
		ISession^ createSession(
			IVector<ITopologyNode^>^ aArrayPtrSourceNodesOfTopology);
	};
}