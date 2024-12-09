#pragma once

#include <Unknwn.h>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace CaptureManager
{
	namespace Core
	{
		MIDL_INTERFACE("01C1D11E-8F52-4529-AF85-DFF47CE6D3BC")
		ICheck : public IUnknown
		{
		public:

			virtual HRESULT STDMETHODCALLTYPE check(void) = 0;

		};
	}
}