#pragma once
#include <Unknwnbase.h>

namespace CaptureManager
{
	namespace Sinks
	{
		MIDL_INTERFACE("77FAEB7D-6D97-40AE-B17B-21BA5C022145")
		IStreamSink : public IUnknown
		{
			virtual HRESULT STDMETHODCALLTYPE getMaxRate(BOOL aThin, float* aPtrRate)=0;
			virtual HRESULT STDMETHODCALLTYPE preroll() = 0;
			virtual HRESULT STDMETHODCALLTYPE shutdown()=0;
		};
	}
}