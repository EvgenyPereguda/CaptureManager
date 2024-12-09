#pragma once
#include <guiddef.h>
#include <map>
#include <string>

namespace CaptureManager
{
	namespace COMServer
	{ 
		MIDL_INTERFACE("AD3BFF6F-5E3B-4312-AEEA-AA99C9FEE7E5")
		IContainer : public IUnknown
		{
		public:
			STDMETHOD(setContainerFormat)(
				REFGUID aRefContainerTypeGUID) PURE;

		private:

		};
	}
}