#pragma once

#include "ISpreaderNodeFactory.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"

namespace CaptureManagerToWinRTProxy
{
	ref class SpreaderNodeFactory sealed :
		public ISpreaderNodeFactory
	{
		CComPtrCustom<::ISpreaderNodeFactory> mISpreaderNodeFactory;
	public:
		SpreaderNodeFactory(IntPtr aIntPtr);

		virtual ITopologyNode^ createSpreaderNode(
			IVector<ITopologyNode^>^ aArrayPtrDownStreamTopologyNodes);
	};
}