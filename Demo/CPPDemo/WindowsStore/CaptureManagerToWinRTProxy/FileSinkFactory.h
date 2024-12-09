#pragma once

#include "IFileSinkFactory.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"

namespace CaptureManagerToWinRTProxy
{
	ref class FileSinkFactory sealed :
		public IFileSinkFactory
	{

		CComPtrCustom<::IFileSinkFactory> mIFileSinkFactory;

	public:
		FileSinkFactory(IntPtr aIntPtr);

		virtual void createOutputNodes(
			IVector<IMediaType^>^ aArrayPtrCompressedMediaTypes,
			String^ aPtrFileName,
			IVector<ITopologyNode^>^ aPtrArrayPtrTopologyOutputNodes);
	};
}