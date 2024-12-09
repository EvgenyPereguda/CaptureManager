#include "pch.h"
#include "EncoderControl.h"
#include "EncoderNodeFactory.h"
#include "MediaType.h"


namespace CaptureManagerToWinRTProxy
{
	EncoderControl::EncoderControl(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mIEncoderControl));

		} while (false);
	}

	String^ EncoderControl::getCollectionOfEncoders()
	{
		String^ lresult = nullptr;

		do
		{
			if (!mIEncoderControl)
				break;

			BSTR lXmlString = nullptr;

			auto lhr = mIEncoderControl->getCollectionOfEncoders(&lXmlString);

			if (FAILED(lhr))
				break;

			if (lXmlString == nullptr)
				break;

			lresult = ref new String(lXmlString);

			SysFreeString(lXmlString);

		} while (false);

		return lresult;
	}

	String^ EncoderControl::getMediaTypeCollectionOfEncoder(
		IMediaType^ aPtrUncompressedMediaType,
		Guid aRefEncoderCLSID)
	{
		String^ lresult = nullptr;

		do
		{
			if (!mIEncoderControl)
				break;

			if (aPtrUncompressedMediaType == nullptr)
				break;

			MediaType^ lMediaType = (MediaType^)aPtrUncompressedMediaType;

			if (lMediaType == nullptr)
				break;

			auto lPtrUnkUncompressedMediaType = reinterpret_cast<IUnknown*>((void*)lMediaType->getMediaType());

			if (lPtrUnkUncompressedMediaType == nullptr)
				break;

			BSTR lXmlString = nullptr;

			auto lhr = mIEncoderControl->getMediaTypeCollectionOfEncoder(
				lPtrUnkUncompressedMediaType,
				aRefEncoderCLSID,
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

	IEncoderNodeFactory^ EncoderControl::createEncoderNodeFactory(
		Guid aRefEncoderCLSID)
	{
		IEncoderNodeFactory^ lresult = nullptr;

		do
		{
			if (!mIEncoderControl)
				break;

			CComPtrCustom<IUnknown> lEncoderNodeFactory;

			auto lhr = mIEncoderControl->createEncoderNodeFactory(
				aRefEncoderCLSID,
				__uuidof(::IEncoderNodeFactory),
				&lEncoderNodeFactory);

			if (FAILED(lhr))
				break;
			
			lresult = ref new EncoderNodeFactory(IntPtr(lEncoderNodeFactory.get()));
			
		} while (false);

		return lresult;
	}

}