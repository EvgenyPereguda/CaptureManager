#pragma once

#include "Interfaces/IWebCamControl.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"

namespace CaptureManagerToWinRTProxy
{
	ref class WebCamControl sealed :
		public IWebCamControl
	{
		CComPtrCustom<::IWebCamControl> mIWebCamControl;
	public:
		WebCamControl(IntPtr aIntPtr);


		virtual String^ getCamParametrs();

		virtual void getCamParametr(
			uint32 aParametrIndex,
			int32 *aCurrentValue,
			int32 *aMin,
			int32 *aMax,
			int32 *aStep,
			int32 *aDefault,
			int32 *aFlag);

		virtual void setCamParametr(
			uint32 aParametrIndex,
			int32 aNewValue,
			int32 aFlag);
	};
}