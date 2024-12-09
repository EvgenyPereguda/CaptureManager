#pragma once
#include <Unknwnbase.h>

namespace CaptureManager
{
	namespace Sinks
	{
		namespace EVR
		{
			MIDL_INTERFACE("1BBFF00F-6425-4E10-81B2-754E9A1D8F2A")
			IPresenterInit : public IUnknown
			{
				virtual HRESULT initializeSharedTarget(
				HANDLE aHandle,
				IUnknown* aPtrTarget) = 0;
			};
		}
	}
}
