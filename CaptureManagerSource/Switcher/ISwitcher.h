#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>

#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"

namespace CaptureManager
{
	namespace Transform
	{
		MIDL_INTERFACE("002BD1DC-84D5-43FC-8418-65CB4FFF54F7")
		ISwitcher : public IUnknown
		{
			virtual HRESULT STDMETHODCALLTYPE pause() = 0;

			virtual HRESULT STDMETHODCALLTYPE resume() = 0;

			virtual HRESULT STDMETHODCALLTYPE reset() = 0;
		};
	}
}