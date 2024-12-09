#pragma once

#include <Unknwn.h>
#include "../Common/MFHeaders.h"

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			MIDL_INTERFACE("CCD10ACD-DE84-4E12-9BDE-E3102B345CDF")
				IDeviceEvent : public IUnknown
			{
				virtual HRESULT registerEventCallback(IUnknown* aPtrUnkTopologyNode, IMFAsyncCallback* aPtrIMFAsyncCallback) = 0;

				virtual HRESULT releaseDeviceEvent() = 0;
			};
		}
	}
}
