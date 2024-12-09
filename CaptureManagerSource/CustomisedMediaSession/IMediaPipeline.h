#pragma once
#include "../Common/MFHeaders.h"


namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{			
			MIDL_INTERFACE("971B67BA-1AB4-4091-9505-92B90F6768D0")
			IMediaPipeline : public IMFAsyncCallback
			{
			public:

				IMediaPipeline()
				{
				}

				virtual ~IMediaPipeline()
				{
				}

				virtual HRESULT startSources(
					const GUID* aConstPtrGUIDTimeFormat,
					const PROPVARIANT* aConstPtrVarStartPosition) = 0;

				virtual HRESULT pauseSources() = 0;

				virtual HRESULT stopSources() = 0;
				
				virtual HRESULT finishStreamSinkStopping() = 0;

				virtual HRESULT finishStreamSinkPausing() = 0;
			};
		}
	}
}