#pragma once

#include <Unknwnbase.h>

struct IMFSample;

namespace CaptureManager
{
	MIDL_INTERFACE("2DD3D3DB-A2EB-4777-B3E9-8C00D7A11DB2")
	ISampleConvertor : public IUnknown
	{
		virtual HRESULT Convert(IMFSample* aPtrInputSample, IMFSample** aPtrPtrOutputSample) = 0;
	};
}