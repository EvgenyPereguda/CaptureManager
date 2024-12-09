#pragma once

#include "IEncoderNodeFactory.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"

namespace CaptureManagerToWinRTProxy
{
	ref class EncoderNodeFactory sealed :
		public IEncoderNodeFactory
	{
		CComPtrCustom<::IEncoderNodeFactory> mIEncoderNodeFactory;
	public:
		EncoderNodeFactory(IntPtr aIntPtr);


		virtual IMediaType^ createCompressedMediaType(
			IMediaType^ aPtrUncompressedMediaType,
			Guid aRefEncodingModeGUID,
			uint32 aEncodingModeValue,
			uint32 aIndexCompressedMediaType);

		virtual ITopologyNode^ createEncoderNode(
			IMediaType^ aPtrUncompressedMediaType,
			Guid aRefEncodingModeGUID,
			uint32 aEncodingModeValue,
			uint32 aIndexCompressedMediaType,
			ITopologyNode^ aPtrDownStreamNode);
	};
}