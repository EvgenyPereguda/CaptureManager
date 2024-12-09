#pragma once
#include <Unknwnbase.h>

struct IMFSample;

namespace CaptureManager
{
	namespace Core
	{

		MIDL_INTERFACE("59A8020F-6B37-4D90-B95B-1039EB36FF54")
		IMemorySampleManager : public IUnknown
		{
			virtual HRESULT getSample(
			IUnknown* aPtrDevice,
			IMFSample** aPtrPtrSample) = 0;

			virtual void clearMemory() = 0;
		};
	}
}