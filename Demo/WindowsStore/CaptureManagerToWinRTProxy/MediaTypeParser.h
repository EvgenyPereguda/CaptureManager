#pragma once

#include "Interfaces/IMediaTypeParser.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"

namespace CaptureManagerToWinRTProxy
{
	ref class MediaTypeParser sealed :
		public IMediaTypeParser
	{
		CComPtrCustom<::IMediaTypeParser> mIMediaTypeParser;

	public:
		MediaTypeParser(IntPtr aIntPtr);

		virtual String^ parse(IMediaType^ aPtrMediaType);
	};
}