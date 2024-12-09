#pragma once
#include "../Common/MFHeaders.h"


namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			MIDL_INTERFACE("D758498B-72B8-4FA0-BAD2-BE1BDA9EEBE6")
			IMediaPipelineProcessorControl : public IUnknown
			{
				virtual HRESULT pause() = 0;

				virtual HRESULT restart() = 0;

				virtual HRESULT removeStreamSink(IMFStreamSink* aPtrStreamSink) = 0;

				virtual HRESULT checkSwitcherNode(const TOPOID& aRefSwitcherNodeID) = 0;

				virtual HRESULT registerStreamSink(const TOPOID& aRefStreamSinkNodeID,
					IMFPresentationClock* aPtrPresentationClock) = 0;
			};
		}
	}
}