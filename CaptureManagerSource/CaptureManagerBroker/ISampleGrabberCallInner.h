#pragma once
#include <Unknwnbase.h>

namespace CaptureManager
{	

	MIDL_INTERFACE("77FAEB7D-6D97-40AE-B17B-21BA5C022145")
	ISampleGrabberCallInner : public IUnknown
	{
	public:
		
		virtual HRESULT readData(
			unsigned char* aPtrData,
			DWORD& aRefByteSize) = 0;
	};
}