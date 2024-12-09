#pragma once

#include "pch.h"
#include "ITopologyNode.h"
#include "IIntPtr.h"

using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml::Controls;


namespace CaptureManagerToWinRTProxy
{
	public interface class IEVRMultiSinkFactory
	{
		void createOutputNodes(
			SwapChainPanel^ aSwapChainPanel,
			IVector<ITopologyNode^>^ aPtrArrayPtrTopologyOutputNodes);

		void createOutputNodes(
			IIntPtr^ aIIntPtr,
			IVector<ITopologyNode^>^ aPtrArrayPtrTopologyOutputNodes);
	};
}