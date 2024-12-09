#include "pch.h"
#include "StreamControl.h"
#include "SpreaderNodeFactory.h"


namespace CaptureManagerToWinRTProxy
{
	StreamControl::StreamControl(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mIStreamControl));

		} while (false);
	}

	String^ StreamControl::getCollectionOfStreamControlNodeFactories()
	{
		String^ lresult = nullptr;

		do
		{
			if (!mIStreamControl)
				break;
			
			BSTR lXmlString = nullptr;

			auto lhr = mIStreamControl->getCollectionOfStreamControlNodeFactories(&lXmlString);

			if (FAILED(lhr))
				break;

			if (lXmlString == nullptr)
				break;

			lresult = ref new String(lXmlString);

			SysFreeString(lXmlString);

		} while (false);

		return lresult;
	}
	
	ISpreaderNodeFactory^ StreamControl::createSpreaderNodeFactory()
	{
		return createStreamControlNodeFactory(__uuidof(::ISpreaderNodeFactory));
	}
	
	ISpreaderNodeFactory^ StreamControl::createStreamControlNodeFactory(Guid aREFIID)
	{
		ISpreaderNodeFactory^ lresult = nullptr;

		do
		{
			if (!mIStreamControl)
				break;
			
			CComPtrCustom<IUnknown> lStreamControlNodeFactory;

			auto lhr = mIStreamControl->createStreamControlNodeFactory(
				aREFIID,
				&lStreamControlNodeFactory);

			if (FAILED(lhr))
				break;

			lresult = ref new SpreaderNodeFactory(IntPtr(lStreamControlNodeFactory.get()));

		} while (false);

		return lresult;
	}
}