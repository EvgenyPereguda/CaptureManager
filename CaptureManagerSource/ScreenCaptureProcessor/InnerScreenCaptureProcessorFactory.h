#pragma once

struct IDXGIOutput1;

namespace CaptureManager
{
	namespace Sources
	{
		namespace ScreenCapture
		{
			struct IInnerScreenCaptureProcessor;

			class InnerScreenCaptureProcessorFactory
			{
			public:

				enum Type
				{
					Regular,
					Landscape
				};

				InnerScreenCaptureProcessorFactory();
				~InnerScreenCaptureProcessorFactory();

				static long create(
					Type aType,
					IDXGIOutput1* aPtrOutput,
					IInnerScreenCaptureProcessor** aPtrPtrIInnerScreenCaptureProcessor);
			};
		}
	}
}