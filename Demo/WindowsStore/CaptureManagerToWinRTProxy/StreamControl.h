#pragma once

#include "Interfaces/IStreamControl.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"

namespace CaptureManagerToWinRTProxy
{
	ref class StreamControl sealed :
		public IStreamControl
	{
		CComPtrCustom<::IStreamControl> mIStreamControl;
	public:
		StreamControl(IntPtr aIntPtr);

		virtual String^ getCollectionOfStreamControlNodeFactories();

		virtual ISpreaderNodeFactory^ createSpreaderNodeFactory();

		virtual ISpreaderNodeFactory^ createStreamControlNodeFactory(Guid aREFIID);
	};
}