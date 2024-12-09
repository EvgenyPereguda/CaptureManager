#pragma once

#include <Unknwnbase.h>

namespace CaptureManager
{
	MIDL_INTERFACE("7A34C65C-5A54-4116-9FD9-1F75330A58B2")
		ILogPrintOutAsyncCallback : public IUnknown
	{
		STDMETHOD(AddCallbackInner)(
			/* [in] */ IUnknown* aPtrUnkCallbackInner) = 0;
	};
}
