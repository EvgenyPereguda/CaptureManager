#include "pch.h"
#include "WebCamControl.h"


namespace CaptureManagerToWinRTProxy
{
	WebCamControl::WebCamControl(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mIWebCamControl));

		} while (false);
	}

	String^ WebCamControl::getCamParametrs()
	{
		String^ lresult = nullptr;

		do
		{
			if (!mIWebCamControl)
				break;

			BSTR lXmlString = nullptr;

			auto lhr = mIWebCamControl->getCamParametrs(&lXmlString);

			if (FAILED(lhr))
				break;

			if (lXmlString == nullptr)
				break;

			lresult = ref new String(lXmlString);

			SysFreeString(lXmlString);

		} while (false);

		return lresult;
	}

	void WebCamControl::getCamParametr(
		uint32 aParametrIndex,
		int32 *aCurrentValue,
		int32 *aMin,
		int32 *aMax,
		int32 *aStep,
		int32 *aDefault,
		int32 *aFlag)
	{
		do
		{
			if (!mIWebCamControl)
				break;
			
			auto lhr = mIWebCamControl->getCamParametr(
				aParametrIndex,
				(LONG*)aCurrentValue,
				(LONG*)aMin,
				(LONG*)aMax,
				(LONG*)aStep,
				(LONG*)aDefault,
				(LONG*)aFlag);

			if (FAILED(lhr))
				break;
			
		} while (false);
	}

	void WebCamControl::setCamParametr(
		uint32 aParametrIndex,
		int32 aNewValue,
		int32 aFlag)
	{
		do
		{
			if (!mIWebCamControl)
				break;

			auto lhr = mIWebCamControl->setCamParametr(
				aParametrIndex,
				aNewValue,
				aFlag);

			if (FAILED(lhr))
				break;

		} while (false);
	}
}