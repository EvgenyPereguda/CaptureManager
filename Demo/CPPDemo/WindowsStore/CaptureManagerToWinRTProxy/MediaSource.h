#pragma once

#include "IMediaSource.h"
#include "ComPtrCustom.h"

using namespace Platform;

namespace CaptureManagerToWinRTProxy
{
	ref class MediaSource sealed :
		public IMediaSource
	{
		CComPtrCustom<IUnknown> mPtrIUnkMediaSource;

	public:
		MediaSource(IntPtr aIntPtr);

		IntPtr getMediaSource();
	};
}