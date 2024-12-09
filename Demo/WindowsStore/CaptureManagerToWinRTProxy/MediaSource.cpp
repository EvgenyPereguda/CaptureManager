#include "pch.h"
#include "MediaSource.h"


namespace CaptureManagerToWinRTProxy
{
	MediaSource::MediaSource(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mPtrIUnkMediaSource));

		} while (false);
	}

	IntPtr MediaSource::getMediaSource()
	{
		IntPtr lresult = nullptr;

		do
		{
			lresult = IntPtr(mPtrIUnkMediaSource.get());

		} while (false);

		return lresult;
	}
}