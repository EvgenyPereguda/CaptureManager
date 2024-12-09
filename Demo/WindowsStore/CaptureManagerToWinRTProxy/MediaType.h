#pragma once

#include "Interfaces/IMediaType.h"
#include "ComPtrCustom.h"


using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	ref class MediaType sealed :
		public IMediaType
	{
		CComPtrCustom<IUnknown> mPtrIUnkMediaType;

	public:
		MediaType(IntPtr aIntPtr);
				
		IntPtr getMediaType();
	};

}