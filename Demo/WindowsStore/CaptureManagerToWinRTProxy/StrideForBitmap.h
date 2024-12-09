#pragma once

#include "Interfaces/IStrideForBitmap.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"

namespace CaptureManagerToWinRTProxy
{
	ref class StrideForBitmap sealed :
		public IStrideForBitmap
	{
		CComPtrCustom<::IStrideForBitmap> mIStrideForBitmap;
	public:
		StrideForBitmap(IntPtr aIntPtr);

		virtual bool getStrideForBitmap(
			Guid aMFVideoFormat,
			uint32 aWidthInPixels,
			int32 *aPtrStride);
	};
}