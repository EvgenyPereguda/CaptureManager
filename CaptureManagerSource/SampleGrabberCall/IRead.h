#pragma once
#include <Unknwn.h>

namespace CaptureManager
{
	namespace Sinks
	{
		namespace SampleGrabberCall
		{
			MIDL_INTERFACE("6D050C9B-5C34-45D8-BAD2-10E9B18B376C")
			IRead : public IUnknown
			{
			public:
				virtual HRESULT readData(
					unsigned char* aPtrData,
					DWORD* aPtrSampleSize) = 0;

			};
		}
	}
}