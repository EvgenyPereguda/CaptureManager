#pragma once
#include <Unknwnbase.h>

namespace CaptureManager
{
	namespace Sources
	{

		MIDL_INTERFACE("267B296D-0C69-46F7-B879-49CBB8C8BAB8")
		IStreamControl : public IUnknown
		{
			virtual HRESULT shutdown() = 0;

			virtual HRESULT isActive(BOOL* aPtrState) = 0;

			virtual HRESULT queueRequestToken(
				IUnknown* aPtrToken) = 0;

			virtual HRESULT QueueEvent(
				MediaEventType aMediaEventType,
				REFGUID aGUIDExtendedType,
				HRESULT aHRStatus,
				IUnknown* aPtrUnk) = 0;

			virtual HRESULT activate(BOOL aActivate)=0;

			virtual HRESULT pause() = 0;

			virtual HRESULT stop() = 0;

			virtual HRESULT start() = 0;		
			
		};
	}
}