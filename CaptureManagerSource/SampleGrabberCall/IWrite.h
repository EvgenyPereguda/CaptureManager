#pragma once
#include <Unknwn.h>

namespace CaptureManager
{
	namespace Sinks
	{
		namespace SampleGrabberCall
		{

			MIDL_INTERFACE("7AAC8A3F-2FD9-4979-A35C-F302E3EE62D8")
			IWrite : public IUnknown
			{
			public:
				
				virtual void writeData(
					const BYTE* aPtrSampleBuffer,
					DWORD aSampleSize) = 0;
			};
		}
	}
}