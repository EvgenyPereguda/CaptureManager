#pragma once

#include <Unknwnbase.h>

namespace CaptureManager
{
	typedef DWORD SessionDescriptor;

	typedef DWORD CallbackEventCodeDescriptor;

	enum SessionCallbackEventCode : CallbackEventCodeDescriptor
	{
		Unknown = 0,
		Error = Unknown + 1,
		Status_Error = Error + 1,
		Execution_Error = Status_Error + 1,
		ItIsReadyToStart = Execution_Error + 1,
		ItIsStarted = ItIsReadyToStart + 1,
		ItIsPaused = ItIsStarted + 1,
		ItIsStopped = ItIsPaused + 1,
		ItIsEnded = ItIsStopped + 1,
		ItIsClosed = ItIsEnded + 1,
		VideoCaptureDeviceRemoved = ItIsClosed + 1,
		SnapTrigger = VideoCaptureDeviceRemoved + 1
	};
	
	MIDL_INTERFACE("E340800C-01D3-414D-9603-8CC2DE3DF8AB")
	ISessionCallbackInner : public IUnknown
	{
		virtual void Invoke(
			CallbackEventCodeDescriptor aCallbackEventCode,
			SessionDescriptor aSessionDescriptor) = 0;
	};
}