#pragma once

#include "pch.h"
#include "IInitilaizeCaptureSource.h"
#include "ICurrentMediaType.h"
#include "ISourceRequestResult.h"

using namespace Platform;

namespace CaptureManagerToWinRTProxy
{
	public interface class ICaptureProcessor
	{
		void initilaize(IInitilaizeCaptureSource^ IInitilaizeCaptureSource);
		void pause();
		void setCurrentMediaType(ICurrentMediaType^ aICurrentMediaType);
		void shutdown();
		void sourceRequest(ISourceRequestResult^ aISourceRequestResult);
		void start(int64 aStartPositionInHundredNanosecondUnits, Guid aGUIDTimeFormat);
		void stop();
	};
}