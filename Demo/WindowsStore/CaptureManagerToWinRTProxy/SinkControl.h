#pragma once

#include "Interfaces/ISinkControl.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"

namespace CaptureManagerToWinRTProxy
{
	ref class SinkControl sealed :
		public ISinkControl
	{

		CComPtrCustom<::ISinkControl> mISinkControl;

	public:
		SinkControl(IntPtr aIntPtr);

		virtual String^ getCollectionOfSinks();
		
		virtual IFileSinkFactory^ createIFileSinkFactory(
			Guid aRefContainerTypeGUID);
		
		//virtual IByteStreamSinkFactory^ createIByteStreamSinkFactory(
		//	Guid aRefContainerTypeGUID);

		virtual IEVRSinkFactory^ createIEVRSinkFactory();

		virtual IEVRMultiSinkFactory^ createIEVRMultiSinkFactory();

		virtual ISampleGrabberCallbackSinkFactory^ createISampleGrabberCallbackSinkFactory(
			Guid aRefContainerTypeGUID);

		virtual ISampleGrabberCallSinkFactory^ createISampleGrabberCallSinkFactory(
			Guid aRefContainerTypeGUID);

	private:

		CComPtrCustom<IUnknown> createSinkFactory(
			Guid aRefContainerTypeGUID,
			Guid aREFIID);
	};
}