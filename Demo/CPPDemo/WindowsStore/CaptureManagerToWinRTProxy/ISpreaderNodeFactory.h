#pragma once

#include "pch.h"
#include "ITopologyNode.h"

using namespace Platform;
using namespace Windows::Foundation::Collections;


namespace CaptureManagerToWinRTProxy
{	
	public interface class ISpreaderNodeFactory
	{
		ITopologyNode^ createSpreaderNode(
			IVector<ITopologyNode^>^ aArrayPtrDownStreamTopologyNodes);
	};
}