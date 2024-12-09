#include "pch.h"
#include "MediaTypeParser.h"
#include "MediaType.h"


namespace CaptureManagerToWinRTProxy
{
	MediaTypeParser::MediaTypeParser(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mIMediaTypeParser));

		} while (false);
	}

	String^ MediaTypeParser::parse(IMediaType^ aPtrMediaType)
	{
		String^ lresult = nullptr;

		do
		{
			if (!mIMediaTypeParser)
				break;

			if (aPtrMediaType == nullptr)
				break;

			MediaType^ lMediaType = (MediaType^)aPtrMediaType;

			if (lMediaType == nullptr)
				break;

			auto lPtrUnkMediaType = reinterpret_cast<IUnknown*>((void*)lMediaType->getMediaType());

			if (lPtrUnkMediaType == nullptr)
				break;

			CComPtrCustom<IUnknown> lOutputNode;

			BSTR lXmlString = nullptr;

			auto lhr = mIMediaTypeParser->parse(
				lPtrUnkMediaType,
				&lXmlString);

			if (FAILED(lhr))
				break;

			if (lXmlString == nullptr)
				break;

			lresult = ref new String(lXmlString);

			SysFreeString(lXmlString);

		} while (false);

		return lresult;
	}
}