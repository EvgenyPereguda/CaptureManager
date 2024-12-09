#pragma once

#include <unordered_map>

#include "ISession.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"

namespace CaptureManagerToWinRTProxy
{
	ref class Session sealed :
		public ISession
	{
		CComPtrCustom<::ISession> mISession;

		std::unordered_map<DWORD, CComPtrCustom<::ISessionCallback>> mISessionCallbacks;		

	public:
		Session(IntPtr aIntPtr);


		virtual Boolean startSession(
			int64 aStartPositionInHundredNanosecondUnits,
			Guid aGUIDTimeFormat);

		virtual Boolean stopSession();

		virtual Boolean pauseSession();

		virtual Boolean closeSession();

		virtual Boolean getSessionDescriptor(uint32 *aPtrSessionDescriptor);

		virtual Boolean attachISessionCallback(ISessionCallback^ aISessionCallback);
	};
}