#pragma once

#include <Unknwnbase.h>

namespace CaptureManager
{
	MIDL_INTERFACE("960C9316-9AE2-40E8-B420-C8BF32AC2A38")
		ILogPrintOutCallbackInner : public IUnknown
	{
		virtual void Invoke(
			/* [in] */ DWORD aLevelType,
			/* [in] */ BSTR aPtrLogString) = 0;


		virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setVerbose(
			/* [in] */ DWORD aLevelType,
			/* [in] */ boolean aState) = 0;
	};
}
