#include "pch.h"
#include "StrideForBitmap.h"


namespace CaptureManagerToWinRTProxy
{
	StrideForBitmap::StrideForBitmap(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mIStrideForBitmap));

		} while (false);
	}

	bool StrideForBitmap::getStrideForBitmap(
		Guid aMFVideoFormat,
		uint32 aWidthInPixels,
		int32 *aPtrStride)
	{
		bool lresult = false;

		do
		{
			if (!mIStrideForBitmap)
				break;
			
			if (aPtrStride == nullptr)
				break;

			CComPtrCustom<IUnknown> lOutputNode;

			BSTR lXmlString = nullptr;

			auto lhr = mIStrideForBitmap->getStrideForBitmap(
				aMFVideoFormat,
				aWidthInPixels,
				(LONG*)aPtrStride);

			if (FAILED(lhr))
				break;

			lresult = true;

		} while (false);

		return lresult;
	}
}