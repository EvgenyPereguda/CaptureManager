#pragma once
#include <Unknwnbase.h>

namespace CaptureManager
{
	namespace Sinks
	{
		namespace EVR
		{
			MIDL_INTERFACE("F835B60E-E54A-434B-BBB6-486DF7CE0E64")
			IRenderingControl : public IUnknown
			{
				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE enableInnerRendering(
				BOOL aMTProtect) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE renderToTarget(
					IUnknown* aPtrRenderTarget,
					BOOL aCopyMode) = 0;
			};
		}
	}
}

