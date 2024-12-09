#pragma once

#include "IEncoderControl.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"

namespace CaptureManagerToWinRTProxy
{
	ref class EncoderControl sealed :
		public IEncoderControl
	{
		CComPtrCustom<::IEncoderControl> mIEncoderControl;
	public:
		EncoderControl(IntPtr aIntPtr);


		virtual String^ getCollectionOfEncoders();

		virtual String^ getMediaTypeCollectionOfEncoder(
			IMediaType^ aPtrUncompressedMediaType,
			Guid aRefEncoderCLSID);

		virtual IEncoderNodeFactory^ createEncoderNodeFactory(
			Guid aRefEncoderCLSID);
	};
}