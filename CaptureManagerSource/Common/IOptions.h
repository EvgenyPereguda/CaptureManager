#pragma once

#include <Unknwn.h>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace CaptureManager
{
	namespace Core
	{
		MIDL_INTERFACE("7D29163F-0F68-45A3-980D-6F96D7B8E421")
		IOptions : public IUnknown
		{
		public:

			virtual HRESULT STDMETHODCALLTYPE setOptions(const OLECHAR* aOptionsString) = 0;

		};
	}
}