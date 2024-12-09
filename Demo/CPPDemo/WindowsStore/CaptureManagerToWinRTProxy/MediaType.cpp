#include "pch.h"
#include "MediaType.h"


namespace CaptureManagerToWinRTProxy
{
	MediaType::MediaType(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mPtrIUnkMediaType));

		} while (false);
	}
	
	IntPtr MediaType::getMediaType()
	{
		IntPtr lresult = nullptr;

		do
		{	
			lresult = IntPtr(mPtrIUnkMediaType.get());

		} while (false);

		return lresult;
	}
}