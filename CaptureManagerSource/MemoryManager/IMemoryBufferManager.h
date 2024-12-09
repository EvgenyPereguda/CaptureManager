#pragma once
#include <Unknwnbase.h>

struct IMFMediaBuffer;

namespace CaptureManager
{
	namespace Core
	{

		MIDL_INTERFACE("AE84DF74-C400-4C35-9474-CD9ED3BC1A79")
		IMemoryBufferManager : public IUnknown
		{
			virtual HRESULT getMediaBuffer(
				IMFMediaBuffer** aPtrPtrMediaBuffer) = 0;

			virtual void clearMemory() = 0;

		};
	}
}