#pragma once

#include "pch.h"

using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	public interface class IWebCamControl
	{
		String^ getCamParametrs();
		
		void getCamParametr(
		uint32 aParametrIndex,
		int32 *aCurrentValue,
		int32 *aMin,
		int32 *aMax,
		int32 *aStep,
		int32 *aDefault,
		int32 *aFlag);

		void setCamParametr(
			uint32 aParametrIndex,
			int32 aNewValue,
			int32 aFlag);
	};
}