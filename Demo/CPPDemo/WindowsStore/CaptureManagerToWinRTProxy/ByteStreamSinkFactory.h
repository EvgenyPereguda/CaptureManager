#pragma once

#include "IByteStreamSinkFactory.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"

namespace CaptureManagerToWinRTProxy
{
	ref class ByteStreamSinkFactory sealed :
		public IByteStreamSinkFactory
	{
		CComPtrCustom<::IByteStreamSinkFactory> mIByteStreamSinkFactory;
	public:
		ByteStreamSinkFactory(IntPtr aIntPtr);

		virtual void createOutputNodes(
			IVector<Object^>^ aArrayPtrCompressedMediaTypes,
			Object^ aPtrByteStream,
			IVector<Object^>^ aPtrArrayPtrTopologyOutputNodes);
	};
}