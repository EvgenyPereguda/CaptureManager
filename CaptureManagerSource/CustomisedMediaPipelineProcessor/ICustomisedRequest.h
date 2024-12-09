#pragma once

#include <Unknwn.h>

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{			
			MIDL_INTERFACE("810299E5-1E4F-49FF-AA4A-A808EA70D81F")
			ICustomisedRequest : public IUnknown
			{
			public:

				enum CustomisedRequestType : int
				{
					SourceRequest = 0,
					SourceStreamRequest,
					SourceStreamPushNewSampleRequest,
					StreamSinkRequest,
					PullNewSampleRequest,
					PushNewSampleRequest
				};

			public:
				
				virtual CustomisedRequestType getCustomisedRequestType() = 0;

				virtual IUnknown* getPtrUnkSender() = 0;

				virtual IUnknown* getPtrUnkSenderTopologyNode() = 0;

				virtual DWORD getIndexOfStreamNode() = 0;
			};
		}
	}
}