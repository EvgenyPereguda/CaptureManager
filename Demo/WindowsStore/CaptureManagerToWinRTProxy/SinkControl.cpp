#include "pch.h"
#include "SinkControl.h"
#include "FileSinkFactory.h"
#include "ByteStreamSinkFactory.h"
#include "EVRSinkFactory.h"
#include "EVRMultiSinkFactory.h"
#include "SampleGrabberCallbackSinkFactory.h"
#include "SampleGrabberCallSinkFactory.h"


namespace CaptureManagerToWinRTProxy
{
	SinkControl::SinkControl(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mISinkControl));

		} while (false);
	}
	
	String^ SinkControl::getCollectionOfSinks()
	{
		Platform::String^ lresult = nullptr;

		do
		{
			if (!mISinkControl)
				break;

			BSTR lXMLstring = nullptr;

			CHK(mISinkControl->getCollectionOfSinks(&lXMLstring));

			if (lXMLstring == nullptr)
				break;

			lresult = ref new String(lXMLstring);

			SysFreeString(lXMLstring);

		} while (false);

		return lresult;
	}

	CComPtrCustom<IUnknown> SinkControl::createSinkFactory(
		Guid aRefContainerTypeGUID,
		Guid aREFIID)
	{
		CComPtrCustom<IUnknown> lresult;

		do
		{
			if (!mISinkControl)
				break;

			BSTR lXMLstring = nullptr;

			CComPtrCustom<IUnknown> lUnkSink;

			CHK(mISinkControl->createSinkFactory(
				aRefContainerTypeGUID,
				aREFIID,
				&lUnkSink));

			lresult = lUnkSink;
			
		} while (false);

		return lresult;
	}

	IFileSinkFactory^ SinkControl::createIFileSinkFactory(Guid aRefContainerTypeGUID)
	{
		IFileSinkFactory^ lresult = nullptr;

		do
		{
			if (!mISinkControl)
				break;

			BSTR lXMLstring = nullptr;

			CComPtrCustom<IUnknown> lUnkSink;

			lUnkSink = createSinkFactory(
				aRefContainerTypeGUID,
				__uuidof(::IFileSinkFactory));

			if (!lUnkSink)
				break;
						
			lresult = ref new FileSinkFactory(IntPtr(lUnkSink.get()));

		} while (false);

		return lresult;
	}

	//IByteStreamSinkFactory^ SinkControl::createIByteStreamSinkFactory(
	//	Guid aRefContainerTypeGUID)
	//{
	//	IByteStreamSinkFactory^ lresult = nullptr;

	//	do
	//	{
	//		if (!mISinkControl)
	//			break;

	//		BSTR lXMLstring = nullptr;

	//		CComPtrCustom<IUnknown> lUnkSink;

	//		auto lObject = createSinkFactory(
	//			aRefContainerTypeGUID,
	//			__uuidof(::IByteStreamSinkFactory));

	//		if (lObject == nullptr)
	//			break;

	//		lresult = ref new ByteStreamSinkFactory(lObject);

	//	} while (false);

	//	return lresult;
	//}

	IEVRSinkFactory^ SinkControl::createIEVRSinkFactory()
	{
		IEVRSinkFactory^ lresult = nullptr;

		do
		{
			if (!mISinkControl)
				break;

			BSTR lXMLstring = nullptr;

			CComPtrCustom<IUnknown> lUnkSink;

			lUnkSink = createSinkFactory(
				Guid::Guid(),
				__uuidof(::IEVRSinkFactory));

			if (!lUnkSink)
				break;
			
			lresult = ref new EVRSinkFactory(IntPtr(lUnkSink.get()));

		} while (false);

		return lresult;
	}

	IEVRMultiSinkFactory^ SinkControl::createIEVRMultiSinkFactory()
	{
		IEVRMultiSinkFactory^ lresult = nullptr;

		do
		{
			if (!mISinkControl)
				break;

			BSTR lXMLstring = nullptr;

			CComPtrCustom<IUnknown> lUnkSink;

			lUnkSink = createSinkFactory(
				Guid::Guid(),
				__uuidof(::IEVRMultiSinkFactory));

			if (!lUnkSink)
				break;

			lresult = ref new EVRMultiSinkFactory(IntPtr(lUnkSink.get()));

		} while (false);

		return lresult;
	}

	ISampleGrabberCallbackSinkFactory^ SinkControl::createISampleGrabberCallbackSinkFactory(
		Guid aRefContainerTypeGUID)
	{
		ISampleGrabberCallbackSinkFactory^ lresult = nullptr;

		do
		{
			if (!mISinkControl)
				break;

			BSTR lXMLstring = nullptr;

			CComPtrCustom<IUnknown> lUnkSink;

			lUnkSink = createSinkFactory(
				aRefContainerTypeGUID,
				__uuidof(::ISampleGrabberCallbackSinkFactory));

			if (!lUnkSink)
				break;

			lresult = ref new SampleGrabberCallbackSinkFactory(IntPtr(lUnkSink.get()));

		} while (false);

		return lresult;
	}

	ISampleGrabberCallSinkFactory^ SinkControl::createISampleGrabberCallSinkFactory(
		Guid aRefContainerTypeGUID)
	{
		ISampleGrabberCallSinkFactory^ lresult = nullptr;

		do
		{
			if (!mISinkControl)
				break;

			BSTR lXMLstring = nullptr;

			CComPtrCustom<IUnknown> lUnkSink;

			lUnkSink = createSinkFactory(
				aRefContainerTypeGUID,
				__uuidof(::ISampleGrabberCallSinkFactory));

			if (!lUnkSink)
				break;

			lresult = ref new SampleGrabberCallSinkFactory(IntPtr(lUnkSink.get()));

		} while (false);

		return lresult;
	}
}

