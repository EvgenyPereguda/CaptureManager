#pragma once

#include "pch.h"
#include "ISessionCallback.h"

using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	public interface class ISession
	{

		Boolean startSession(
			int64 aStartPositionInHundredNanosecondUnits,
			Guid aGUIDTimeFormat);

		Boolean stopSession();

		Boolean pauseSession();

		Boolean closeSession();

		Boolean getSessionDescriptor(uint32 *aPtrSessionDescriptor);

		Boolean attachISessionCallback(ISessionCallback^ aISessionCallback);
	};
}