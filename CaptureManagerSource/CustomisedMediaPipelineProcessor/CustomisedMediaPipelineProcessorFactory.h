#pragma once

#include <Unknwn.h>

#include "../TopologyResolver/CollectionOfIDs.h"

struct IMFTopology;
struct IMFPresentationClock;

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			struct IMediaPipeline;

			class CustomisedMediaSession;

			class CustomisedMediaPipelineProcessorFactory
			{
			public:
				static HRESULT createCustomisedMediaPipelineProcessor(
					CustomisedMediaSession* aPtrCustomisedMediaSession,
					CollectionOfIDs& aRefCollectionOfIDs,
					IMFTopology* aPtrTopology,
					IMFPresentationClock* aPtrPresentationClock,
					IMediaPipeline** aPtrPtrIMediaPipeline);
			private:
				CustomisedMediaPipelineProcessorFactory() = delete;
				~CustomisedMediaPipelineProcessorFactory() = delete;
				CustomisedMediaPipelineProcessorFactory(const CustomisedMediaPipelineProcessorFactory&) = delete;
				CustomisedMediaPipelineProcessorFactory& operator=(const CustomisedMediaPipelineProcessorFactory&) = delete;
			};
		}
	}
}