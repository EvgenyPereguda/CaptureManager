#pragma once

#include "pch.h"
#include "IMediaType.h"
#include "ITopologyNode.h"

using namespace Platform;
using namespace Windows::Foundation::Collections;


namespace CaptureManagerToWinRTProxy
{
	public interface class IFileSinkFactory
	{
		void createOutputNodes(
			IVector<IMediaType^>^ aArrayPtrCompressedMediaTypes,
			String^ aPtrFileName,
			IVector<ITopologyNode^>^ aPtrArrayPtrTopologyOutputNodes);
	};
}