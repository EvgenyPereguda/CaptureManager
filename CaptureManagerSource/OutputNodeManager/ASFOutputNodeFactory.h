#pragma once
#include <guiddef.h>
#include <map>
#include <string>

#include "../Common/BaseUnknown.h"
#include "../CaptureManagerBroker/IOutputNodeFactory.h"
#include "../CaptureManagerBroker/SinkCommon.h"

namespace CaptureManager
{
	namespace Sinks
	{
		namespace OutputNodeFactory
		{
			class ASFOutputNodeFactory :
				public BaseUnknown<IOutputNodeFactory>

			{
			public:

				static GUIDToNamePair getGUIDToNamePair();

				// IOutputNodeFactory interface

				STDMETHOD(createOutputNodes)(
					VARIANT aArrayPtrCompressedMediaTypes,
					BSTR aFileName,
					VARIANT* aArrayPtrTopologyOutputNodes);

				STDMETHOD(createOutputNodes)(
					VARIANT aArrayPtrCompressedMediaTypes,
					IUnknown *aPtrByteStreamActivate,
					VARIANT *aArrayPtrTopologyOutputNodes);

			};
		}
	}
}