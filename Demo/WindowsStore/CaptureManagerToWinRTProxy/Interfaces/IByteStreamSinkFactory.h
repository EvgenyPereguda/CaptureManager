#pragma once

#include "pch.h"

using namespace Platform;
using namespace Windows::Foundation::Collections;


namespace CaptureManagerToWinRTProxy
{
	interface class IByteStreamSinkFactory
	{
		void createOutputNodes(
			IVector<Object^>^ aArrayPtrCompressedMediaTypes,
			Object^ aPtrByteStream,
			IVector<Object^>^ aPtrArrayPtrTopologyOutputNodes);
	};
}