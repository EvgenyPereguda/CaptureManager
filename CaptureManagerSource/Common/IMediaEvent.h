#pragma once
#include <Unknwnbase.h>

namespace CaptureManager
{
	namespace Sources
	{

		MIDL_INTERFACE("3B66046C-01FC-424F-BBC5-2667555A4954")
			IMediaEvent : public IUnknown
		{
			virtual HRESULT STDMETHODCALLTYPE PostEvent(
				DWORD aMediaEventType,
				REFGUID aGUIDExtendedType,
				HRESULT aHRStatus,
				IUnknown* aPtrUnk) = 0;

		};
	}
}
