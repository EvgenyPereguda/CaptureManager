#include "pch.h"
#include "VersionControl.h"


namespace CaptureManagerToWinRTProxy
{
	VersionControl::VersionControl(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mIVersionControl));

		} while (false);
	}

	String^ VersionControl::getVersion(
		uint32 *aPtrMAJOR,
		uint32 *aPtrMINOR,
		uint32 *aPtrPATCH)
	{
		String^ lresult(nullptr);

		do
		{
			if (!mIVersionControl)
				break;

			if (aPtrMAJOR == nullptr)
				break;

			if (aPtrMINOR == nullptr)
				break;

			if (aPtrPATCH == nullptr)
				break;

			BSTR lAdditionalLabel = nullptr;

			auto lhr = mIVersionControl->getVersion(
				(DWORD*)aPtrMAJOR,
				(DWORD*)aPtrMINOR,
				(DWORD*)aPtrPATCH,
				&lAdditionalLabel);

			if (FAILED(lhr))
				break;

			lresult = ref new String(lAdditionalLabel);

			SysFreeString(lAdditionalLabel);

		} while (false);

		return lresult;
	}

	String^ VersionControl::getXMLStringVersion()
	{
		String^ lresult(nullptr);

		do
		{
			if (!mIVersionControl)
				break;
			
			BSTR lXmlString = nullptr;

			auto lhr = mIVersionControl->getXMLStringVersion(
				&lXmlString);

			if (FAILED(lhr))
				break;

			lresult = ref new String(lXmlString);

			SysFreeString(lXmlString);

		} while (false);

		return lresult;
	}

	Boolean VersionControl::checkVersion(
		uint32 aMAJOR,
		uint32 aMINOR,
		uint32 aPATCH)
	{
		Boolean lresult(false);

		do
		{
			if (!mIVersionControl)
				break;

			boolean lResult;
						
			auto lhr = mIVersionControl->checkVersion(
				aMAJOR,
				aMINOR,
				aPATCH,
				&lResult);

			if (FAILED(lhr))
				break;

			lresult = lResult == TRUE;

		} while (false);

		return lresult;
	}
}