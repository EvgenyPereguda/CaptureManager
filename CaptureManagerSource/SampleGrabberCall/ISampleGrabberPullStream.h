#pragma once
#include <Unknwnbase.h>

namespace CaptureManager
{
	namespace Sinks
	{
		namespace SampleGrabberPull
		{

			MIDL_INTERFACE("EAD2EA22-B508-495F-A805-D14607A01962")
			ISampleGrabberPullStream : public IUnknown
			{
				virtual STDMETHODIMP start()PURE;

				virtual STDMETHODIMP stop()PURE;
			};
		}
	}
}