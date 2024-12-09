#pragma once

#include <vector>
#include <string>
#include "../Common/IInnerCaptureProcessor.h"
#include "../Common/ComPtrCustom.h"

namespace CaptureManager
{
	namespace Sources
	{
		namespace AudioEndpointCapture
		{
			class AudioEndpointCaptureProcessorFactory
			{
			public:

				static HRESULT createAudioEndpointCaptureProcessors(
					std::vector<CComPtrCustom<IInnerCaptureProcessor>>& aVectorAudioEndpointCaptureProcessors);

				static HRESULT createAudioEndpointCaptureProcessor(
					std::wstring aSymbolicLink,
					IInnerCaptureProcessor** aPtrPtrIInnerCaptureProcessor);

			private:
				AudioEndpointCaptureProcessorFactory() = delete;
				~AudioEndpointCaptureProcessorFactory() = delete;
				AudioEndpointCaptureProcessorFactory(const AudioEndpointCaptureProcessorFactory&) = delete;
				AudioEndpointCaptureProcessorFactory& operator=(const AudioEndpointCaptureProcessorFactory&) = delete;

			};
		}
	}
}