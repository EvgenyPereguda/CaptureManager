#pragma once

#include <Unknwn.h>
#include <string>


namespace CaptureManager
{
	
	struct ISampleGrabberCallInner;

	namespace Sinks
	{
		class SampleGrabberCallFactory
		{
		public:

			static HRESULT createSampleGrabberOutputNode(
				REFGUID aRefMajorType,
				REFGUID aRefSubType,
				std::wstring& aReadMode,
				LONG aSampleByteSize,
				ISampleGrabberCallInner** aPtrPtrISampleGrabber);

		private:
			SampleGrabberCallFactory();
			virtual ~SampleGrabberCallFactory();
		};
	}
}