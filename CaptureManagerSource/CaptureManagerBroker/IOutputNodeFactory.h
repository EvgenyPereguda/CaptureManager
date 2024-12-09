#pragma once
#include <guiddef.h>
#include <map>
#include <string>
#include <Unknwnbase.h>

namespace CaptureManager
{
	MIDL_INTERFACE("FDD07B70-B134-4C47-9FDF-CF634E81612C")
	IOutputNodeFactory : public IUnknown
	{
	public:

		STDMETHOD(createOutputNodes)(
			VARIANT aArrayPtrCompressedMediaTypes,
			BSTR aFileName,
			VARIANT* aArrayPtrTopologyOutputNodes)PURE;

		STDMETHOD(createOutputNodes)(
			VARIANT aArrayPtrCompressedMediaTypes,
			IUnknown *aPtrByteStreamActivate,
			VARIANT *aArrayPtrTopologyOutputNodes)PURE;

	};
}