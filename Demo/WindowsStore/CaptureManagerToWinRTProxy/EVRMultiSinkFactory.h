#pragma once

#include "Interfaces/IEVRMultiSinkFactory.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"

using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml::Controls;

namespace CaptureManagerToWinRTProxy
{


	ref class EVRMultiSinkFactory sealed
		: public IEVRMultiSinkFactory
	{
		CComPtrCustom<::IEVRMultiSinkFactory> mIEVRMultiSinkFactory;

	public:
		EVRMultiSinkFactory(IntPtr aIntPtr);

		virtual void createOutputNodes(
			SwapChainPanel^ aSwapChainPanel,
			IVector<ITopologyNode^>^ aPtrArrayPtrTopologyOutputNodes);
		
		virtual void createOutputNodes(
			IIntPtr^ aIIntPtr,
			IVector<ITopologyNode^>^ aPtrArrayPtrTopologyOutputNodes);
	};
}

