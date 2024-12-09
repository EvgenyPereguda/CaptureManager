#pragma once

#include "IVersionControl.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"

namespace CaptureManagerToWinRTProxy
{
	ref class VersionControl sealed :
		public IVersionControl
	{
		CComPtrCustom<::IVersionControl> mIVersionControl;
	public:
		VersionControl(IntPtr aIntPtr);


		virtual String^ getVersion(
			uint32 *aPtrMAJOR,
			uint32 *aPtrMINOR,
			uint32 *aPtrPATCH);

		virtual String^ getXMLStringVersion();

		virtual Boolean checkVersion(
			uint32 aMAJOR,
			uint32 aMINOR,
			uint32 aPATCH);
	};
}